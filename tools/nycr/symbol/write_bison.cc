/* <tools/nycr/symbol/write_bison.cc>

   Implements <tools/nycr/symbol/write_bison.h>.

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

#include <tools/nycr/symbol/write_bison.h>

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

static void CollectOperators(const TKind *kind, vector<const TOperator *> &ops);

static bool CompareOps(const TOperator *lhs, const TOperator *rhs);

static void WriteBisonCc(const char *root, const char *branch, const char *atom, const TLanguage *language);

static void WriteBisonH(const char *root, const char *branch, const char *atom, const TLanguage *language);

static void WriteRule(const TKind *kind, ostream &strm);

static void WriteTypeDecl(const TKind *kind, ostream &strm);

static void WriteUnionMember(const TKind *kind, ostream &strm);

void Tools::Nycr::Symbol::WriteBison(const char *root, const char *branch, const char *atom, const TLanguage *language) {
  WriteBisonH(root, branch, atom, language);
  WriteBisonCc(root, branch, atom, language);
}

static bool CompareOps(const TOperator *lhs, const TOperator *rhs) {
  assert(lhs);
  assert(rhs);
  size_t
      lhs_idx = lhs->GetPrecLevelIdx(),
      rhs_idx = rhs->GetPrecLevelIdx();
  return lhs_idx < rhs_idx || (lhs_idx == rhs_idx && lhs->GetAssoc() < rhs->GetAssoc());
}

static void CollectOperators(const TKind *kind, vector<const TOperator *> &ops) {
  assert(kind);
  assert(&ops);
  class TVisitor : public TKind::TVisitor {
    public:
    TVisitor(vector<const TOperator *> &ops) : Ops(ops) {}
    virtual void operator()(const TBase *) const {}
    virtual void operator()(const TLanguage *) const {}
    virtual void operator()(const TOperator *that) const { Ops.push_back(that); }
    virtual void operator()(const TKeyword *) const {}
    virtual void operator()(const TRule *) const {}
    private:
    vector<const TOperator *> &Ops;
  };
  kind->Accept(TVisitor(ops));
}

static void WriteBisonCc(const char *root, const char *branch, const char *atom, const TLanguage *language) {
  assert(root);
  assert(language);
  ofstream strm;
  CreateOutputFile(root, branch, atom, language, ".y", strm);
  strm
      << "%{" << endl
      << "#include <tools/nycr/error.h>" << endl
      << "#include <" << TPath(branch, atom,language) << ".cst.h>" << endl
      << TUsingNamespace(language)
      << "extern " << TType(language->GetName()) << " *" << TUpper(language->GetName()) << "_;" << endl
      << "#define YYLOC_DEFAULT " << TUnderscore(language) << "yylloc_default" << endl
      << "#define YYMAXDEPTH 1000000" << endl
      << "#define YYINITDEPTH 1000000" << endl
      << "%}" << endl << endl
      << "%code {" << endl
      << "  int " << TUnderscore(language) << "lex(YYSTYPE *, YYLTYPE *);" << endl
      << "  void " << TUnderscore(language) << "error(const YYLTYPE *, char const *);" << endl
      << "}" << endl << endl
      << "%defines" << endl
      << "%locations" << endl
      << "%pure-parser" << endl
      << "%name-prefix \"" << TUnderscore(language) <<"\"" << endl
      << "%glr-parser" << endl
      << "%error-verbose" << endl;
  if (language->GetExpectedSr()) {
    strm << "%expect " << *(language->GetExpectedSr()) << endl;
  }
  if (language->GetExpectedRr()) {
    strm << "%expect-rr " << *(language->GetExpectedRr()) << endl;
  }
  strm
    << endl
    << "%union {" << endl;
  ForEachKnownKind(language, bind(WriteUnionMember, _1, ref(strm)));
  strm
      << "}" << endl << endl;
  ForEachKnownKind(language, bind(WriteTypeDecl, _1, ref(strm)));
  vector<const TOperator *> ops;
  ForEachKnownKind(language, bind(CollectOperators, _1, ref(ops)));
  sort(ops.begin(), ops.end(), CompareOps);
  for (size_t i = 0; i < ops.size(); ++i) {
    if (!i || ops[i]->GetPrecLevelIdx() != ops[i - 1]->GetPrecLevelIdx() || ops[i]->GetAssoc() != ops[i - 1]->GetAssoc()) {
      const char *keyword;
      switch (ops[i]->GetAssoc()) {
        case TOperator::Left: {
          keyword = "left";
          break;
        }
        case TOperator::NonAssoc: {
          keyword = "nonassoc";
          break;
        }
        case TOperator::Right: {
          keyword = "right";
          break;
        }
        NO_DEFAULT_CASE;
      }
      strm << endl << '%' << keyword;
    }
    strm << ' ' << TUpper(ops[i]->GetName());
  }
  strm
      << endl << endl
      << "%start " << TUpper(language->GetName()) << endl << endl
      << "%%" << endl;
  ForEachKnownKind(language, bind(WriteRule, _1, ref(strm)));
  strm
      << endl
      << "%%" << endl << endl
      << "void " << TUnderscore(language) << "error(const YYLTYPE *loc, char const *msg) {" << endl
      << "  new ::Tools::Nycr::TError(loc->first_line, loc->first_column, loc->last_line, loc->last_column, msg);" << endl
      << '}' << endl;
}

static void WriteBisonH(const char *root, const char *branch, const char *atom, const TLanguage *language) {
  ofstream strm;
  CreateOutputFile(root, branch, atom, language, ".bison.h", strm);
  strm
      << "#pragma once" << endl;
}

static void WriteRule(const TKind *kind, ostream &strm) {
  assert(kind);
  assert(&strm);
  class TVisitor : public TKind::TVisitor {
    public:
    TVisitor(ostream &strm) : Strm(strm) {}
    virtual void operator()(const TBase *that) const {
      OnStart(that);
      const TBase::TSubKinds &sub_kinds = that->GetSubKinds();
      for (auto iter = sub_kinds.begin(); iter != sub_kinds.end(); ++iter) {
        Strm
            << "  " << ((iter == sub_kinds.begin()) ? ':' : '|') << ' ' << TUpper((*iter)->GetName()) << " {" << endl
            << "    $$ = $1;" << endl
            << "  }" << endl;
      }
      OnFinish();
    }
    virtual void operator()(const TLanguage *that) const { OnCompound(that, true); }
    virtual void operator()(const TOperator *) const {}
    virtual void operator()(const TKeyword *) const {}
    virtual void operator()(const TRule *that) const { OnCompound(that, false); }
    private:
    void OnStart(const TKind *that) const {
      Strm << endl << TUpper(that->GetName()) << endl;
    }
    void OnFinish() const {
      Strm << ';' << endl;
    }
    void OnCompound(const TCompound *that, bool is_language) const {
      OnStart(that);
      Strm << "  :";
      const TCompound::TMembersInOrder &members = that->GetMembersInOrder();
      if (!members.empty()) {
        for (auto iter = members.begin(); iter != members.end(); ++iter) {
          const TKind *kind = (*iter)->TryGetKind();
          if (kind) {
            Strm << ' ' << TUpper(kind->GetName());
          } else {
            Strm << " error";
          }
        }
      } else {
        Strm << " /* empty */";
      }
      const TOperator *oper = that->TryGetOperator();
      if (oper) {
        Strm << " %prec " << TUpper(oper->GetName());
      }
      Strm
          << " {" << endl
          << "    $$ = new " << TType(that->GetName()) << "(";
      bool sep = false;
      for (size_t i = 0; i < members.size(); ++i) {
        if (members[i]->TryGetKind()) {
          if (sep) {
            Strm << ", ";
          } else {
            sep = true;
          }
          Strm << "$" << (i + 1);
        }
      }
      Strm << ");" << endl;
      if (is_language) {
        Strm << "    " << TUpper(that->GetName()) << "_ = $$;" << endl;
      }
      Strm << "  }" << endl;
      OnFinish();
    }
    ostream &Strm;
  };
  kind->Accept(TVisitor(strm));
}

static void WriteTypeDecl(const TKind *kind, ostream &strm) {
  assert(kind);
  assert(&strm);
  class TVisitor : public TKind::TVisitor {
    public:
    TVisitor(ostream &strm) : Strm(strm) {}
    virtual void operator()(const TBase *that) const { OnNonAtom(that); }
    virtual void operator()(const TLanguage *that) const { OnNonAtom(that); }
    virtual void operator()(const TOperator *that) const { OnAtom(that); }
    virtual void operator()(const TKeyword *that) const { OnAtom(that); }
    virtual void operator()(const TRule *that) const { OnNonAtom(that); }
    private:
    void OnAtom(const TAtom *that) const {
      Strm << "%token <" << TLower(that->GetName()) << "> " << TUpper(that->GetName()) << ';' << endl;
    }
    void OnNonAtom(const TKind *that) const {
      Strm << "%type <" << TLower(that->GetName()) << "> " << TUpper(that->GetName()) << ';' << endl;
    }
    ostream &Strm;
  };
  kind->Accept(TVisitor(strm));
}

static void WriteUnionMember(const TKind *kind, ostream &strm) {
  assert(kind);
  assert(&strm);
  strm << "  " << TType(kind->GetName()) << " *" << TLower(kind->GetName()) << ';' << endl;
}
