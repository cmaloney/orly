/* <stig/type/managed_type.h>

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
#include <mutex>
#include <unordered_map>

#include <base/assert_true.h>
#include <base/hash.h>
#include <stig/type/impl.h>

#define IMPL_INTERNED_TYPE(TFinal, TArgs...) \
    template <> \
    std::recursive_mutex *::Stig::Type::TInternedType<TFinal, TArgs>::Mutex = nullptr; \
    template <> \
    typename ::Stig::Type::TInternedType<TFinal, TArgs>::TTypeByKey *::Stig::Type::TInternedType<TFinal, TArgs>::TypeByKey = nullptr;

#define IMPL_UNARY_TYPE(TFinal) \
    IMPL_INTERNED_TYPE(TFinal, ::Stig::Type::TType)

#define IMPL_SINGLETON_TYPE(TFinal) \
    template <> \
    TFinal::TPtr *::Stig::Type::TSingletonType<TFinal>::Ptr = nullptr;

namespace Stig {

  namespace Type {

    class TTypeCzar;

    //TODO: This is a candidate for invasive containment...
    template <typename TFinal, typename... TArgs>
    class TInternedType : public TType::TImpl {
      private:
      typedef std::shared_ptr<const TFinal> TPtr;
      typedef std::weak_ptr<const TFinal> TWeak;

      protected:
      typedef std::tuple<TArgs...> TKey;

      public:
      /* TODO: Sort of ug that people can see this */
      typedef std::unordered_map<TKey, TWeak> TTypeByKey;

      void Accept(const TType::TVisitor &visitor) const {
        assert(this);
        assert(&visitor);
        visitor(Base::AssertTrue(dynamic_cast<const TFinal*>(this)));
      }

      protected:

      /* TODO */
      template <typename... TCompatArgs>
      static TType Get(TCompatArgs &&... args) {
        assert(Mutex);  // If this fails, you likely don't have an instance of TTypeCzar.
        assert(TypeByKey);  // If this fails, you likely don't have an instance of TTypeCzar.
        std::lock_guard<std::recursive_mutex> lock(*Mutex);
        TWeak &weak = (*TypeByKey)[TKey(args...)];
        TPtr strong = weak.lock();
        if (!strong) {
          strong = TPtr(new TFinal(std::forward<TCompatArgs>(args)...), TInternedType::DeleteImpl);
          weak = strong;
        }
        return strong->AsType();
      }

      template <typename... TCompatArgs>
      TInternedType(TCompatArgs &&...key) : Key(std::forward<TCompatArgs>(key)...) {}

      ~TInternedType() {
        assert(this);
        if (TypeByKey) {
          TypeByKey->erase(Key);
        }
      }

      /* TODO */
      static void DeleteImpl(TInternedType *impl) {
        assert(impl);
        if (Mutex) {
          if (Mutex->try_lock()) {
            if (impl->CanDeleteImpl()) {
              delete impl;
            }
            Mutex->unlock();
          }
        } else {
          delete impl;
        }
      }

      bool CanDeleteImpl() const {
        assert(this);

        //TODO: Would be nice to use the weak pointer we get from TType::TImpl's shared_from_this here.

        /* quick fix to get indy running :s */
        auto res = TypeByKey->find(Key);
        return res != TypeByKey->end() ? res->second.expired() : true;

        #if 0
        TWeak &result = TypeByKey->find(Key)->second;
        TPtr ptr = result.lock();
        return ptr == 0;
        #endif
      }

      const TKey &GetKey() const {
        assert(this);
        return Key;
      }

      private:

      static void New() {
        assert(!Mutex);
        assert(!TypeByKey);
        Mutex = new std::recursive_mutex();
        TypeByKey = new TTypeByKey();
      }

      static void Delete() {
        assert(TypeByKey);
        assert(Mutex);
        delete TypeByKey;
        delete Mutex;
        TypeByKey = nullptr;
        Mutex = nullptr;
      }

      /* Interner storage */
      static std::recursive_mutex *Mutex;

      static TTypeByKey *TypeByKey;

      // TODO: That we store the key both here and in the map is less than ideal.
      TKey Key;

      friend class ::Stig::Type::TTypeCzar;

    };  // TInternedType<TFinal, TArgs...>

    template <typename TFinal>
    class TSingletonType : public TType::TImpl {
      public:
      typedef std::shared_ptr<const TFinal> TPtr;

      static TType Get() {
        assert(Ptr);  // If this fails, you likely don't have an instance of TTypeCzar.
        return (*Ptr)->AsType();
      }

      void Accept(const TType::TVisitor &visitor) const {
        assert(&visitor);
        assert(Ptr);
        visitor(Ptr->get());
      }

      protected:
      TSingletonType() {}

      ~TSingletonType() {}

      static TPtr *Ptr;

      private:

      static void New() {
        assert(!Ptr);
        Ptr = new TPtr(new TFinal());
      }

      static void Delete() {
        assert(Ptr);
        delete Ptr;
        Ptr = nullptr;
      }

      friend class ::Stig::Type::TTypeCzar;

    };  // TSingletonType<TFinal>

    template <typename TFinal>
    class TUnaryType : public TInternedType<TFinal, Type::TType> {
      public:

      static TType Get(const Type::TType &type) {
        assert(&type);
        return TInternedType<TFinal, Type::TType>::Get(type);
      }

      const TType &GetElem() const {
        assert(this);
        //NOTE: The this-> really shuoldn't be necessary...
        return std::get<0>(this->GetKey());
      }

      protected:

      //TODO: Perfect forwarding variant.
      TUnaryType(const Type::TType &elem) : TInternedType<TFinal, Type::TType>(elem) {}

    };  // TUnaryType<TFinal>

  }  // Type

}  // Stig
