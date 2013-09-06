/**
 *
 * Validates env manager is working correctly
 * 
 */

#include <cppunit>
#include "env_manager.hpp"
#include <stdlib.h>
#include <string>


class test_env_manager : public test_class {

  CPPUNIT_TEST_SUITE(test_env_manager);
  CPPUNIT_TEST(basic_happy_test);
  CPPUNIT_TEST(basic_sad_test);
  CPPUNIT_TEST_SUITE_END();

public:

  // Test where nothing should go wrong
  void basic_happy_test()
  {
    setenv("foo", "Yeah...foo...", 1);
    setenv("bar", "4", 1);
    int bar = 0;
    std::string foo("");
    fr::data::env_manager mgr;
    mgr.add("foo")->mandatory()->bind_to<std::string>(&foo);
    mgr.add("bar")->mandatory()->bind_to<int>(&bar);
    try {
      mgr.validate();
    } catch (std::string err) {
      CPPUNIT_FAIL(err);
    }
    CPPUNIT_ASSERT(foo == std::string("Yeah...foo..."));
    CPPUNIT_ASSERT(bar == 4);
  }

  // Make sure validate throws
  void basic_sad_test()
  {
    unsetenv("foo");
    std::string foo("");
    fr::data::env_manager mgr;
    mgr.add("foo", "", "Foo should fail")->mandatory()->bind_to<std::string>(&foo);
    bool it_threw = false;
    try {
      mgr.validate();
    } catch (std::string err) {
      CPPUNIT_ASSERT(err.find("foo") != std::string::npos);
      // Verify description was added
      CPPUNIT_ASSERT(err.find("should fail") != std::string::npos);
      it_threw = true;
    }
    // Just in case it didn't
    CPPUNIT_ASSERT(it_threw);
  }


};

CPPUNIT_TEST_SUITE_REGISTRATION(test_env_manager);
