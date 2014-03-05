/* <tools/nycr/symbol/write_xml.h>

   Write the xml document for the language represented by a set of nycr symbols.

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

#pragma once

#include <tools/nycr/symbol/language.h>

namespace Tools {

  namespace Nycr {

    namespace Symbol {

      enum { Close, Open };
      enum { NoIndent, Indent };
      enum { NoEndl, Endl };

      /* TODO */
      class TXmlTag {
      public:

	/* TODO */
	enum TXmlTagType {
	  AnonymousMember,
	  Association,
	  Base,
	  Document,
	  ErrorMember,
          ExpectedShiftReduce,
          ExpectedReduceReduce,
	  Keyword,
	  Kind,
	  Language,
	  Member,
	  Name,
	  NamedMember,
	  Namespace,
	  Namespaces,
	  Operator,
	  Parent,
	  Part,
	  Pattern,
	  Precedence,
          Priority,
	  Rule
	};

	/* TODO */
        static const char *TXmlTagStr[];

	/* TODO */
      TXmlTag(TXmlTagType type, bool is_open, bool has_indent = Indent, bool has_endl = Endl)
	: Type(type), IsOpen(is_open), HasIndent(has_indent), HasEndl(has_endl) {}

	/* TODO */
	void Write(std::ostream &strm) const;

      private:

	/* TODO */
	const TXmlTagType Type;
	const bool IsOpen, HasIndent, HasEndl;

      }; // TXmlTag

      /* TODO */
      void WriteXml(const char *root, const char *branch, const char *atom, const TLanguage *language);

      /* TODO */
      inline std::ostream &operator<<(std::ostream &strm, const TXmlTag &that) {
        assert(&that);
        that.Write(strm);
        return strm;
      }

    } // Symbol

  } // Nycr

} // Tools
