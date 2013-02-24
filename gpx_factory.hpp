/**
 * A GPX factory. For each coordinate in the GPX file, it will signal
 * listeners to "available" with a double time and a fr::coordinates::
 * lat_long point. This could be used to build a vector or something.
 *
 * I'm writing this to some sample data dumped by the Mytracks android
 * application, so I might miss some stuff from the standard.
 */

#ifndef _HPP_GPX_FACTORY
#define _HPP_GPX_FACTORY

#include <boost/signals/signal2.hpp>
#include "engine.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include "lat_long.hpp"
#include "node_engine.hpp"
#include "string_converts.hpp"
#include <sys/time.h>
#include <time.h>
#include "timezone_manager.hpp"
#include "xml_node.hpp"

namespace fr {

  namespace data {

    class gpx_factory {
    private:

      std::string gpx_filename;
      const std::string trkpt = std::string("trkpt");
      const std::string ele = std::string("ele");
      const std::string node_time = std::string("time");
      const std::string lat = std::string("lat");
      const std::string lon = std::string("lon");

    public:
      gpx_factory(const std::string &filename) : gpx_filename(filename)
      {
      }

      ~gpx_factory()
      {
      }

      // available provides timestamp (as a double) and a lat_long
      boost::signals2::signal<void(double, const fr::coordinates::lat_long &)> available;

      // Reads in the file and starts spitting out points through
      // available
      void process()
      {
	// GPX coordinates use GMT (Well, UTC, but close enough.)
	fr::time::timezone_manager tz("GMT");
	cppxml::node_engine engine(gpx_filename);
	cppxml::node_pointer document;
	document = engine.get_document();
	cppxml::child_vector xml_points = document->find_children(trkpt);
	cppxml::child_vector::iterator i;
	fr::time::string_to<double> string_to_double;
	fr::time::string_to<timeval> string_to_time;
	for (i = xml_points.begin(); i != xml_points.end(); ++i) {
	  std::shared_ptr<std::string> lat_string, lon_string;
	  double point_lat, point_lon, time_at, elevation;
	  timeval timeval_at;
	  time_at = 0.0;
	  elevation = 0.0;
	  // Dereference the iterator to get at the shared pointer
	  lat_string = (*i)->find_attribute(lat);
	  lon_string = (*i)->find_attribute(lon);
	  if (nullptr == lat_string.get()) {
	    throw std::string("Error: Missing lat attribute in gpx node.");
	  }
	  if (nullptr == lon_string.get()) {
	    throw std::string("Error: Missing lon attribute in gpx node.");
	  }
	  point_lat = string_to_double(*lat_string);
	  point_lon = string_to_double(*lon_string);
	  cppxml::node_pointer ele_node = (*i)->find_child(ele);
	  if (nullptr != ele_node.get()) {
	    std::string ele_string = ele_node->get_text();
	    if (ele_string.length() > 0) {
	      elevation = string_to_double(ele_string);
	    }
	  }
	  cppxml::node_pointer time_node = (*i)->find_child(node_time);
	  if (nullptr != time_node.get()) {
	    std::string time_string = time_node->get_text();
	    if (time_string.length() > 0) {
	      timeval_at = string_to_time(time_string);

	      time_at = (double) timeval_at.tv_sec + ((double) timeval_at.tv_usec / 1000000.0);
	    }
	  }
	  fr::coordinates::lat_long ll(point_lat, point_lon, elevation);

	  available(time_at, ll);
	}
      }
    };

  }
}

#endif

