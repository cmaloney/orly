/* <tools/nycr/symbol/output_file.h>

   Handles the creation of output files.

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

#include <cassert>
#include <fstream>

#include <tools/nycr/symbol/language.h>

namespace Tools {

  namespace Nycr {

    namespace Symbol {

      /* A stream insertion helper for writing language paths. */
      class TPath {
        public:

        /* Do-little. */
        TPath(const char *branch, const char *atom, const TLanguage *language)
            : Atom(atom), Branch(branch), Language(language)  {
          assert(language);
        }

        /* Write the path, like 'here/is/my/language'. */
        void Write(std::ostream &strm) const;

        private:

        //The atom which the includes will live
        const char *Atom;

        //The JHM File Branch in which the includes will live
        const char *Branch;

        /* The language on which we operate. */
        const TLanguage *Language;

      };  // TPath

      /* A stream insertion helper for writing language paths. */
      class TScope {
        public:

        /* Do-little. */
        TScope(const TLanguage *language)
            : Language(language) {
          assert(language);
        }

        /* Write the path, like 'here/is/my/language'. */
        void Write(std::ostream &strm) const;

        private:

        /* The language on which we operate. */
        const TLanguage *Language;

      };  // TScope

      /* A stream insertion helper for writing 'using namespace' clauses. */
      class TUsingNamespace {
        public:

        /* Do-little. */
        TUsingNamespace(const TLanguage *language)
            : Language(language) {
          assert(language);
        }

        /* Write the clause. */
        void Write(std::ostream &strm) const;

        private:

        /* The language on which we operate. */
        const TLanguage *Language;

      };  // TUsingNamespace

      /* A stream insertion helper for writing c prefixes based on language name. */
      class TUnderscore {
        public:

        /* Do-little. */
        TUnderscore(const TLanguage *language)
            : Language(language) {
          assert(language);
        }

        /* Write the clause. */
        void Write(std::ostream &strm) const;

        private:

        /* The language on which we operate. */
        const TLanguage *Language;

      };  // TUnderscore


    /* TODO */
    enum TCommentStyle { CStyle, XmlStyle };

    /* Creates an output file and writes its header comment. Returns an open stream to the file. */
    void CreateOutputFile(const char *root, const char *branch, const char *atom, const TLanguage *language, const char *ext, std::ofstream &strm, TCommentStyle comment_style = CStyle);

/* Standard inserter for Tools::Nycr::Symbol::TPath. */
inline std::ostream &operator<<(std::ostream &strm, const Tools::Nycr::Symbol::TPath &that) {
  assert(&that);
  that.Write(strm);
  return strm;
}

/* Standard inserter for Tools::Nycr::Symbol::TScope. */
inline std::ostream &operator<<(std::ostream &strm, const Tools::Nycr::Symbol::TScope &that) {
  assert(&that);
  that.Write(strm);
  return strm;
}

/* Standard inserter for Tools::Nycr::Symbol::TUsingNamespace. */
inline std::ostream &operator<<(std::ostream &strm, const Tools::Nycr::Symbol::TUsingNamespace &that) {
  assert(&that);
  that.Write(strm);
  return strm;
}


/* Standard inserter for Tools::Nycr::Symbol::TUnderscore. */
inline std::ostream &operator<<(std::ostream &strm, const Tools::Nycr::Symbol::TUnderscore &that) {
  assert(&that);
  that.Write(strm);
  return strm;
}

    }  // Symbol

  }  // Nycr

}  // Tools