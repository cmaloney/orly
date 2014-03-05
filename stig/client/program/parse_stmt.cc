/* <stig/client/program/parse_stmt.cc>

   Implements <stig/client/program/parse_stmt.h>.

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

#include <stig/client/program/parse_stmt.h>

#include <memory>

#include <base/thrower.h>
#include <stig/client/program/throw_syntax_errors.h>

using namespace std;
using namespace Base;
using namespace Stig::Client::Program;

static void ForStmt(const TProgram *program, const TForStmt &cb) {
  ThrowSyntaxErrors();
  assert(program);
  assert(&cb);
  auto stmt = dynamic_cast<const TStmt *>(program->GetTop());
  if (!stmt) {
    DEFINE_ERROR(error_t, runtime_error, "not a stmt");
    THROW_ERROR(error_t);
  }
  cb(stmt);
}

void Stig::Client::Program::ParseStmtFile(const char *path, const TForStmt &cb) {
  ForStmt(unique_ptr<TProgram>(TProgram::ParseFile(path)).get(), cb);
  Tools::Nycr::TNode::DeleteEach();
}

void Stig::Client::Program::ParseStmtStr(const char *str, const TForStmt &cb) {
  ForStmt(unique_ptr<TProgram>(TProgram::ParseStr(str)).get(), cb);
  Tools::Nycr::TNode::DeleteEach();
}
