/* <tools/nycr/symbol/write_xml.cc>

   Implements <tools/nycr/symbol/write_xml.h>.

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

#include <tools/nycr/symbol/write_xml.h>

#include <cassert>
#include <fstream>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>

#include <base/no_default_case.h>
#include <tools/nycr/escape.h>
#include <tools/nycr/indent.h>
#include <tools/nycr/symbol/anonymous_member.h>
#include <tools/nycr/symbol/atom.h>
#include <tools/nycr/symbol/base.h>
#include <tools/nycr/symbol/for_each_known_kind.h>
#include <tools/nycr/symbol/keyword.h>
#include <tools/nycr/symbol/named_member.h>
#include <tools/nycr/symbol/operator.h>
#include <tools/nycr/symbol/output_file.h>
#include <tools/nycr/symbol/prec_level.h>
#include <tools/nycr/symbol/rule.h>
#include <tools/nycr/symbol/write_nycr.h>

using namespace std;
using namespace placeholders;
using namespace Tools::Nycr;
using namespace Tools::Nycr::Symbol;

static unsigned int depth = 0; // depth of current tag

static void WriteElem(const TKind *kind, ostream &strm);

const char *TXmlTag::TXmlTagStr[] = {
  "anonymous_member",
  "association",
  "base",
  "document",
  "error_member",
  "expected_shift_reduce",
  "expected_reduce_reduce",
  "keyword",
  "kind",
  "language",
  "member",
  "name",
  "named_member",
  "namespace",
  "namespaces",
  "operator",
  "parent",
  "part",
  "pattern",
  "precedence",
  "priority",
  "rule"
};

void TXmlTag::Write(ostream &strm) const {
  assert(this);
  if (!IsOpen) --depth;
  if (HasIndent) strm << TIndent(depth);
  strm << '<';
  if (!IsOpen) strm << '/';
  strm << TXmlTagStr[Type] << '>';
  if (HasEndl) strm << endl;
  if (IsOpen) ++depth;
}

void Tools::Nycr::Symbol::WriteXml(const char *root, const char *branch, const char *atom, const TLanguage *language) {
  assert(language);
  ofstream strm;
  CreateOutputFile(root, branch, atom, language, ".xml", strm, XmlStyle);
  strm << TXmlTag(TXmlTag::Document, Open);
  ForEachKnownKind(language, bind(WriteElem, _1, ref(strm)));
  strm << TXmlTag(TXmlTag::Document, Close);
}

static void WriteElem(const TKind *kind, ostream &strm) {
  assert(kind);
  class TVisitor : public TKind::TVisitor {
  public:
    TVisitor(ostream &strm) : Strm(strm) {}
    virtual void operator()(const TBase *that) const {
      assert(this);
      Strm << TXmlTag(TXmlTag::Base, Open);
      WriteLhs(that);
      Strm << TXmlTag(TXmlTag::Base, Close);
    }
    virtual void operator()(const TLanguage *that) const {
      assert(this);
      Strm << TXmlTag(TXmlTag::Language, Open);
      WriteLhs(that);
      WriteRhs(that);
      Strm << TXmlTag(TXmlTag::Namespaces, Open);
      const TLanguage::TNamespaces &namespaces = that->GetNamespaces();
      for (auto iter = namespaces.begin();
      iter != namespaces.end(); ++iter) {
      Strm << TXmlTag(TXmlTag::Namespace, Open)
           << TXml(*iter)
           << TXmlTag(TXmlTag::Namespace, Close);
      }
      Strm << TXmlTag(TXmlTag::Namespaces, Close) << endl;
      if (that->GetExpectedSr()) {
        Strm << TXmlTag(TXmlTag::ExpectedShiftReduce, Open, Indent, NoEndl)
             << *(that->GetExpectedSr())
             << TXmlTag(TXmlTag::ExpectedShiftReduce, Close, NoIndent, Endl);
      }
      if (that->GetExpectedRr()) {
        Strm << TXmlTag(TXmlTag::ExpectedReduceReduce, Open, Indent, NoEndl)
             << *(that->GetExpectedRr())
             << TXmlTag(TXmlTag::ExpectedReduceReduce, Close, NoIndent, Endl);
      }
      Strm << TXmlTag(TXmlTag::Language, Close);
    }
    virtual void operator()(const TOperator *that) const {
      assert(this);
      Strm << TXmlTag(TXmlTag::Operator, Open);
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

      Strm << TXmlTag(TXmlTag::Precedence, Open, Indent, NoEndl)
	   << TXml(that->GetPrecLevel()->GetName())
	   << TXmlTag(TXmlTag::Precedence, Close, NoIndent, Endl)
           << TXmlTag(TXmlTag::Association, Open, Indent, NoEndl)
	   << assoc
	   << TXmlTag(TXmlTag::Association, Close, NoIndent, Endl)
	   << TXmlTag(TXmlTag::Operator, Close);
    }
    virtual void operator()(const TKeyword *that) const {
      assert(this);
      Strm << TXmlTag(TXmlTag::Keyword, Open);
      WriteLhs(that);
      WritePattern(that);
      Strm << TXmlTag(TXmlTag::Keyword, Close);
    }
    virtual void operator()(const TRule *that) const {
      assert(this);
      Strm << TXmlTag(TXmlTag::Rule, Open);
      WriteLhs(that);
      WriteRhs(that);
      Strm << TXmlTag(TXmlTag::Rule, Close);
    }
  private:
    void WriteLhs(const TKind *that) const {
      Strm << TXml(that->GetName());
      const TBase *base = that->GetBase();
      if (base) {
	Strm << TXmlTag(TXmlTag::Parent, Open)
	     << TXml(base->GetName())
	     << TXmlTag(TXmlTag::Parent, Close);
      }
    }
    void WritePattern(const TAtom *that) const {
      assert(this);
      assert(that);
      Strm << TXmlTag(TXmlTag::Pattern, Open, Indent, NoEndl)
	   << TEscape(that->GetPattern(), TEscape::XmlStyle)
	   << TXmlTag(TXmlTag::Pattern, Close, NoIndent, Endl);
      if (that->GetPri()) {
        Strm << TXmlTag(TXmlTag::Priority, Open, Indent, NoEndl)
             << *(that->GetPri())
             << TXmlTag(TXmlTag::Priority, Close, NoIndent, Endl);
      }
    }
    void WriteRhs(const TCompound *that) const {
      assert(this);
      assert(that);
      const TCompound::TMembersInOrder &members_in_order = that->GetMembersInOrder();
      if (!members_in_order.empty()) {
	Strm << TXmlTag(TXmlTag::Member, Open);
	for (auto iter = members_in_order.begin();
	     iter != members_in_order.end(); ++iter) {
	  (*iter)->WriteXml(Strm);
	}
	Strm << TXmlTag(TXmlTag::Member, Close);
      } else {
	Strm << TXmlTag(TXmlTag::Member, Open, Indent, NoEndl)
	     << "empty"
	     << TXmlTag(TXmlTag::Member, Close, NoIndent, Endl);
      }
      const TOperator *oper = that->TryGetOperator();
      if (oper) {
        Strm << TXmlTag(TXmlTag::Precedence, Open, Indent, NoEndl)
             << TXml(oper->GetName())
             << TXmlTag(TXmlTag::Precedence, Close, NoIndent, Endl);
      }
    }
    ostream &Strm;
  };
  kind->Accept(TVisitor(strm));
}