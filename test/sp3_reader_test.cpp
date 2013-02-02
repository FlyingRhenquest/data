/**
 * Verify my NGA reader can read at least a couple lines of the included
 * SP3 file.
 *
 * Copyright 2013 Bruce Ide
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <boost/bind.hpp>
#include <cppunit>
#include "sp3_factory.hpp"
#include <utility>
#include <math.h>
#include <vector>
#include <iomanip>

class sp3_reader_test : public test_class {

  CPPUNIT_TEST_SUITE(sp3_reader_test);
  CPPUNIT_TEST(check_some_lines);
  CPPUNIT_TEST_SUITE_END();

  std::vector<fr::data::sp3_factory::sp3_pair> data;

  bool equalish(double left, double right)
  {
    if ((fabs(left) - fabs(right)) < .0001) {
      return true;
    }
    return false;
  }

  void got_ecef_vel(const std::string &satellite_name, const fr::data::sp3_factory::sp3_pair &line)
  {
    if (satellite_name == "1") {
      data.push_back(line);
    }
  }

public:
  void check_some_lines()
  {
    fr::data::sp3_factory factory("nga16556.eph");
    factory.data_available.connect(boost::bind(&sp3_reader_test::got_ecef_vel, this, _1, _2));
    factory.process();
    // That's all there is to it. I should now have a full vector of
    // ephemeris lines.
    CPPUNIT_ASSERT(data.size() > 2);
    fr::coordinates::ecef_vel line = data[0].second;
    CPPUNIT_ASSERT(equalish(line.get_x(), 9499209.153));
    CPPUNIT_ASSERT(equalish(line.get_y(), 13635225.244));
    CPPUNIT_ASSERT(equalish(line.get_z(), -20730158.212));
    CPPUNIT_ASSERT(equalish(line.get_dx(), -26090984.781));
    CPPUNIT_ASSERT(equalish(line.get_dy(), 3407283.545));
    CPPUNIT_ASSERT(equalish(line.get_dz(), -9719569.472));
    line = data[1].second;
    CPPUNIT_ASSERT(equalish(line.get_x(), 7117866.876));
    CPPUNIT_ASSERT(equalish(line.get_y(), 14008819.424));
    CPPUNIT_ASSERT(equalish(line.get_z(), -21424232.489));
    CPPUNIT_ASSERT(equalish(line.get_dx(),-26738103.096 ));
    CPPUNIT_ASSERT(equalish(line.get_dy(), 4900065.956 ));
    CPPUNIT_ASSERT(equalish(line.get_dz(), -5682204.522 ));      
  }

};

CPPUNIT_TEST_SUITE_REGISTRATION(sp3_reader_test);
