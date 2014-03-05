/* <base/time_maps.cc>

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

#include <base/time_maps.h>

using namespace Base;
using namespace Stig;

static Type::TObj::TElems *TimeDiffMapFactory() {
  return new Type::TObj::TElems(
				 {{"is_forward"     , Type::TBool::Get()},
				  {"day"            , Type::TInt::Get()},
				  {"hour"           , Type::TInt::Get()},
				  {"minute"         , Type::TInt::Get()},
				  {"second"         , Type::TInt::Get()},
				  {"nanosecond"    , Type::TInt::Get()}} );

}

const TSafeGlobal<Type::TObj::TElems> Base::Chrono::TimeDiffMap(TimeDiffMapFactory);

static Type::TObj::TElems *TimePntMapFactory() {
  return new Type::TObj::TElems(
	{{"year"      , Type::TInt::Get()},
	 {"month"     , Type::TInt::Get()},
	 {"day"       , Type::TInt::Get()},
         {"hour"      , Type::TInt::Get()},
         {"minute"    , Type::TInt::Get()},
         {"second"    , Type::TInt::Get()},
         {"nanosecond"    , Type::TInt::Get()},
	 {"utc_offset", Type::TInt::Get()}} );
}

const TSafeGlobal<Type::TObj::TElems> Base::Chrono::TimePntMap(TimePntMapFactory);

bool Base::Chrono::IsTimeObj(const Stig::Type::TObj *type) {
  assert(type);
  return (type->GetElems() == *TimeDiffMap || type->GetElems() == *TimePntMap);
}