/**
 * Verify env_var works as expected
 */

#include <cppunit>
#include "env_var.hpp"
#include <stdlib.h>

using namespace fr::data;

class test_env_var : public test_class
{
  CPPUNIT_TEST_SUITE(test_env_var);
  CPPUNIT_TEST(basic_test);
  CPPUNIT_TEST_SUITE_END();
public:

  void basic_test()
  {
    setenv("foo", "bar", 1);
    unsetenv("bar");
    unsetenv("baz");
    env_var foo("foo");
    env_var bar("bar", "baz");
    env_var baz("baz");
    baz.make_mandatory();
    CPPUNIT_ASSERT(foo.validate());
    CPPUNIT_ASSERT(bar.validate());
    CPPUNIT_ASSERT(!baz.validate()); // baz validation should fail
    CPPUNIT_ASSERT(foo.get_value() == std::string("bar"));
    CPPUNIT_ASSERT(bar.get_value() == std::string("baz"));
  }
};

CPPUNIT_TEST_SUITE_REGISTRATION(test_env_var);
