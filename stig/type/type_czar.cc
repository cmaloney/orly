/* <stig/type/type_czar.cc>

   Implements <stig/type/type_czar.h>

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

#include <stig/type/type_czar.h>

#include <stig/type.h>

using namespace Stig::Type;

TTypeCzar::TTypeCzar() {
  TAddr::New();
  TAny::New();
  TBool::New();
  TDict::New();
  TErr::New();
  TFunc::New();
  TId::New();
  TInt::New();
  TList::New();
  TMutable::New();
  TObj::New();
  TOpt::New();
  TReal::New();
  TSeq::New();
  TSet::New();
  TStr::New();
  TTimeDiff::New();
  TTimePnt::New();
  TUnknown::New();
}

TTypeCzar::~TTypeCzar() {
  TUnknown::Delete();
  TTimePnt::Delete();
  TTimeDiff::Delete();
  TStr::Delete();
  TSet::Delete();
  TSeq::Delete();
  TReal::Delete();
  TOpt::Delete();
  TObj::Delete();
  TMutable::Delete();
  TList::Delete();
  TInt::Delete();
  TId::Delete();
  TFunc::Delete();
  TErr::Delete();
  TDict::Delete();
  TBool::Delete();
  TAny::Delete();
  TAddr::Delete();
}