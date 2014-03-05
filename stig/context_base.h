/* <stig/context_base.h>

   TODO

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

#pragma once

#include <stig/atom/kit2.h>
#include <stig/indy/key.h>
#include <stig/spa/flux_capacitor/kv.h>

namespace Stig {

  /* TODO */
  class TContextBase {
    NO_COPY_SEMANTICS(TContextBase);
    public:

    /* TODO */
    virtual ~TContextBase() {}

    /* TODO */
    virtual Indy::TKey operator[](const Indy::TIndexKey &key) = 0;

    /* TODO */
    virtual bool Exists(const Indy::TIndexKey &key) = 0;

    /* TODO */
    inline Atom::TCore::TExtensibleArena *GetArena() const {
      assert(this);
      return Arena;
    }

    protected:

    /* TODO */
    TContextBase(Atom::TCore::TExtensibleArena *arena)
        : Arena(arena) {}

    /* TODO */
    Atom::TCore::TExtensibleArena *Arena;

    private:

  };  // TContextBase

}  // Stig
