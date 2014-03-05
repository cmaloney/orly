/* <stig/manual.cc>

   This is an example of a manually-built package.

   Compile it like this:

      g++ -std=c++11 -fPIC -shared -g -Wno-unused-variable -Wno-type-limits -Werror -Wno-parentheses -Wall -Wextra -Wno-unused-parameter  \
        -I<your /src dir>  \
        -o<your .so dir>/manual.1.so  \
        manual.cc

   Copyright 2010-2014 Stig LLC

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <stig/rt.h>
#include <stig/package/api.h>
#include <stig/package/rt.h>
#include <stig/spa/flux_capacitor/rt.h>
#include <stig/var/mutation.h>

using namespace std;
using namespace Stig;
using namespace Stig::Rt;

/* Our actual function.  Isn't it cute? */
static int64_t Sum(Package::TContext &ctx, const int64_t &a, const int64_t &b) {
  return a + b;
}

namespace Wrapper {

  /* The wrapper for Sum, which looks up and converts the arguments to native form,
     calls the actual Sum function, then converts the result back to a core.
     (One entry like this per function in this package.) */
  Atom::TCore Sum(Package::TContext &ctx, const Package::TArgMap &args) {
    assert(&ctx);
    assert(&args);
    assert(args.size() == 2);
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    return Atom::TCore(
        ctx.GetArena(),
        Native::State::New(
            ::Sum(
                ctx,
                Sabot::AsNative<int64_t>(*Sabot::State::TAny::TWrapper(args.at("a").GetState(state_alloc))),
                Sabot::AsNative<int64_t>(*Sabot::State::TAny::TWrapper(args.at("b").GetState(state_alloc)))
            ),
            state_alloc
        )
    );
  }

}

namespace Info {

  /* The name of this package. */
  static const char *Name = "manual";

  /* The version number of this package. */
  static uint64_t Version = 1;

  /* The meta-data describing Sum and a pointer to the Sum's wrapper.
    (One entry like this per function in this package.) */
  static const Package::TFuncInfo Sum = {
    /* Parameter names and types. */ {
      { "a", Type::TInt::Get() },
      { "b", Type::TInt::Get() }
    },
    /* Return type. */ Type::TInt::Get(),
    /* Wrapper.     */ Wrapper::Sum
  };

  /* Information about the contents of this package. */
  static const Package::TInfo Package = {
    Name,
    Version,
    /* List the functions in this package. */ {
      { "sum", &Sum }
    },
    /* List the tests in this package. */ {}
  };

  /* Externally visible object describing this package. */
  static Package::TLinkInfo Link = {
    Name,
    Version,
    &Package,
    /* Included packages. */ {}
  };

}

/* Mandatory function. */
extern "C" Package::TLinkInfo *GetLinkInfo() {
  return &Info::Link;
}

/* Mandatory function. */
extern "C" int32_t GetApiVersion() {
  return Info::Version;
}
