/* <stig/sabot/type_dumper.h>

   Dump a human-readable description of a sabot type.

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

#include <cassert>
#include <ostream>

#include <stig/sabot/type.h>

namespace Stig {

  namespace Sabot {

    /* Dump a human-readable description of a sabot type. */
    class TTypeDumper final
        : public TTypeVisitor {
      public:

      /* Caches a reference to the stream. */
      TTypeDumper(std::ostream &strm)
          : Strm(strm) {
        assert(&strm);
      }

      /* Overrides. */
      virtual void operator()(const Type::TInt8 &type) const override;
      virtual void operator()(const Type::TInt16 &type) const override;
      virtual void operator()(const Type::TInt32 &type) const override;
      virtual void operator()(const Type::TInt64 &type) const override;
      virtual void operator()(const Type::TUInt8 &type) const override;
      virtual void operator()(const Type::TUInt16 &type) const override;
      virtual void operator()(const Type::TUInt32 &type) const override;
      virtual void operator()(const Type::TUInt64 &type) const override;
      virtual void operator()(const Type::TBool &type) const override;
      virtual void operator()(const Type::TChar &type) const override;
      virtual void operator()(const Type::TFloat &type) const override;
      virtual void operator()(const Type::TDouble &type) const override;
      virtual void operator()(const Type::TDuration &type) const override;
      virtual void operator()(const Type::TTimePoint &type) const override;
      virtual void operator()(const Type::TUuid &type) const override;
      virtual void operator()(const Type::TBlob &type) const override;
      virtual void operator()(const Type::TStr &type) const override;
      virtual void operator()(const Type::TTombstone &type) const override;
      virtual void operator()(const Type::TVoid &type) const override;
      virtual void operator()(const Type::TDesc &type) const override;
      virtual void operator()(const Type::TFree &type) const override;
      virtual void operator()(const Type::TOpt &type) const override;
      virtual void operator()(const Type::TSet &type) const override;
      virtual void operator()(const Type::TVector &type) const override;
      virtual void operator()(const Type::TMap &type) const override;
      virtual void operator()(const Type::TRecord &type) const override;
      virtual void operator()(const Type::TTuple &type) const override;

      private:

      /* Handler for binary types. */
      void OnBinaryType(const char *name, const Type::TBinary &binary) const;

      /* Handler for unary types. */
      void OnUnaryType(const char *name, const Type::TUnary &unary) const;

      /* The stream to which we dump. */
      std::ostream &Strm;

    };  // TTypeDumper

  }  // Sabot

}  // Stig
