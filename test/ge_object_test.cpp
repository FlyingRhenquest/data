/**
 * Tests ge_object. Except I don't really feel like writing a
 * to_string test tonight, and to_string is about all there
 * really is to that object. I'll at least put a thing in
 * for testing that the object type gets created correctly
 * and make a todo for the to_string stuff. I'm probably
 * going to rip all that code out later on and replace it
 * with something that sucks a little less anyway.
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

#include <cppunit>
#include "ge_object.hpp"
#include "lat_long.hpp"
#include <vector>

class ge_object_test : public test_class {

  CPPUNIT_TEST_SUITE(ge_object_test);
  CPPUNIT_TEST(test_useless_stuff);
  CPPUNIT_TEST_SUITE_END();

public:

  void test_useless_stuff()
  {
    fr::coordinates::lat_long some_point(1.0, 1.0, 0.0);
    fr::data::ge_object obj(some_point);
    CPPUNIT_ASSERT(obj.get_object_type() == "Point");
    std::vector<fr::coordinates::lat_long> points;
    fr::coordinates::lat_long  another_point(2.0, 2.0, 0.0);
    points.push_back(some_point);
    points.push_back(another_point);
    fr::data::ge_object line_obj(points);

    CPPUNIT_ASSERT(line_obj.get_object_type() == "LineString");
    points.push_back(some_point); // make it a polygon
    fr::data::ge_object poly_obj(points);
    std::cout << std::endl << poly_obj.to_string() << std::endl;

    CPPUNIT_ASSERT(poly_obj.get_object_type() == "Polygon");
  }

};

CPPUNIT_TEST_SUITE_REGISTRATION(ge_object_test);
