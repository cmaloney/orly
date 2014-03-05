/* <stig/type/comp_visitor.h>

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

#include <stig/error.h>
#include <stig/pos_range.h>
#include <stig/type/has_optional.h>
#include <stig/type/equal_visitor.h>

namespace Stig {

  namespace Type {

    /* Type visitor for == and !=. This is also used for type checking assign mutation. */
    class TEqCompVisitor
        : public TEqualVisitor {
      NO_COPY_SEMANTICS(TEqCompVisitor);
      public:

      TEqCompVisitor(TType &type, const TPosRange &pos_range)
          : TEqualVisitor(type, pos_range) {}

      virtual void operator()(const TAddr *lhs, const TAddr *rhs) const {
        bool is_optional = false;
        if (lhs->GetElems().size() != rhs->GetElems().size()) {
          throw TExprError(HERE, PosRange,
              "Equality test or mutation assignment for addresses of different lengths are not allowed");
        }
        auto lhs_iter = lhs->GetElems().begin();
        auto rhs_iter = rhs->GetElems().begin();
        for (; lhs_iter != lhs->GetElems().end(); ++lhs_iter, ++rhs_iter) {
          if (lhs_iter->first != rhs_iter->first) {
            throw TExprError(HERE, PosRange,
                "Equality test or mutation assignment for addresses of different directions are not allowed");
          }
          TType::Accept(lhs_iter->second, rhs_iter->second, *this);
          if (Type.Is<TOpt>() || HasOptional(Type)) {
            is_optional = true;
          }
        }
        // TODO. Remove this throw once we support comparison between addresses with optionals
        if (is_optional) {
          throw TExprError(HERE, PosRange, "Equality test or mutation assignment for addresses with optionals is not supported");
        }
        Type = is_optional ? TOpt::Get(TBool::Get()) : TBool::Get();
      }
      virtual void operator()(const TBool *, const TBool *) const {
        Type = TBool::Get();
      }
      virtual void operator()(const TDict *lhs, const TDict *rhs) const {
        bool is_optional = false;
        TType::Accept(lhs->GetKey(), rhs->GetKey(), *this);
        if (Type.Is<TOpt>() || HasOptional(Type)) {
          is_optional = true;
        }
        TType::Accept(lhs->GetVal(), rhs->GetVal(), *this);
        if (Type.Is<TOpt>() || HasOptional(Type)) {
          is_optional = true;
        }
        Type = is_optional ? TOpt::Get(TBool::Get()) : TBool::Get();
      }
      virtual void operator()(const TId *, const TId *) const {
        Type = TBool::Get();
      }
      virtual void operator()(const TInt *, const TInt *) const {
        Type = TBool::Get();
      }
      virtual void operator()(const TList *lhs, const TList *rhs) const {
        TType::Accept(lhs->GetElem(), rhs->GetElem(), *this);
      }
      virtual void operator()(const TObj *lhs, const TObj *rhs) const {
        bool is_optional = false;
        if (!TObj::AreComparable(lhs, rhs, is_optional)) {
          throw TExprError(HERE, PosRange,
              "Equality test or mutation assignment for objects requires one to be a subset of the other");
        }
        Type = is_optional ? TOpt::Get(TBool::Get()) : TBool::Get();
      }
      virtual void operator()(const TReal *, const TReal *) const {
        Type = TBool::Get();
      }
      virtual void operator()(const TSet *lhs, const TSet *rhs) const {
        TType::Accept(lhs->GetElem(), rhs->GetElem(), *this);
      }
      virtual void operator()(const TStr *, const TStr *) const {
        Type = TBool::Get();
      }
      virtual void operator()(const TTimeDiff *, const TTimeDiff *) const {
        Type = TBool::Get();
      }
      virtual void operator()(const TTimePnt *, const TTimePnt *) const {
        Type = TBool::Get();
      }

    };  // TEqCompVisitor

    /* Type visitor for <, <=, >, >= */
    class TIneqCompVisitor
        : public TEqualVisitor {
      NO_COPY_SEMANTICS(TIneqCompVisitor);
      public:

      TIneqCompVisitor(TType &type, const TPosRange &pos_range)
          : TEqualVisitor(type, pos_range) {}

      virtual void operator()(const TAddr *lhs, const TAddr *rhs) const {
        bool is_optional = false;
        auto lhs_iter = lhs->GetElems().begin();
        auto rhs_iter = rhs->GetElems().begin();
        for (; lhs_iter != lhs->GetElems().end() && rhs_iter != rhs->GetElems().end(); ++lhs_iter, ++rhs_iter) {
          if (lhs_iter->first != rhs_iter->first) {
            throw TExprError(HERE, PosRange, "Comparisons between addresses of different directions are not allowed");
          }
          TType::Accept(lhs_iter->second, rhs_iter->second, *this);
          if (Type.Is<TOpt>() || HasOptional(Type)) {
            is_optional = true;
          }
        }
        // TODO. Remove this throw once we support comparison between addresses with optionals
        if (is_optional) {
          throw TExprError(HERE, PosRange, "Comparison between addresses with optionals is not supported");
        }
        Type = is_optional ? TOpt::Get(TBool::Get()) : TBool::Get();
      }
      virtual void operator()(const TBool *, const TBool *) const {
        throw TExprError(HERE, PosRange, "Booleans can only be tested for equality or inequality");
      }
      virtual void operator()(const TDict *, const TDict *) const {
        throw TExprError(HERE, PosRange, "Dictionaries can only be tested for equality or inequality");
      }
      virtual void operator()(const TId *, const TId *) const {
        throw TExprError(HERE, PosRange, "Ids can only be tested for equality or inequality");
      }
      virtual void operator()(const TInt *, const TInt *) const {
        Type = TBool::Get();
      }
      virtual void operator()(const TList *lhs, const TList *rhs) const {
        TType::Accept(lhs->GetElem(), rhs->GetElem(), *this);
      }
      virtual void operator()(const TObj *, const TObj *) const {
        throw TExprError(HERE, PosRange, "Objects can only be tested for equality or inequality");
      }
      virtual void operator()(const TReal *, const TReal *) const {
        Type = TBool::Get();
      }
      virtual void operator()(const TSet *lhs, const TSet *rhs) const {
        TType::Accept(lhs->GetElem(), rhs->GetElem(), TEqCompVisitor(Type, PosRange));
      }
      virtual void operator()(const TStr *, const TStr *) const {
        Type = TBool::Get();
      }
      virtual void operator()(const TTimeDiff *, const TTimeDiff *) const {
        Type = TBool::Get();
      }
      virtual void operator()(const TTimePnt *, const TTimePnt *) const {
        Type = TBool::Get();
      }

    };  // TIneqCompVisitor

  }  // Expr

}  // Stig