/* <base/cmd.cc>

   Implements <base/cmd.h>

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

#include <base/cmd.h>

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>

#include <sys/ioctl.h>

#include <base/no_default_case.h>
#include <base/zero.h>
#include <starsha/note.h>

using namespace std;
using namespace placeholders;
using namespace Base;

TCmd::TMeta::~TMeta() {
  assert(this);
  for (auto param: Params) {
    delete param;
  }
}

void TCmd::TMeta::Dump(ostream &strm, const TCmd *cmd) const {
  assert(this);
  assert(&strm);
  for (auto param: Params) {
    auto arg = param->TryGetArg();
    if (arg) {
      param->WriteCmd(strm);
      strm << "::" << arg->GetName() << " = ";
      arg->Write(strm, cmd);
      strm << ';' << endl;
    }
  }
}

size_t TCmd::TMeta::Parse(TCmd *cmd, int argc, char *argv[]) const {
  assert(this);
  size_t count = 0;
  Parse(cmd, argc, argv, bind(WriteToStdErr, _1, ref(count)));
  return count;
}

bool TCmd::TMeta::Parse(TCmd *cmd, int argc, char *argv[], const TMessageConsumer &cb) const {
  assert(this);
  return TParser(this).Parse(cmd, argc, argv, cb);
}

void TCmd::TMeta::WriteAfterDesc(ostream &) const {}

void TCmd::TMeta::WriteHelp(ostream &strm, const TCmd *cmd, size_t line_size, size_t pad_size) const {
  assert(this);
  assert(&strm);
  if (!line_size) {
    struct winsize ws;
    ioctl(0, TIOCGWINSZ, &ws);
    line_size = ws.ws_col - 1;
  }
  line_size = max(line_size, pad_size + 10);
  strm << "usage: " << cmd->Prog;
  vector<pair<string, string>> by_pos;
  map<string, string> by_name;
  size_t pos_size = 0, named_size = 0;
  for (auto param: Params) {
    param->WriteLineItem(strm);
    pair<string, string> item;
    /* extra */ {
      ostringstream strm;
      param->WriteHelp(strm, cmd);
      item.second = strm.str();
    }
    auto arg = param->TryGetArg();
    if (param->GetIsByName()) {
      /* extra */ {
        ostringstream strm;
        strm << "  --" << param->GetDiagnosticName();
        if (arg && arg->GetIsRequired()) {
          strm << " <";
          arg->WriteType(strm);
          strm << '>';
        }
        item.first = strm.str();
      }
      named_size = max(named_size, item.first.size());
      by_name.insert(item);
    } else {
      assert(arg);
      item.first = arg->GetName();
      pos_size = max(pos_size, item.first.size());
      by_pos.push_back(item);
    }
  }
  strm << endl;
  if (Desc) {
    strm << endl << Desc << endl;
  }
  WriteAfterDesc(strm);
  if (pos_size) {
    strm << endl;
    pos_size += pad_size;
    for (const pair<string, string> &item: by_pos) {
      WriteWrapped(strm, pos_size, line_size, item.first, item.second);
    }
  }
  if (named_size) {
    strm << endl << "OPTIONS:" << endl;
    named_size += pad_size;
    for (const pair<string, string> &item: by_name) {
      WriteWrapped(strm, named_size, line_size, item.first, item.second);
    }
  }
}

bool TCmd::TMeta::WriteToStdErr(const string &msg, size_t &count) {
  assert(&msg);
  assert(&count);
  cerr << msg << endl;
  ++count;
  return true;
}

TCmd::TMeta::TMeta(const char *desc)
    : Desc(desc), AmbigPosEnd(false) {
  Param(&TCmd::IsHelp, "is_help", Optional, "help\0h\0?\0", "Show this help message.");
  Param(&TCmd::ArgsFiles, "args", Optional, "args\0", "Read more args from a file.");
}

bool TCmd::TMeta::TParam::TArg::Parse(TCmd *cmd, const char *arg_text, const TMessageConsumer &cb) const {
  assert(this);
  if (arg_text) {
    try {
      istringstream strm(arg_text);
      Read(strm, cmd);
      if (strm.fail()) {
        DEFINE_ERROR(error_t, invalid_argument, "syntax error");
        THROW_ERROR(error_t);
      }
    } catch (const exception &ex) {
      ostringstream strm;
      strm << '"' << Name << "\": " << ex.what();
      if (!cb(strm.str())) {
        return false;
      }
    }
  } else {
    AssignDefault(cmd);
  }
  return true;
}

TCmd::TMeta::TParam::~TParam() {}

bool TCmd::TMeta::TParam::ForEachName(const function<bool (const char *)> &cb) const {
  assert(this);
  assert(&cb);
  if (Names) {
    for (const char *name = Names; *name; name += strlen(name) + 1) {
      if (!cb(name)) {
        return false;
      }
    }
  }
  return true;
}

bool TCmd::TMeta::TParam::Matches(const char *target_name) const {
  assert(this);
  assert(target_name);
  if (Names) {
    for (const char *name = Names; *name; name += strlen(name) + 1) {
      if (!strcmp(name, target_name)) {
        return true;
      }
    }
  }
  return false;
}

void TCmd::TMeta::TParam::WriteHelp(ostream &strm, const TCmd *cmd) const {
  assert(this);
  assert(&strm);
  strm << Desc;
  int cnt = 0;
  ForEachHelpPiece(strm, cmd, [&strm, &cnt] {
    if (cnt) {
      strm << "; ";
    } else {
      strm << " (";
    }
    ++cnt;
    return true;
  });
  if (cnt) {
    strm << ')';
  }
}

void TCmd::TMeta::TParam::WriteLineItem(ostream &strm) const {
  assert(this);
  assert(&strm);
  if (!Names) {
    const char *lhs, *rhs;
    switch (Basis) {
      case Required: {
        lhs = "";
        rhs = "";
        break;
      }
      case Optional: {
        lhs = "[";
        rhs = "]";
        break;
      }
      NO_DEFAULT_CASE;
    }
    const char *tail;
    switch (GetRecurrence()) {
      case Once: {
        tail = "";
        break;
      }
      case Many:
      case NoDupes: {
        tail = "...";
        break;
      }
      NO_DEFAULT_CASE;
    }
    strm << ' ' << lhs << GetArg()->GetName() << tail << rhs;
  }
}

bool TCmd::TMeta::TParam::ForEachHelpPiece(ostream &strm, const TCmd *cmd, const function<bool ()> &cb) const {
  assert(this);
  assert(&strm);
  assert(cmd);
  assert(&cb);
  const TArg *arg = TryGetArg();
  if (!Names && arg && arg->GetIsRequired()) {
    if (!cb()) {
      return false;
    }
    arg->WriteType(strm);
  }
  if (!cb()) {
    return false;
  }
  switch (Basis) {
    case Required: {
      strm << "required";
      break;
    }
    case Optional: {
      if (arg && arg->GetIsRequired()) {
        strm << "default: ";
        arg->Write(strm, cmd);
      } else {
        strm << "optional";
      }
      break;
    }
    NO_DEFAULT_CASE;
  }
  switch (GetRecurrence()) {
    case Once: {
      break;
    }
    case Many: {
      if (!cb()) {
        return false;
      }
      strm << "repeatable";
      break;
    }
    case NoDupes: {
      if (!cb()) {
        return false;
      }
      strm << "repeatable, no duplicate values";
      break;
    }
    NO_DEFAULT_CASE;
  }
  int cnt = 0;
  ForEachName([&strm, &cb, &cnt](const char *name) {
    if (cnt) {
      if (cnt == 1) {
        if (!cb()) {
          return false;
        }
        strm << "alias: ";
      } else {
        strm << ", ";
      }
      strm << name;
    }
    ++cnt;
    return true;
  });
  return true;
}

void TCmd::TMeta::AppendParam(TParam *param) {
  assert(this);
  assert(param);
  try {
    if (param->GetIsByName()) {
      param->ForEachName([this](const char *name) {
        if (!UsedNames.insert(name).second) {
          DEFINE_ERROR(error_t, invalid_argument, "ambiguous name");
          THROW_ERROR(error_t) << '"' << name << '"';
        }
        return true;
      });
    } else {
      if (AmbigPosEnd) {
        DEFINE_ERROR(error_t, invalid_argument, "ambiguous position");
        THROW_ERROR(error_t) << '"' << param->GetDiagnosticName() << '"';
      }
      AmbigPosEnd = param->GetBasis() != Required || param->GetRecurrence() != Once;
    }
    Params.push_back(param);
  } catch (...) {
    delete param;
    throw;
  }
}

void TCmd::TMeta::WriteWrapped(ostream &strm, size_t col1_size, size_t line_size, const string &col1, const string &col2) {
  assert(&strm);
  assert(&col1);
  assert(&col2);
  strm << col1;
  for (size_t i = 0; i < col1_size - col1.size(); ++i) {
    strm << ' ';
  }
  const char
      *start = col2.data(),
      *limit = start + col2.size();
  /* Loop over the help text, isolating words and emitting them as we go.  The
     words are delimited by whitespace.  We skip over extra whitespace, instead
     emitting a single space between words.  If we fill up the second column and
     still have more to write, then drop down a line, indent past the first
     column, and continue.  This will produce a wrapped block of text in the
     second column. */
  const char *word_start = 0;  // non-null when we've found a word
  size_t pos = col1_size;      // our current horz. position on the line
  while (start < limit || word_start) {
    /* Pop the next character.  Assume extra whitespace at the end of the text,
       to delimit the last word. */
    const char *cursor = start;
    char c = (start < limit) ? *start++ : ' ';
    if (word_start) {
      /* We've already found the start of a word, so now we're looking for the
         end of it. */
      size_t word_size = cursor - word_start;
      if (pos + word_size + ((pos > col1_size)? 1 : 0) >= line_size) {
        /* We might not have found the end of the word yet, but the length of
           the word so far is already too long to print on the current line.
           So we'll drop down and indent. */
        strm << endl;
        for (pos = 0; pos < col1_size; ++pos) {
          strm << ' ';
        }
      }
      if (isspace(c) || word_size >= line_size) {
        /* We've found whitespace (or the word is just really long), so it's
           time to print. */
        if (pos > col1_size) {
          /* This isn't the first word on the line, so emit a space to separate
             it from the previous word. */
          strm << ' ';
          ++pos;
        }
        /* Write the word, advance our position, and start looking for the next
           word. */
        strm.write(word_start, word_size);
        pos += word_size;
        word_start = 0;
      }
    } else if (!isspace(c)) {
      /* We've found the start of a new word. */
      word_start = cursor;
    }
  }
  strm << endl;
}

TCmd::TMeta::TParser::TParser(const TMeta *meta) {
  assert(meta);
  option op;
  Base::Zero(op);
  for (auto param: meta->Params) {
    if (param->GetIsByName()) {
      param->ForEachName([this, &op, param](const char *name) {
        op.name = name;
        auto arg = param->TryGetArg();
        op.has_arg = arg ? (arg->GetIsRequired() ? required_argument : optional_argument) : no_argument;
        op.val = Options.size() + Offset;
        Options.push_back(op);
        ByName.push_back(param);
        return true;
      });
    } else {
      ByPos.push_back(param);
    }
  }
  Base::Zero(op);
  Options.push_back(op);
}

const TCmd::TMeta::TParam *TCmd::TMeta::TParser::TryFindParam(const char *name) const {
  assert(this);
  for (const TParam *param: ByName) {
    if (param->Matches(name)) {
      return param;
    }
  }
  return nullptr;
}

bool TCmd::TMeta::TParser::TRun::operator()(const TParser *parser, TCmd *cmd, int argc, char *argv[], const TMessageConsumer &cb) {
  assert(this);
  assert(parser);
  assert(&cb);
  char *path = argv[0], *slash = strrchr(path, '/');
  if (slash) {
    *slash = '\0';
    cmd->Dir = path;
    cmd->Prog = slash + 1;
  } else {
    cmd->Prog = path;
  }
  auto options = &(parser->Options[0]);
  int idx;
  /* extra */ {
    lock_guard<mutex> lock(Mutex);
    optind = 1;
    opterr = 0;
    while ((idx = getopt_long_only(argc, argv, "", options, 0)) >= 0) {
      if (idx == '?') {
        ostringstream strm;
        strm << '"' << argv[optind - 1] << "\": unknown option";
        if (!cb(strm.str())) {
          return false;
        }
      } else {
        assert(idx >= Offset);
        idx -= Offset;
        if (!OnRecognition(parser->ByName.at(idx), cmd, optarg, cb)) {
          return false;
        }
        if (cmd->IsHelp) {
          return true;
        }
      }
    }
    idx = optind;
  }
  auto by_pos = parser->ByPos.begin();
  for (int i = idx; i < argc; ++i) {
    if (by_pos != parser->ByPos.end()) {
      auto param = *by_pos;
      if (!OnRecognition(param, cmd, argv[i], cb)) {
        return false;
      }
      if (param->GetRecurrence() == TMeta::Once) {
        ++by_pos;
      }
    } else {
      ostringstream strm;
      strm << '"' << argv[i] << ": extra argument";
      if (!cb(strm.str())) {
        return false;
      }
    }
  }
  for (const string &args_file: cmd->ArgsFiles) {
    using note_t = Starsha::TNote;
    ifstream strm(args_file.c_str());
    unique_ptr<note_t> root(note_t::Read(strm));
    root->ForEachChild(
        [this, parser, cmd, &cb, &args_file](const note_t *note) {
          bool keep_going;
          const TParam *param = parser->TryFindParam(note->GetKey().c_str());
          if (param) {
            const string &value = note->GetValue();
            keep_going = OnRecognition(param, cmd, value.size() ? value.c_str() : nullptr, cb);
          } else {
            ostringstream strm;
            strm << '"' << note->GetKey() << "\": unknown option in \"" << args_file << '"';
            keep_going = cb(strm.str());
          }
          return keep_going;
        }
    );
  }
  return CheckForRequired(parser->ByName, cb) && CheckForRequired(parser->ByPos, cb) && cmd->CheckArgs(cb);
}

bool TCmd::TMeta::TParser::TRun::CheckForRequired(const vector<const TParam *> &params, const TMessageConsumer &cb) const {
  assert(this);
  assert(&params);
  assert(&cb);
  for (auto param: params) {
    if (param->GetBasis() == Required && CountByParam.find(param) == CountByParam.end()) {
      ostringstream strm;
      strm << '"' << param->GetDiagnosticName() << "\": not given";
      if (!cb(strm.str())) {
        return false;
      }
    }
  }
  return true;
}

bool TCmd::TMeta::TParser::TRun::OnRecognition(const TParam *param, TCmd *cmd, const char *arg_text, const TMessageConsumer &cb) {
  assert(this);
  assert(param);
  assert(&cb);
  size_t &count = CountByParam.insert(make_pair(param, Zero)).first->second;
  ++count;
  if (count > 1 && param->GetRecurrence() == Once) {
    ostringstream strm;
    strm << '"' << param->GetDiagnosticName() << "\": given more than once";
    return cb(strm.str());
  }
  return param->OnRecognition(cmd, arg_text, cb);
}

mutex TCmd::TMeta::TParser::TRun::Mutex;

const int TCmd::TMeta::TParser::Offset = 1000;

const size_t TCmd::TMeta::TParser::Zero = 0;

const bool TCmd::TMeta::TParam::TArg::ValInfo<bool>::Default = true;

TCmd::~TCmd() {}

bool TCmd::CheckArgs(const TMeta::TMessageConsumer &) {
  return true;
}

void TCmd::Parse(int argc, char *argv[], const TMeta &meta) {
  assert(this);
  assert(&meta);
  if (meta.Parse(this, argc, argv)) {
    exit(EXIT_FAILURE);
  }
  if (IsHelp) {
    meta.WriteHelp(cout, this);
    exit(EXIT_SUCCESS);
  }
}
