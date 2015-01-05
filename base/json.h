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

#include <cstdint>
#include <functional>
#include <istream>
#include <map>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <base/exception.h>

namespace Base {

/* A sum of the value types allowable in JSON. */
class TJson final {
  public:
  /* Thrown when extracting from a malformed stream. */
  DEFINE_ERROR(TSyntaxError, std::runtime_error, "json syntax error");

  /* Aliases for constructed types from which we may construct. */
  using TArray = std::vector<TJson>;
  using TObject = std::map<std::string, TJson>;
  using TString = std::string;

  /* The types of callbacks used by ForEachElem(). */
  using TArrayCb = std::function<bool(const TJson &)>;
  using TObjectCb = std::function<bool(const std::string &, const TJson &)>;
  using TArrayCbNonConst = std::function<bool(TJson &)>;
  using TObjectCbNonConst = std::function<bool(const std::string &, TJson &)>;

  /* The kinds of states we can be in. */
  enum TKind { Null, Bool, Number, Array, Object, String };

  static TJson Read(const char *filename);

  /* Construct as a default instance of the given kind. */
  TJson(TKind kind = Null) noexcept;

  /* The donor is left null. */
  TJson(TJson &&that) noexcept;

  /* Deep Copy */
  TJson(const TJson &that);

  /* Construct as the bool. */
  TJson(bool that) noexcept;

  /* Construct as the number */
  TJson(int8_t that) noexcept;

  /* Construct as the number */
  TJson(int16_t that) noexcept;

  /* Construct as the number */
  TJson(int32_t that) noexcept;

  /* Construct as the number */
  TJson(int64_t that) noexcept;

  /* Construct as the number */
  TJson(uint8_t that) noexcept;

  /* Construct as the number */
  TJson(uint16_t that) noexcept;

  /* Construct as the number */
  TJson(uint32_t that) noexcept;

  /* Construct as the number */
  TJson(uint64_t that) noexcept;

  /* Construct as an array, leaving the donor empty. */
  TJson(TArray &&that) noexcept;

  /* Construct as a copy of the array. */
  TJson(const TArray &that);

  /* Construct as an object, leaving the donor empty. */
  TJson(TObject &&that) noexcept;

  /* Construct as a copy of the object. */
  TJson(const TObject &that);

  /* Construct as a string of a single character. */
  TJson(char that);

  /* Construct as a string of a single character. */
  TJson(const char *that);

  /* Construct as a string, leaving the donor empty. */
  TJson(TString &&that) noexcept;

  /* Construct as a copy of the string. */
  TJson(const TString &that);

  /* Construct an array of copies of the example. */
  TJson(size_t size, const TJson &example);

  /* Contained elements, if any, are also destroyed. */
  ~TJson();

  /* The donor is left null. */
  TJson &operator=(TJson &&that) noexcept;

  /* Deep-copy. */
  TJson &operator=(const TJson &that);

  /* True iff. this object and that one are in the same state. */
  bool operator==(const TJson &that) const noexcept;

  /* True iff. this object and that one are not in the same state. */
  bool operator!=(const TJson &that) const noexcept;

  /* Subscript to an element contained in an array. */
  TJson &operator[](size_t that);

  /* Subscript to an element contained in an array. */
  const TJson &operator[](size_t that) const;

  /* Find or create an element contained in an object. */
  TJson &operator[](TString &&that);

  /* Find or create an element contained in an object. */
  TJson &operator[](const TString &that);

  /* Find or create an element contained in an object. */
  const TJson &operator[](const TString &that) const;

  /* Returns true if the object contains the given key */
  bool Contains(const TString &that) const;

  /* The kind of state we're in. */
  TKind GetKind() const noexcept;

  /* The number of elements in an array or object or the number of
     characters in a string. */
  size_t GetSize() const noexcept;

  const TArray &GetArray() const noexcept;
  TArray &GetArray() noexcept;
  bool GetBool() const noexcept;
  const TObject &GetObject() const noexcept;
  TObject &GetObject() noexcept;
  int64_t GetNumber() const noexcept;
  const TString &GetString() const noexcept;

  /* Parse from the stream. */
  void Read(std::istream &strm);

  /* Return to the default-constructed state (which is null). */
  TJson &Reset() noexcept;

  /* Swap states. */
  TJson &Swap(TJson &that) noexcept;

  /* A pointer to the named element in the object, or null if we have no
     such element. */
  const TJson *TryFind(const std::string &key) const;

  /* Format to the stream. */
  void Write(std::ostream &strm) const;

  private:
  /* See accessor. */
  TKind Kind;

  /* The union of our potential states.  At most one of these is valid
     at a time.  'Kind', above, determines which, if any, is valid. */
  union {
    bool Bool_;
    int64_t Number_;
    TObject Object_;
    TArray Array_;
    std::string String_;
  };

};  // TJson

/* Std stream extractor. */
std::istream &operator>>(std::istream &strm, TJson &that);

/* Std stream inserter. */
std::ostream &operator<<(std::ostream &strm, const TJson &that);
std::ostream &operator<<(std::ostream &strm, const TJson::TKind &kind);

}  // Base

namespace std {

/* Standard swapper. */
template <>
inline void swap<Base::TJson>(Base::TJson &lhs, Base::TJson &rhs) noexcept {
  lhs.Swap(rhs);
}
}
