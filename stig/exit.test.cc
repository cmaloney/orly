/* <stig/exit.test.cc>

   Unit test for whether or not TService quits on destruction, which it should.

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

#include <stig/spa/service.h>

#include <stig/honcho.h>

#include <test/kit.h>

using namespace std;
using namespace Stig;
using namespace Stig::Spa;

FIXTURE(Startup) {
  THoncho honcho;
  TService service;
  /* Parse command-line arguments */
  service.SetPackageDir("/tmp/");
  /* Set checkpoint path and package directory */
  TUUID session;
  service.CreateSession(Base::TOpt<TUUID>(TUUID()), 1000, session);
  TUUID private_pov;
  service.CreatePrivatePov(session, {}, 1000, false, private_pov);
}