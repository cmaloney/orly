#include <base/json.h>

#include <cassert>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <limits>
#include <utility>

#include <base/likely.h>
#include <base/thrower.h>
#include <base/unreachable.h>

using namespace Base;

static void WriteString(std::ostream &strm, const std::string &text) {
  strm << '"';
  const auto end = text.end();
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
      default: { strm << c; }
    }
  }
  strm << '"';
}

/* Read from the given input string to find a JSON string which starts/ends with '"' and properly
   unescape escaped
   characters. */
static std::string ReadQuotedString(std::istream &strm) {
  assert(&strm);
  /* Eat the opening quote. */
  if(unlikely(strm.peek() != '"')) {
    THROWER(TJson::TSyntaxError) << "missing opening quote";
  }
  strm.ignore();
  /* Loop over the input until we find the closing quote, accumulating
     characters as we go. */
  std::string accum;
  accum.reserve(200);
  bool keep_going = true;
  do {
    auto c = strm.peek();
    switch(c) {
      /* Closing quote. */
      case '"': {
        strm.ignore();
        keep_going = false;
        break;
      }
      /* Escape sequence. */
      case '\\': {
        strm.ignore();
        switch(strm.peek()) {
          case '\\': {
            accum += '\\';
            break;
          }
          case '"': {
            accum += '\"';
            break;
          }
          case '/': {
            accum += '/';
            break;
          }
          case 'b': {
            accum += '\b';
            break;
          }
          case 'f': {
            accum += '\f';
            break;
          }
          case 'n': {
            accum += '\n';
            break;
          }
          case 'r': {
            accum += '\r';
            break;
          }
          case 't': {
            accum += '\t';
            break;
          }
          case 'u': {
            THROWER(TJson::TSyntaxError) << "Unicode escapes aren't implemented";
          }
          default: { THROWER(TJson::TSyntaxError) << "bad escape sequence"; }
        }  // switch
        strm.ignore();
        break;
      }
      /* Normal character or EOF. */
      default: {
        if(unlikely(c < 0)) {
          THROWER(TJson::TSyntaxError) << "missing closing quote";
        }
        accum += char(c);
        strm.ignore();
      }
    }  // switch
  } while(keep_going);
  return accum;
}

/* Read our extension to JSON, a raw string. */
static std::string ReadRawString(std::istream &strm) {
  // Raw string (String containing anything but whitespace and 'special' json chars [{}],:"
  // NOTE: null, true, and false are all found by matching raw strings.
  std::string text;
  text.reserve(64);
  for(;;) {
    int c = strm.peek();
    if(unlikely(!strm.good() || isspace(c) || c == '[' || c == '{' || c == '}' || c == ']' ||
                c == ',' || c == ':' || c == '"')) {
      break;
    }
    strm.ignore();
    text += char(c);
  }
  return text;
}

static std::string ReadString(std::istream &strm) {
  std::string text;
  if(strm.peek() == '"') {
    text = ReadQuotedString(strm);
  } else {
    text = ReadRawString(strm);
  }
  return text;
}

/* Skip whitespace, then see if a comma-seprated list is going to
   continue or not.  We're looking for a closing mark or, if we're not
   at the start of the list, a comma. */
static bool ParseSep(std::istream &strm, char close_mark, bool at_start) {
  assert(&strm);
  int c = std::ws(strm).peek();
  if(c == close_mark) {
    strm.ignore();
    return false;
  }
  if(!at_start) {
    if(c != ',') {
      THROWER(TJson::TSyntaxError) << "Expected a ',' but found a '" << char(c) << '\'';
    }
    strm.ignore();
  }

  // If we close right after the comma, we have a trailing comma and are actually done
  c = std::ws(strm).peek();
  if(c == close_mark) {
    strm.ignore();
    return false;
  }
  return true;
}

/* The stream must yield given char or throw a syntax error. */
static void Match(std::istream &strm, char expected) {
  assert(&strm);
  if(strm.peek() != expected) {
    THROWER(TJson::TSyntaxError) << "Expected '" << expected << "' but didn't find it. Found '"
                              << char(strm.peek()) << '\'';
  }
  strm.ignore();
}

TJson TJson::Read(const char *filename) {
  std::ifstream in(filename);
  if(!in.is_open()) {
    THROWER(std::runtime_error) << "Unable to open file " << std::quoted(filename);
  }
  TJson ret;
  try {
    ret.Read(in);
  } catch(const TSyntaxError &ex) {
    THROWER(std::runtime_error) << "in " << std::quoted(filename) << ':' << ex.what();
  }
  return ret;
}

TJson::TJson(TKind kind) noexcept {
  switch(kind) {
    /* Do nothing. */
    case Null: {
      break;
    }
    /* Init the built-in. */
    case Bool: {
      Bool_ = false;
      break;
    }
    case Number: {
      Number_ = 0;
      break;
    }
    /* Default-construct the object. */
    case Array: {
      new (&Array_) TArray();
      break;
    }
    case Object: {
      new (&Object_) TObject();
      break;
    }
    case String: {
      new (&String_) TString();
      break;
    }
  }
  Kind = kind;
}

TJson::TJson(TJson &&that) noexcept {
  assert(&that);
  switch(that.Kind) {
    /* Do nothing. */
    case Null: {
      break;
    }
    /* Init the built-in. */
    case Bool: {
      Bool_ = that.Bool_;
      break;
    }
    case Number: {
      Number_ = that.Number_;
      break;
    }
    /* Move-construct the object. */
    case Array: {
      new (&Array_) TArray(std::move(that.Array_));
      that.Array_.~TArray();
      break;
    }
    case Object: {
      new (&Object_) TObject(std::move(that.Object_));
      that.Object_.~TObject();
      break;
    }
    case String: {
      new (&String_) TString(std::move(that.String_));
      that.String_.~TString();
      break;
    }
  }
  Kind = that.Kind;
  that.Kind = Null;
}

TJson::TJson(const TJson &that) {
  assert(&that);
  switch(that.Kind) {
    /* Do nothing. */
    case Null: {
      break;
    }
    /* Init the built-in. */
    case Bool: {
      Bool_ = that.Bool_;
      break;
    }
    case Number: {
      Number_ = that.Number_;
      break;
    }
    /* Copy-construct the object. */
    case Array: {
      new (&Array_) TArray(that.Array_);
      break;
    }
    case Object: {
      new (&Object_) TObject(that.Object_);
      break;
    }
    case String: {
      new (&String_) TString(that.String_);
      break;
    }
  }
  Kind = that.Kind;
}

TJson::TJson(bool that) noexcept : Kind(Bool), Bool_(that) {}
TJson::TJson(int8_t that) noexcept : Kind(Number), Number_(that) {}
TJson::TJson(int16_t that) noexcept : Kind(Number), Number_(that) {}
TJson::TJson(int32_t that) noexcept : Kind(Number), Number_(that) {}
TJson::TJson(int64_t that) noexcept : Kind(Number), Number_(that) {}
TJson::TJson(uint8_t that) noexcept : Kind(Number), Number_(that) {}
TJson::TJson(uint16_t that) noexcept : Kind(Number), Number_(that) {}
TJson::TJson(uint32_t that) noexcept : Kind(Number), Number_(int64_t(that)) {}
TJson::TJson(uint64_t that) noexcept : Kind(Number), Number_(int64_t(that)) {
  assert(that < std::numeric_limits<uint32_t>::max());
}

TJson::TJson(TArray &&that) noexcept : Kind(Array), Array_(std::move(that)) {}
TJson::TJson(const TArray &that) : Kind(Array), Array_(that) {}
TJson::TJson(TObject &&that) noexcept : Kind(Object), Object_(std::move(that)) {}
TJson::TJson(const TObject &that) : Kind(Object), Object_(that) {}
TJson::TJson(char that) : Kind(String), String_(&that, 1) {}
TJson::TJson(const char *that) : Kind(String), String_(that ? that : "") {}
TJson::TJson(TString &&that) noexcept : Kind(String), String_(std::move(that)) {}
TJson::TJson(const TString &that) : Kind(String), String_(that) {}
TJson::TJson(size_t size, const TJson &example) : Kind(Array), Array_(size, example) {}

/* Contained elements, if any, are also destroyed. */
TJson::~TJson() {
  assert(this);
  switch(Kind) {
    /* Do nothing. */
    case Null: {
      break;
    }
    case Bool: {
      break;
    }
    case Number: {
      break;
    }
    /* Destroy the object. */
    case Array: {
      Array_.~TArray();
      break;
    }
    case Object: {
      Object_.~TObject();
      break;
    }
    case String: {
      String_.~TString();
      break;
    }
  }
}

TJson &TJson::operator=(TJson &&that) noexcept {
  assert(this);
  assert(&that);
  this->~TJson();
  new (this) TJson(std::move(that));
  return *this;
}

TJson &TJson::operator=(const TJson &that) {
  assert(this);
  assert(&that);
  return *this = TJson(that);
}

bool TJson::operator==(const TJson &that) const noexcept {
  assert(this);
  assert(&that);
  bool success = (Kind == that.Kind);
  if(success) {
    switch(Kind) {
      case Null: {
        break;
      }
      case Bool: {
        success = (Bool_ == that.Bool_);
        break;
      }
      case Number: {
        success = (Number_ == that.Number_);
        break;
      }
      case Array: {
        success = (Array_ == that.Array_);
        break;
      }
      case Object: {
        success = (Object_ == that.Object_);
        break;
      }
      case String: {
        success = (String_ == that.String_);
        break;
      }
    }
  }
  return success;
}

bool TJson::operator!=(const TJson &that) const noexcept {
  assert(this);
  return !(*this == that);
}

TJson &TJson::operator[](size_t that) {
  assert(this);
  assert(Kind == Array);
  assert(that < Array_.size());
  return Array_[that];
}

const TJson &TJson::operator[](size_t that) const {
  assert(this);
  assert(Kind == Array);
  assert(that < Array_.size());
  return Array_[that];
}

TJson &TJson::operator[](TString &&that) {
  assert(this);
  assert(&that);
  assert(Kind == Object);
  return Object_[std::move(that)];
}

TJson &TJson::operator[](const TString &that) {
  assert(this);
  assert(&that);
  assert(Kind == Object);
  return Object_[that];
}

const TJson &TJson::operator[](const TString &that) const {
  assert(this);
  assert(&that);
  assert(Kind == Object);
  const TJson *elem = TryFind(that);
  assert(elem);
  return *elem;
}

bool TJson::Contains(const TString &that) const {
  assert(this);
  return TryFind(that);
}

/* The kind of state we're in. */
TJson::TKind TJson::GetKind() const noexcept {
  assert(this);
  return Kind;
}

/* The number of elements in an array or object or the number of
   characters in a string. */
size_t TJson::GetSize() const noexcept {
  assert(this);
  switch(Kind) {
    case Array: {
      return Array_.size();
    }
    case Object: {
      return Object_.size();
    }
    case String: {
      return String_.size();
    }
    case Null:
    case Bool:
    case Number: {
      Unreachable(HERE);
    }
  }
  Unreachable(HERE); // Placaet GCC
}

const TJson::TArray &TJson::GetArray() const noexcept {
  assert(this);
  assert(Kind == Array);
  return Array_;
}

TJson::TArray &TJson::GetArray() noexcept {
  assert(this);
  assert(Kind == Array);
  return Array_;
}

bool TJson::GetBool() const noexcept {
  assert(this);
  assert(Kind == Bool);
  return Bool_;
}

const TJson::TObject &TJson::GetObject() const noexcept {
  assert(this);
  assert(Kind == Object);
  return Object_;
}

TJson::TObject &TJson::GetObject() noexcept {
  assert(this);
  assert(Kind == Object);
  return Object_;
}

int64_t TJson::GetNumber() const noexcept {
  assert(this);
  assert(Kind == Number);
  return Number_;
}

const TJson::TString &TJson::GetString() const noexcept {
  assert(this);
  assert(Kind == String);
  return String_;
}

void TJson::Read(std::istream &strm) {
  assert(this);
  assert(&strm);
  int c = std::ws(strm).peek();
  switch(c) {
    case '[': {
      strm.ignore();
      TJson elem;
      TArray temp;
      for(;;) {
        if(!ParseSep(strm, ']', temp.empty())) {
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
      for(;;) {
        if(!ParseSep(strm, '}', temp.empty())) {
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
      if(c == '+' || c == '-' || isdigit(c)) {
        int64_t temp;
        strm >> temp;
        *this = TJson(std::move(temp));
        break;
      } else {
        // Raw string (String containing anything but whitespace and 'special' json chars [{}],:"
        // NOTE: null, true, and false are all found by matching raw strings.
        std::string text = ReadRawString(strm);
        if (text.empty()) {
          Reset();
        } else if(text == "null") {
          Reset();
        } else if(text == "true") {
          *this = true;
        } else if(text == "false") {
          *this = false;
        } else {
          *this = TJson(move(text));
        }
        break;
      }
    }
  }  // switch
}

TJson &TJson::Reset() noexcept {
  assert(this);
  this->~TJson();
  Kind = Null;
  return *this;
}

TJson &TJson::Swap(TJson &that) noexcept {
  assert(this);
  assert(&that);
  TJson temp = std::move(*this);
  new (this) TJson(std::move(that));
  new (&that) TJson(std::move(temp));
  return *this;
}

const TJson *TJson::TryFind(const std::string &key) const {
  assert(this);
  assert(Kind == Object);
  auto iter = Object_.find(key);
  return (iter != Object_.end()) ? &(iter->second) : nullptr;
}

/* Format to the stream. */
void TJson::Write(std::ostream &strm) const {
  assert(this);
  assert(&strm);
  switch(Kind) {
    case Null: {
      strm << "null";
      break;
    }
    case Bool: {
      strm << (Bool_ ? "true" : "false");
      break;
    }
    case Number: {
      strm << std::fixed << Number_;
      strm.unsetf(std::ios_base::floatfield);
      break;
    }
    case Array: {
      strm << '[';
      bool sep = false;
      for(const auto &elem : Array_) {
        if(sep) {
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
      for(const auto &elem : Object_) {
        if(sep) {
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

std::istream &Base::operator>>(std::istream &strm, TJson &that) {
  assert(&that);
  that.Read(strm);
  return strm;
}

std::ostream &Base::operator<<(std::ostream &strm, const TJson &that) {
  assert(&that);
  that.Write(strm);
  return strm;
}

std::ostream &Base::operator<<(std::ostream &strm, const TJson::TKind &kind) {
  assert(&kind);
  switch(kind) {
    case TJson::Null: {
      strm << "null";
      break;
    }
    case TJson::Bool: {
      strm << "bool";
      break;
    }
    case TJson::Number: {
      strm << "number";
      break;
    }
    case TJson::Array: {
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
