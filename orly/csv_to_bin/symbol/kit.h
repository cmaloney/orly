/* <orly/csv_to_bin/symbol/kit.h>

   Generates C++ code for a CSV importer.

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

      /* TODO */
      DEFINE_ERROR(TSemanticError, std::runtime_error, "semantic error");

      /* TODO */
      enum class TType {
        Bool, Id, Int, Real, Str, TimeDiff, TimePnt
      };

      /* TODO */
      class TCol;
      class TPrimaryKey;
      class TSecondaryKey;

      /* TODO */
      class TTable final {
        NO_COPY(TTable);
        public:

        /* Do-little. */
        TTable() {}

        /* TODO */
        void AddCol(std::unique_ptr<TCol> &&col);

        /* TODO */
        void AddSecondaryKey(std::unique_ptr<TSecondaryKey> &&key);

        /* TODO */
        const TCol *FindCol(const std::string &name) const;

        /* TODO */
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

        /* TODO */
        size_t GetColCount() const noexcept {
          assert(this);
          return Cols.size();
        }

        /* TODO */
        const TPrimaryKey *GetPrimaryKey() const noexcept {
          assert(this);
          assert(PrimaryKey);
          return PrimaryKey.get();
        }

        /* TODO */
        size_t GetSecondaryKeyCount() const noexcept {
          assert(this);
          return SecondaryKeys.size();
        }

        /* TODO */
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

        /* TODO */
        const TCol *TryFindCol(const std::string &name) const;

        /* TODO */
        const TSecondaryKey *TryFindSecondaryKey(
            const std::string &name) const;

        /* TODO */
        void Verify() const;

        private:

        /* TODO */
        std::vector<std::unique_ptr<TCol>> Cols;

        /* TODO */
        std::unique_ptr<TPrimaryKey> PrimaryKey;

        /* TODO */
        std::vector<std::unique_ptr<TSecondaryKey>> SecondaryKeys;

      };  // TTable

      /* TODO */
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

      /* TODO */
      class TKey {
        NO_COPY(TKey);
        public:

        /* TODO */
        enum TOrder { Asc, Desc };

        /* TODO */
        struct TField {
          const TCol *Col;
          TOrder Order;
        };  // TKey::TField

        /* TODO */
        virtual ~TKey() {}

        /* TODO */
        const std::vector<TField> &GetFields() const noexcept {
          assert(this);
          return Fields;
        }

        protected:

        /* TODO */
        TKey(std::vector<TField> fields)
            : Fields(std::move(fields)) {}

        private:

        /* See accessor. */
        std::vector<TField> Fields;

      };  // TKey

      /* TODO */
      class TPrimaryKey final
          : public TKey {
        public:

        /* TODO */
        TPrimaryKey(std::vector<TField> fields)
            : TKey(std::move(fields)) {}

      };  // TPrimaryKey

      /* TODO */
      class TSecondaryKey final
          : public TKey {
        public:

        /* TODO */
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