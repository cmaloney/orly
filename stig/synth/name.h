/* <stig/synth/name.h>

   A wrapper around a name used in the CST.

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

#include <algorithm>
#include <cassert>
#include <string>

#include <base/no_copy_semantics.h>

#include <inv_con/unordered_multimap.h>
#include <stig/stig.package.cst.h>
#include <stig/pos_range.h>

namespace Stig {

  namespace Synth {

    /* Constructs a new TName which the caller takes ownership of*/
    Package::Syntax::TName *BuildNode(const char *name);

    /* A wrapper around a name used in the CST. */
    class TName {
      public:

      /* A convenience typedef for the CST TName node. */
      typedef Package::Syntax::TName TNode;

      /* Default constructor */
      TName() {
        Lexeme = &DefaultLexeme;
        Text = &DefaultLexeme.GetText();
        // PosRange = &DefaultLexeme.GetPosRange();
      }

      /* Construct pointing to the given node.
         If we don't get a node, assume the 'null' state. */
      TName(const TNode *node = nullptr) {
        SetLexeme(node);
      }

      /* Construct a name object from an arbitrary string and position-range. */
      TName(const std::string &name, const TPosRange &pos_range)
          : Lexeme(nullptr) {
        Text = new std::string(name);
        try {
          PosRange = new TPosRange(pos_range);
        } catch (...) {
          delete Text;
          throw;
        }
      }

      /* Copy the specified TName. */
      TName(const TName &that)
          : Text(nullptr), PosRange(nullptr), Lexeme(nullptr) {
        assert(&that);
        *this = that;
      }

      /* Move constructor */
      TName(TName &&that)
          : Text(nullptr), PosRange(nullptr), Lexeme(nullptr) {
        assert(&that);
        *this = that;
      }

      /* Destroy the name, freeing resources if necessary. */
      ~TName() {
        assert(this);
        Cleanup();
      }

      /* Are we not null? */
      operator bool() const {
        assert(this);
        return Lexeme != &DefaultLexeme;
      }

      /* Point to the given node.
         If we don't get a node, assume the 'null' state. */
      TName &operator=(const TNode *node) {
        assert(this);
        Cleanup();
        SetLexeme(node);
        return *this;
      }

      /* Regular assignment operator */
      TName &operator=(const TName &that) {
        assert(this);
        assert(&that);
        assert(that.Text);
        assert(that.PosRange);
        if (that.Lexeme) {
          /* 'that' refers to a TName with an associated CST TName node.
             No string and TPosRange allocations are necessary: Just copy
             the pointers from 'that'. */
          Cleanup();
          Lexeme = that.Lexeme;
          Text = that.Text;
          PosRange = that.PosRange;
        } else {
          /* 'that' refers to a TName with an allocated string and an allocated
             TPosRange.  We can't just copy the pointers: We have to allocate
             a new string and a new TPosRange.  This is exception-safe: This
             TName's state is preserved when allocation exceptions arise. */
          auto new_text = new std::string(*that.Text);
          TPosRange *new_pos_range = nullptr;
          try {
            new_pos_range = new TPosRange(*that.PosRange);
          } catch (...) {
            delete new_text;
            throw;
          }
          Cleanup();
          Lexeme = nullptr;
          Text = new_text;
          PosRange = new_pos_range;
        }
        return *this;
      }

      /* Move assignment operator */
      TName &operator=(TName &&that) {
        assert(this);
        assert(&that);
        Cleanup();
        std::swap(Lexeme, that.Lexeme);
        std::swap(Text, that.Text);
        std::swap(PosRange, that.PosRange);
        return *this;
      }

      /* Two names are equal if their texts are equal or if they are both null.
         Positions are not considered. */
      bool operator==(const TName &that) const {
        assert(this);
        assert(&that);
        return GetText() == that.GetText();
      }

      /* Two names are unequal if their texts are unequal of if one is null and the other is not.
         Positions are not considered. */
      bool operator!=(const TName &that) const {
        assert(this);
        assert(&that);
        return GetText() != that.GetText();
      }

      /* */
      bool operator<(const TName &that) const {
        assert(this);
        assert(&that);
        return GetText() < that.GetText();
      }

      /* The hash is based on the text of the name.
         Position is not considered. */
      size_t GetHash() const {
        assert(this);
        return std::hash<std::string>()(GetText());
      }

      /* The position in the source text at which this name was given.
         If we're null, our position is (1, 1)-(1, 1). */
      const TPosRange &GetPosRange() const {
        assert(this);
        return *PosRange;
      }

      /* The text of the name.
         If we're null, this is the empty string. */
      const std::string &GetText() const {
        assert(this);
        return *Text;
      }

      private:

      /* A convenience typedef for the Nycr lexeme. */
      typedef Tools::Nycr::TLexeme TLexeme;

      /* Free resources if necessary. */
      void Cleanup() {
        assert(this);
        if (!Lexeme) {
          delete Text;
          delete PosRange;
          Text = nullptr;
          PosRange = nullptr;
        }
      }

      /* Point at the node's lexeme.
         If there's no node, point at the default lexeme. */
      void SetLexeme(const TNode *node) {
        assert(this);
        Lexeme = node ? &node->GetLexeme() : &DefaultLexeme;
        Text = &Lexeme->GetText();
        PosRange = &Lexeme->GetPosRange();
      }

      /* the name */
      const std::string *Text;

      /* the position at which the name occurs */
      const TPosRange *PosRange;

      /* the lexeme representing this name */
      const TLexeme *Lexeme;

      /* the default lexeme, used for null names */
      static const TLexeme DefaultLexeme;

    };  // TName

  }  // Synth

}  // Stig

namespace std {

  /* Standard hasher for Stig::Synth::TName. */
  template <>
  struct hash<Stig::Synth::TName> {

    typedef size_t result_type;
    typedef Stig::Synth::TName argument_type;

    result_type operator()(const argument_type &that) const {
      assert(&that);
      return that.GetHash();
    }

  };  // hash<Stig::Synth::TName>

}  // std
