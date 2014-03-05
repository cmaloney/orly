/* <stig/spa/stig_args.cc>

   Implements <stig/spa/stig_args.h>.

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

#include <stig/spa/stig_args.h>

#include <sstream> //TODO

#include <stig/spa/error.h>
#include <stig/spa/command.h>

using namespace std;
using namespace Server;
using namespace Stig::Spa;

//TODO: Write a unit test for this bugger.
TArgs::TArgs(const char *qs) : VerifiedAllUsed(false) {
  if(!qs) {
    return;
  }

  const char *csr = qs;

  if(*csr == '?') {
    ++csr;
  }

  enum { Key, Val} state = Key;

  TStrPiece key(csr, csr);
  TStrPiece val;

  bool reached_end = false;

  while(!reached_end) {
    switch(*csr) {
      case '\0':
        reached_end = true;
      case '&':
      case ';': {
        switch(state) {
          case Key: {
            key.SetLimit(csr);
            break;
          }
          case Val: {
            val.SetLimit(csr);
            break;
          }
        }
        if(key.GetSize() == 0) {
          if(reached_end) {
            continue;
          }
          throw TArgError(HERE, csr - qs, "Missing query string key");
        }
        string tmp;
        UrlDecode(key, tmp);
        Args.insert(make_pair(tmp, val));

        ++csr;
        key.SetLimit(csr);
        key.SetStart(csr);
        val.Reset();
        state = Key;

        break;
      }

      case '=': {
        switch(state) {
          case Key: {
            key.SetLimit(csr);
            ++csr;
            val.SetLimit(csr);
            val.SetStart(csr);
            state = Val;
            break;
          }
          case Val: {
            throw TArgError(HERE, csr - qs, "Unexpected '='");
          }
        }
        break;
      }

      case '#': {
        throw TArgError(HERE, csr - qs, "'#' fragments are not supported");
      }

      default: {
        ++csr;
        break;
      }
    }
  }
}

TArgs::~TArgs() {
  if(!VerifiedAllUsed) {
    //TODO: Selectively disable based on whether or not this is a "Dev" build. We make this have no chance of occuring by having spa.cc error properly.
    Base::TError::Abort(HERE); //The user must call VerifyAllUsed exactly once during the TArgs lifetime.
  }
}

void TArgs::FakeVerify() {
  assert(this);
  VerifiedAllUsed = true;
}

//TODO
bool TArgs::TryGrabArg(const TStrPiece &name, string &out) {
  assert(this);

  //TODO: The inline string construction here is massive ug.
  auto it = Args.find(string(name.GetStart(), name.GetSize()));
  if(it == Args.end()) {
    return false;
  }

  UrlDecode(it->second, out);
  Args.erase(it);

  return true;
}

void TArgs::GrabArg(const TStrPiece &name, string &out) {
  assert(this);
  assert(&name);
  assert(&out);

  if(!TryGrabArg(name, out)) {
    ostringstream s;
    s<<"missing argument "<<name;
    throw TNeededArgError(HERE, s.str().c_str());
  }
}


//TODO: Convert to Base::Convert.
//TODO: This is very copy/pasty. Really should abstract it out more. arg value error builder :)
void TArgs::Convert(const std::string &str, bool &val) {
  assert(this);
  assert(&str);
  assert(&val);

  if(str == "true" || str == "t" || str == "1") {
    val = true;
  } else if (str == "false" || str == "f" || str == "1") {
    val = false;
  } else {
    throw TIntArgError("bool");
  }
}

void TArgs::Convert(const std::string &str, int &val) {
  assert(this);
  assert(&str);
  assert(&val);

  istringstream iss(str);
  iss>>val;

  if(iss.fail()) {
    throw TIntArgError("int");
  }
}

void TArgs::Convert(const std::string &str, uint64_t &val) {
  assert(this);
  assert(&str);
  assert(&val);

  istringstream iss(str);
  iss>>val;


  if(iss.fail()) {
    throw TIntArgError("int64_t");
  }
}

void TArgs::Convert(const std::string &str, std::chrono::milliseconds &val) {
  assert(this);
  assert(&str);
  assert(&val);

  uint64_t count;

  TArgs::Convert(str, count);
  val = chrono::milliseconds(count);
}

void TArgs::Convert(const std::string &str, long &val) {
  assert(this);
  assert(&str);
  assert(&val);

  istringstream iss(str);
  iss>>val;

  if(iss.fail()) {
    throw TIntArgError("long");
  }
}

void TArgs::Convert(const std::string &str, TUUID &uuid) {
  assert(this);
  assert(&str);
  assert(&uuid);

  if(!TUUID::IsValidUuid(str.c_str())) {
    throw TIntArgError("uuid");
  }
  uuid=TUUID(str.c_str());
}

void TArgs::Convert(const std::string &str, Var::TVar &var) {
  if(!Stig::Spa::ParseCommand(str.c_str(), var)) {
    throw TArgValueError(HERE, "stig literal");
  }
}

bool TArgs::HasVerified() const {
  assert(this);
  return VerifiedAllUsed;
}

void TArgs::VerifyAllUsed() {
  assert(this);
  VerifiedAllUsed = true;
  if(Args.size() > 0) {
    stringstream arg_collector;
    arg_collector<<"Unnecessary arguments: ";
    for(auto arg: Args) {
      arg_collector<<'"'<<arg.first<<"\" ";
    }
    throw TArgValueError(HERE, arg_collector.str().c_str());
  }
}

template<>
void TArgs::Get(const TStrPiece &name, std::string &str) {
  assert(this);
  assert(&name);
  assert(&str);

  GrabArg(name, str);
}