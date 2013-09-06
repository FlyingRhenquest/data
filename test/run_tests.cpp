/**
 * Test runner (Blatantly stolen from the Cppunit tutorial.)
 */

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "timezone_manager.hpp"

int main(int argc, char **argv)
{
  fr::time::timezone_manager manager("GMT"); // Force test timezone to GMT
  CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest(registry.makeTest());
  bool success = runner.run("", false);
  return success;
}
