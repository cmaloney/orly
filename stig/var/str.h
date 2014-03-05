/* <stig/var/str.h>

   A Stig str, which is a std::string.

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
#include <stig/var/impl.h>

namespace Stig {

  namespace Var {

    class TStr
        : public TVar::TImpl {
      public:

      /* TODO */
      virtual Var::TVar &Index(const TVar &);

      /* TODO */
      virtual TStr &Add(const TVar &);

      /* TODO */
      virtual TStr &And(const TVar &);

      /* TODO */
      virtual TStr &Div(const TVar &);

      /* TODO */
      virtual TStr &Exp(const TVar &);

      /* TODO */
      virtual TStr &Intersection(const TVar &);

      /* TODO */
      virtual TStr &Mod(const TVar &);

      /* TODO */
      virtual TStr &Mult(const TVar &);

      /* TODO */
      virtual TStr &Or(const TVar &);

      /* TODO */
      virtual TStr &Sub(const TVar &);

      /* TODO */
      virtual TStr &SymmetricDiff(const TVar &);

      /* TODO */
      virtual TStr &Union(const TVar &);

      /* TODO */
      virtual TStr &Xor(const TVar &);

      /* TODO */
      virtual size_t GetHash() const;

      /* TODO */
      const std::string &GetVal() const {
        assert(this);
        return Val;
      }

      /* TODO */
      virtual Type::TType GetType() const;

      /* TODO */
      virtual void Touch();

      /* TODO */
      static TVar New(const std::string &that);

      /* TODO */
      virtual void Write(std::ostream &strm) const;

      private:

      /* TODO */
      TStr(const std::string &that);

      /* TODO */
      virtual ~TStr();

      /* TODO */
      virtual void Accept(const TVisitor &visitor) const;

      /* TODO */
      virtual TVar Copy() const;

      /* TODO */
      std::string Val;

    };  // TStr

    /* TODO */
    template <>
    struct TVar::TDt<std::string> {

      /* TODO */
      std::string static As(const TVar &that) {
        TStr *ptr = dynamic_cast<TStr *>(that.Impl.get());
        if (ptr) {
          return ptr->GetVal();
        }
        std::cerr << "Var is a " << that.GetType() << std::endl;
        throw Rt::TSystemError(HERE, "Trying to cast dynamic Var to string. Var is not a string.");
      }

    };  // TDt<std::string>

  }  // Var

}  // Stig
