/**
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
 *
 * Interface to env_var and env_memento. This provides a simple function,
 * "add", which adds an env_var to the vector of variables it holds and
 * then returns a memento for that variable. So you can do something like
 * mgr.add("variable")->mandatory()->bind_to<std::string>(&variable).
 *
 * It also provides a validate, which iterates through all the mementos
 * it creates and calls their validate methods. If any mementos fail 
 * validation, it will accumulate all the missing names in a string and
 * throw it at the end.
 *
 * It also provides a "get" method in case you don't want to just bind
 * all your enviroment variables to typed C++ objects. Get simply
 * iterates through all the env_vars the object contains until it finds
 * one with the same name, and then returns that.
 */

#ifndef _HPP_ENV_MANAGER
#define _HPP_ENV_MANAGER

#include "env_var.hpp"
#include "env_memento.hpp"
#include <string>
#include <vector>

namespace fr {

  namespace data {

    class env_manager {
      // If you want to store enough env_vars that it'd be important,
      // I could replace this with a map. I don't usually expect there
      // to be that many env_vars, though.
      std::vector<env_var::pointer> env_vars;
      std::vector<env_memento *> mementos;

    public:

      env_manager()
      {
      }

      // I could let you copy it, but if I did that I'd
      // want to make copies of all the mementos so
      // the destructor wouldn't crash the program.
      env_manager(const env_manager &copy) = delete;

      ~env_manager()
      {
	for (env_memento *m : mementos) {
	  delete(m);
	}
      }

      env_memento *add(std::string name, std::string default_value = "", std::string description = "")
      {
	env_var::pointer new_var = std::make_shared<env_var>(name, default_value, description);
	env_memento *mem = new env_memento(new_var.get());
	env_vars.push_back(new_var);
	mementos.push_back(mem);
	return mem;
      }

      // Can return an empty pointer -- you'll need to check
      // for nullpointer specifically
      env_var::pointer get(std::string name)
      {
	env_var::pointer retval;
	for (env_var::pointer p : env_vars) {
	  if (p->get_name() == name) {
	    retval = p;
	    break;
	  }
	}
	return retval;
      }

      void validate()
      {
	bool will_throw = false;
	std::string err("The following variables are not defined and declared mandatory:\n");
	for(env_memento *mem : mementos) {
	  if (!mem->validate()) {
	    will_throw = true;
	    err.append(mem->get_var()->get_name());
	    std::string desc = mem->get_var()->get_description();
	    if (desc.size() > 0) {
	      err.append(": ");
	      err.append(desc);
	    }
	    err.append("\n");
	  }
	}
	if (will_throw) {
	  throw err;
	}
      }

    };

  }

}

#endif
