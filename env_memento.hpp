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
 * A memento that can do things to env_vars. The only things you
 * can do to env_vars is make them mandatory and run validate
 * on them, and this object can do that. The other mildly
 * nifty thing it can do is bind an env_var to a C++ typed
 * variable. All its operations return "this", so you can
 * chain them along:
 *
 * mem.make_mandatory()->bind_to<int>(&foo);
 *
 * This class uses the string converters in my time
 * library. Since there's a time specialization for that
 * class, you can read a time in the format it expects
 * into a timeval at no extra charge!
 */

#ifndef _HPP_ENV_MEMENTO
#define _HPP_ENV_MEMENTO

#include "env_var.hpp"
#include "string_converts.hpp"
#include <functional>

namespace fr {

  namespace data {

    class env_memento {
      // Keeps a raw pointer to your env var -- don't leave a memento laying around
      // after nuking your env var objects!
      env_var *var_ptr;
      std::function<void()> binder;

    public:
      env_memento(env_var *var_ptr) : var_ptr(var_ptr)
      {
      }

      env_memento(const env_memento &copy) : var_ptr(copy.var_ptr), binder(copy.binder)
      {
      }

      ~env_memento()
      {
      }

      env_var *get_var() const
      {
	return var_ptr;
      }

      env_memento *mandatory()
      {
	var_ptr->make_mandatory();
	return this;
      }

      /**
       * OK so technically validate doesn't return this, but the
       * end user isn't supposed call validate through the memento
       * anyway (See: env_manager)
       */

      bool validate()
      {
	bool retval = var_ptr->validate();
	// Note that you CAN shoot yourself in the foot here if you
	// make non-string values non-mandatory. Generally if you
	// want to bind a variable to an env_value you should
	// make it mandatory. A non-mandatory empty value assigned
	// to a string can safely be zero length. Other types, not
	// so much.
	if (retval && binder) {
	  binder();
	}
	return retval;
      }

      /**
       * Binds a typed C++ variable to an enviroment variable.
       * These are assigned when the memento's validate is
       * run.
       */

      template <typename user_type>
      env_memento *bind_to(user_type *user_variable)
      {
	binder = [=](){
	  fr::time::string_to<user_type> user_type_converter;
	  *user_variable = user_type_converter(var_ptr->get_value());
	};
	return this;
      }
      
    };

  }
}

#endif
