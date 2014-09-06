

#include <string>
#include <functional>

namespace ptest {
  size_t add(const std::string&, const std::string&, std::function<void()>);

  struct error : std::runtime_error {
    std::string file;
    size_t line;

    error(const std::string& file, size_t line, const std::string& msg) :
      runtime_error(msg), file(file), line(line) { }
  };
}

#define ASSERT(cond) if (!(cond)) throw ptest::error(__FILE__, \
  __LINE__, #cond)

#define TEST(name) \
static void ptest_##name(); \
static size_t registration_##name = ptest::add(__FILE__, #name, ptest_##name); \
static void ptest_##name()


