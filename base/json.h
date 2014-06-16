/* <base/json.h>

   A sum of the value types allowable in JSON.

   Copyright 2010-2014 OrlyAtomics, Inc.

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
#include <cstdint>
#include <fstream>
#include <functional>
#include <iomanip>
#include <istream>
#include <ostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <base/thrower.h>

namespace Base {

  /* A sum of the value types allowable in JSON. */
  class TJson final {
    public:

    /* Thrown when extracting from a malformed stream. */
    DEFINE_ERROR(TSyntaxError, std::runtime_error, "json syntax error");


    /* Aliases for constructed types from which we may construct. */
    using TArray  = std::vector<TJson>;
    using TObject = std::map<std::string, TJson>;
    using TString = std::string;

    /* The types of callbacks used by ForEachElem(). */
    using TArrayCb  = std::function<bool (const TJson &)>;
    using TObjectCb = std::function<bool (const std::string&, const TJson &)>;
    using TArrayCbNonConst  = std::function<bool (TJson &)>;
    using TObjectCbNonConst = std::function<bool (const std::string&, TJson &)>;

    /* A visitor to our state. */
    struct TVisitor {

      /* For the convenience of our descendants. */
      using TArray  = TJson::TArray;
      using TObject = TJson::TObject;
      using TString = TJson::TString;

      /* Override to handle the state. */
      virtual void operator()() const = 0;
      virtual void operator()(bool) const = 0;
      virtual void operator()(double) const = 0;
      virtual void operator()(const TArray &) const = 0;
      virtual void operator()(const TObject &) const = 0;
      virtual void operator()(const TString &) const = 0;

    };  // TJson::TVisitor

    /* The kinds of states we can be in. */
    enum TKind { Null, Bool, Number, Array, Object, String };

    //TODO: We don't currently escape unicode sequences / wide characters
    static void WriteString(std::ostream &strm, const std::string &text) {
      strm << '"';
      const auto end = text.end();

      //TODO: We build a lot of these escape / unescape things. Should have a <base/> library function for doing it.
      for(auto it = text.begin(); it != end; ++it) {
        char c = *it;
        switch(c) {
          case '\\': {
            strm << R"(\\)";
            break;
          }
          case '\"': {
            strm << R"(\")";
            break;
          }
          case '/': {
            strm << R"(\/)";
            break;
          }
          case '\b': {
            strm << R"(\b)";
            break;
          }
          case '\f': {
            strm << R"(\f)";
            break;
          }
          case '\n': {
            strm << R"(\n)";
            break;
          }
          case '\r': {
            strm << R"(\r)";
            break;
          }
          case '\t': {
            strm << R"(\t)";
            break;
          }
          default: {
            strm << c;
          }
        }
      }
      strm << '"';
    }

    /* Read from the given input string to find a JSON string which starts/ends with '"' and properly unescape escaped
       characters. */
    static std::string ReadQuotedString(std::istream &strm) {
      DEFINE_ERROR(not_implemented_t, TSyntaxError, "not currently implemented");
      std::string text;
      //NOTE: Tested adding a reserve() call here. Didn't greatly effect compile time

      // Keep around the old flags so we can put strm back how we found it;
      auto flags = strm.flags(strm.flags() & ~std::ios_base::skipws);
      try  {
        // Helper function
        auto read_character = [&strm] () {
          char ret;
          strm >> ret;
          if (!strm.good()) {
            THROW_ERROR(TSyntaxError) << "Unexpected I/O error while reading string (likely end of string)";
          }
          return ret;
        };

        if (read_character() != '"') {
          THROW_ERROR(TSyntaxError) << "Expected '\"' at start of string but didn't find it.";
        }

        // Escape character processing helper. In it's own function to make the core loop simpler.
        auto get_escape = [&read_character]() {
          char c = read_character();
          switch(c) {
            case '\\':
              return '\\';
            case '"':
              return '"';
            case '/':
              return '/';
            case 'b':
              return '\b';
            case 'f':
              return '\f';
            case 'n':
              return '\n';
            case 'r':
              return '\r';
            case 't':
              return '\t';
            case 'u':
              THROW_ERROR(not_implemented_t) << " parsing of unicode escape sequences '\\u four-hex-digits'";
            default:
              THROW_ERROR(TSyntaxError) << "Invalid escape sequence '\\" << c << '\'';
          }
          assert(false);
          __builtin_unreachable();
        };
        // Loop over each input character processing escape sequences and
        while(true) {
          char c = read_character();
          if (c == '"') {
            break;
          } else if (c == '\\') {
            c = get_escape();
          }
          text += c;
        }
      } catch (...) {
        strm.flags(flags);
        throw;
      }
      //Reset flags to what they were
      strm.flags(flags);
      return text;
    }

    /* Read our extension to JSON, a raw string. */
    static std::string ReadRawString(std::istream &strm) {
      // Raw string (String containing anything but whitespace and 'special' json chars [{}],:"
      // NOTE: null, true, and false are all found by matching raw strings.
      std::string text;
      for (;;) {
        int c = strm.peek();
        if (!strm.good() || isspace(c) || c == '[' || c == '{' || c == '}' || c == ']' || c == ',' || c == ':' ||
            c == '"') {
          break;
        }
        strm.ignore();
        text += char(c);
      }
      return text;
    }

    static std::string ReadString(std::istream &strm) {
      std::string text;
      if (strm.peek() == '"') {
        text = ReadQuotedString(strm);
      } else {
        text = ReadRawString(strm);
      }
      return text;
    }

    static TJson Read(const char *filename) {
      std::ifstream in(filename);
      if (!in.is_open()) {
        THROW_ERROR(std::runtime_error) << "Unable to open file " << std::quoted(filename);
      }
      TJson ret;
      try {
        ret.Read(in);
      } catch (const TSyntaxError &ex) {
        THROW_ERROR(std::runtime_error) << "in " << std::quoted(filename) << ':' << ex.what();
      }
      return ret;
    }

    /* Construct as a default instance of the given kind. */
    TJson(TKind kind = Null) noexcept {
      switch (kind) {
        /* Do nothing. */
        case Null:   { break; }
        /* Init the built-in. */
        case Bool:   { Bool_   = false; break; }
        case Number: { Number_ = 0;     break; }
        /* Default-construct the object. */
        case Array:  { new (&Array_ ) TArray (); break; }
        case Object: { new (&Object_) TObject(); break; }
        case String: { new (&String_) TString(); break; }
      }
      Kind = kind;
    }

    /* The donor is left null. */
    TJson(TJson &&that) noexcept {
      assert(&that);
      switch (that.Kind) {
        /* Do nothing. */
        case Null:   { break; }
        /* Init the built-in. */
        case Bool:   { Bool_   = that.Bool_;   break; }
        case Number: { Number_ = that.Number_; break; }
        /* Move-construct the object. */
        case Array:  { new (&Array_ ) TArray (std::move(that.Array_ )); that.Array_ .~TArray (); break; }
        case Object: { new (&Object_) TObject(std::move(that.Object_)); that.Object_.~TObject(); break; }
        case String: { new (&String_) TString(std::move(that.String_)); that.String_.~TString(); break; }
      }
      Kind = that.Kind;
      that.Kind = Null;
    }

    /* Deep-copy. */
    TJson(const TJson &that) {
      assert(&that);
      switch (that.Kind) {
        /* Do nothing. */
        case Null:   { break; }
        /* Init the built-in. */
        case Bool:   { Bool_   = that.Bool_;   break; }
        case Number: { Number_ = that.Number_; break; }
        /* Copy-construct the object. */
        case Array:  { new (&Array_ ) TArray (that.Array_ ); break; }
        case Object: { new (&Object_) TObject(that.Object_); break; }
        case String: { new (&String_) TString(that.String_); break; }
      }
      Kind = that.Kind;
    }

    /* Construct as the bool. */
    TJson(bool that) noexcept
        : Kind(Bool), Bool_(that) {}

    /* Construct as the number */
    TJson(int8_t that) noexcept
        : Kind(Number), Number_(that) {}

    /* Construct as the number */
    TJson(int16_t that) noexcept
        : Kind(Number), Number_(that) {}

    /* Construct as the number */
    TJson(int32_t that) noexcept
        : Kind(Number), Number_(that) {}

    /* Construct as the number */
    TJson(int64_t that) noexcept
        : Kind(Number), Number_(that) {}

    /* Construct as the number */
    TJson(uint8_t that) noexcept
        : Kind(Number), Number_(that) {}

    /* Construct as the number */
    TJson(uint16_t that) noexcept
        : Kind(Number), Number_(that) {}

    /* Construct as the number */
    TJson(uint32_t that) noexcept
        : Kind(Number), Number_(that) {}

    /* Construct as the number */
    TJson(uint64_t that) noexcept
        : Kind(Number), Number_(that) {}

    /* Construct as the number */
    TJson(float that) noexcept
        : Kind(Number), Number_(that) {}

    /* Construct as the number */
    TJson(double that) noexcept
        : Kind(Number), Number_(that) {}

    /* Construct as an array, leaving the donor empty. */
    TJson(TArray &&that) noexcept
        : Kind(Array), Array_(std::move(that)) {}

    /* Construct as a copy of the array. */
    TJson(const TArray &that)
        : Kind(Array), Array_(that) {}

    /* Construct as an object, leaving the donor empty. */
    TJson(TObject &&that) noexcept
        : Kind(Object), Object_(std::move(that)) {}

    /* Construct as a copy of the object. */
    TJson(const TObject &that)
        : Kind(Object), Object_(that) {}

    /* Construct as a string of a single character. */
    TJson(char that)
        : Kind(String), String_(&that, 1) {}

    /* Construct as a string of a single character. */
    TJson(const char *that)
        : Kind(String), String_(that ? that : "") {}

    /* Construct as a string, leaving the donor empty. */
    TJson(TString &&that) noexcept
        : Kind(String), String_(std::move(that)) {}

    /* Construct as a copy of the string. */
    TJson(const TString &that)
        : Kind(String), String_(that) {}

    /* Construct an array of copies of the example. */
    TJson(size_t size, const TJson &example)
        : Kind(Array), Array_(size, example) {}

    /* Contained elements, if any, are also destroyed. */
    ~TJson() {
      assert(this);
      switch (Kind) {
        /* Do nothing. */
        case Null:   { break; }
        case Bool:   { break; }
        case Number: { break; }
        /* Destroy the object. */
        case Array:  { Array_ .~TArray (); break; }
        case Object: { Object_.~TObject(); break; }
        case String: { String_.~TString(); break; }
      }
    }

    /* The donor is left null. */
    TJson &operator=(TJson &&that) noexcept {
      assert(this);
      assert(&that);
      this->~TJson();
      new (this) TJson(std::move(that));
      return *this;
    }

    /* Deep-copy. */
    TJson &operator=(const TJson &that) {
      assert(this);
      assert(&that);
      return *this = TJson(that);
    }

    /* True iff. this object and that one are in the same state. */
    bool operator==(const TJson &that) const noexcept {
      assert(this);
      assert(&that);
      bool success = (Kind == that.Kind);
      if (success) {
        switch (Kind) {
          case Null:   { break; }
          case Bool:   { success = (Bool_   == that.Bool_  ); break; }
          case Number: { success = (Number_ == that.Number_); break; }
          case Array:  { success = (Array_  == that.Array_ ); break; }
          case Object: { success = (Object_ == that.Object_); break; }
          case String: { success = (String_ == that.String_); break; }
        }
      }
      return success;
    }

    /* True iff. this object and that one are not in the same state. */
    bool operator!=(const TJson &that) const noexcept {
      assert(this);
      return !(*this == that);
    }

    /* Subscript to an element contained in an array. */
    TJson &operator[](size_t that) {
      assert(this);
      assert(Kind == Array);
      assert(that < Array_.size());
      return Array_[that];
    }

    /* Subscript to an element contained in an array. */
    const TJson &operator[](size_t that) const {
      assert(this);
      assert(Kind == Array);
      assert(that < Array_.size());
      return Array_[that];
    }

    /* Find or create an element contained in an object. */
    TJson &operator[](TString &&that) {
      assert(this);
      assert(&that);
      assert(Kind == Object);
      return Object_[std::move(that)];
    }

    /* Find or create an element contained in an object. */
    TJson &operator[](const TString &that) {
      assert(this);
      assert(&that);
      assert(Kind == Object);
      return Object_[that];
    }

    /* Find or create an element contained in an object. */
    const TJson &operator[](const TString &that) const {
      assert(this);
      assert(&that);
      assert(Kind == Object);
      const TJson *elem = TryFind(that);
      assert(elem);
      return *elem;
    }

    /* Accept the visitor. */
    void Accept(const TVisitor &visitor) const {
      assert(this);
      assert(&visitor);
      switch (Kind) {
        case Null:   { visitor(       ); break; }
        case Bool:   { visitor(Bool_  ); break; }
        case Number: { visitor(Number_); break; }
        case Array:  { visitor(Array_ ); break; }
        case Object: { visitor(Object_); break; }
        case String: { visitor(String_); break; }
      }
    }

    /* Returns true if the object contains the given key */
    bool Contains(const TString &that) const {
      assert(this);
      return TryFind(that);
    }

    /* Call back for each element contained in an array. */
    bool ForEachElem(const TArrayCb &cb) const {
      assert(this);
      assert(&cb);
      assert(cb);
      assert(Kind == Array);
      for (const auto &elem: Array_) {
        if (!cb(elem)) {
          return false;
        }
      }
      return true;
    }

    /* Call back for each element contained in an object. */
    bool ForEachElem(const TObjectCb &cb) const {
      assert(this);
      assert(&cb);
      assert(cb);
      assert(Kind == Object);
      for (const auto &elem: Object_) {
        if (!cb(elem.first, elem.second)) {
          return false;
        }
      }
      return true;
    }

    /* Call back for each element contained in an array. */
    bool ForEachElem(const TArrayCbNonConst &cb) {
      assert(this);
      assert(&cb);
      assert(cb);
      assert(Kind == Array);
      for (auto &elem: Array_) {
        if (!cb(elem)) {
          return false;
        }
      }
      return true;
    }

    /* Call back for each element contained in an object. */
    bool ForEachElem(const TObjectCbNonConst &cb) {
      assert(this);
      assert(&cb);
      assert(cb);
      assert(Kind == Object);
      for (auto &elem: Object_) {
        if (!cb(elem.first, elem.second)) {
          return false;
        }
      }
      return true;
    }

    /* A string formatted from our state. */
    std::string Format() const {
      assert(this);
      std::ostringstream strm;
      Write(strm);
      return strm.str();
    }

    /* The kind of state we're in. */
    TKind GetKind() const noexcept {
      assert(this);
      return Kind;
    }

    /* The number of elements in an array or object or the number of
       characters in a string. */
    size_t GetSize() const noexcept {
      assert(this);
      switch (Kind) {
        case Array:  { return Array_ .size(); }
        case Object: { return Object_.size(); }
        case String: { return String_.size(); }
        default: {
          assert(false);
          return 0;
        }
      }
    }

    TArray &GetArray() {
      assert(this);
      assert(Kind == Array);
      return Array_;
    }

    bool GetBool() const noexcept {
      assert(this);
      assert(Kind == Bool);
      return Bool_;
    }

    double GetNumber() const noexcept {
      assert(this);
      assert(Kind == Number);
      return Number_;
    }

    const TString &GetString() const noexcept {
      assert(this);
      assert(Kind == String);
      return String_;
    }

    bool IsNull() const noexcept {
      assert(this);
      return Kind == Null;
    }

    /* Parse from the stream. */
    void Read(std::istream &strm) {
      assert(this);
      assert(&strm);
      int c = std::ws(strm).peek();
      switch (c) {
        case '[': {
          strm.ignore();
          TJson elem;
          TArray temp;
          for (;;) {
            if (!ParseSep(strm, ']', temp.empty())) {
              break;
            }
            elem.Read(strm);
            temp.emplace_back(std::move(elem));
          }
          *this = TJson(std::move(temp));
          break;
        }
        case '{': {
          strm.ignore();
          std::string key;
          TJson val;
          TObject temp;
          for (;;) {
            if (!ParseSep(strm, '}', temp.empty())) {
              break;
            }
            strm >> std::ws;
            key = ReadString(strm);
            strm >> std::ws;
            Match(strm, ':');
            val.Read(strm);
            temp[std::move(key)] = std::move(val);
          }
          *this = TJson(std::move(temp));
          break;
        }
        case '"': {
          *this = TJson(ReadQuotedString(strm));
          break;
        }
        default: {
          if (c == '+' || c == '-' || isdigit(c)) {
            double temp;
            strm >> temp;
            *this = TJson(std::move(temp));
            break;
          } else {
            // Raw string (String containing anything but whitespace and 'special' json chars [{}],:"
            // NOTE: null, true, and false are all found by matching raw strings.
            std::string text = ReadRawString(strm);
            if (text == "null") {
              Reset();
            } else if (text == "true") {
              *this = true;
            } else if (text == "false") {
              *this = false;
            } else {
              *this = TJson(move(text));
            }
            break;
          }
          THROW_ERROR(TSyntaxError) << "Unexpected '" << char(c) << "' at start of input";
        }
      }  // switch
    }

    /* Return to the default-constructed state (which is null). */
    TJson &Reset() noexcept {
      assert(this);
      this->~TJson();
      Kind = Null;
      return *this;
    }

    /* Swap states. */
    TJson &Swap(TJson &that) noexcept {
      assert(this);
      assert(&that);
      TJson temp = std::move(*this);
      new (this) TJson(std::move(that));
      new (&that) TJson(std::move(temp));
      return *this;
    }

    /* A pointer to the named element in the object, or null if we have no
       such element. */
    const TJson *TryFind(const std::string &key) const {
      assert(this);
      assert(Kind == Object);
      auto iter = Object_.find(key);
      return (iter != Object_.end()) ? &(iter->second) : nullptr;
    }

    /* Format to the stream. */
    void Write(std::ostream &strm) const {
      assert(this);
      assert(&strm);
      switch (Kind) {
        case Null: {
          strm << "null";
          break;
        }
        case Bool: {
          strm << (Bool_ ? "true" : "false");
          break;
        }
        case Number: {
          strm << Number_;
          break;
        }
        case Array: {
          strm << '[';
          bool sep = false;
          for (const auto &elem: Array_) {
            if (sep) {
              strm << ',';
            } else {
              sep = true;
            }
            elem.Write(strm);
          }
          strm << ']';
          break;
        }
        case Object: {
          strm << '{';
          bool sep = false;
          for (const auto &elem: Object_) {
            if (sep) {
              strm << ',';
            } else {
              sep = true;
            }
            WriteString(strm, elem.first);
            strm << ':';
            elem.second.Write(strm);
          }
          strm << '}';
          break;
        }
        case String: {
          WriteString(strm, String_);
          break;
        }
      }
    }

    /* Parse a text. */
    template <typename TArg>
    static TJson Parse(TArg &&arg) {
      std::istringstream strm(std::forward<TArg>(arg));
      TJson result;
      result.Read(strm);
      return std::move(result);
    }

    private:

    /* Skip whitespace, then see if a comma-seprated list is going to
       continue or not.  We're looking for a closing mark or, if we're not
       at the start of the list, a comma. */
    static bool ParseSep(
        std::istream &strm, char close_mark, bool at_start) {
      assert(&strm);
      int c = std::ws(strm).peek();
      if (c == close_mark) {
        strm.ignore();
        return false;
      }
      if (!at_start) {
        if (c != ',') {
          THROW_ERROR(TSyntaxError) << "Expected a ',' but found a '" << char(c) << '\'';
        }
        strm.ignore();
      }
      return true;
    }

    /* The stream must yield given char or throw a syntax error. */
    static void Match(std::istream &strm, char expected) {
      assert(&strm);
      if (strm.peek() != expected) {
        THROW_ERROR(TSyntaxError) << "Expected '" << expected << "' but didn't find it. Found '" << char(strm.peek())
                                  << '\'';
      }
      strm.ignore();
    }

    /* See accessor. */
    TKind Kind;

    /* The union of our potential states.  At most one of these is valid
       at a time.  'Kind', above, determines which, if any, is valid. */
    union {
      bool        Bool_;
      double      Number_;
      TObject     Object_;
      TArray      Array_;
      std::string String_;
    };

  };  // TJson

  inline void swap(Base::TJson &lhs, Base::TJson &rhs) noexcept {
    lhs.Swap(rhs);
  }

  /* Std stream extractor. */
  inline std::istream &operator>>(std::istream &strm, TJson &that) {
    assert(&that);
    that.Read(strm);
    return strm;
  }

  /* Std stream inserter. */
  inline std::ostream &operator<<(std::ostream &strm, const TJson &that) {
    assert(&that);
    that.Write(strm);
    return strm;
  }

  inline std::ostream &operator<<(std::ostream &strm, const TJson::TKind &kind) {
    assert(&kind);
      switch (kind) {
        case TJson::Null:   {
          strm << "null";
          break;
        }
        case TJson::Bool:   {
          strm << "bool";
          break;
        }
        case TJson::Number: {
          strm << "number";
          break;
        }
        case TJson::Array:  {
          strm << "array";
          break;
        }
        case TJson::Object: {
          strm << "object";
          break;
        }
        case TJson::String: {
          strm << "string";
          break;
        }
      }
      return strm;
  }

}  // Base


namespace std {

  /* Standard swapper. */
  template <>
  inline void swap<Base::TJson>(Base::TJson &lhs, Base::TJson &rhs) noexcept {
    lhs.Swap(rhs);
  }

}
