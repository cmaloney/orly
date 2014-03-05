/* <stig/type/mutable.h>

   This type indicates that we __may__ know the address of the variable.

   Mutable is a parametric type, depending on both the type of the address which may be present, as well as the type
   being stored in the address.

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

#include <stig/type/managed_type.h>
#include <stig/type/part.h>

namespace Stig {

  namespace Type {

    class TMutable : public TInternedType<TMutable, TType, std::vector<TPart::TPtr>, TType, TType> {
      NO_COPY_SEMANTICS(TMutable);
      public:

      typedef std::vector<TPart::TPtr> TParts;

      /* Constructs a new mutable type. Neither the address or the type at the address may be mutable themselves. For
         addresses, there is a left-matching heuristic. That is to say, a mutable that is identical in val type, but
         and matches the prefix of the mutable type required by, say, a function,*/
      static TType Get(const TType &addr, const TType &val);
      static TType Get(const TType &addr, const TType &val, const TType &src_at_addr);
      static TType Get(const TType &addr, const TParts &parts, const TType &val, const TType &src_at_addr);

      TType GetAddr() const;
      const TParts &GetParts() const;
      TType GetVal() const;
      TType GetSrcAtAddr() const;
      void Write(std::ostream &strm) const final;

      private:
      TMutable(const TType &addr, const TParts &parts, const TType &val, const TType &src_at_addr);
      virtual ~TMutable();

      friend class TInternedType;

    };  // TMutable

  }  // Type

}  // Stig
