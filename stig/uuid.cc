/* <stig/uuid.cc>

   Implemntation of <stig/uuid.h>

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

#include <stig/uuid.h>

#include <cassert>
#include <cstring>

#include <base/error.h>
#include <base/syntax_error.h>

using namespace Stig;

TUUID::TUUID() {
  uuid_generate_time(UUID);
}

TUUID::TUUID(const TUUID &that) {
  uuid_copy(UUID, that.UUID);
}

TUUID::TUUID(const uuid_t &that) {
  uuid_copy(UUID, that);
}

TUUID::TUUID(const char *in) {
  if (uuid_parse(in, UUID) != 0) {
    throw Base::TSyntaxError(HERE, "Not a valid uuid");
  }
}

TUUID &TUUID::operator=(const TUUID &that) {
  assert(this);
  assert(&that);
  if (&that != this) {
    uuid_copy(UUID, that.UUID);
  }
  return *this;
}

bool TUUID::operator==(const TUUID &that) const {
  assert(this);
  assert(&that);
  return uuid_compare(UUID, that.UUID) == 0;
}

bool TUUID::operator!=(const TUUID &that) const {
  assert(this);
  assert(&that);
  return !(*this == that);
}

bool TUUID::operator<(const TUUID &that) const {
  assert(this);
  assert(&that);
  return uuid_compare(UUID, that.UUID) < 0;
}

bool TUUID::operator<=(const TUUID &that) const {
  assert(this);
  assert(&that);
  return (*this < that) || (*this == that);
}

bool TUUID::operator>(const TUUID &that) const {
  assert(this);
  assert(&that);
  return uuid_compare(UUID, that.UUID) > 0;
}

/* TODO */
bool TUUID::operator>=(const TUUID &that) const {
  assert(this);
  assert(&that);
  return (*this > that) || (*this == that);
}

/* TODO */
std::string TUUID::AsString() const {
  assert(this);
  std::stringstream strm;
  Write(strm);
  return strm.str();
}

/* TODO */
int TUUID::Compare(const TUUID &that) const {
  assert(this);
  assert(&that);
  return uuid_compare(UUID, that.UUID);
}

/* TODO */
size_t TUUID::GetHash() const {
  assert(this);
  size_t *ptr = reinterpret_cast<size_t *>(const_cast<unsigned char *>(UUID));
  return *ptr;
}

/* TODO */
void TUUID::Write(std::ostream &stream) const {
  char buf[37];
  uuid_unparse(UUID, buf);
  stream << buf;
}

bool TUUID::IsValidUuid(const char *s) {
  //TODO: Make more sophisticated.
  return strlen(s) == 36;
}