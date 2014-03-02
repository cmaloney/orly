/* <stig/rpc/all_types.cc> 

   Implementation of <stig/rpc/all_types.h>.

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

#include <stig/rpc/all_types.h>

using namespace std;
using namespace Stig::Rpc;

TType::TVisitor::~TVisitor() {}

TType::TDoubleVisitor::~TDoubleVisitor() {}

void TType::TDoubleVisitor::Visit(const TType *lhs, const TType *rhs) const {
  assert(this);
  assert(lhs);
  assert(rhs);
  lhs->Accept(TLhsVisitor(rhs, *this));
}

void TType::TDoubleVisitor::TLhsVisitor::operator()(const Type::TAddr     *lhs) const { Rhs->Accept(TRhsVisitor<Type::TAddr    >(lhs, DoubleVisitor)); }
void TType::TDoubleVisitor::TLhsVisitor::operator()(const Type::TBool     *lhs) const { Rhs->Accept(TRhsVisitor<Type::TBool    >(lhs, DoubleVisitor)); }
void TType::TDoubleVisitor::TLhsVisitor::operator()(const Type::TDict     *lhs) const { Rhs->Accept(TRhsVisitor<Type::TDict    >(lhs, DoubleVisitor)); }
void TType::TDoubleVisitor::TLhsVisitor::operator()(const Type::TId       *lhs) const { Rhs->Accept(TRhsVisitor<Type::TId      >(lhs, DoubleVisitor)); }
void TType::TDoubleVisitor::TLhsVisitor::operator()(const Type::TInt      *lhs) const { Rhs->Accept(TRhsVisitor<Type::TInt     >(lhs, DoubleVisitor)); }
void TType::TDoubleVisitor::TLhsVisitor::operator()(const Type::TList     *lhs) const { Rhs->Accept(TRhsVisitor<Type::TList    >(lhs, DoubleVisitor)); }
void TType::TDoubleVisitor::TLhsVisitor::operator()(const Type::TObj      *lhs) const { Rhs->Accept(TRhsVisitor<Type::TObj     >(lhs, DoubleVisitor)); }
void TType::TDoubleVisitor::TLhsVisitor::operator()(const Type::TOpt      *lhs) const { Rhs->Accept(TRhsVisitor<Type::TOpt     >(lhs, DoubleVisitor)); }
void TType::TDoubleVisitor::TLhsVisitor::operator()(const Type::TReal     *lhs) const { Rhs->Accept(TRhsVisitor<Type::TReal    >(lhs, DoubleVisitor)); }
void TType::TDoubleVisitor::TLhsVisitor::operator()(const Type::TSet      *lhs) const { Rhs->Accept(TRhsVisitor<Type::TSet     >(lhs, DoubleVisitor)); }
void TType::TDoubleVisitor::TLhsVisitor::operator()(const Type::TStr      *lhs) const { Rhs->Accept(TRhsVisitor<Type::TStr     >(lhs, DoubleVisitor)); }
void TType::TDoubleVisitor::TLhsVisitor::operator()(const Type::TTimeDiff *lhs) const { Rhs->Accept(TRhsVisitor<Type::TTimeDiff>(lhs, DoubleVisitor)); }
void TType::TDoubleVisitor::TLhsVisitor::operator()(const Type::TTimePnt  *lhs) const { Rhs->Accept(TRhsVisitor<Type::TTimePnt >(lhs, DoubleVisitor)); }

