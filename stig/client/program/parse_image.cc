/* <stig/client/program/parse_image.cc>

   Implements <stig/client/program/parse_image.h>.

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

#include <stig/client/program/parse_image.h>

#include <memory>

#include <base/thrower.h>
#include <stig/client/program/throw_syntax_errors.h>
#include <stig/client/program/translate_expr.h>

using namespace std;
using namespace Base;
using namespace Stig;
using namespace Stig::Client::Program;

static bool ForEachXact(const TProgram *program, const TForXact &cb) {
  ThrowSyntaxErrors();
  assert(program);
  assert(&cb);
  auto image = dynamic_cast<const TImage *>(program->GetTop());
  if (!image) {
    DEFINE_ERROR(error_t, runtime_error, "not an image");
    THROW_ERROR(error_t);
  }
  auto opt_xact_seq = image->GetOptXactSeq();
  for (;;) {
    auto xact_seq = dynamic_cast<const TXactSeq *>(opt_xact_seq);
    if (!xact_seq) {
      break;
    }
    if (!cb(xact_seq->GetXact())) {
      return false;
    }
    opt_xact_seq = xact_seq->GetOptXactSeq();
  }
  return true;
}

bool Stig::Client::Program::ParseImageFile(const char *path, const TForXact &cb) {
  bool result = ForEachXact(unique_ptr<TProgram>(TProgram::ParseFile(path)).get(), cb);
  Tools::Nycr::TNode::DeleteEach();
  return result;
}

bool Stig::Client::Program::ParseImageStr(const char *str, const TForXact &cb) {
  bool result = ForEachXact(unique_ptr<TProgram>(TProgram::ParseStr(str)).get(), cb);
  Tools::Nycr::TNode::DeleteEach();
  return result;
}

bool Stig::Client::Program::TranslateXact(const TXact *xact, const TForKv &cb) {
  assert(xact);
  assert(&cb);
  void
      *lhs_alloc = alloca(Sabot::State::GetMaxStateSize()),
      *rhs_alloc = alloca(Sabot::State::GetMaxStateSize());
  auto opt_seq = xact->GetOptKvSeq();
  for (;;) {
    auto seq = dynamic_cast<const TKvSeq *>(opt_seq);
    if (!seq) {
      break;
    }
    auto kv = dynamic_cast<const TKv *>(seq->GetAnyKv());
    if (!kv) {
      DEFINE_ERROR(error_t, runtime_error, "malformed kv entry in image file");
      THROW_ERROR(error_t);
    }
    Sabot::State::TAny::TWrapper
        lhs(NewStateSabot(kv->GetLhs(), lhs_alloc)),
        rhs(NewStateSabot(kv->GetRhs(), rhs_alloc));
    if (!cb(lhs.get(), rhs.get())) {
      return false;
    }
    opt_seq = seq->GetOptKvSeq();
  }
  return true;
}
