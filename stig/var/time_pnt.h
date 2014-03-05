/* <stig/var/time_pnt.h>

   A Stig time_pnt, which is Stig::Base::Chrono::TTimePnt

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

#include <base/chrono.h>
#include <stig/rt/runtime_error.h>
#include <stig/var/impl.h>

namespace Stig {

  namespace Var {

    class TTimePnt
        : public TVar::TImpl {
      public:

      /* TODO */
      virtual Var::TVar &Index(const TVar &);

      /* TODO */
      virtual TTimePnt &Add(const TVar &);

      /* TODO */
      virtual TTimePnt &And(const TVar &);

      /* TODO */
      virtual TTimePnt &Div(const TVar &);

      /* TODO */
      virtual TTimePnt &Exp(const TVar &);

      /* TODO */
      virtual TTimePnt &Intersection(const TVar &);

      /* TODO */
      virtual TTimePnt &Mod(const TVar &);

      /* TODO */
      virtual TTimePnt &Mult(const TVar &);

      /* TODO */
      virtual TTimePnt &Or(const TVar &);

      /* TODO */
      virtual TTimePnt &Sub(const TVar &);

      /* TODO */
      virtual TTimePnt &SymmetricDiff(const TVar &);

      /* TODO */
      virtual TTimePnt &Union(const TVar &);

      /* TODO */
      virtual TTimePnt &Xor(const TVar &);

      /* TODO */
      Base::Chrono::TTimePnt GetVal() const {
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
      virtual void Write(std::ostream &strm) const;

      /* TODO */
      static TVar New(const Base::Chrono::TTimePnt &that);

      private:

      /* TODO */
      TTimePnt(const Base::Chrono::TTimePnt &that);

      /* TODO */
      virtual ~TTimePnt();

      /* TODO */
      virtual void Accept(const TVisitor &visitor) const;

      /* TODO */
      virtual TVar Copy() const;

      /* TODO */
      Base::Chrono::TTimePnt Val;

    };  // TTimePnt

    /* TODO */
    template <>
    struct TVar::TDt<Base::Chrono::TTimePnt> {

      /* TODO */
      Base::Chrono::TTimePnt static As(const TVar &that) {
        TTimePnt *ptr = dynamic_cast<TTimePnt *>(that.Impl.get());
        if (ptr) {
          return ptr->GetVal();
        }
        std::cerr << "Var is a " << that.GetType() << std::endl;
        throw Rt::TSystemError(HERE, "Trying to cast dynamic Var to time_pnt. Var is not a time_pnt.");
      }

    };  // TDt<Base::Chrono::TTimePnt>

  }  // Var

}  // Stig
