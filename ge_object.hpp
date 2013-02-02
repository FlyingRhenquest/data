/**
 * A very simple (And probably practically useless) google earth object.
 * It can be constructed with coordinates and barf out KML fragments 
 * to render those objects in google earth.
 *
 * If you construct it with a single coordinate, it will assume it's
 * a point and put a placemark there (With some optional text.)
 *
 * If you construct it with a vector of coordinates, it will look
 * at the first and last points in the vector. If they're the same,
 * it will assume you want to render a polygon and render a polygon.
 * If they're not the same it will assume you want to render a line.
 *
 * You still have to build the KML document itself (The document
 * tags and all that crap) and get it in to google earth somehow.
 * Potentially the easiest and least cool way to do this would be to
 * write a KML file and *yawn*... 
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

#ifndef _HPP_GE_OBJECT
#define _HPP_GE_OBJECT

#include "coordinates.hpp"
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>

namespace fr {

  namespace data {

    class ge_object {
      typedef std::vector<fr::coordinates::lat_long> coordinate_vector;
      coordinate_vector coordinates;
      fr::coordinates::lat_long coordinate;
      std::string ge_object_type;
      std::string name;
      std::string description;
      std::string style_url;
      bool altitude_mode;
      bool extrude;
      bool tessellate;

      // Convert vector to lat/long and test for line or
      // polygon type. Note this even operates on vectors
      // of lat/longs, which is a bit inefficient, but you
      // kind of get what you pay for, don't you?
      
      template <typename incoming_vector>
      void convert_vector(incoming_vector &v)
      {
	for (typename incoming_vector::value_type point : v) {
	  fr::coordinates::lat_long pointll = fr::coordinates::converter<fr::coordinates::lat_long>()(point);
	  coordinates.push_back(pointll);
	}
	if ((coordinates.front().get_lat() == coordinates.back().get_lat()) && (coordinates.front().get_long() == coordinates.back().get_long()) && (coordinates.front().get_alt() == coordinates.back().get_alt())) {
	  // This is a polygon
	  ge_object_type = "Polygon";
	} else {
	  // This is a line
	  ge_object_type = "LineString";
	}
      }
      
      void barf_coordinates(std::stringstream &buffer)
      {
	for (fr::coordinates::lat_long point : coordinates) {
	  buffer << point.get_long() << "," << point.get_lat() << "," << point.get_alt() << std::endl;
	}
      }

      void emit_mode(std::stringstream &buffer, bool mode)
      {
	if (mode) {
	  buffer << "1";
	} else {
	  buffer << "0";
	}
      }

      void emit_extrude(std::stringstream &buffer)
      {
	buffer << "<extrude>";
	emit_mode(buffer, extrude);
	buffer << "</extrude>" << std::endl;
      }

      void emit_tessellate(std::stringstream &buffer)
      {
	buffer << "<tessellate>";
	emit_mode(buffer, tessellate);
	buffer << "</tessellate>" << std::endl;
      }

    public:
      ge_object(const fr::coordinates::lat_long &point) : coordinate(point), altitude_mode(false), extrude(false), tessellate(false)
      {
	ge_object_type = "Point";
      }

      ge_object(const fr::coordinates::ecef_vel &point) : altitude_mode(false), extrude(false), tessellate(false)
      {
	coordinate = fr::coordinates::converter<fr::coordinates::lat_long>()(point);
	ge_object_type = "Point";
      }

      // tod_eci vel requires a time, since I transform to ecef prior to latlong
      ge_object(const fr::coordinates::tod_eci_vel &point, double time_at) : altitude_mode(false), extrude(false), tessellate(false)
      {
	coordinate = fr::coordinates::converter<fr::coordinates::lat_long>()(point,time_at);
	ge_object_type = "Point";
      }
      
      ge_object(const std::vector<fr::coordinates::lat_long> &points) : altitude_mode(false), extrude(false), tessellate(false)
      {
	convert_vector(points);
      }

      ge_object(const std::vector<fr::coordinates::ecef_vel> &points) : altitude_mode(false), extrude(false), tessellate(false)
      {
	convert_vector(points);
      }

      ~ge_object()
      {
      }

      std::string to_string()
      {
	std::stringstream buffer("");
	buffer << std::fixed;
	buffer << "<Placemark>" << std::endl;
	buffer << "<name>" << name << "</name>" << std::endl;
	buffer << "<description>" << description << "</description>" << std::endl;
	if (!style_url.empty()) {
	  buffer << "<styleUrl>" << style_url << "</styleUrl>" << std::endl;
	}
	if (ge_object_type == "Point") {
	  buffer << "<Point>" << std::endl;
	  buffer << "  <coordinates>" << coordinate.get_long() << "," << coordinate.get_lat() << "," << coordinate.get_alt() << "</coordinates>" << std::endl;
	  buffer << "</Point>" << std::endl;
	} else if (ge_object_type == "Polygon") {
	  buffer << "<Polygon>" << std::endl;
	  emit_extrude(buffer);
	  emit_tessellate(buffer);
	  if (altitude_mode) {
	    buffer << "<altitudeMode>absolute</altitudeMode>" << std::endl;
	  }
	  buffer << "<outerBoundryIs>" << std::endl;
	  buffer << "<LinearRing>" << std::endl;
	  buffer << "<coordinates>" << std::endl;
	  barf_coordinates(buffer);
	  buffer << "</coordinates>" << std::endl;
	  buffer << "</LinearRing>" << std::endl;
	  buffer << "</outerBoundryIs>" << std::endl;
	  buffer << "</Polygon>" << std::endl;
	} else if (ge_object_type == "LineString") {
	  buffer << "<LineString>" << std::endl;
	  emit_extrude(buffer);
	  emit_tessellate(buffer);
	  if (altitude_mode) {
	    buffer << "<altitudeMode>absolute</altitudeMode>" << std::endl;
	  }
	  buffer << "<coordinates>" << std::endl;
	  barf_coordinates(buffer);
	  buffer << "</coordinates>" << std::endl;
	  buffer << "</LineString>" << std::endl;
	  
	} else {
	  std::cout << "Unknown object type" << std::endl;
	}
	buffer << "</Placemark>" << std::endl;
	return buffer.str();
      }

      std::string get_object_type() const 
      {
	return ge_object_type;
      }
     
      void set_name(const std::string &name)
      {
	this->name = name;
      }

      void set_description(const std::string &description)
      {
	this->description = description;
      }

      void set_style_url(const std::string &style_url)
      {
	this->style_url = style_url;
      }

      void set_altitude_mode(bool mode)
      {
	altitude_mode = mode;
      }

      void set_extrude(bool mode)
      {
	extrude = mode;
      }

      void set_tessellate(bool mode)
      {
	tessellate = mode;
      }

    };
    
  }
}

#endif
