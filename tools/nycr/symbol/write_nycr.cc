/* <tools/nycr/symbol/write_nycr.cc>

   Implements <tools/nycr/symbol/write_nycr.h>.

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

#include <tools/nycr/symbol/write_nycr.h>

#include <cassert>
#include <fstream>
#include <functional>

#include <base/no_default_case.h>
#include <tools/nycr/escape.h>
#include <tools/nycr/symbol/base.h>
#include <tools/nycr/symbol/for_each_known_kind.h>
#include <tools/nycr/symbol/keyword.h>
#include <tools/nycr/symbol/operator.h>
#include <tools/nycr/symbol/output_file.h>
#include <tools/nycr/symbol/prec_level.h>
#include <tools/nycr/symbol/rule.h>

using namespace std;
using namespace placeholders;
using namespace Tools::Nycr::Symbol;

void Tools::Nycr::Symbol::WriteNycr(const char *root, const char *branch, const char *atom, const TLanguage *language) {
  assert(root);
  assert(language);
  ofstream strm;
  CreateOutputFile(root, branch, atom, language, ".nycr", strm);
  ForEachKnownKind(language, bind(WriteNycrDecl, _1, ref(strm)));
}

void Tools::Nycr::Symbol::WriteNycrDecl(const TKind *kind, ostream &strm) {
  assert(kind);
  assert(&strm);
  class TVisitor : public TKind::TVisitor {
    public:
    TVisitor(ostream &strm) : Strm(strm) {}
    virtual void operator()(const TBase *that) const {
      WriteLhs(that);
    }
    virtual void operator()(const TLanguage *that) const {
      WriteLhs(that);
      WriteRhs(that);
      Strm << " <";
      const TLanguage::TNamespaces &namespaces = that->GetNamespaces();
      for (auto iter = namespaces.begin();
           iter != namespaces.end(); ++iter) {
        if (iter != namespaces.begin()) {
          Strm << '/';
        }
        Strm << TLower(*iter);
      }
      Strm << '>';
      if (that->GetExpectedSr()) {
        Strm << " sr " << *(that->GetExpectedSr());
      }
      if (that->GetExpectedRr()) {
        Strm << " rr " << *(that->GetExpectedRr());
      }
    }
    virtual void operator()(const TOperator *that) const {
      WriteLhs(that);
      WritePattern(that);
      const char *assoc;
      switch (that->GetAssoc()) {
        case TOperator::Left: {
          assoc = "left";
          break;
        }
        case TOperator::NonAssoc: {
          assoc = "nonassoc";
          break;
        }
        case TOperator::Right: {
          assoc = "right";
          break;
        }
        NO_DEFAULT_CASE;
      }
      Strm << ' ' << TLower(that->GetPrecLevel()->GetName()) << ' ' << assoc;
    }
    virtual void operator()(const TKeyword *that) const {
      WriteLhs(that);
      WritePattern(that);
    }
    virtual void operator()(const TRule *that) const {
      WriteLhs(that);
      WriteRhs(that);
    }
    private:
    void WriteLhs(const TKind *that) const {
      Strm << TLower(that->GetName());
      const TBase *base = that->GetBase();
      if (base) {
        Strm << " : " << TLower(base->GetName());
      }
    }
    void WritePattern(const TAtom *that) const {
      Strm << " = " << TEscape(that->GetPattern());
      if (that->GetPri()) {
        Strm << " pri " << *(that->GetPri());
      }
    }
    void WriteRhs(const TCompound *that) const {
      assert(this);
      assert(that);
      Strm << " -> ";
      const TCompound::TMembersInOrder &members_in_order = that->GetMembersInOrder();
      if (!members_in_order.empty()) {
        for (auto iter = members_in_order.begin();
             iter != members_in_order.end(); ++iter) {
          if (iter != members_in_order.begin()) {
            Strm << ' ';
          }
          (*iter)->WriteRhs(Strm);
        }
      } else {
        Strm << "empty";
      }
      const TOperator *oper = that->TryGetOperator();
      if (oper) {
        Strm << " prec " << TLower(oper->GetName());
      }
    }
    ostream &Strm;
  };
  kind->Accept(TVisitor(strm));
  strm << ';' << endl;
}
