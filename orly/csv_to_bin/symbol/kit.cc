/* <orly/csv_to_bin/symbol/kit.cc>

   Implements <orly/csv_to_bin/symbol/kit.h>.

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

#include <orly/csv_to_bin/symbol/kit.h>

using namespace std;
using namespace Orly::CsvToBin::Symbol;

void TTable::AddCol(std::unique_ptr<TCol> &&col) {
  assert(this);
  assert(&col);
  assert(col);
  if (TryFindCol(col->GetName())) {
    THROW_ERROR(TSemanticError)
      << "multiple definitions for column \"" << col->GetName() << '"';
  }
  Cols.push_back(std::move(col));
}

void TTable::AddSecondaryKey(std::unique_ptr<TSecondaryKey> &&key) {
  assert(this);
  assert(&key);
  assert(key);
  if (TryFindSecondaryKey(key->GetName())) {
    THROW_ERROR(TSemanticError)
      << "multiple definitions for secondary key \"" << key->GetName() << '"';
  }
  SecondaryKeys.push_back(std::move(key));
}

void TTable::AddText(const TCol *col) {
  assert(this);
  assert(col);
  if (col->GetType() != TType::Str) {
    THROW_ERROR(TSemanticError)
      << "cannot make column \"" << col->GetName() << "\" text-searchable; "
         "it is not a string";
  }
  if (!Texts.insert(col).second) {
    THROW_ERROR(TSemanticError)
      << "cannot make column \"" << col->GetName() << "\" text-searchable; "
         "it is already text-searchable";
  }
}

const TCol *TTable::FindCol(const std::string &name) const {
  assert(this);
  const auto *col = TryFindCol(name);
  if (!col) {
    THROW_ERROR(TSemanticError)
      << "no definition for column \"" << name << '"';
  }
  return col;
}

bool TTable::ForEachColNotInKey(
    const std::function<bool (const TCol *)> &cb, const TKey *key) const {
  assert(this);
  assert(&cb);
  assert(cb);
  assert(&key);
  for (const auto &col: Cols) {
    bool found = false;
    for (const auto &field : key->GetFields()) {
      if (field.Col == col.get()) {
        found = true;
        break;
      }
    }
    if (!found && !cb(col.get())) {
      return false;
    }
  }
  return true;
}

size_t TTable::NumColNotCoveredByKey(const TKey *key) const {
  assert(this);
  assert(key);
  return GetColCount() - key->GetFields().size();
}

const TCol *TTable::TryFindCol(const string &name) const {
  assert(this);
  for (const auto &col: Cols) {
    if (col->GetName() == name) {
      return col.get();
    }
  }
  return nullptr;
}

const TSecondaryKey *TTable::TryFindSecondaryKey(const string &name) const {
  assert(this);
  for (const auto &key: SecondaryKeys) {
    if (key->GetName() == name) {
      return key.get();
    }
  }
  return nullptr;
}

void TTable::Verify() const {
  assert(this);
  if (Cols.empty()) {
    THROW_ERROR(TSemanticError) << "no columns defined";
  }
  if (!PrimaryKey) {
    THROW_ERROR(TSemanticError) << "no primary key defined";
  }
}
