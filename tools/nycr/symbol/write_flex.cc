/* <tools/nycr/symbol/write_flex.cc>

   Implements <tools/nycr/symbol/write_flex.h>.

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

#include <tools/nycr/symbol/write_flex.h>

#include <algorithm>
#include <cassert>
#include <fstream>
#include <functional>
#include <vector>

#include <base/no_default_case.h>
#include <tools/nycr/symbol/base.h>
#include <tools/nycr/symbol/for_each_known_kind.h>
#include <tools/nycr/symbol/keyword.h>
#include <tools/nycr/symbol/operator.h>
#include <tools/nycr/symbol/output_file.h>
#include <tools/nycr/symbol/rule.h>
#include <tools/nycr/symbol/write_nycr.h>

using namespace std;
using namespace placeholders;
using namespace Tools::Nycr::Symbol;

static void CollectAtoms(const TKind *kind, vector<const TAtom *> &atoms);

static bool CompareAtoms(const TAtom *lhs, const TAtom *rhs);

static void WriteFlexCc(const char *root, const char *branch, const char *atom, const TLanguage *language);

static void WriteFlexH(const char *root, const char *branch, const char *atom, const TLanguage *language);

void Tools::Nycr::Symbol::WriteFlex(const char *root, const char *branch, const char *atom, const TLanguage *language) {
  WriteFlexH(root, branch, atom, language);
  WriteFlexCc(root, branch, atom, language);
}

static void CollectAtoms(const TKind *kind, vector<const TAtom *> &atoms) {
  assert(kind);
  assert(&atoms);
  class TVisitor : public TKind::TVisitor {
    public:
    TVisitor(vector<const TAtom *> &atoms) : Atoms(atoms) {}
    virtual void operator()(const TBase *) const {}
    virtual void operator()(const TLanguage *) const {}
    virtual void operator()(const TOperator *that) const { OnAtom(that); }
    virtual void operator()(const TKeyword *that) const { OnAtom(that); }
    virtual void operator()(const TRule *) const {}
    private:
    void OnAtom(const TAtom *that) const {
      if (!that->GetPattern().empty()) {
        Atoms.push_back(that);
      }
    }
    vector<const TAtom *> &Atoms;
  };
  kind->Accept(TVisitor(atoms));
}

static bool CompareAtoms(const TAtom *lhs, const TAtom *rhs) {
  assert(lhs);
  assert(rhs);
  int
      lhs_pri = lhs->GetPriAsInt(),
      rhs_pri = rhs->GetPriAsInt();
  return lhs_pri < rhs_pri || (lhs_pri == rhs_pri && lhs->GetPattern() < rhs->GetPattern());
}

static void WriteFlexCc(const char *root, const char *branch, const char *atom, const TLanguage *language) {
  assert(root);
  assert(language);
  ofstream strm;
  CreateOutputFile(root, branch, atom, language, ".l", strm);
  strm
      << "%option noyywrap nodefault bison-bridge bison-locations yylineno" << endl
      << "%option prefix=\"" << TUnderscore(language) << "\"" << endl
      << "%x COMMENT" << endl << endl
      << "%{" << endl
      << "#include <cstdlib>" << endl
      << "#include <cstring>" << endl
      << "#include <" << TPath(branch, atom, language) << ".cst.h>" << endl
      << TUsingNamespace(language)
      << "#include <" << TPath(branch, atom, language) << ".bison.hh>" << endl
      << "void " << TUnderscore(language) << "error(const YYLTYPE *, char const *);" << endl
      << "void " << TUnderscore(language) << "yylloc_default(YYLTYPE &lhs, YYLTYPE *rhs, int n) {" << endl
      << "  if (n) {" << endl
      << "    lhs.first_line   = rhs[1].first_line;" << endl
      << "    lhs.first_column = rhs[1].first_column;" << endl
      << "    lhs.last_line    = rhs[n].last_line;" << endl
      << "    lhs.last_column  = rhs[n].last_column;" << endl
      << "  } else {" << endl
      << "    lhs.first_line   = rhs[0].first_line;" << endl
      << "    lhs.first_column = rhs[0].first_column;" << endl
      << "    lhs.last_line    = rhs[0].last_line;" << endl
      << "    lhs.last_column  = rhs[0].last_column;" << endl
      << "  }" << endl
      << "}" << endl
      << "int " << TUnderscore(language) << "column = 1;" << endl
      << "int " << TUnderscore(language) << "depth;" << endl
      << "#define YY_USER_ACTION \\" << endl
      << "  yylloc->first_line = " << TUnderscore(language) << "lineno; \\" << endl
      << "  yylloc->first_column = " << TUnderscore(language) << "column; \\" << endl
      << "  yylloc->last_line = " << TUnderscore(language) << "lineno; \\" << endl
      << "  yylloc->last_column = " << TUnderscore(language) << "column + " << TUnderscore(language) << "leng; \\" << endl
      << "  " << TUnderscore(language) << "column += " << TUnderscore(language) << "leng;" << endl
      << endl
      << endl
      << "YY_BUFFER_STATE " << TUnderscore(language) << "nycr_str_input_buffer;" << endl
      << endl
      << "extern void " << TUnderscore(language) << "NycrPrepStr(const char *str) {" << endl
      << "  " << TUnderscore(language) << "nycr_str_input_buffer = " << TUnderscore(language) << "_scan_string(str);" << endl
      << "}" << endl
      << endl
      << "extern void " << TUnderscore(language) << "NycrCleanStr() {" << endl
      << "  " << TUnderscore(language) << "_delete_buffer(" << TUnderscore(language) << "nycr_str_input_buffer);" << endl
      << "}" << endl
      << endl
      << "%}" << endl << endl
      << "SINGLE_QUOTE \\'" << endl
      << "DOUBLE_QUOTE \\\"" << endl
      << "BACKSLASH \\\\" << endl
      << "RAW_STRING_MARK (r|R)" << endl
      << "ESCAPE_SEQUENCE {BACKSLASH}({SINGLE_QUOTE}|{DOUBLE_QUOTE}|{BACKSLASH}|n|r|t|x[[:xdigit:]]{2})" << endl
      << "SINGLE_QUOTED_STRING {SINGLE_QUOTE}([^'\\n\\t\\\\]|{ESCAPE_SEQUENCE})*{SINGLE_QUOTE}" << endl
      << "DOUBLE_QUOTED_STRING {DOUBLE_QUOTE}([^\"\\n\\t\\\\]|{ESCAPE_SEQUENCE})*{DOUBLE_QUOTE}" << endl
      << "SINGLE_QUOTED_RAW_STRING {RAW_STRING_MARK}{SINGLE_QUOTE}[^'\\n\\t]*{SINGLE_QUOTE}" << endl
      << "DOUBLE_QUOTED_RAW_STRING {RAW_STRING_MARK}{DOUBLE_QUOTE}[^\"\\n\\t]*{DOUBLE_QUOTE}" << endl << endl
      << "%%" << endl;
  vector<const TAtom *> atoms;
  ForEachKnownKind(language, bind(CollectAtoms, _1, ref(atoms)));
  sort(atoms.begin(), atoms.end(), CompareAtoms);
  for (auto iter = atoms.begin(); iter != atoms.end(); ++iter) {
    const TAtom *atom = *iter;
    strm
        << endl
        << atom->GetPattern() << " {" << endl
        << "  yylval->" << TLower(atom->GetName()) << " = new " << TType(atom->GetName())
        << "(yylloc->first_line, yylloc->first_column, yylloc->last_line, yylloc->last_column, " << TUnderscore(language) << "text, " << TUnderscore(language) << "leng);" << endl
        << "  return " << TUpper(atom->GetName()) << ';' << endl
        << '}' << endl;
  }
  strm
      << endl
      << "[ \\t]+ {}" << endl << endl
      << "\\n+ {" << endl
      << "  " << TUnderscore(language) << "column = 1;" << endl
      << '}' << endl << endl
      << "\"/*\" {" << endl
      << "  BEGIN(COMMENT);" << endl
      << "  " << TUnderscore(language) << "depth = 1;" << endl
      << '}' << endl << endl
      << "<COMMENT>\"/*\" {" << endl
      << "  ++" << TUnderscore(language) << "depth;" << endl
      << '}' << endl << endl
      << "<COMMENT>\"*/\" {" << endl
      << "  --" << TUnderscore(language) << "depth;" << endl
      << "  if (" << TUnderscore(language) << "depth <= 0) {" << endl
      << "    BEGIN(INITIAL);" << endl
      << "  }" << endl
      << '}' << endl << endl
      << "<COMMENT>\\n {" << endl
      << "  " << TUnderscore(language) << "column = 1;" << endl
      << '}' << endl << endl
      << "<COMMENT>. {}" << endl << endl
      << "<COMMENT><<EOF>> {" << endl
      << "  " << TUnderscore(language) << "error(yylloc, \"EOF in comment\");" << endl
      << "  " << TUnderscore(language) << "depth = 0;" << endl
      << "  BEGIN(INITIAL);" << endl
      << '}' << endl << endl
      << ". {" << endl
      << "  " << TUnderscore(language) << "error(yylloc, \"illegal char\");" << endl
      << '}' << endl;
}

static void WriteFlexH(const char *root, const char *branch, const char *atom, const TLanguage *language) {
  ofstream strm;
  CreateOutputFile(root, branch, atom, language, ".flex.h", strm);
  strm
      << "#pragma once" << endl;
}