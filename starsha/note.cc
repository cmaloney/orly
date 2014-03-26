/* <starsha/note.cc>

   Implements <starsha/note.h>.

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

#include <starsha/note.h>

#include <cctype>
#include <cstdio>
#include <cstring>

#include <base/thrower.h>

using namespace std;
using namespace Starsha;

void TNote::TEsc::Write(ostream &strm) const {
  assert(this);
  assert(&strm);
  strm << '"';
  for (char c: Text) {
    switch (c) {
      case '\n': {
        strm << "\\n";
        break;
      }
      case '\r': {
        strm << "\\r";
        break;
      }
      case '\t': {
        strm << "\\t";
        break;
      }
      case '"':
      case '\\': {
        strm << '\\' << c;
        break;
      }
      default: {
        if (isprint(c)) {
          strm << c;
        } else {
          char buf[10];
          sprintf(buf, "\\x%02x", static_cast<int>(c));
          strm << buf;
        }
      }
    }  // switch (c)
  }  // for (c...)
  strm << '"';
}

void TNote::TIndent::Write(ostream &strm) const {
  assert(this);
  assert(&strm);
  for (size_t i = 0; i < Depth; ++i) {
    strm << "  ";
  }
}

TNote::~TNote() {
  assert(this);
  SetParent(0);
  while (FirstChild) {
    delete FirstChild;
  }
}

void TNote::ForEachChild(const function<bool (TNote *)> &cb) const {
  assert(this);
  assert(&cb);
  for (auto child = FirstChild; child; child = child->NextSibling) {
    if (!cb(child)) {
      break;
    }
  }
}

void TNote::ForEachChild(const char *key, const function<bool (TNote *)> &cb) const {
  assert(this);
  assert(key);
  assert(&cb);
  for (auto child = FirstChild; child; child = child->NextSibling) {
    if (child->Key == key && !cb(child)) {
      break;
    }
  }
}

TNote *TNote::GetChild(const char *key) const {
  assert(this);
  assert(key);
  TNote *result = TryGetChild(key);
  if (!result) {
    THROW << "no note with key \"" << key << '"';
  }
  return result;
}

void TNote::SetParent(TNote *parent) {
  assert(this);
  if (Parent) {
    GetPrevConj() = NextSibling;
    GetNextConj() = PrevSibling;
    NextSibling = 0;
  }
  Parent = parent;
  if (parent) {
    PrevSibling = parent->LastChild;
    GetPrevConj() = this;
    GetNextConj() = this;
  } else {
    PrevSibling = 0;
  }
}

TNote *TNote::TryGetChild(const char *key) const {
  assert(this);
  assert(key);
  TNote *result = 0;
  for (TNote *child = FirstChild; child; child = child->NextSibling) {
    if (child->Key == key) {
      if (result) {
        THROW << "multiple notes with key \"" << key << '"';
      }
      result = child;
    }
  }
  return result;
}

void TNote::Write(ostream &strm) const {
  assert(this);
  assert(&strm);
  TIndent indent(this);
  strm << indent << TEsc(Key);
  if (!Value.empty()) {
    strm << " = " << TEsc(Value);
  }
  if (FirstChild) {
    strm << " {\n";
    for (auto child = FirstChild; child; child = child->NextSibling) {
      child->Write(strm);
    }
    strm << indent << "}\n";
  } else {
    strm << ";\n";
  }
}

TNote *TNote::Read(istream &strm) {
  TNote *note = TryRead(strm);
  if (!note) {
    THROW << "expected a note but didn't get one";
  }
  return note;
}

TNote *TNote::TryRead(istream &strm) {
  assert(&strm);
  string key, value;
  TNote *note;
  // TODO: Copied parsing logic in TryReadAppend
  if (TryReadString(strm, key)) {
    if (TryReadPunc(strm, "=") >= 0) {
      ReadString(strm, value);
    }
    note = new TNote(key, value);
    try {
      if (ReadPunc(strm, ";{") == '{') {
        for (;;) {
          TNote *child = TryRead(strm);
          if (!child) {
            break;
          }
          child->SetParent(note);
        }
        ReadPunc(strm, "}");
      }
    } catch (...) {
      delete note;
      throw;
    }
  } else {
    note = 0;
  }
  return note;
}

//NOTE: This doesn't start at root. This is much easier to write at this juncture, so oh well.
//NOTE: When using this, call repeatedly until it returns false.
bool TNote::TryReadAppend(istream &strm, TNote *root) {
  assert(&strm);
  assert(root);

  string key, value;
  // TODO: Copied parsing logic from TryRead
  if (TryReadString(strm, key)) {
    TNote *curpos = root->GetOrNewChild(key.c_str());
    if (TryReadPunc(strm, "=") >= 0) {
      ReadString(strm, value);
      curpos->Value = value;
    }
    if (ReadPunc(strm, ";{") == '{') {
      while(TryReadAppend(strm, curpos));
      ReadPunc(strm, "}");
    }
    return true;
  } else {
    return false;

  }
}


int TNote::ReadPunc(istream &strm, const char *punc) {
  int c = TryReadPunc(strm, punc);
  if (!c) {
    THROW << "expected one of \"" << punc << "\" but didn't get it";
  }
  return c;
}

void TNote::ReadString(istream &strm, string &out) {
  if (!TryReadString(strm, out)) {
    THROW << "expected a string but didn't get one";
  }
}

int TNote::TryReadPunc(istream &strm, const char *punc) {
  assert(&strm);
  assert(punc);
  int c;
  for (;;) {
    c = strm.peek();
    if (!isspace(c)) {
      break;
    }
    strm.ignore();
  }
  if (c >= 0) {
    if (strchr(punc, c)) {
      strm.ignore();
    } else {
      c = -1;
    }
  }
  return c;
}

bool TNote::TryReadString(istream &strm, string &out) {
  assert(&strm);
  assert(&out);
  out.clear();
  if (TryReadPunc(strm, "\"") == '"') {
    for (;;) {
      int c = strm.peek();
      if (c == '"') {
        strm.ignore();
        break;
      }
      if (c == '\\') {
        strm.ignore();
        c = strm.peek();
        switch (c) {
          case 'n': {
            c = '\n';
            strm.ignore();
            break;
          }
          case 'r': {
            c = '\r';
            strm.ignore();
            break;
          }
          case 't': {
            c = '\t';
            strm.ignore();
            break;
          }
          case '"':
          case '\\': {
            strm.ignore();
            break;
          }
          case 'x': {
            strm.ignore();
            char buf[3];
            strm.read(buf, 2);
            buf[2] = 0;
            char *end;
            c = strtoul(buf, &end, 16);
            if (end != buf + 2) {
              THROW << "illegal hex escape \"" << buf << "\" in string";
            }
            break;
          }
          default: {
            THROW << "illegal escape character " << c << " in string";
          }
        }
        out += static_cast<char>(c);
        continue;
      }
      if (isprint(c)) {
        out += static_cast<char>(c);
        strm.ignore();
        continue;
      }
      THROW << "illegal character " << c << " in string";
    }
  } else {
    int c = strm.peek();
    if (!isalnum(c) && c != '_') {
      return false;
    }
    do {
      strm.ignore();
      out += static_cast<char>(c);
      c = strm.peek();
    } while (isalnum(c) || c == '_');
  }
  return true;
}
