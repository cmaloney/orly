/* <starsha/note.h>

   TODO

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
#include <functional>
#include <istream>
#include <ostream>
#include <sstream>
#include <string>

#include <base/no_copy_semantics.h>

namespace Starsha {

  /* TODO */
  class TNote {
    NO_COPY_SEMANTICS(TNote);
    public:

    /* TODO */
    class TEsc {
      public:

      /* TODO */
      TEsc(const std::string &text)
          : Text(text) {}

      /* TODO */
      void Write(std::ostream &strm) const;

      private:

      /* TODO */
      const std::string &Text;

    };  // TEsc

    /* TODO */
    class TIndent {
      public:

      /* TODO */
      TIndent(const TNote *note) {
        assert(note);
        Depth = note->GetDepth();
      }

      /* TODO */
      TIndent(const TIndent &that) {
        assert(&that);
        Depth = that.Depth + 1;
      }

      /* TODO */
      void Write(std::ostream &strm) const;

      private:

      /* TODO */
      size_t Depth;

    };  // TIndent

    /* TODO */
    TNote(const std::string &key)
        : Key(key) {
      Init();
    }

    /* TODO */
    TNote(const std::string &key, const std::string &value)
        : Key(key), Value(value) {
      Init();
    }

    /* TODO */
    TNote(TNote *parent, const std::string &key)
        : Key(key) {
      Init();
      SetParent(parent);
    }

    /* TODO */
    TNote(TNote *parent, const std::string &key, const std::string &value)
        : Key(key), Value(value) {
      Init();
      SetParent(parent);
    }

    /* TODO */
    ~TNote();

    /* TODO */
    void ForEachChild(const std::function<bool (TNote *)> &cb) const;

    /* TODO */
    void ForEachChild(const char *key, const std::function<bool (TNote *)> &cb) const;

    /* TODO */
    TNote *GetChild(const char *key) const;

    /* TODO */
    template <typename TVal>
    void GetChildValueAs(const char *key, TVal &val) const {
      assert(this);
      assert(&val);
      std::istringstream strm(GetChild(key)->Value);
      strm >> val;
    }

    /* TODO */
    size_t GetDepth() const {
      assert(this);
      return Parent ? Parent->GetDepth() + 1 : 0;
    }

    /* TODO */
    TNote *GetFirstChild() const {
      assert(this);
      return FirstChild;
    }

    /* TODO */
    const std::string &GetKey() const {
      assert(this);
      return Key;
    }

    /* TODO */
    TNote *GetLastChild() const {
      assert(this);
      return LastChild;
    }

    /* TODO */
    TNote *GetNextSibling() const {
      assert(this);
      return NextSibling;
    }

    /* TODO */
    TNote *GetOrNewChild(const char *key) {
      assert(this);
      TNote *child = TryGetChild(key);
      if (!child) {
        child = new TNote(this, key);
      }
      return child;
    }

    /* TODO */
    TNote *GetParent() const {
      assert(this);
      return Parent;
    }

    /* TODO */
    TNote *GetPrevSibling() const {
      assert(this);
      return PrevSibling;
    }

    /* TODO */
    const std::string &GetValue() const {
      assert(this);
      return Value;
    }

    /* TODO */
    TNote *ReplaceOrNewChild(const char *key) {
      assert(this);
      delete TryGetChild(key);
      return new TNote(this, key);
    }

    /* TODO */
    template <typename TVal>
    void SetChildValueAs(const char *key, const TVal &val) {
      assert(this);
      assert(&val);
      std::ostringstream strm;
      strm << val;
      GetOrNewChild(key)->Value = strm.str();
    }

    /* TODO */
    void SetParent(TNote *parent);

    /* TODO */
    TNote *TryGetChild(const char *key) const;

    /* TODO */
    void Write(std::ostream &strm) const;

    /* TODO */
    static TNote *Read(std::istream &strm);

    /* TODO */
    static TNote *TryRead(std::istream &strm);

    // Reads from the specified istream and appends elemnts from it to the given note tree.
    static bool TryReadAppend(std::istream &strm, TNote *root);

    private:

    /* TODO */
    TNote *&GetNextConj() const {
      assert(this);
      assert(Parent);
      return NextSibling ? NextSibling->PrevSibling : Parent->LastChild;
    }

    /* TODO */
    TNote *&GetPrevConj() const {
      assert(this);
      assert(Parent);
      return PrevSibling ? PrevSibling->NextSibling : Parent->FirstChild;
    }

    /* TODO */
    void Indent(std::ostream &strm) const;

    /* TODO */
    void Init() {
      assert(this);
      assert(!Key.empty());
      FirstChild = 0;
      LastChild = 0;
      Parent = 0;
      PrevSibling = 0;
      NextSibling = 0;
    }

    /* TODO */
    static int ReadPunc(std::istream &strm, const char *punc);

    /* TODO */
    static void ReadString(std::istream &strm, std::string &out);

    /* TODO */
    static int TryReadPunc(std::istream &strm, const char *punc);

    /* TODO */
    static bool TryReadString(std::istream &strm, std::string &out);

    /* TODO */
    TNote *FirstChild, *LastChild, *Parent, *PrevSibling, *NextSibling;

    /* TODO */
    std::string Key, Value;

  };  // TNote

  /* TODO */
  inline std::ostream &operator<<(std::ostream &strm, const TNote::TEsc &that) {
    assert(&that);
    that.Write(strm);
    return strm;
  }

  /* TODO */
  inline std::ostream &operator<<(std::ostream &strm, const TNote::TIndent &that) {
    assert(&that);
    that.Write(strm);
    return strm;
  }

}  // Starsha
