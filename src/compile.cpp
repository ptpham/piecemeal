#include "piecemeal/compile.hpp"
#include "private/logic_export.hpp"
#include "piecemeal/stdaux.hpp"
#include "piecemeal/stdfmt.hpp"
#include "piecemeal/logic.hpp"

namespace piecemeal {
  namespace compile {
    template <class T, class F>
    void node_traversal(T node, F fn) {
      vector<T> stack = { node };
      vector<size_t> depths = { 0 };
      while (stack.size() > 0) {
        auto next = stack.back();
        stack.pop_back();
        auto depth = depths.back();
        depths.pop_back();
        fn(next, depth);
        
        for(auto i = 0; i < next->size(); i++) {
          stack.push_back(next->at(next->size() - i - 1));
          depths.push_back(depth + 1);
        }
      }
    }

    template <class T>
    vector<T> gather_leaves(T node) {
      vector<T> result;
      node_traversal(node, [&](T child, size_t depth) {
        if (child->size() > 0) return;
        result.push_back(child);
      });
      return result;
    }

    template <class T, size_t N>
    prop<T,N> extract_literal(unordered_dimap<string>& tokens,
      dag::node<string> node) {
      prop<T,N> result(0);

      auto leaves = gather_leaves(node);
      for (size_t i = 0; i < leaves.size(); i++) {
        if (!is_var(leaves[i])) result[i] = tokens.at(leaves[i]->value);
        else result[i] = prop<T,N>::empty();
      };
      return result;
    }

    template <class T, size_t N>
    prop<T,N> extract_push(unordered_dimap<string>& vars,
      dag::node<string> node) {
      prop<T,N> result;

      auto leaves = gather_leaves(node);
      for (size_t i = 0; i < leaves.size(); i++) {
        if (!is_var(leaves[i])) continue;
        result[i] = vars.at(leaves[i]->value);
      }
      return result;
    }

    template <class T, size_t N>
    string recover(const vector<string>& lookup,
      prop<T,N> p, array<uint8_t,N> depth) {
      size_t current = 0, end = 0;
      string result;

      // Compact the non-empty values of p to the left
      for (size_t i = 0; i < N; i++) {
        if (p[i] == prop<T,N>::empty()) p[i] = 0;
        if (p[i] == 0) { depth[i] = 0; continue; }
        swap(depth[end], depth[i]);
        swap(p[end], p[i]);
        end++;
      }

      // Generate the string
      for (size_t i = 0; i < N; i++) {
        bool upward = current > depth[i];
        while (current < depth[i]) { result += "("; current++; }
        while (current > depth[i]) { result += ")"; current--; }

        bool valid_next = i < N - 1 && p[i+1] != 0 && p[i+1] != prop<T,N>::empty();
        if (valid_next && upward) result += " ";
        result += lookup[p[i]];
        if (valid_next && depth[i + 1] >= current) result += " ";
      }

      // Close all parens
      while (current > 0) { result += ")"; current--; }
      if (result.size() > 0 && result[0] != '(') result = "(" + result + ")";
      return result;
    }

    template <class T, size_t N>
    term<T,N> parse_term(unordered_dimap<string>& tokens,
      unordered_dimap<string>& vars, dag::node<string> node) {
      auto head = extract_literal<T,N>(tokens, node);
      auto push = extract_push<T,N>(vars, node);
      return term<T,N>(head, push, logic::invert(push));
    }

    pair<bool,bool> distinct_var_mask(dag::node<string> node) {
      auto first = node->at(1);
      auto second = node->at(2);
      return make_pair(is_var(first), is_var(second));
    }

    template <class T, size_t N>
    void parse_distinct(unordered_dimap<string>& tokens,
      unordered_dimap<string>& vars, rule<T,N>& rule,
      dag::node<string> term) {
      
      auto mask = distinct_var_mask(term);
      if (!mask.first && !mask.second) return;
      auto left = term->at(1), right = term->at(2);
      auto* left_map = &vars, *right_map = &vars;

      auto* target = &rule.distincts;
      if (mask.first ^ mask.second) {
        target = &rule.prevents;
        if (!mask.first) swap(left, right);
        right_map = &tokens;
      }
      
      T first = left_map->at(left->value);
      T second = right_map->at(right->value);
      target->push_back({first,second});
    }

    template <class T, size_t N>
    array<uint8_t,N> parse_leaf_depth(dag::node<string> node) {
      array<uint8_t,N> result = stdaux::filled_array<uint8_t,N>(0);
      size_t last = 0;

      node_traversal(node, [&](dag::node<string> child, size_t depth) {
        if (child->size() > 0) return;
        result[last++] = depth;
      });
      return result;
    }

    template <class T, size_t N>
    void parse_sentence(parse<T,N>& parse, dag::node<string> child) {
      if (child->size() == 0) return;
      auto& depths = parse.depths;
      rule<T,N> rule;

      // Construct helper function to add the depth structure to the parse
      auto add_depths = [&](dag::node<string> node, const prop<T,N>& prop) {
        if (node->size() == 0) node = dag::convert<string>({ node });
        auto depth = parse_leaf_depth<T,N>(node); 
        auto id = prop[0];

        if (depths.find(id) != depths.end() && depths[id] != depth) {
          throw runtime_error("Relation has inconsistent structure " + tree::dumps(node));
        } depths[id] = depth;
      };

      // Find and index all variables for a consistent ordering
      unordered_dimap<string> vars;
      node_traversal(child, [&](dag::node<string> node, size_t depth) {
        if (is_var(node)) vars.at(node->value);
      });

      // Relations and propositions will not start with the leading '<='
      if (child->size() == 1 || child->at(0)->value != "<=") {
        auto extracted = extract_literal<T,N>(parse.tokens, child);
        parse.props.emplace(extracted);
        add_depths(child, extracted);
        return;
      }

      // Extract head and various body terms for full rules
      rule.head = parse_term<T,N>(parse.tokens, vars, child->at(1));
      for (size_t i = 2; i < child->size(); i++) {
        auto term = child->at(i);

        // Handle distinct terms
        if (term->size() > 1 && term->at(0)->value == "distinct") {
          parse_distinct(parse.tokens, vars, rule, term);
          continue;
        }

        // Handle positive and negative terms
        auto* dst = &rule.positives;
        if (term->size() > 1 && term->at(0)->value == "not") {
          dst = &rule.negatives;
          term = term->at(1);
        }

        auto extracted = parse_term<T,N>(parse.tokens, vars, term);
        add_depths(term, extracted.literal);
        dst->push_back(extracted);
      }

      parse.rules.push_back(rule);
    }

    template <class T, size_t N>
    parse<T,N> parse_sentences(dag::node<string> root) {
      parse<T,N> result;
      for (auto& child : *root) parse_sentence(result, child);
      return result;
    }

#define EXPORT(T,N) \
    template term<T,N> parse_term(unordered_dimap<string>& tokens, \
      unordered_dimap<string>& vars, dag::node<string> node); \
    template parse<T,N> parse_sentences(dag::node<string>); \
    template string recover(const vector<string>&, prop<T,N>, \
      array<uint8_t,N>);

    DEFAULT_LOGIC_EXPORT
#undef EXPORT
  }
}



