/* <stig/expr/skip.cc>

   Implements <stig/expr/skip.h>.

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

#include <stig/expr/skip.h>

#include <stig/error.h>
#include <stig/expr/visitor.h>
#include <stig/pos_range.h>
#include <stig/type.h>

using namespace Stig;
using namespace Stig::Expr;

class TSkipTypeVisitor
    : public Type::TType::TDoubleVisitor {
  public:

  /* TODO */
  TSkipTypeVisitor(Type::TType &type, const TPosRange &pos_range)
      : PosRange(pos_range), Type(type) {}

  /* TODO */
  virtual void operator()(const Type::TAddr *, const Type::TAddr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAddr *, const Type::TAny *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAddr *, const Type::TBool *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAddr *, const Type::TDict *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAddr *, const Type::TErr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAddr *, const Type::TFunc *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAddr *, const Type::TId *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAddr *, const Type::TInt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAddr *, const Type::TList *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAddr *, const Type::TMutable *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAddr *, const Type::TOpt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAddr *, const Type::TObj *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAddr *, const Type::TReal *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAddr *, const Type::TSet *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAddr *, const Type::TSeq *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAddr *, const Type::TStr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAddr *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAddr *, const Type::TTimePnt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAny  *, const Type::TAddr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAny  *, const Type::TAny *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAny  *, const Type::TBool *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAny  *, const Type::TDict *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAny  *, const Type::TErr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAny  *, const Type::TFunc *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAny  *, const Type::TId *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAny  *, const Type::TInt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAny  *, const Type::TList *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAny  *, const Type::TMutable *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAny  *, const Type::TOpt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAny  *, const Type::TObj *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAny  *, const Type::TReal *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAny  *, const Type::TSet *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAny  *, const Type::TSeq *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAny  *, const Type::TStr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAny  *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TAny  *, const Type::TTimePnt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TBool *, const Type::TAddr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TBool *, const Type::TAny *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TBool *, const Type::TBool *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TBool *, const Type::TDict *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TBool *, const Type::TErr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TBool *, const Type::TFunc *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TBool *, const Type::TId *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TBool *, const Type::TInt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TBool *, const Type::TList *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TBool *, const Type::TMutable *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TBool *, const Type::TOpt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TBool *, const Type::TObj *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TBool *, const Type::TReal *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TBool *, const Type::TSet *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TBool *, const Type::TSeq *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TBool *, const Type::TStr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TBool *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TBool *, const Type::TTimePnt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TDict *, const Type::TAddr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TDict *, const Type::TAny *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TDict *, const Type::TBool *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TDict *, const Type::TDict *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TDict *, const Type::TErr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TDict *, const Type::TFunc *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TDict *, const Type::TId *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TDict *, const Type::TInt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TDict *, const Type::TList *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TDict *, const Type::TMutable *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TDict *, const Type::TOpt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TDict *, const Type::TObj *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TDict *, const Type::TReal *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TDict *, const Type::TSet *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TDict *, const Type::TSeq *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TDict *, const Type::TStr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TDict *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TDict *, const Type::TTimePnt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TErr  *, const Type::TAddr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TErr  *, const Type::TAny *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TErr  *, const Type::TBool *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TErr  *, const Type::TDict *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TErr  *, const Type::TErr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TErr  *, const Type::TFunc *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TErr  *, const Type::TId *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TErr  *, const Type::TInt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TErr  *, const Type::TList *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TErr  *, const Type::TMutable *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TErr  *, const Type::TOpt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TErr  *, const Type::TObj *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TErr  *, const Type::TReal *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TErr  *, const Type::TSet *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TErr  *, const Type::TSeq *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TErr  *, const Type::TStr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TErr  *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TErr  *, const Type::TTimePnt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TFunc *, const Type::TAddr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TFunc *, const Type::TAny *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TFunc *, const Type::TBool *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TFunc *, const Type::TDict *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TFunc *, const Type::TErr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TFunc *, const Type::TFunc *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TFunc *, const Type::TId *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TFunc *, const Type::TInt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TFunc *, const Type::TList *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TFunc *, const Type::TMutable *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TFunc *, const Type::TOpt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TFunc *, const Type::TObj *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TFunc *, const Type::TReal *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TFunc *, const Type::TSet *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TFunc *, const Type::TSeq *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TFunc *, const Type::TStr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TFunc *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TFunc *, const Type::TTimePnt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TId   *, const Type::TAddr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TId   *, const Type::TAny *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TId   *, const Type::TBool *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TId   *, const Type::TDict *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TId   *, const Type::TErr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TId   *, const Type::TFunc *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TId   *, const Type::TId *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TId   *, const Type::TInt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TId   *, const Type::TList *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TId   *, const Type::TMutable *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TId   *, const Type::TOpt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TId   *, const Type::TObj *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TId   *, const Type::TReal *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TId   *, const Type::TSet *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TId   *, const Type::TSeq *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TId   *, const Type::TStr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TId   *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TId   *, const Type::TTimePnt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TInt  *, const Type::TAddr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TInt  *, const Type::TAny *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TInt  *, const Type::TBool *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TInt  *, const Type::TDict *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TInt  *, const Type::TErr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TInt  *, const Type::TFunc *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TInt  *, const Type::TId *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TInt  *, const Type::TInt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TInt  *, const Type::TList *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TInt  *, const Type::TMutable *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TInt  *, const Type::TOpt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TInt  *, const Type::TObj *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TInt  *, const Type::TReal *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TInt  *, const Type::TSet *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TInt  *, const Type::TSeq *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TInt  *, const Type::TStr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TInt  *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TInt  *, const Type::TTimePnt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TList *, const Type::TAddr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TList *, const Type::TAny *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TList *, const Type::TBool *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TList *, const Type::TDict *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TList *, const Type::TErr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TList *, const Type::TFunc *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TList *, const Type::TId *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TList *, const Type::TInt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TList *, const Type::TList *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TList *, const Type::TMutable *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TList *, const Type::TOpt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TList *, const Type::TObj *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TList *, const Type::TReal *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TList *, const Type::TSet *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TList *, const Type::TSeq *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TList *, const Type::TStr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TList *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TList *, const Type::TTimePnt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TMutable *, const Type::TAddr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TMutable *, const Type::TAny *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TMutable *, const Type::TBool *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TMutable *, const Type::TDict *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TMutable *, const Type::TErr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TMutable *, const Type::TFunc *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TMutable *, const Type::TId *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TMutable *, const Type::TInt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TMutable *, const Type::TList *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TMutable *, const Type::TMutable *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TMutable *, const Type::TOpt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TMutable *, const Type::TObj *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TMutable *, const Type::TReal *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TMutable *, const Type::TSet *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TMutable *, const Type::TSeq *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TMutable *, const Type::TStr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TMutable *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TMutable *, const Type::TTimePnt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TOpt  *, const Type::TAddr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TOpt  *, const Type::TAny *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TOpt  *, const Type::TBool *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TOpt  *, const Type::TDict *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TOpt  *, const Type::TErr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TOpt  *, const Type::TFunc *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TOpt  *, const Type::TId *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TOpt  *, const Type::TInt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TOpt  *, const Type::TList *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TOpt  *, const Type::TMutable *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TOpt  *, const Type::TOpt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TOpt  *, const Type::TObj *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TOpt  *, const Type::TReal *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TOpt  *, const Type::TSet *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TOpt  *, const Type::TSeq *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TOpt  *, const Type::TStr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TOpt  *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TOpt  *, const Type::TTimePnt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TObj  *, const Type::TAddr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TObj  *, const Type::TAny *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TObj  *, const Type::TBool *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TObj  *, const Type::TDict *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TObj  *, const Type::TErr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TObj  *, const Type::TFunc *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TObj  *, const Type::TId *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TObj  *, const Type::TInt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TObj  *, const Type::TList *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TObj  *, const Type::TMutable *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TObj  *, const Type::TOpt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TObj  *, const Type::TObj *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TObj  *, const Type::TReal *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TObj  *, const Type::TSet *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TObj  *, const Type::TSeq *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TObj  *, const Type::TStr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TObj  *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TObj  *, const Type::TTimePnt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TReal *, const Type::TAddr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TReal *, const Type::TAny *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TReal *, const Type::TBool *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TReal *, const Type::TDict *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TReal *, const Type::TErr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TReal *, const Type::TFunc *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TReal *, const Type::TId *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TReal *, const Type::TInt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TReal *, const Type::TList *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TReal *, const Type::TMutable *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TReal *, const Type::TOpt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TReal *, const Type::TObj *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TReal *, const Type::TReal *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TReal *, const Type::TSet *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TReal *, const Type::TSeq *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TReal *, const Type::TStr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TReal *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TReal *, const Type::TTimePnt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSet  *, const Type::TAddr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSet  *, const Type::TAny *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSet  *, const Type::TBool *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSet  *, const Type::TDict *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSet  *, const Type::TErr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSet  *, const Type::TFunc *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSet  *, const Type::TId *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSet  *, const Type::TInt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSet  *, const Type::TList *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSet  *, const Type::TMutable *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSet  *, const Type::TOpt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSet  *, const Type::TObj *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSet  *, const Type::TReal *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSet  *, const Type::TSet *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSet  *, const Type::TSeq *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSet  *, const Type::TStr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSet  *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSet  *, const Type::TTimePnt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSeq  *, const Type::TAddr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSeq  *, const Type::TAny *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSeq  *, const Type::TBool *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSeq  *, const Type::TDict *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSeq  *, const Type::TErr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSeq  *, const Type::TFunc *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSeq  *, const Type::TId *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSeq  *lhs, const Type::TInt *) const {
    Type = lhs->AsType();
  }
  virtual void operator()(const Type::TSeq  *, const Type::TList *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSeq  *, const Type::TMutable *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSeq  *, const Type::TOpt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSeq  *, const Type::TObj *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSeq  *, const Type::TReal *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSeq  *, const Type::TSet *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSeq  *, const Type::TSeq *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSeq  *, const Type::TStr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSeq  *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TSeq  *, const Type::TTimePnt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TStr  *, const Type::TAddr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TStr  *, const Type::TAny *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TStr  *, const Type::TBool *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TStr  *, const Type::TDict *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TStr  *, const Type::TErr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TStr  *, const Type::TFunc *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TStr  *, const Type::TId *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TStr  *, const Type::TInt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TStr  *, const Type::TList *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TStr  *, const Type::TMutable *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TStr  *, const Type::TOpt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TStr  *, const Type::TObj *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TStr  *, const Type::TReal *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TStr  *, const Type::TSet *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TStr  *, const Type::TSeq *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TStr  *, const Type::TStr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TStr  *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TStr  *, const Type::TTimePnt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimeDiff *, const Type::TAddr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimeDiff *, const Type::TAny *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimeDiff *, const Type::TBool *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimeDiff *, const Type::TDict *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimeDiff *, const Type::TErr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimeDiff *, const Type::TFunc *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimeDiff *, const Type::TId *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimeDiff *, const Type::TInt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimeDiff *, const Type::TList *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimeDiff *, const Type::TMutable *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimeDiff *, const Type::TOpt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimeDiff *, const Type::TObj *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimeDiff *, const Type::TReal *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimeDiff *, const Type::TSet *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimeDiff *, const Type::TSeq *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimeDiff *, const Type::TStr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimeDiff *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimeDiff *, const Type::TTimePnt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimePnt *, const Type::TAddr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimePnt *, const Type::TAny *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimePnt *, const Type::TBool *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimePnt *, const Type::TDict *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimePnt *, const Type::TErr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimePnt *, const Type::TFunc *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimePnt *, const Type::TId *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimePnt *, const Type::TInt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimePnt *, const Type::TList *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimePnt *, const Type::TMutable *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimePnt *, const Type::TOpt *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimePnt *, const Type::TObj *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimePnt *, const Type::TReal *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimePnt *, const Type::TSet *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimePnt *, const Type::TSeq *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimePnt *, const Type::TStr *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimePnt *, const Type::TTimeDiff *) const { throw TExprError(HERE, PosRange); }
  virtual void operator()(const Type::TTimePnt *, const Type::TTimePnt *) const { throw TExprError(HERE, PosRange); }

  private:

  /* TODO */
  const TPosRange &PosRange;

  /* TODO */
  Type::TType &Type;

};  // TSkipTypeVisitor

TExpr::TPtr TSkip::New(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range) {
  return TSkip::TPtr(new TSkip(lhs, rhs, pos_range));
}

TSkip::TSkip(const TExpr::TPtr &lhs, const TExpr::TPtr &rhs, const TPosRange &pos_range)
    : TBinary(lhs, rhs, pos_range) {}

void TSkip::Accept(const TVisitor &visitor) const {
  assert(this);
  assert(&visitor);
  visitor(this);
}

Type::TType TSkip::GetType() const {
  assert(this);
  Type::TType type;
  Type::TType::Accept(GetLhs()->GetType(), GetRhs()->GetType(), TSkipTypeVisitor(type, GetPosRange()));
  return type;
}
