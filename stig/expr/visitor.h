/* <stig/expr/visitor.h>

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

#include <stig/expr/expr.h>

namespace Stig {

  namespace Expr {

    class TAdd;
    class TAddr;
    class TAddrMember;
    class TAddrOf;
    class TAnd;
    class TAndThen;
    class TAs;
    class TAssert;
    class TCeiling;
    class TCollatedBy;
    class TCollectedBy;
    class TDict;
    class TDiv;
    class TEffect;
    class TEqEq;
    class TExists;
    class TExp;
    class TFilter;
    class TFloor;
    class TFree;
    class TFunctionApp;
    class TGt;
    class TGtEq;
    class TIfElse;
    class TIn;
    class TIntersection;
    class TIsEmpty;
    class TIsKnown;
    class TIsKnownExpr;
    class TIsUnknown;
    class TKeys;
    class TKnown;
    class TLengthOf;
    class TLhs;
    class TList;
    class TLiteral;
    class TLog;
    class TLog2;
    class TLog10;
    class TLt;
    class TLtEq;
    class TMatch;
    class TMod;
    class TMult;
    class TNeq;
    class TNegative;
    class TNot;
    class TNow;
    class TObj;
    class TObjMember;
    class TOr;
    class TOrElse;
    class TPositive;
    class TRange;
    class TRead;
    class TReduce;
    class TRef;
    class TRhs;
    class TReverseOf;
    class TSessionId;
    class TSequenceOf;
    class TSet;
    class TSkip;
    class TSlice;
    class TSort;
    class TSplit;
    class TStart;
    class TSub;
    class TSymmetricDiff;
    class TTake;
    class TThat;
    class TTimeObj;
    class TToLower;
    class TToUpper;
    class TUnion;
    class TUnknown;
    class TUserId;
    class TWhere;
    class TWhile;
    class TXor;

    class TExpr::TVisitor {
      public:

      virtual ~TVisitor() = default;

      virtual void operator()(const TAdd           *) const = 0;
      virtual void operator()(const TAddr          *) const = 0;
      virtual void operator()(const TAddrMember    *) const = 0;
      virtual void operator()(const TAddrOf        *) const = 0;
      virtual void operator()(const TAnd           *) const = 0;
      virtual void operator()(const TAndThen       *) const = 0;
      virtual void operator()(const TAs            *) const = 0;
      virtual void operator()(const TAssert        *) const = 0;
      virtual void operator()(const TCeiling       *) const = 0;
      virtual void operator()(const TCollatedBy    *) const = 0;
      virtual void operator()(const TCollectedBy   *) const = 0;
      virtual void operator()(const TDict          *) const = 0;
      virtual void operator()(const TDiv           *) const = 0;
      virtual void operator()(const TEffect        *) const = 0;
      virtual void operator()(const TEqEq          *) const = 0;
      virtual void operator()(const TExists        *) const = 0;
      virtual void operator()(const TExp           *) const = 0;
      virtual void operator()(const TFilter        *) const = 0;
      virtual void operator()(const TFloor         *) const = 0;
      virtual void operator()(const TFree          *) const = 0;
      virtual void operator()(const TFunctionApp   *) const = 0;
      virtual void operator()(const TGt            *) const = 0;
      virtual void operator()(const TGtEq          *) const = 0;
      virtual void operator()(const TIfElse        *) const = 0;
      virtual void operator()(const TIn            *) const = 0;
      virtual void operator()(const TIntersection  *) const = 0;
      virtual void operator()(const TIsEmpty       *) const = 0;
      virtual void operator()(const TIsKnown       *) const = 0;
      virtual void operator()(const TIsKnownExpr   *) const = 0;
      virtual void operator()(const TIsUnknown     *) const = 0;
      virtual void operator()(const TKeys          *) const = 0;
      virtual void operator()(const TKnown         *) const = 0;
      virtual void operator()(const TLengthOf      *) const = 0;
      virtual void operator()(const TLhs           *) const = 0;
      virtual void operator()(const TList          *) const = 0;
      virtual void operator()(const TLiteral       *) const = 0;
      virtual void operator()(const TLog           *) const = 0;
      virtual void operator()(const TLog2          *) const = 0;
      virtual void operator()(const TLog10         *) const = 0;
      virtual void operator()(const TLt            *) const = 0;
      virtual void operator()(const TLtEq          *) const = 0;
      virtual void operator()(const TMatch         *) const = 0;
      virtual void operator()(const TMod           *) const = 0;
      virtual void operator()(const TMult          *) const = 0;
      virtual void operator()(const TNeq           *) const = 0;
      virtual void operator()(const TNegative      *) const = 0;
      virtual void operator()(const TNot           *) const = 0;
      virtual void operator()(const TNow           *) const = 0;
      virtual void operator()(const TObj           *) const = 0;
      virtual void operator()(const TObjMember     *) const = 0;
      virtual void operator()(const TOr            *) const = 0;
      virtual void operator()(const TOrElse        *) const = 0;
      virtual void operator()(const TPositive      *) const = 0;
      virtual void operator()(const TRange         *) const = 0;
      virtual void operator()(const TRead          *) const = 0;
      virtual void operator()(const TReduce        *) const = 0;
      virtual void operator()(const TRef           *) const = 0;
      virtual void operator()(const TReverseOf     *) const = 0;
      virtual void operator()(const TRhs           *) const = 0;
      virtual void operator()(const TSessionId     *) const = 0;
      virtual void operator()(const TSequenceOf    *) const = 0;
      virtual void operator()(const TSet           *) const = 0;
      virtual void operator()(const TSkip          *) const = 0;
      virtual void operator()(const TSlice         *) const = 0;
      virtual void operator()(const TSort          *) const = 0;
      virtual void operator()(const TSplit         *) const = 0;
      virtual void operator()(const TStart         *) const = 0;
      virtual void operator()(const TSub           *) const = 0;
      virtual void operator()(const TSymmetricDiff *) const = 0;
      virtual void operator()(const TTake          *) const = 0;
      virtual void operator()(const TThat          *) const = 0;
      virtual void operator()(const TTimeObj       *) const = 0;
      virtual void operator()(const TToLower       *) const = 0;
      virtual void operator()(const TToUpper       *) const = 0;
      virtual void operator()(const TUnion         *) const = 0;
      virtual void operator()(const TUnknown       *) const = 0;
      virtual void operator()(const TUserId        *) const = 0;
      virtual void operator()(const TWhere         *) const = 0;
      virtual void operator()(const TWhile         *) const = 0;
      virtual void operator()(const TXor           *) const = 0;

      protected:

      TVisitor() {} // TODO: = default;

    };  // TExpr::TVisitor

  }  // Expr

}  // Stig
