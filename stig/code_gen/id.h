/* <stig/code_gen/id.h>

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

#include <cstdint>
#include <stdexcept>

#include <base/no_copy_semantics.h>
#include <stig/code_gen/cpp_printer.h>

namespace Stig {

  namespace CodeGen {

    enum class TIdKind {Arg, Func, Test, Var};

    //NOTE: because we're constexpr, we have to be one line which is kinda ugly. Oh well.
    constexpr char GetKindPrefix(TIdKind kind) {
      return kind == TIdKind::Arg ? 'a' :
          kind == TIdKind::Func ? 'f' :
          kind == TIdKind::Test ? 't' :
          kind == TIdKind::Var ? 'v' :
          throw std::logic_error("kind mustbe Arg, Func, Test, or Var.");
    }

    /* A unique identifier within a scope. */
    template<TIdKind Kind>
    class TId {
      public:

      /* Generates unique ids. */
      class TGen {
        NO_COPY_SEMANTICS(TGen);
        public:

        TGen() : NextId(0) {}

        virtual ~TGen() noexcept {}

        TId New() {
          return(TId(NextId++));
        }

        private:
        uint32_t NextId;
      };

      uint32_t GetId() const {
        assert(this);
        return Id;
      }

      void Write(TCppPrinter &out) const {
        assert(this);
        out << GetKindPrefix(Kind) << Id;
      }

      private:
      TId(uint32_t id) : Id(id) {}
      uint32_t Id;
    }; // TId

    template <TIdKind Kind>
    TCppPrinter &operator<<(TCppPrinter &out, const TId<Kind> &id) {
      assert(&out);
      assert(&id);

      id.Write(out);

      return out;
    }

  } // CodeGen

} // Stig