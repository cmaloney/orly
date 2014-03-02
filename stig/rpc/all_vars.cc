/* <stig/rpc/all_vars.cc> 

   Implementation of <stig/rpc/all_vars.h>.

   Copyright 2010-2014 Tagged
   
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
   
     http://www.apache.org/licenses/LICENSE-2.0
   
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <stig/rpc/all_vars.h>

using namespace std;
using namespace Stig::Rpc;

TVar::TVisitor::~TVisitor() {}

TVar::TDoubleVisitor::~TDoubleVisitor() {}

void TVar::TDoubleVisitor::Visit(const TVar *lhs, const TVar *rhs) const {
  assert(this);
  assert(lhs);
  assert(rhs);
  lhs->Accept(TLhsVisitor(rhs, *this));
}

void TVar::TDoubleVisitor::TLhsVisitor::operator()(const Var::TAddr     *lhs) const { Rhs->Accept(TRhsVisitor<Var::TAddr    >(lhs, DoubleVisitor)); }
void TVar::TDoubleVisitor::TLhsVisitor::operator()(const Var::TBool     *lhs) const { Rhs->Accept(TRhsVisitor<Var::TBool    >(lhs, DoubleVisitor)); }
void TVar::TDoubleVisitor::TLhsVisitor::operator()(const Var::TDict     *lhs) const { Rhs->Accept(TRhsVisitor<Var::TDict    >(lhs, DoubleVisitor)); }
void TVar::TDoubleVisitor::TLhsVisitor::operator()(const Var::TId       *lhs) const { Rhs->Accept(TRhsVisitor<Var::TId      >(lhs, DoubleVisitor)); }
void TVar::TDoubleVisitor::TLhsVisitor::operator()(const Var::TInt      *lhs) const { Rhs->Accept(TRhsVisitor<Var::TInt     >(lhs, DoubleVisitor)); }
void TVar::TDoubleVisitor::TLhsVisitor::operator()(const Var::TList     *lhs) const { Rhs->Accept(TRhsVisitor<Var::TList    >(lhs, DoubleVisitor)); }
void TVar::TDoubleVisitor::TLhsVisitor::operator()(const Var::TObj      *lhs) const { Rhs->Accept(TRhsVisitor<Var::TObj     >(lhs, DoubleVisitor)); }
void TVar::TDoubleVisitor::TLhsVisitor::operator()(const Var::TOpt      *lhs) const { Rhs->Accept(TRhsVisitor<Var::TOpt     >(lhs, DoubleVisitor)); }
void TVar::TDoubleVisitor::TLhsVisitor::operator()(const Var::TReal     *lhs) const { Rhs->Accept(TRhsVisitor<Var::TReal    >(lhs, DoubleVisitor)); }
void TVar::TDoubleVisitor::TLhsVisitor::operator()(const Var::TSet      *lhs) const { Rhs->Accept(TRhsVisitor<Var::TSet     >(lhs, DoubleVisitor)); }
void TVar::TDoubleVisitor::TLhsVisitor::operator()(const Var::TStr      *lhs) const { Rhs->Accept(TRhsVisitor<Var::TStr     >(lhs, DoubleVisitor)); }
void TVar::TDoubleVisitor::TLhsVisitor::operator()(const Var::TTimeDiff *lhs) const { Rhs->Accept(TRhsVisitor<Var::TTimeDiff>(lhs, DoubleVisitor)); }
void TVar::TDoubleVisitor::TLhsVisitor::operator()(const Var::TTimePnt  *lhs) const { Rhs->Accept(TRhsVisitor<Var::TTimePnt >(lhs, DoubleVisitor)); }

