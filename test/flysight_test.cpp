/**
 * Verify I can read flysight data from the test data file (Which
 * contains less than 1 second of data for brevity's sake.)
 */

#include <boost/bind.hpp>
#include "cppunit"
#include "flysight_factory.hpp"
#include <math.h>
#include <utility>
#include <vector>

class flysight_test : public test_class {

  CPPUNIT_TEST_SUITE(flysight_test);
  CPPUNIT_TEST(test_read);
  CPPUNIT_TEST_SUITE_END();

  typedef std::pair<double, fr::coordinates::lat_long> coordinate_pair;
  typedef std::vector<coordinate_pair> coordinate_vector;

  // I'm ignoring a lot of data from the factory which I probably should
  // write some tests to validate one of these days
  void populate_vector(double at_time, const fr::coordinates::lat_long &loc, coordinate_vector *populate)
  {
    populate->push_back(std::make_pair(at_time, loc));
  }

public:
  
  void test_read()
  {
    fr::coordinates::lat_long point;
    try {
      coordinate_vector points;
      fr::data::flysight_factory factory("flysight_test.csv");
      
      factory.available.connect(boost::bind(&flysight_test::populate_vector, this, _1, _2, &points));
      factory.process();
      CPPUNIT_ASSERT(points.size() == 4);
      point = points[0].second;
      // Make sure we read it in approximately the correct place
    } catch (std::string err) {
      CPPUNIT_FAIL(err);
    }
    CPPUNIT_ASSERT(floor(point.get_lat()) == 53.0);
    CPPUNIT_ASSERT(floor(point.get_long()) == -115.0);
    CPPUNIT_ASSERT(floor(point.get_alt()) == 4576.0);
  }

};

CPPUNIT_TEST_SUITE_REGISTRATION(flysight_test);
