/* <tools/nycr/symbol/language.h>

   The production at which a language starts.

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
#include <unordered_set>
#include <vector>

#include <base/no_copy_semantics.h>
#include <base/opt.h>
#include <tools/nycr/symbol/compound.h>

namespace Tools {

  namespace Nycr {

    namespace Symbol {

      /* TODO */
      class TLanguage
          : public TCompound {
        public:

        /* TODO */
        typedef std::unordered_set<TLanguage *> TLanguages;

        /* TODO */
        typedef std::vector<TName> TNamespaces;

        /* TODO */
        TLanguage(
            const TName &name, TAnyBase *base, const TNamespaces &namespaces,
            const Base::TOpt<int> &expected_sr, const Base::TOpt<int> &expected_rr)
            : TCompound(name, base), Namespaces(namespaces), ExpectedSr(expected_sr), ExpectedRr(expected_rr) {
          Languages.insert(this);
        }

        /* TODO */
        virtual ~TLanguage();

        /* TODO */
        virtual void Accept(const TVisitor &visitor) const;

        /* TODO */
        const Base::TOpt<int> &GetExpectedRr() const {
          assert(this);
          return ExpectedRr;
        }

        /* TODO */
        const Base::TOpt<int> &GetExpectedSr() const {
          assert(this);
          return ExpectedSr;
        }

        /* TODO */
        const TNamespaces &GetNamespaces() const {
          assert(this);
          return Namespaces;
        }

        /* TODO */
        static const TLanguages &GetLanguages() {
          return Languages;
        }

        private:

        /* TODO */
        TNamespaces Namespaces;

        /* TODO */
        Base::TOpt<int> ExpectedSr, ExpectedRr;

        /* TODO */
        static TLanguages Languages;

      };  // TLanguage

    }  // Symbol

  }  // Nycr

}  // Tools
