/**
 * This is a factory for fr::coordinates::ecef_vel coordinates.
 * It reads a file in the standard SP3 format and uses
 * boost::signal2 to notify listeners when new ecef_vel lines are
 * ready.
 *
 * To read a file:
 * 1) Create an sp3_factory with a filename
 * 2) Write a function that accepts a const std::string reference and
 *    a const fr::data::sp3_factory::sp3_pair
 *    reference and does something with it (like stuffs it into a vector.)
 *    The string is the satellite name (1-32 for GPS satellites.)
 * 3) Connect the function to the data_available signal (factory.data_available.connect(function)
 * 4) Run factory.process()
 *
 * Copyright (c) 2013 Bruce Ide
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

#include <boost/signals2.hpp>
#include <iostream>
#include "ecef_vel.hpp"
#include <fstream>
#include <string>
#include <string.h>
#include <time.h>
#include <utility>

#ifndef _HPP_SP3_FACTORY
#define _HPP_SP3_FACTORY

namespace fr {

  namespace data {

    class sp3_factory {
      double time_at;
      double x,y,z,dx,dy,dz;
      bool have_xyz;
      bool have_deltas;
      std::string filename;
      std::string satellite_name;

      void notify_listeners()
      {
	if (have_xyz && have_deltas) {
	  data_available(satellite_name, std::make_pair(time_at, fr::coordinates::ecef_vel(x,y,z,dx,dy,dz)));
	  reset();
	}
      }

      void read_time(std::fstream &f)
      {
	struct tm my_time;
	double seconds; // fractional seconds
	memset(&my_time, '\0', sizeof(struct tm));
	f >> my_time.tm_year;
	my_time.tm_year -= 1900;
	f >> my_time.tm_mon;
	my_time.tm_mon--; // Zero indexed in C
	f >> my_time.tm_mday;
	f >> my_time.tm_hour;
	f >> seconds;
	my_time.tm_sec = (int) seconds;
	seconds -= my_time.tm_sec;
	time_t retval = timegm(&my_time);
	time_at = retval + seconds;
	notify_listeners();
      }

      // All the velocity/location lines all look the same
      void read_line(std::fstream &f, double &v1, double &v2, double &v3)
      {
	f >> satellite_name;
	f >> v1;
	f >> v2;
	f >> v3;
	// Units are in KM, I want them in meters
	v1 *= 1000;
	v2 *= 1000;
	v3 *= 1000;
      }

      void eat_line(std::fstream &f)
      {
	char buffer[120];
	f.getline(buffer, sizeof(buffer));
      }

      void read_position(std::fstream &f)
      {
	read_line(f,x,y,z);
	have_xyz = true;
	notify_listeners();
      }

      void read_velocity(std::fstream &f)
      {
	read_line(f,dx,dy,dz);
	have_deltas = true;
	notify_listeners();
      }

      void reset()
      {
	x = y = z = dx = dy = dz = 0.0;
	have_xyz = have_deltas = false;
	satellite_name = "";
      }

    public:
      // SP3 pair consists of a time and the coordinate data at that time.
      typedef std::pair<double,fr::coordinates::ecef_vel> sp3_pair;
      // Signal is signalled when a coordinate is available. Notifies
      // with the satelltie name and the ecef data (time and points.)
      boost::signals2::signal<void(const std::string &, const sp3_pair &)> data_available;

      sp3_factory(const std::string &filename) : filename(filename)
      {
	reset();
      }

      ~sp3_factory()
      {
      }

      void process()
      {
	std::fstream f(filename, std::fstream::in);
	if (!f.fail()) {
	  while(f.good()) {
	    char start;
	    f >> start;
	    switch(start) {
	    case '*':
	      // This is a time line
	      read_time(f);
	      break;
	    case 'P':
	      // This is a position line
	      read_position(f);
	      break;
	    case 'V':
	      // This is a velocity line
	      read_velocity(f);
	      break;
	    }
	    // Discard to end of line
	    eat_line(f);
	  }
	} else {
	  std::cout << "Unable to open " << filename << std::endl;
	}
      }

    };

  }
}

#endif
