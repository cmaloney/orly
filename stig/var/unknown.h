/* <stig/var/unknown.h>

   The 'unknown' value, which is Stig's equivalent of null, none, nil, etc.

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

#include <stig/var/impl.h>

namespace Stig {

  namespace Var {

    class TUnknown
        : public TVar::TImpl {
      public:

      /* TODO */
      virtual Var::TVar &Index(const TVar &);

      /* TODO */
      virtual TUnknown &Add(const TVar &);

      /* TODO */
      virtual TUnknown &And(const TVar &);

      /* TODO */
      virtual TUnknown &Div(const TVar &);

      /* TODO */
      virtual TUnknown &Exp(const TVar &);

      /* TODO */
      virtual TUnknown &Intersection(const TVar &);

      /* TODO */
      virtual TUnknown &Mod(const TVar &);

      /* TODO */
      virtual TUnknown &Mult(const TVar &);

      /* TODO */
      virtual TUnknown &Or(const TVar &);

      /* TODO */
      virtual TUnknown &Sub(const TVar &);

      /* TODO */
      virtual TUnknown &SymmetricDiff(const TVar &);

      /* TODO */
      virtual TUnknown &Union(const TVar &);

      /* TODO */
      virtual TUnknown &Xor(const TVar &);

      /* TODO */
      virtual size_t GetHash() const;

      /* TODO */
      virtual Type::TType GetType() const;

      /* TODO */
      virtual void Touch();

      /* TODO */
      virtual void Write(std::ostream &) const;

      /* TODO */
      static TVar New();

      private:

      /* TODO */
      TUnknown() {}

      /* TODO */
      virtual ~TUnknown();

      /* TODO */
      virtual void Accept(const TVisitor &visitor) const;

      /* TODO */
      virtual TVar Copy() const;

      /* TODO */
      friend class TVar;

    };  // TUnknown

  }  // Var

}  // Stig
