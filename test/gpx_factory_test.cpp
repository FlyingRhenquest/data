/**
 * Verify GPX factory can read a sample GPX file from Mytracks.
 */

#include <boost/bind.hpp>
#include "cppunit"
#include "gpx_factory.hpp"
#include <iomanip>
#include <iostream>
#include <utility>
#include <vector>

class gpx_factory_test : public test_class {
  CPPUNIT_TEST_SUITE(gpx_factory_test);
  CPPUNIT_TEST(test_read);
  CPPUNIT_TEST_SUITE_END();
  
  typedef std::pair<double, fr::coordinates::lat_long> gpx_pair;
  typedef std::vector<gpx_pair> gpx_vector;

  void add_line(gpx_vector *storage, double at_time, const fr::coordinates::lat_long &ll)
  {
    storage->push_back(std::make_pair(at_time, ll));
  }

  bool equalish(double left, double right)
  {
    return fabs(left - right) < .0000001;
  }

public:

  void test_read()
  {
    fr::data::gpx_factory factory("Test.gpx");
    gpx_vector storage;
    factory.available.connect(boost::bind(&gpx_factory_test::add_line, this, &storage, _1, _2));
    factory.process();
    // We should now have a vector of GPX data...
    CPPUNIT_ASSERT(storage.size() > 0);
    gpx_vector::iterator i = storage.begin();

    CPPUNIT_ASSERT(equalish(i->first, 1361641839.898));
    CPPUNIT_ASSERT(equalish(i->second.get_lat(), 40.150207));
    CPPUNIT_ASSERT(equalish(i->second.get_long(), -105.107035));
    CPPUNIT_ASSERT(equalish(i->second.get_alt(), 1522.7));
  }

};

CPPUNIT_TEST_SUITE_REGISTRATION(gpx_factory_test);
