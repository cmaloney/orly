/* <stig/var/free.h>

   The 'free' value, which is Stig's way of searching.

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

    class TFree
        : public TVar::TImpl {
      public:

      /* TODO */
      virtual Var::TVar &Index(const TVar &);

      /* TODO */
      virtual TFree &Add(const TVar &);

      /* TODO */
      virtual TFree &And(const TVar &);

      /* TODO */
      virtual TFree &Div(const TVar &);

      /* TODO */
      virtual TFree &Exp(const TVar &);

      /* TODO */
      virtual TFree &Intersection(const TVar &);

      /* TODO */
      virtual TFree &Mod(const TVar &);

      /* TODO */
      virtual TFree &Mult(const TVar &);

      /* TODO */
      virtual TFree &Or(const TVar &);

      /* TODO */
      virtual TFree &Sub(const TVar &);

      /* TODO */
      virtual TFree &SymmetricDiff(const TVar &);

      /* TODO */
      virtual TFree &Union(const TVar &);

      /* TODO */
      virtual TFree &Xor(const TVar &);

      /* TODO */
      virtual size_t GetHash() const;

      /* TODO */
      virtual Type::TType GetType() const;

      /* TODO */
      virtual void Touch();

      /* TODO */
      virtual void Write(std::ostream &) const;

      /* TODO */
      static TVar New(const Type::TType &type);

      private:

      /* TODO */
      TFree(const Type::TType &type) : Type(type) {}

      /* TODO */
      virtual ~TFree();

      /* TODO */
      virtual void Accept(const TVisitor &visitor) const;

      /* TODO */
      virtual TVar Copy() const;

      /* TODO */
      Type::TType Type;

      /* TODO */
      friend class TVar;

    };  // TFree

  }  // Var

}  // Stig
