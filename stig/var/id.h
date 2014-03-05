/* <stig/var/id.h>

   A Stig id, which is Stig::TUUID.

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

#include <iostream>

#include <stig/rt/runtime_error.h>
#include <stig/uuid.h>
#include <stig/var/impl.h>

namespace Stig {

  namespace Var {

    class TId
        : public TVar::TImpl {
      public:

      /* TODO */
      virtual Var::TVar &Index(const TVar &);

      /* TODO */
      virtual TId &Add(const TVar &);

      /* TODO */
      virtual TId &And(const TVar &);

      /* TODO */
      virtual TId &Div(const TVar &);

      /* TODO */
      virtual TId &Exp(const TVar &);

      /* TODO */
      virtual TId &Intersection(const TVar &);

      /* TODO */
      virtual TId &Mod(const TVar &);

      /* TODO */
      virtual TId &Mult(const TVar &);

      /* TODO */
      virtual TId &Or(const TVar &);

      /* TODO */
      virtual TId &Sub(const TVar &);

      /* TODO */
      virtual TId &SymmetricDiff(const TVar &);

      /* TODO */
      virtual TId &Union(const TVar &);

      /* TODO */
      virtual TId &Xor(const TVar &);

      /* TODO */
      TUUID GetVal() const {
        assert(this);
        return Val;
      }

      /* TODO */
      virtual size_t GetHash() const;

      /* TODO */
      virtual Type::TType GetType() const;

      /* TODO */
      virtual void Touch();

      /* TODO */
      virtual void Write(std::ostream &) const;

      /* TODO */
      static TVar New(const TUUID &that);

      private:

      /* TODO */
      TId(const TUUID &that);

      /* TODO */
      virtual ~TId();

      /* TODO */
      virtual void Accept(const TVisitor &visitor) const;

      /* TODO */
      virtual TVar Copy() const;

      /* TODO */
      TUUID Val;

    };  // TId

    /* TODO */
    template <>
    struct TVar::TDt<TUUID> {

      /* TODO */
      TUUID static As(const TVar &that) {
        TId *ptr = dynamic_cast<TId *>(that.Impl.get());
        if (ptr) {
          return ptr->GetVal();
        }
        std::cerr << "Var is a " << that.GetType() << std::endl;
        throw Rt::TSystemError(HERE, "Trying to cast dynamic Var to id. Var is not an id.");
      }

    };  // TDt<TUUID>

  }  // Var

}  // Stig
