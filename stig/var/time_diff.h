/* <stig/var/time_diff.h>

   A Stig time_diff, which is Stig::Base::Chrono::TTimeDiff.

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

    class TTimeDiff
        : public TVar::TImpl {
      public:

      /* TODO */
      virtual Var::TVar &Index(const TVar &);

      /* TODO */
      virtual TTimeDiff &Add(const TVar &);

      /* TODO */
      virtual TTimeDiff &And(const TVar &);

      /* TODO */
      virtual TTimeDiff &Div(const TVar &);

      /* TODO */
      virtual TTimeDiff &Exp(const TVar &);

      /* TODO */
      virtual TTimeDiff &Intersection(const TVar &);

      /* TODO */
      virtual TTimeDiff &Mod(const TVar &);

      /* TODO */
      virtual TTimeDiff &Mult(const TVar &);

      /* TODO */
      virtual TTimeDiff &Or(const TVar &);

      /* TODO */
      virtual TTimeDiff &Sub(const TVar &);

      /* TODO */
      virtual TTimeDiff &SymmetricDiff(const TVar &);

      /* TODO */
      virtual TTimeDiff &Union(const TVar &);

      /* TODO */
      virtual TTimeDiff &Xor(const TVar &);

      /* TODO */
      Base::Chrono::TTimeDiff GetVal() const {
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
      static TVar New(const Base::Chrono::TTimeDiff &that);

      private:

      /* TODO */
      TTimeDiff(const Base::Chrono::TTimeDiff &that);

      /* TODO */
      virtual ~TTimeDiff();

      /* TODO */
      virtual void Accept(const TVisitor &visitor) const;

      /* TODO */
      virtual TVar Copy() const;

      /* TODO */
      Base::Chrono::TTimeDiff Val;

    };  // TTimeDiff

    /* TODO */
    template <>
    struct TVar::TDt<Base::Chrono::TTimeDiff> {

      /* TODO */
      Base::Chrono::TTimeDiff static As(const TVar &that) {
        TTimeDiff *ptr = dynamic_cast<TTimeDiff *>(that.Impl.get());
        if (ptr) {
          return ptr->GetVal();
        }
        std::cerr << "Var is a " << that.GetType() << std::endl;
        throw Rt::TSystemError(HERE, "Trying to cast dynamic Var to time_diff. Var is not a time_diff.");
      }

    };  // TDt<Base::Chrono::TTimeDiff>

  }  // Var

}  // Stig
