/**
 * Verifies env_memento works
 */

#include <cppunit>
#include "env_memento.hpp"
#include "env_var.hpp"
#include <stdlib.h>
#include <sys/time.h>

using namespace fr::data;

class test_env_memento : public test_class {

  CPPUNIT_TEST_SUITE(test_env_memento);
  CPPUNIT_TEST(basic_test);
  CPPUNIT_TEST(time_test);
  CPPUNIT_TEST_SUITE_END();

public:

  void basic_test()
  {
    unsetenv("wibble");
    unsetenv("tribble");
    env_var wibble("wibble", "wibble", "wibble wibble wibble!");
    env_memento memento(&wibble);
    std::string wibval;
    memento.mandatory()->bind_to<std::string>(&wibval);
    CPPUNIT_ASSERT(memento.validate());
    CPPUNIT_ASSERT(wibval == "wibble");
    env_var tribble("tribble");
    env_memento trimento(&tribble);
    trimento.mandatory();
    CPPUNIT_ASSERT(!trimento.validate());
  }

  // I honestly don't know why you'd want to store a time in this
  // format in the environment but if you want to, by God I support it!

  void time_test()
  {
    std::string expected("2013-09-05T14:30:23.500000Z");
    setenv("TEST_TIME", expected.c_str(), 1);
    timeval tv;
    memset(&tv, 0, sizeof(timeval));
    env_var v("TEST_TIME");
    env_memento mento(&v); // mmm mentos...
    mento.mandatory()->bind_to<timeval>(&tv);
    mento.validate();
    fr::time::to_string<timeval> time_to_string;
    std::string actual = time_to_string(tv, "%Y-%m-%dT%H:%M:%S.%fZ");

    CPPUNIT_ASSERT(expected == actual);
  }

};

CPPUNIT_TEST_SUITE_REGISTRATION(test_env_memento);
