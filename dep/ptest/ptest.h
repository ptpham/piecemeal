

#include <string>
#include <functional>

namespace ptest {
  size_t add(const std::string&, std::function<void()>);
}

#define ASSERT(cond) if (!(cond)) throw std::runtime_error(string(__FILE__) + \
  ":" + std::to_string(__LINE__) + " " + #cond)

#define TEST(name) \
static void func_##name(); \
static size_t registration_##name = ptest::add(string(__FILE__)+ ":" + #name, func_##name); \
static void func_##name()


