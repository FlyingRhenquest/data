/**
 * A factory for points from the flysight GPS device. Timestamps will
 * be in GMT
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

#ifndef _HPP_FLYSIGHT_FACTORY
#define _HPP_FLYSIGHT_FACTORY

#include <boost/signals2.hpp>
#include "flysight_extra_data.hpp"
#include <fstream>
#include <iostream>
#include "lat_long.hpp"
#include "string_converts.hpp"
#include <string>
#include <sys/time.h>
#include <time.h>

namespace fr {

  namespace data {

    class flysight_factory {
      std::string data_filename;
    public:

      // Available signals time, coordinate (lat/long/alt [msl/meters])
      // and extra_data

      boost::signals2::signal<void(double, const fr::coordinates::lat_long &, const flysight_extra_data &)> available;

      flysight_factory(const std::string &filename) : data_filename(filename)
      {
      }

      flysight_factory(const flysight_factory &copy) : data_filename(copy.data_filename)
      {
      }

      ~flysight_factory()
      {
      }

      void process()
      {
	std::ifstream in(data_filename);
	if (!in.is_open()) {
	  std::string err("ERROR: ");
	  err.append("Error opening ");
	  err.append(data_filename);
	  throw err;
	}
	// The data file I was provided had a couple of header lines
	// which I assume are probably there on all the files. In any
	// event I'm just going to eat a couple of lines.
	std::string line;
	getline(in,line);
	getline(in,line);
	// Whip some some string converters (I know this is a little
	// weird but I recall I had a pretty good reason at the time.)
	fr::time::string_to<double> string_to_double;
	fr::time::string_to<timeval> string_to_time;
	fr::time::string_to<int> string_to_int;

	while(in.good() && !in.eof()) {	  
	  const char delim=',';
	  std::string time_s;
	  std::string lat;
	  std::string lon;
	  std::string msl;
	  std::string vel_n;
	  std::string vel_e;
	  std::string vel_down;
	  std::string horiz_acc;
	  std::string vert_acc;
	  std::string speed_acc;
	  std::string gps_fix_type;
	  std::string satellites_locked;
	  timeval tv;
	  double time_for_notification;
	  getline(in, time_s, delim);
	  getline(in, lat, delim);
	  getline(in, lon, delim);
	  getline(in, msl, delim);
	  getline(in, vel_n, delim);
	  getline(in, vel_e, delim);
	  getline(in, vel_down, delim);
	  getline(in, horiz_acc, delim);
	  getline(in, vert_acc, delim);
	  getline(in, speed_acc, delim);
	  getline(in, gps_fix_type, delim);
	  getline(in, satellites_locked);
	  tv = string_to_time(time_s);
	  time_for_notification = tv.tv_sec;
	  time_for_notification += tv.tv_usec / 1000000.0;
	  fr::coordinates::lat_long ll(string_to_double(lat), string_to_double(lon), string_to_double(msl));
	  flysight_extra_data data;
	  data.vel_north = string_to_double(vel_n);
	  data.vel_east = string_to_double(vel_e);
	  data.vel_down = string_to_double(vel_down);
	  data.horiz_acc = string_to_double(horiz_acc);
	  data.vert_acc = string_to_double(vert_acc);
	  data.speed_acc = string_to_double(speed_acc);
	  data.gps_fix_type = string_to_int(gps_fix_type);
	  data.satellites_locked = string_to_int(satellites_locked);
	  available(time_for_notification, ll, data);
	  in.peek(); // Forces EOF at the end
	}
      }

    };

  }
}

#endif
