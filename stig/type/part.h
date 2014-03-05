/* <stig/type/part.h>

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

#include <memory>
#include <string>

#include <base/interner.h>
#include <base/safe_global.h>
#include <stig/type/impl.h>

namespace Stig {

  namespace Type {

    class TAddrMember;
    class TListIndex;
    class TDictMember;
    class TObjMember;

    class TPart {
      public:

      class TVisitor {
        public:

        virtual ~TVisitor() {}

        virtual void operator()(const TAddrMember *that) const = 0;
        virtual void operator()(const TListIndex *that) const = 0;
        virtual void operator()(const TDictMember *that) const = 0;
        virtual void operator()(const TObjMember  *that) const = 0;

        protected:

        TVisitor() {}

      };  // TVisitor

      typedef std::shared_ptr<const TPart> TPtr;

      virtual ~TPart() {}

      virtual void Accept(const TVisitor &visitor) const = 0;

      protected:

      TPart() {}

    };  // TPart

    class TAddrMember
        : public TPart {
      public:

      typedef std::shared_ptr<const TAddrMember> TPtr;

      static TPtr Get(size_t index) {
        return Interner->Get(index);
      }

      size_t GetIndex() const {
        assert(this);
        return Index;
      }

      void Accept(const TVisitor &visitor) const {
        assert(this);
        assert(&visitor);
        visitor(this);
      }

      private:

      static Base::TSafeGlobal<Base::TInterner<TAddrMember, size_t>> Interner;

      TAddrMember(size_t index)
          : Index(index) {}

      size_t Index;

      friend class Base::TInterner<TAddrMember, size_t>;

    };  // TAddrMember

    class TListIndex
        : public TPart {
      public:

      typedef std::shared_ptr<const TListIndex> TPtr;

      static const Base::TSafeGlobal<TPtr> ListIndex;

      void Accept(const TVisitor &visitor) const {
        assert(this);
        assert(&visitor);
        visitor(this);
      }

      private:

      TListIndex() {}

      friend TPtr *GetNewListIndex();

    };  // TListIndex

    class TDictMember
        : public TPart {
      public:

      typedef std::shared_ptr<const TDictMember> TPtr;

      static TPtr Get(const Type::TType &key_type) {
        return Interner->Get(key_type);
      }

      void Accept(const TVisitor &visitor) const {
        assert(this);
        assert(&visitor);
        visitor(this);
      }

      const Type::TType &GetKeyType() const {
        assert(this);
        return KeyType;
      }

      private:

      static Base::TSafeGlobal<Base::TInterner<TDictMember, Type::TType>> Interner;

      TDictMember(const Type::TType &key_type)
          : KeyType(key_type) {}

      Type::TType KeyType;

      friend class Base::TInterner<TDictMember, Type::TType>;

    };  // TDictMember

    class TObjMember
        : public TPart {
      public:

      typedef std::shared_ptr<const TObjMember> TPtr;

      static TPtr Get(const std::string &name) {
        return Interner->Get(name);
      }

      void Accept(const TVisitor &visitor) const {
        assert(this);
        assert(&visitor);
        visitor(this);
      }

      const std::string &GetName() const {
        assert(this);
        return Name;
      }

      private:

      static Base::TSafeGlobal<Base::TInterner<TObjMember, std::string>> Interner;

      TObjMember(const std::string &name)
          : Name(name) {}

      std::string Name;

      friend class Base::TInterner<TObjMember, std::string>;

    };  // TObjMember

  }  // Type

}  // Stig
