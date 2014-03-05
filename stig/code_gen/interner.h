/* <stig/code_gen/interner.h>

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

#include <memory>

#include <base/hash.h>
#include <base/interner.h>
#include <base/no_copy_semantics.h>
#include <stig/code_gen/basic_ctor.h>
#include <stig/code_gen/binary.h>
#include <stig/code_gen/built_in_call.h>
#include <stig/code_gen/call.h>
#include <stig/code_gen/exists.h>
#include <stig/code_gen/scope.h>
#include <stig/code_gen/context_var.h>
#include <stig/code_gen/implicit_func.h>
#include <stig/code_gen/keys.h>
#include <stig/code_gen/literal.h>
#include <stig/code_gen/member.h>
#include <stig/code_gen/range.h>
#include <stig/code_gen/slice.h>
#include <stig/code_gen/sort.h>
#include <stig/code_gen/typed_leaf.h>
#include <stig/code_gen/unary.h>

namespace Stig {

  namespace CodeGen {

    class TInterner {
      NO_COPY_SEMANTICS(TInterner);
      public:

      //Note: TInterner is constructed by and exclusively owned by TCodeScope.

      //TODO: It would be nice to remove these trivial wrappers.
      template <typename... TArgs>
      TBasicCtor<TAddrContainer>::TPtr GetAddrCtor(TArgs &&... args) {
        return Get(AddrInterner, std::forward<TArgs>(args)...);
      }

      template <typename... TArgs>
      TLiteral::TPtr GetAddrMember(TArgs &&... args) {
        return Get(AddrMemberInterner, std::forward<TArgs>(args)...);
      }

      template <typename... TArgs>
      TBinary::TPtr GetBinary(TArgs &&... args) {
        return Get(BinaryInterner, std::forward<TArgs>(args)...);
      }

      template <typename... TArgs>
      TBuiltInCall::TPtr GetBuiltInCall(TArgs &&...args) {
        return Get(BuiltInCallInterner, std::forward<TArgs>(args)...);
      }

      template <typename... TArgs>
      TCall::TPtr GetCall(TArgs &&... args) {
        return Get(CallInterner, std::forward<TArgs>(args)...);
      }

      TContextVar::TPtr GetContextVar(const L0::TPackage *package, TContextVar::TOp op) {
        return ContextVarInterner.Get(package, op);
      }

      /* TODO: Requres dictionary comparison...
      template <typename... TArgs>
      TBasicCtor<TDictContainer>::TPtr GetDictCtor(const Type::TType &type, TDictContainer &&elems) {
        return DictInterner.Get(type, elems);
      } */

      template <typename...TArgs>
      TKeys::TPtr GetKeys(TArgs &&... args) {
        return Get(KeysInterner, std::forward<TArgs>(args)...);
      }

      template <typename... TArgs>
      TBasicCtor<TListContainer>::TPtr GetListCtor(TArgs &&... args) {
        return Get(ListInterner, std::forward<TArgs>(args)...);
      }

      template <typename... TArgs>
      TLiteral::TPtr GetLiteral(TArgs &&... args) {
        return Get(LiteralInterner, std::forward<TArgs>(args)...);
      }

      /* TODO: Requires set comparison...
      template <typename... TArgs>
      TMap::TPtr GetMap(TArgs &&...args) {
        return Get(MapInterner, std:::forward<TArgs>(args)...);
      } */

      template <typename... TArgs>
      TLiteral::TPtr GetObjMember(TArgs &&... args) {
        return Get(ObjMemberInterner, std::forward<TArgs>(args)...);
      }

      template <typename... TArgs>
      TRange::TPtr GetRange(TArgs &&... args) {
        return Get(RangeInterner, std::forward<TArgs>(args)...);
      }

      /* TODO: Requires set comparison...
      TBasicCtor<TSetContainer>::TPtr GetSetCtor(const Type::TType &type, TSetContainer &&elems) {
        return SetInterner.Get(type, std::move(elems));
      } */

      template <typename... TArgs>
      TSlice::TPtr GetSlice(TArgs &&... args) {
        return Get(SliceInterner, std::forward<TArgs>(args)...);
      }

      template <typename... TArgs>
      TSort::TPtr GetSort(TArgs &&... args) {
        return Get(SortInterner, std::forward<TArgs>(args)...);
      }

      template <typename... TArgs>
      TTypedLeaf::TPtr GetTypedLeaf(TArgs &&... args) {
        return Get(TypedLeafInterner, std::forward<TArgs>(args)...);
      }

      template <typename... TArgs>
      TLiteral::TPtr GetExists(TArgs &&... args) {
        return Get(ExistsInterner, std::forward<TArgs>(args)...);
      }

      template <typename... TArgs>
      TLiteral::TPtr GetUnary(TArgs &&... args) {
        return Get(UnaryInterner, std::forward<TArgs>(args)...);
      }

      private:
      /* TODO: This isn't as simple and clean as I would like it to be, but should be usable. */
      template <typename TObj_, typename... TArgs>
      class TStorage {
        NO_COPY_SEMANTICS(TStorage);
        public:

        typedef std::tuple<TArgs...> TKey;
        typedef TObj_ TObj;
        typedef std::shared_ptr<const TObj> TPtr;
        typedef std::weak_ptr<const TObj> TWeak;

        //TODO: The redundant storage of a scope pointer in each holder is less than ideal.
        TStorage() = default;

        /* TODO */
        std::unordered_map<TKey, std::weak_ptr<const TObj>> ObjByKey;
      };

      TInterner(TCodeScope *scope) : Scope(scope) {}

      template <typename TStore, typename... TCompatArgs>
      typename TStore::TPtr Get(TStore &store, TCompatArgs &&...args) {
        assert(this);
        typedef typename TStore::TWeak TWeak;
        typedef typename TStore::TPtr TPtr;
        //TOOD: This is copied from Base::TInterner (With just AddLocal logic added).
        TWeak &weak_ptr = store.ObjByKey.insert(std::make_pair(typename TStore::TKey(args...), TWeak())).first->second;
        TPtr shared_ptr = weak_ptr.lock();
        bool is_new = !shared_ptr;
        if (is_new) {
          shared_ptr = std::make_shared<const typename TStore::TObj>(std::forward<TCompatArgs>(args)...);
          weak_ptr = shared_ptr;
        } else {
          if(!shared_ptr->HasId()) {
            Scope->AddLocal(shared_ptr);
          }
        }

        return shared_ptr;
      }

      template <typename TStore, typename... TCompatArgs>
      bool Has(TStore &store, TCompatArgs &&...args) {
        assert(this);

        auto it = store.ObjByKey.find(TKey(args...));
        return it == store.ObjByKey.end() ? false : !it->second.expired();
      }

      TCodeScope *Scope;

      //TODO: This is not exactly efficient. Oh well.
      //TODO: The return type should not be part of the interner. It is defined by the two parameters.
      TStorage<TBasicCtor<TAddrContainer>, const L0::TPackage *, Type::TType, TAddrContainer> AddrInterner;
      TStorage<TAddrMember, const L0::TPackage *, Type::TType, TInline::TPtr, size_t> AddrMemberInterner;
      TStorage<TBinary, const L0::TPackage *, Type::TType, TBinary::TOp, TInline::TPtr, TInline::TPtr> BinaryInterner;
      TStorage<TBuiltInCall, const L0::TPackage *, TBuiltInCall::TFunctionPtr, TBuiltInCall::TArguments> BuiltInCallInterner;
      TStorage<TCall, const L0::TPackage *, TFunction::TPtr, TCall::TArgs> CallInterner;
      Base::TInterner<TContextVar, const L0::TPackage *, TContextVar::TOp> ContextVarInterner;
      //TODO (See Getter): TStorage<TBasicCtor<TDictContainer>, Type::TType, TDictContainer> DictInterner;
      TStorage<TBasicCtor<TListContainer>, const L0::TPackage *, Type::TType, TListContainer> ListInterner;
      TStorage<TKeys, const L0::TPackage *, Type::TType, Type::TType, TKeys::TAddrElems> KeysInterner;
      TStorage<TLiteral, const L0::TPackage *, Var::TVar> LiteralInterner;
      //TODO (See Getter): TStorage<TMap, Type::TType, TMap::TSeqs, TImplicitFunc::TPtr> MapInterner;
      TStorage<TObjMember, const L0::TPackage *, Type::TType, TInline::TPtr, std::string> ObjMemberInterner;
      TStorage<TRange, const L0::TPackage *, TInline::TPtr, TInline::TPtr, TInline::TPtr, bool> RangeInterner;
      //TODO (See Getter): TStorage<TBasicCtor<TSetContainer>, Type::TType, TSetContainer> SetInterner;
      TStorage<TSlice, const L0::TPackage *, Type::TType, TInline::TPtr, TInline::TPtr, TInline::TPtr, bool> SliceInterner;
      TStorage<TSort, const L0::TPackage *, Type::TType, TInline::TPtr, TImplicitFunc::TPtr> SortInterner;
      TStorage<TTypedLeaf, const L0::TPackage *, TTypedLeaf::TKind, Type::TType, TAddrDir> TypedLeafInterner;
      TStorage<TExists, const L0::TPackage *, Type::TType, Type::TType, TInline::TPtr> ExistsInterner;
      TStorage<TUnary, const L0::TPackage *, Type::TType, TUnary::TOp, TInline::TPtr> UnaryInterner;

      friend class TCodeScope;
    };


  } // CodeGen

} // Stig