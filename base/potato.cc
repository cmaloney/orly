/* <base/potato.cc>

   Implements <base/potato.h>.

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

#include <base/potato.h>

#include <base/os_error.h>

using namespace std;
using namespace Base;

TPotato::TLock::~TLock() {}

void TPotato::TLock::PreDtor() {
  assert(this);
  if (pthread_rwlock_unlock(&(Potato->Lockable))) {
    TOsError::Abort(HERE);
  }
}

TPotato::TExclusiveLock::TExclusiveLock(const shared_ptr<TPotato> &potato)
    : TLock(potato) {
  TOsError::IfNe0(HERE, pthread_rwlock_wrlock(&(potato->Lockable)));
}

TPotato::TExclusiveLock::~TExclusiveLock() {
  assert(this);
  PreDtor();
}

TPotato::TSharedLock::TSharedLock(const shared_ptr<TPotato> &potato)
    : TLock(potato) {
  TOsError::IfNe0(HERE, pthread_rwlock_rdlock(&(potato->Lockable)));
}

TPotato::TSharedLock::~TSharedLock() {
  assert(this);
  PreDtor();
}

TPotato::TPotato() {
  TOsError::IfNe0(HERE, pthread_rwlock_init(&Lockable, 0));
}

TPotato::~TPotato() {
  assert(this);
  if (pthread_rwlock_destroy(&Lockable)) {
    TOsError::Abort(HERE);
  }
}
