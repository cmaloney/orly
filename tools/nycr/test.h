/* <tools/nycr/test.h>

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
#include <cctype>
#include <stdexcept>

#include <base/no_copy_semantics.h>

namespace Tools {
  namespace Nycr {
    namespace Test {
      class TNode {
        NO_COPY_SEMANTICS(TNode);
      public:

        TNode(const std::string &name)
          : Name(name), FirstChild(0), LastChild(0), Parent(0), PrevSibling(0), NextSibling(0) {}

        ~TNode() {
          assert(this);
          while (FirstChild) {
            delete FirstChild;
            FirstChild = NextSibling;
          }
          RemoveFromParent();
        }

        const std::string &GetKind() const {
          assert(this);
          return Kind;
        }

        const std::string &GetName() const {
          assert(this);
          return Name;
        }

        void InsertIntoParent(TNode *parent, const std::string &kind) {
          assert(this);
          assert(parent);
          assert(parent != this);
          assert(&kind);
          RemoveFromParent();
          Kind = kind;
          Parent = parent;
          PrevSibling = parent->LastChild;
          NextSibling = 0;
          GetPrevLink() = this;
          GetNextLink() = this;
        }

        void RemoveFromParent() {
          assert(this);
          if (Parent) {
            GetPrevLink() = NextSibling;
            GetNextLink() = PrevSibling;
            Parent = 0;
            PrevSibling = 0;
            NextSibling = 0;
            Name.clear();
          }
        }

        TNode *TryGetFirstChild() const {
          assert(this);
          return FirstChild;
        }

        TNode *TryGetLastChild() const {
          assert(this);
          return LastChild;
        }

        TNode *TryGetParent() const {
          assert(this);
          return Parent;
        }

        TNode *TryGetPrevSibling() const {
          assert(this);
          return PrevSibling;
        }

        TNode *TryGetNextSibling() const {
          assert(this);
          return NextSibling;
        }

      private:

        TNode *&GetPrevLink() const {
          assert(this);
          assert(Parent);
          return PrevSibling ? PrevSibling->NextSibling : Parent->FirstChild;
        }

        TNode *&GetNextLink() const {
          assert(this);
          assert(Parent);
          return NextSibling ? NextSibling->PrevSibling : Parent->LastChild;
        }

        std::string Kind, Name;

        TNode *FirstChild, *LastChild, *Parent, *PrevSibling, *NextSibling;

      };  // TNode;

      static void SkipWhitespace(const char *&str) {
        assert(&str);
        while (!isgraph(*str)) ++str;
      }

      static bool TryMatch(const char *&str, char expected) {
        assert(&str);
        SkipWhitespace(str);
        bool result = *str == expected;
        if (result) ++str;
        return result;
      }

      static void Match(const char *&str, char expected) {
        if (!TryMatch(str, expected)) {
          throw std::runtime_error("syntax error");
        }
      }

      static void ParseString(const char *&str, std::string &out) {
        assert(&str);
        assert(&out);
        SkipWhitespace(str);
        const char *cur = str;
        while (isgraph(*cur) && !ispunct(*cur)) ++cur;
        out.assign(str, cur);
        str = cur;
      }

      static inline TNode *ParseNode(const char *&str) {
        std::string name, kind;
        ParseString(str, name);
        TNode *node = new TNode(name);
        try {
          if (TryMatch(str, '{')) {
            while (!TryMatch(str, '}')) {
              ParseString(str, kind);
              Match(str, '-');
              Match(str, '>');
              ParseNode(str)->InsertIntoParent(node, kind);
            }
          } else {
            Match(str, ';');
          }
        } catch (...) {
          delete node;
          throw;
        }
        return node;
      }
    }
  }
}