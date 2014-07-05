/* <orly/csv_to_bin/symbol/kit.h>

   Symbols representing a CSV table to be imported, its columns, and its
   primary and secondary keys.

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
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <base/class_traits.h>
#include <base/thrower.h>

namespace Orly {

  namespace CsvToBin {

    namespace Symbol {

      /* Thrown various places here. */
      DEFINE_ERROR(TSemanticError, std::runtime_error, "semantic error");

      /* The types of columns we accept.  This is a subset of Orly. */
      enum class TType {
        Bool, Id, Int, Real, Str, TimeDiff, TimePnt
      };

      /* Forward-declared for the benefit of TTable. */
      class TCol;
      class TKey;
      class TPrimaryKey;
      class TSecondaryKey;

      /* A table to be imported.  After you construct this, add columns to
         it, set its primary key, add any secondary keys you want, and
         call Verify() to make sure it's all ok. */
      class TTable final {
        NO_COPY(TTable);
        public:

        /* Do-little. */
        TTable() {}

        /* Add the given column to the table.  If the column's name
           conflicts with that of another column, we throw.  */
        void AddCol(std::unique_ptr<TCol> &&col);

        /* Add the given secondary key to the table.  If the key's name
           conflicts with that of another key, we throw.  */
        void AddSecondaryKey(std::unique_ptr<TSecondaryKey> &&key);

        /* Return the named column or throw.  Never null. */
        const TCol *FindCol(const std::string &name) const;

        /* Call back for each column. */
        bool ForEachCol(
            const std::function<bool (const TCol *)> &cb) const {
          assert(this);
          assert(&cb);
          assert(cb);
          for (const auto &col: Cols) {
            if (!cb(col.get())) {
              return false;
            }
          }
          return true;
        }

        /* Call back for each column that is not part of the given key. */
        bool ForEachColNotInKey(
            const std::function<bool (const TCol *)> &cb, const TKey *key) const;

        /* Returns the number of columns not covered by the given key. */
        size_t NumColNotCoveredByKey(const TKey *key) const {
          assert(this);
          assert(key);
          size_t n = 0UL;
          ForEachColNotInKey([&](const TCol *) -> bool {
            ++n;
            return true;
          }, key);
          return n;
        }

        /* Call back for each secondary key. */
        bool ForEachSecondaryKey(
            const std::function<bool (const TSecondaryKey *)> &cb) const {
          assert(this);
          assert(&cb);
          assert(cb);
          for (const auto &key: SecondaryKeys) {
            if (!cb(key.get())) {
              return false;
            }
          }
          return true;
        }

        /* The number of columns in the table. */
        size_t GetColCount() const noexcept {
          assert(this);
          return Cols.size();
        }

        /* The primary key of the table.  Never null. */
        const TPrimaryKey *GetPrimaryKey() const noexcept {
          assert(this);
          assert(PrimaryKey);
          return PrimaryKey.get();
        }

        /* The number of secondary keys in the table. */
        size_t GetSecondaryKeyCount() const noexcept {
          assert(this);
          return SecondaryKeys.size();
        }

        /* Assign the table its primary key.  You must do this exactly
           once. */
        void SetPrimaryKey(std::unique_ptr<TPrimaryKey> &&key) {
          assert(this);
          assert(&key);
          assert(key);
          if (PrimaryKey) {
            THROW_ERROR(TSemanticError)
                << "primary key already set";
          }
          PrimaryKey = std::move(key);
        }

        /* Return the named column, or null if it doesn't exist. */
        const TCol *TryFindCol(const std::string &name) const;

        /* Return the named secondary key, or null if it doesn't exist. */
        const TSecondaryKey *TryFindSecondaryKey(
            const std::string &name) const;

        /* If the table has columns and a primary key, do nothing; otherwise,
           throw. */
        void Verify() const;

        private:

        /* The columns in the order added. */
        std::vector<std::unique_ptr<TCol>> Cols;

        /* The primary key, when set. */
        std::unique_ptr<TPrimaryKey> PrimaryKey;

        /* The secondary keys in the order added. */
        std::vector<std::unique_ptr<TSecondaryKey>> SecondaryKeys;

      };  // TTable

      /* A column of a table. */
      class TCol final {
        NO_COPY(TCol);
        public:

        /* Cache the arguments. */
        TCol(std::string name, TType type, bool is_null)
            : Name(std::move(name)), Type(type), Null(is_null) {}

        /* The name of the column for the purposes of reference. */
        const std::string &GetName() const noexcept {
          assert(this);
          return Name;
        }

        /* The type stored in this column. */
        TType GetType() const noexcept {
          assert(this);
          return Type;
        }

        /* True iff. this column can contain an explicit null. */
        bool IsNull() const noexcept {
          assert(this);
          return Null;
        }

        private:

        /* See accessor. */
        std::string Name;

        /* See accessor. */
        TType Type;

        /* See accessor. */
        bool Null;

      };  // TCol

      /* The base for primary and secondary keys. */
      class TKey {
        NO_COPY(TKey);
        public:

        /* The order in which to index a column's contents. */
        enum TOrder { Asc, Desc };

        /* A field within the index. */
        struct TField {
          const TCol *Col;
          TOrder Order;
        };  // TKey::TField

        /* Do-little. */
        virtual ~TKey() {}

        /* The fields indexed, in order. */
        const std::vector<TField> &GetFields() const noexcept {
          assert(this);
          return Fields;
        }

        protected:

        /* Cache the fields.  The column pointers must not be null. */
        TKey(std::vector<TField> fields)
            : Fields(std::move(fields)) {}

        private:

        /* See accessor. */
        std::vector<TField> Fields;

      };  // TKey

      /* A primary key of a table. */
      class TPrimaryKey final
          : public TKey {
        public:

        /* Cache the fields. */
        TPrimaryKey(std::vector<TField> fields)
            : TKey(std::move(fields)) {}

      };  // TPrimaryKey

      /* A secondary key of a table. */
      class TSecondaryKey final
          : public TKey {
        public:

        /* Cache the name and the fields. */
        TSecondaryKey(std::string name, std::vector<TField> fields)
            : TKey(std::move(fields)), Name(std::move(name)) {}

        /* The name of the column for the purposes of reference. */
        const std::string &GetName() const noexcept {
          assert(this);
          return Name;
        }

        private:

        /* See accessor. */
        std::string Name;

      };  // TSecondaryKey

    }  // Symbol

  }  // Csv2Bin

}  // Orly
