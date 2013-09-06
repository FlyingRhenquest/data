/**
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
 * An environment variable class. Retrieves an environment variable from
 * the shell environment. Allows a default to be set if the environment
 * variable isn't set. Also allows the variable to be made mandatory.
 * When the env_var's "validate()" is called, the class will attempt
 * to set the enviroment variable from the environment or its default.
 * If the variable is mandatory and doesn't end up set by the end
 * of validate, validation will fail. This will return a false
 * so you can collect all the required variables for future reporting.
 */

#ifndef _HPP_ENV_VAR
#define _HPP_ENV_VAR

#include <string>
#include <stdlib.h>
#include <memory>

namespace fr {

  namespace data {

    class env_var {
      std::string name;
      std::string value;
      std::string default_value;
      std::string description;
      bool mandatory;

    public:
      typedef std::shared_ptr<env_var> pointer;
      
      env_var(std::string name, std::string default_value = "", std::string description = "") : name(name), default_value(default_value), description(description), mandatory(false)
      {
      }

      env_var(const env_var &copy) : name(copy.name), value(copy.value), default_value(copy.default_value), description(description), mandatory(copy.mandatory)
      {
      }

      ~env_var()
      {
      }

      std::string get_name() const
      {
	return name;
      }

      std::string get_value() const
      {
	return value;
      }

      std::string get_description() const
      {
	return description;
      }

      void make_mandatory()
      {
	mandatory = true;
      }

      bool validate()
      {
	bool retval = true;
	// First try to get it from the environment
	char *from_env = getenv(name.c_str());
	if (nullptr != from_env) {
	  value = std::string(from_env);
	} else {
	  if (default_value.size() > 0) {
	    value = default_value;
	  }
	}
	if (mandatory && value.size() == 0) {
	  retval = false;
	}
	return retval;
      }

    };

  }

}

#endif
