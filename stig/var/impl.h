/* <stig/var/impl.h>

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

#include <algorithm>
#include <cassert>
#include <chrono>
#include <memory>
#include <stddef.h>
#include <unordered_map>

#include <base/chrono.h>
#include <base/error.h>
#include <base/no_copy_semantics.h>
#include <stig/shared_enum.h>
//NOTE: We would include <stig/rt.h> but that includes <stig/rt/built_in.h> which includes this
#include <stig/rt/containers.h>
#include <stig/rt/error.h>
#include <stig/type/impl.h>
#include <stig/uuid.h>

namespace Stig {

  namespace Var {

    /* TODO */
    class TAddr;
    class TBool;
    class TDict;
    class TErr;
    class TFree;
    class TId;
    class TInt;
    class TList;
    class TMutable;
    class TObj;
    class TOpt;
    class TReal;
    class TSet;
    class TStr;
    class TTimeDiff;
    class TTimePnt;
    class TUnknown;

    // template <typename TVal> struct TRead;

    /* TODO */
    class TVar {
      public:

      /* TODO */
      template <typename TVal>
      struct TDt {

        /* TODO */
        TVal static As(const TVar &);

      };

      /* TODO */
      class TVisitor;

      /* TODO */
      class TDoubleVisitor;

      /* TODO */
      class TImpl {
        NO_COPY_SEMANTICS(TImpl);
        public:

        /* TODO */
        virtual Var::TVar &Index(const TVar &) = 0;

        /* TODO */
        virtual TImpl &Add(const TVar &) = 0;

        /* TODO */
        virtual TImpl &And(const TVar &) = 0;

        /* TODO */
        virtual TImpl &Div(const TVar &) = 0;

        /* TODO */
        virtual TImpl &Exp(const TVar &) = 0;

        /* TODO */
        virtual TImpl &Intersection(const TVar &) = 0;

        /* TODO */
        virtual TImpl &Mod(const TVar &) = 0;

        /* TODO */
        virtual TImpl &Mult(const TVar &) = 0;

        /* TODO */
        virtual TImpl &Or(const TVar &) = 0;

        /* TODO */
        virtual TImpl &Sub(const TVar &) = 0;

        /* TODO */
        virtual TImpl &SymmetricDiff(const TVar &) = 0;

        /* TODO */
        virtual TImpl &Union(const TVar &) = 0;

        /* TODO */
        virtual TImpl &Xor(const TVar &) = 0;

        protected:

        /* TODO */
        typedef TVar::TVisitor TVisitor;

        /* TODO */
        TImpl() {}

        /* TODO */
        virtual ~TImpl();

        /* TODO */
        TVar AsVar() {
          return TVar(this);
        }

        /* TODO */
        virtual void Accept(const TVisitor &visitor) const = 0;

        /* TODO */
        virtual TVar Copy() const = 0;

        /* TODO */
        virtual size_t GetHash() const = 0;

        /* TODO */
        virtual Type::TType GetType() const = 0;

        /* TODO */
        virtual void Touch() = 0;

        /* TODO */
        virtual void Write(std::ostream &) const = 0;

        private:

        /* TODO */
        static void Delete(TImpl *impl);

        /* TODO */
        friend class TVar;

      };  // TImpl

      /* Construct a new TVar as an unknown.
         See <stig/var/unknown.h>. */
      TVar();

      /* TODO */
      ~TVar();

      /* Construct a new TVar from a bool.
         See <stig/var/bool.h>. */
      TVar(const bool &that);

      /* Construct a new TVar from an int.
         See <stig/var/int.h>. */
      TVar(const int64_t &that);
      TVar(const int &that);

      /* Construct a new TVar from a double.
         See <stig/var/real.h>. */
      TVar(const double &that);

      /* Construct a new TVar from a std::string.
         See <stig/var/str.h>. */
      TVar(const std::string &that);

      /* Construct a new TVar from a uuid.
         See <stig/var/id.h>. */
      TVar(const TUUID &that);

      /* Construct a new TVar from a time_diff.
         See <stig/var/time_diff.h>. */
      TVar(const Base::Chrono::TTimeDiff &that);

      /* Construct a new TVar from a time_pnt.
         See <stig/var/time_pnt.h>. */
      TVar(const Base::Chrono::TTimePnt &that);

      /* TODO */
      template <typename TVal>
      TVar(const Rt::TSet<TVal> &that) {
        *this = Set(that);
      }

      /* TODO */
      template <typename TVal>
      TVar(const std::vector<TVal> &that) {
        *this = List(that);
      }

      /*  Construct a new TVar from a rt mutable.
         See <stig/var/mutable.h>. */
      template <typename TAddr, typename TVal>
      TVar(const Rt::TMutable<TAddr, TVal> &that) {
        *this = Mutable(that);
      }

      /* TODO */
      template <typename TKey, typename TVal>
      TVar(const Rt::TDict<TKey, TVal> &that) {
        *this = Dict(that);
      }

      /* TODO */
      template <typename TVal>
      TVar(const Rt::TError<TVal> &that);

      /* TODO
      template <typename... TElements>
      TVar(const Rt::TAddr<TElements...> &that) {
        *this = Addr(that);
      } */

      /* TODO */
      template <typename TVal>
      TVar(const Rt::TOpt<TVal> &that) {
        *this = Opt(that);
      }

      /* Construct a new TVar from anything else.  There are various explicit
         specializations (after the TVar class definition) for the elementary
         runtime types and templates specialized thereon.  This totally wide-open
         version is used for compound types.
         See <stig/var/obj.h>. */
      template <typename TCompound>
      explicit TVar(const TCompound &that);

      /* TODO */
      TVar(TVar &&that);

      /* TODO */
      TVar(const TVar &that);

      /* TODO */
      static TVar Addr(const std::vector<std::pair<TAddrDir, TVar>> &that);

      /* TODO
      template <typename... TElements>
      static TVar Addr(const Rt::TAddr<TElements...> &that);
      */

      /* TODO */
      static TVar Dict(const Rt::TDict<TVar, TVar> &that, const Type::TType &key_type, const Type::TType &val_type);

      /* TODO */
      template <typename TKey, typename TVal>
      static TVar Dict(const Rt::TDict<TKey, TVal> &that);

      /* TODO */
      static TVar Free(const Type::TType &type);

      /* TODO */
      static TVar List(const std::vector<TVar> &that, const Type::TType &type);

      /* TODO */
      template <typename TVal>
      static TVar List(const std::vector<TVal> &that);

      /*  Construct a new TVar from a mutable.
         See <stig/var/mutable.h>. */
      template <typename TAddr, typename TVal>
      static TVar Mutable(const Rt::TMutable<TAddr, TVal> &that);

      /* TODO */
      static TVar Obj(const std::unordered_map<std::string, TVar> &map_);

      /* TODO */
      static TVar Opt(const Rt::TOpt<TVar> &opt, const Type::TType &type);

      /* TODO */
      template <typename TVal>
      static TVar Opt(const Rt::TOpt<TVal> &that);

      /* TODO */
      static TVar Set(const Rt::TSet<TVar> &that, const Type::TType &type);

      /* TODO */
      template <typename TVal>
      static TVar Set(const Rt::TSet<TVal> &that);

      /* TODO */
      TVar Copy() const;

      template <typename TTarget>
      const TTarget *As() const;

      template <typename TTarget>
      bool Is() const;

      template <typename TTarget>
      const TTarget *TryAs() const;

      /* TODO */
      TVar &operator=(TVar &&that);

      /* TODO */
      TVar &operator=(const TVar &that);

      /* TODO */
      bool operator==(const TVar &that) const;

      /* TODO */
      bool operator!=(const TVar &that) const {
        return !(*this == that);
      }

      /* TODO */
      bool operator<(const TVar &that) const;

      /* TODO */
      bool operator<=(const TVar &that) const {
        assert(this);
        assert(&that);
        return (*this < that) || (*this == that);
      }

      /* TODO */
      bool operator>(const TVar &that) const;

      /* TODO */
      bool operator>=(const TVar &that) const {
        assert(this);
        assert(&that);
        return (*this > that) || (*this == that);
      }

      /* TODO */
      explicit operator bool() const;

      /* TODO */
      Var::TVar &Index(const TVar &key) {
        assert(this);
        return Impl->Index(key);
      }

      /* TODO */
      Var::TVar &Add(const TVar &rhs) {
        assert(*this);
        Impl->Add(rhs);
        return *this;
      }

      /* TODO */
      Var::TVar &And(const TVar &rhs) {
        assert(*this);
        Impl->And(rhs);
        return *this;
      }

      /* TODO */
      Var::TVar &Div(const TVar &rhs) {
        assert(*this);
        Impl->Div(rhs);
        return *this;
      }

      /* TODO */
      Var::TVar &Exp(const TVar &rhs) {
        assert(*this);
        Impl->Exp(rhs);
        return *this;
      }

      /* TODO */
      Var::TVar &Intersection(const TVar &rhs) {
        assert(*this);
        Impl->Intersection(rhs);
        return *this;
      }

      /* TODO */
      Var::TVar &Mod(const TVar &rhs) {
        assert(*this);
        Impl->Mod(rhs);
        return *this;
      }

      /* TODO */
      Var::TVar &Mult(const TVar &rhs) {
        assert(*this);
        Impl->Mult(rhs);
        return *this;
      }

      /* TODO */
      Var::TVar &Or(const TVar &rhs) {
        assert(*this);
        Impl->Or(rhs);
        return *this;
      }

      /* TODO */
      Var::TVar &Sub(const TVar &rhs) {
        assert(*this);
        Impl->Sub(rhs);
        return *this;
      }

      /* TODO */
      Var::TVar &SymmetricDiff(const TVar &rhs) {
        assert(*this);
        Impl->SymmetricDiff(rhs);
        return *this;
      }

      /* TODO */
      Var::TVar &Union(const TVar &rhs) {
        assert(*this);
        Impl->Union(rhs);
        return *this;
      }

      /* TODO */
      Var::TVar &Xor(const TVar &rhs) {
        assert(*this);
        Impl->Xor(rhs);
        return *this;
      }

      /* TODO */
      void Accept(const TVisitor &visitor) const {
        assert(this);
        assert(Impl);
        Impl->Accept(visitor);
      }

      /* TODO */
      int Compare(const TVar &that) const;

      /* TODO */
      size_t GetHash() const {
        assert(this);
        return Impl->GetHash();
      }

      /* TODO */
      Type::TType GetType() const {
        assert(this);
        return Impl->GetType();
      }

      /* TODO */
      TVar &Reset() {
        assert(this);
        return *this = TVar();
      }

      /* TODO */
      void Touch() {
        assert(this);
        Impl->Touch();
      }

      /* TODO */
      static void Accept(
          const TVar &lhs, const TVar &rhs,
          const TDoubleVisitor &double_visitor);

      /* TODO */
      void Write(std::ostream &stream) const {
        assert(this);
        Impl->Write(stream);
      }

      private:

      /* TODO */
      TVar(TImpl *impl)
          : Impl(impl, TImpl::Delete) {
        assert(impl);
      }

      /* TODO */
      void Init();

      /* TODO */
      std::shared_ptr<TImpl> Impl;

      template <typename TVal> friend TVal DynamicCast(const TVar &var);

      template <typename TVal> friend struct TRead;

    };  // TVar

    /* TODO */
    template <>
    struct TVar::TDt<TVar> {

      /* TODO */
      TVar static As(const TVar &rhs) {
        return rhs;
      }

    };

    /* TODO */
    class TVar::TVisitor {
      public:

      /* TODO */
      virtual ~TVisitor();

      /* TODO */
      virtual void operator()(const Var::TAddr *that) const = 0;
      virtual void operator()(const Var::TBool *that) const = 0;
      virtual void operator()(const Var::TDict *that) const = 0;
      virtual void operator()(const Var::TErr *that) const = 0;
      virtual void operator()(const Var::TFree *that) const = 0;
      virtual void operator()(const Var::TId *that) const = 0;
      virtual void operator()(const Var::TInt *that) const = 0;
      virtual void operator()(const Var::TList *that) const = 0;
      virtual void operator()(const Var::TMutable *that) const = 0;
      virtual void operator()(const Var::TObj *that) const = 0;
      virtual void operator()(const Var::TOpt *that) const = 0;
      virtual void operator()(const Var::TReal *that) const = 0;
      virtual void operator()(const Var::TSet *that) const = 0;
      virtual void operator()(const Var::TStr *that) const = 0;
      virtual void operator()(const Var::TTimeDiff *that) const = 0;
      virtual void operator()(const Var::TTimePnt *that) const = 0;
      virtual void operator()(const Var::TUnknown *) const {throw Base::TImpossibleError(HERE);}

      protected:

      /* TODO */
      TVisitor() {}

    };  // TVisitor

    /* TODO */
    class TVar::TDoubleVisitor {
      public:

      /* TODO */
      virtual ~TDoubleVisitor();

      /* TODO */
      virtual void operator()(const Var::TAddr *lhs, const Var::TAddr *rhs) const = 0;
      virtual void operator()(const Var::TAddr *lhs, const Var::TBool *rhs) const = 0;
      virtual void operator()(const Var::TAddr *lhs, const Var::TDict *rhs) const = 0;
      virtual void operator()(const Var::TAddr *lhs, const Var::TErr *rhs) const = 0;
      virtual void operator()(const Var::TAddr *lhs, const Var::TFree *rhs) const = 0;
      virtual void operator()(const Var::TAddr *lhs, const Var::TId *rhs) const = 0;
      virtual void operator()(const Var::TAddr *lhs, const Var::TInt *rhs) const = 0;
      virtual void operator()(const Var::TAddr *lhs, const Var::TList *rhs) const = 0;
      virtual void operator()(const Var::TAddr *lhs, const Var::TMutable *rhs) const = 0;
      virtual void operator()(const Var::TAddr *lhs, const Var::TOpt *rhs) const = 0;
      virtual void operator()(const Var::TAddr *lhs, const Var::TObj *rhs) const = 0;
      virtual void operator()(const Var::TAddr *lhs, const Var::TReal *rhs) const = 0;
      virtual void operator()(const Var::TAddr *lhs, const Var::TSet *rhs) const = 0;
      virtual void operator()(const Var::TAddr *lhs, const Var::TStr *rhs) const = 0;
      virtual void operator()(const Var::TAddr *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TAddr *lhs, const Var::TTimePnt *rhs) const = 0;
      void operator()(const Var::TAddr *, const Var::TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const Var::TBool *lhs, const Var::TAddr *rhs) const = 0;
      virtual void operator()(const Var::TBool *lhs, const Var::TBool *rhs) const = 0;
      virtual void operator()(const Var::TBool *lhs, const Var::TDict *rhs) const = 0;
      virtual void operator()(const Var::TBool *lhs, const Var::TErr *rhs) const = 0;
      virtual void operator()(const Var::TBool *lhs, const Var::TFree *rhs) const = 0;
      virtual void operator()(const Var::TBool *lhs, const Var::TId *rhs) const = 0;
      virtual void operator()(const Var::TBool *lhs, const Var::TInt *rhs) const = 0;
      virtual void operator()(const Var::TBool *lhs, const Var::TList *rhs) const = 0;
      virtual void operator()(const Var::TBool *lhs, const Var::TMutable *rhs) const = 0;
      virtual void operator()(const Var::TBool *lhs, const Var::TOpt *rhs) const = 0;
      virtual void operator()(const Var::TBool *lhs, const Var::TObj *rhs) const = 0;
      virtual void operator()(const Var::TBool *lhs, const Var::TReal *rhs) const = 0;
      virtual void operator()(const Var::TBool *lhs, const Var::TSet *rhs) const = 0;
      virtual void operator()(const Var::TBool *lhs, const Var::TStr *rhs) const = 0;
      virtual void operator()(const Var::TBool *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TBool *lhs, const Var::TTimePnt *rhs) const = 0;
      void operator()(const Var::TBool *, const Var::TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const Var::TDict *lhs, const Var::TAddr *rhs) const = 0;
      virtual void operator()(const Var::TDict *lhs, const Var::TBool *rhs) const = 0;
      virtual void operator()(const Var::TDict *lhs, const Var::TDict *rhs) const = 0;
      virtual void operator()(const Var::TDict *lhs, const Var::TErr *rhs) const = 0;
      virtual void operator()(const Var::TDict *lhs, const Var::TFree *rhs) const = 0;
      virtual void operator()(const Var::TDict *lhs, const Var::TId *rhs) const = 0;
      virtual void operator()(const Var::TDict *lhs, const Var::TInt *rhs) const = 0;
      virtual void operator()(const Var::TDict *lhs, const Var::TList *rhs) const = 0;
      virtual void operator()(const Var::TDict *lhs, const Var::TMutable *rhs) const = 0;
      virtual void operator()(const Var::TDict *lhs, const Var::TOpt *rhs) const = 0;
      virtual void operator()(const Var::TDict *lhs, const Var::TObj *rhs) const = 0;
      virtual void operator()(const Var::TDict *lhs, const Var::TReal *rhs) const = 0;
      virtual void operator()(const Var::TDict *lhs, const Var::TSet *rhs) const = 0;
      virtual void operator()(const Var::TDict *lhs, const Var::TStr *rhs) const = 0;
      virtual void operator()(const Var::TDict *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TDict *lhs, const Var::TTimePnt *rhs) const = 0;
      void operator()(const Var::TDict *, const Var::TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const Var::TErr *lhs, const Var::TAddr *rhs) const = 0;
      virtual void operator()(const Var::TErr *lhs, const Var::TBool *rhs) const = 0;
      virtual void operator()(const Var::TErr *lhs, const Var::TDict *rhs) const = 0;
      virtual void operator()(const Var::TErr *lhs, const Var::TErr *rhs) const = 0;
      virtual void operator()(const Var::TErr *lhs, const Var::TFree *rhs) const = 0;
      virtual void operator()(const Var::TErr *lhs, const Var::TId *rhs) const = 0;
      virtual void operator()(const Var::TErr *lhs, const Var::TInt *rhs) const = 0;
      virtual void operator()(const Var::TErr *lhs, const Var::TList *rhs) const = 0;
      virtual void operator()(const Var::TErr *lhs, const Var::TMutable *rhs) const = 0;
      virtual void operator()(const Var::TErr *lhs, const Var::TOpt *rhs) const = 0;
      virtual void operator()(const Var::TErr *lhs, const Var::TObj *rhs) const = 0;
      virtual void operator()(const Var::TErr *lhs, const Var::TReal *rhs) const = 0;
      virtual void operator()(const Var::TErr *lhs, const Var::TSet *rhs) const = 0;
      virtual void operator()(const Var::TErr *lhs, const Var::TStr *rhs) const = 0;
      virtual void operator()(const Var::TErr *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TErr *lhs, const Var::TTimePnt *rhs) const = 0;
      void operator()(const Var::TErr *, const Var::TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const Var::TFree *lhs, const Var::TAddr *rhs) const = 0;
      virtual void operator()(const Var::TFree *lhs, const Var::TBool *rhs) const = 0;
      virtual void operator()(const Var::TFree *lhs, const Var::TDict *rhs) const = 0;
      virtual void operator()(const Var::TFree *lhs, const Var::TErr *rhs) const = 0;
      virtual void operator()(const Var::TFree *lhs, const Var::TFree *rhs) const = 0;
      virtual void operator()(const Var::TFree *lhs, const Var::TId *rhs) const = 0;
      virtual void operator()(const Var::TFree *lhs, const Var::TInt *rhs) const = 0;
      virtual void operator()(const Var::TFree *lhs, const Var::TList *rhs) const = 0;
      virtual void operator()(const Var::TFree *lhs, const Var::TMutable *rhs) const = 0;
      virtual void operator()(const Var::TFree *lhs, const Var::TOpt *rhs) const = 0;
      virtual void operator()(const Var::TFree *lhs, const Var::TObj *rhs) const = 0;
      virtual void operator()(const Var::TFree *lhs, const Var::TReal *rhs) const = 0;
      virtual void operator()(const Var::TFree *lhs, const Var::TSet *rhs) const = 0;
      virtual void operator()(const Var::TFree *lhs, const Var::TStr *rhs) const = 0;
      virtual void operator()(const Var::TFree *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TFree *lhs, const Var::TTimePnt *rhs) const = 0;
      void operator()(const Var::TFree *, const Var::TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const Var::TId *lhs, const Var::TAddr *rhs) const = 0;
      virtual void operator()(const Var::TId *lhs, const Var::TBool *rhs) const = 0;
      virtual void operator()(const Var::TId *lhs, const Var::TDict *rhs) const = 0;
      virtual void operator()(const Var::TId *lhs, const Var::TErr *rhs) const = 0;
      virtual void operator()(const Var::TId *lhs, const Var::TFree *rhs) const = 0;
      virtual void operator()(const Var::TId *lhs, const Var::TId *rhs) const = 0;
      virtual void operator()(const Var::TId *lhs, const Var::TInt *rhs) const = 0;
      virtual void operator()(const Var::TId *lhs, const Var::TList *rhs) const = 0;
      virtual void operator()(const Var::TId *lhs, const Var::TMutable *rhs) const = 0;
      virtual void operator()(const Var::TId *lhs, const Var::TOpt *rhs) const = 0;
      virtual void operator()(const Var::TId *lhs, const Var::TObj *rhs) const = 0;
      virtual void operator()(const Var::TId *lhs, const Var::TReal *rhs) const = 0;
      virtual void operator()(const Var::TId *lhs, const Var::TSet *rhs) const = 0;
      virtual void operator()(const Var::TId *lhs, const Var::TStr *rhs) const = 0;
      virtual void operator()(const Var::TId *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TId *lhs, const Var::TTimePnt *rhs) const = 0;
      void operator()(const Var::TId *, const Var::TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const Var::TInt *lhs, const Var::TAddr *rhs) const = 0;
      virtual void operator()(const Var::TInt *lhs, const Var::TBool *rhs) const = 0;
      virtual void operator()(const Var::TInt *lhs, const Var::TDict *rhs) const = 0;
      virtual void operator()(const Var::TInt *lhs, const Var::TErr *rhs) const = 0;
      virtual void operator()(const Var::TInt *lhs, const Var::TFree *rhs) const = 0;
      virtual void operator()(const Var::TInt *lhs, const Var::TId *rhs) const = 0;
      virtual void operator()(const Var::TInt *lhs, const Var::TInt *rhs) const = 0;
      virtual void operator()(const Var::TInt *lhs, const Var::TList *rhs) const = 0;
      virtual void operator()(const Var::TInt *lhs, const Var::TMutable *rhs) const = 0;
      virtual void operator()(const Var::TInt *lhs, const Var::TOpt *rhs) const = 0;
      virtual void operator()(const Var::TInt *lhs, const Var::TObj *rhs) const = 0;
      virtual void operator()(const Var::TInt *lhs, const Var::TReal *rhs) const = 0;
      virtual void operator()(const Var::TInt *lhs, const Var::TSet *rhs) const = 0;
      virtual void operator()(const Var::TInt *lhs, const Var::TStr *rhs) const = 0;
      virtual void operator()(const Var::TInt *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TInt *lhs, const Var::TTimePnt *rhs) const = 0;
      void operator()(const Var::TInt *, const Var::TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const Var::TList *lhs, const Var::TAddr *rhs) const = 0;
      virtual void operator()(const Var::TList *lhs, const Var::TBool *rhs) const = 0;
      virtual void operator()(const Var::TList *lhs, const Var::TDict *rhs) const = 0;
      virtual void operator()(const Var::TList *lhs, const Var::TErr *rhs) const = 0;
      virtual void operator()(const Var::TList *lhs, const Var::TFree *rhs) const = 0;
      virtual void operator()(const Var::TList *lhs, const Var::TId *rhs) const = 0;
      virtual void operator()(const Var::TList *lhs, const Var::TInt *rhs) const = 0;
      virtual void operator()(const Var::TList *lhs, const Var::TList *rhs) const = 0;
      virtual void operator()(const Var::TList *lhs, const Var::TMutable *rhs) const = 0;
      virtual void operator()(const Var::TList *lhs, const Var::TOpt *rhs) const = 0;
      virtual void operator()(const Var::TList *lhs, const Var::TObj *rhs) const = 0;
      virtual void operator()(const Var::TList *lhs, const Var::TReal *rhs) const = 0;
      virtual void operator()(const Var::TList *lhs, const Var::TSet *rhs) const = 0;
      virtual void operator()(const Var::TList *lhs, const Var::TStr *rhs) const = 0;
      virtual void operator()(const Var::TList *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TList *lhs, const Var::TTimePnt *rhs) const = 0;
      void operator()(const Var::TList *, const Var::TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const Var::TMutable *lhs, const Var::TAddr *rhs) const = 0;
      virtual void operator()(const Var::TMutable *lhs, const Var::TBool *rhs) const = 0;
      virtual void operator()(const Var::TMutable *lhs, const Var::TDict *rhs) const = 0;
      virtual void operator()(const Var::TMutable *lhs, const Var::TErr *rhs) const = 0;
      virtual void operator()(const Var::TMutable *lhs, const Var::TFree *rhs) const = 0;
      virtual void operator()(const Var::TMutable *lhs, const Var::TId *rhs) const = 0;
      virtual void operator()(const Var::TMutable *lhs, const Var::TInt *rhs) const = 0;
      virtual void operator()(const Var::TMutable *lhs, const Var::TList *rhs) const = 0;
      virtual void operator()(const Var::TMutable *lhs, const Var::TMutable *rhs) const = 0;
      virtual void operator()(const Var::TMutable *lhs, const Var::TOpt *rhs) const = 0;
      virtual void operator()(const Var::TMutable *lhs, const Var::TObj *rhs) const = 0;
      virtual void operator()(const Var::TMutable *lhs, const Var::TReal *rhs) const = 0;
      virtual void operator()(const Var::TMutable *lhs, const Var::TSet *rhs) const = 0;
      virtual void operator()(const Var::TMutable *lhs, const Var::TStr *rhs) const = 0;
      virtual void operator()(const Var::TMutable *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TMutable *lhs, const Var::TTimePnt *rhs) const = 0;
      void operator()(const Var::TMutable *, const Var::TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const Var::TOpt *lhs, const Var::TAddr *rhs) const = 0;
      virtual void operator()(const Var::TOpt *lhs, const Var::TBool *rhs) const = 0;
      virtual void operator()(const Var::TOpt *lhs, const Var::TDict *rhs) const = 0;
      virtual void operator()(const Var::TOpt *lhs, const Var::TErr *rhs) const = 0;
      virtual void operator()(const Var::TOpt *lhs, const Var::TFree *rhs) const = 0;
      virtual void operator()(const Var::TOpt *lhs, const Var::TId *rhs) const = 0;
      virtual void operator()(const Var::TOpt *lhs, const Var::TInt *rhs) const = 0;
      virtual void operator()(const Var::TOpt *lhs, const Var::TList *rhs) const = 0;
      virtual void operator()(const Var::TOpt *lhs, const Var::TMutable *rhs) const = 0;
      virtual void operator()(const Var::TOpt *lhs, const Var::TOpt *rhs) const = 0;
      virtual void operator()(const Var::TOpt *lhs, const Var::TObj *rhs) const = 0;
      virtual void operator()(const Var::TOpt *lhs, const Var::TReal *rhs) const = 0;
      virtual void operator()(const Var::TOpt *lhs, const Var::TSet *rhs) const = 0;
      virtual void operator()(const Var::TOpt *lhs, const Var::TStr *rhs) const = 0;
      virtual void operator()(const Var::TOpt *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TOpt *lhs, const Var::TTimePnt *rhs) const = 0;
      void operator()(const Var::TOpt *, const Var::TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const Var::TObj *lhs, const Var::TAddr *rhs) const = 0;
      virtual void operator()(const Var::TObj *lhs, const Var::TBool *rhs) const = 0;
      virtual void operator()(const Var::TObj *lhs, const Var::TDict *rhs) const = 0;
      virtual void operator()(const Var::TObj *lhs, const Var::TErr *rhs) const = 0;
      virtual void operator()(const Var::TObj *lhs, const Var::TFree *rhs) const = 0;
      virtual void operator()(const Var::TObj *lhs, const Var::TId *rhs) const = 0;
      virtual void operator()(const Var::TObj *lhs, const Var::TInt *rhs) const = 0;
      virtual void operator()(const Var::TObj *lhs, const Var::TList *rhs) const = 0;
      virtual void operator()(const Var::TObj *lhs, const Var::TMutable *rhs) const = 0;
      virtual void operator()(const Var::TObj *lhs, const Var::TOpt *rhs) const = 0;
      virtual void operator()(const Var::TObj *lhs, const Var::TObj *rhs) const = 0;
      virtual void operator()(const Var::TObj *lhs, const Var::TReal *rhs) const = 0;
      virtual void operator()(const Var::TObj *lhs, const Var::TSet *rhs) const = 0;
      virtual void operator()(const Var::TObj *lhs, const Var::TStr *rhs) const = 0;
      virtual void operator()(const Var::TObj *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TObj *lhs, const Var::TTimePnt *rhs) const = 0;
      void operator()(const Var::TObj *, const Var::TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const Var::TReal *lhs, const Var::TAddr *rhs) const = 0;
      virtual void operator()(const Var::TReal *lhs, const Var::TBool *rhs) const = 0;
      virtual void operator()(const Var::TReal *lhs, const Var::TDict *rhs) const = 0;
      virtual void operator()(const Var::TReal *lhs, const Var::TErr *rhs) const = 0;
      virtual void operator()(const Var::TReal *lhs, const Var::TFree *rhs) const = 0;
      virtual void operator()(const Var::TReal *lhs, const Var::TId *rhs) const = 0;
      virtual void operator()(const Var::TReal *lhs, const Var::TInt *rhs) const = 0;
      virtual void operator()(const Var::TReal *lhs, const Var::TList *rhs) const = 0;
      virtual void operator()(const Var::TReal *lhs, const Var::TMutable *rhs) const = 0;
      virtual void operator()(const Var::TReal *lhs, const Var::TOpt *rhs) const = 0;
      virtual void operator()(const Var::TReal *lhs, const Var::TObj *rhs) const = 0;
      virtual void operator()(const Var::TReal *lhs, const Var::TReal *rhs) const = 0;
      virtual void operator()(const Var::TReal *lhs, const Var::TSet *rhs) const = 0;
      virtual void operator()(const Var::TReal *lhs, const Var::TStr *rhs) const = 0;
      virtual void operator()(const Var::TReal *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TReal *lhs, const Var::TTimePnt *rhs) const = 0;
      void operator()(const Var::TReal *, const Var::TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const Var::TSet *lhs, const Var::TAddr *rhs) const = 0;
      virtual void operator()(const Var::TSet *lhs, const Var::TBool *rhs) const = 0;
      virtual void operator()(const Var::TSet *lhs, const Var::TDict *rhs) const = 0;
      virtual void operator()(const Var::TSet *lhs, const Var::TErr *rhs) const = 0;
      virtual void operator()(const Var::TSet *lhs, const Var::TFree *rhs) const = 0;
      virtual void operator()(const Var::TSet *lhs, const Var::TId *rhs) const = 0;
      virtual void operator()(const Var::TSet *lhs, const Var::TInt *rhs) const = 0;
      virtual void operator()(const Var::TSet *lhs, const Var::TList *rhs) const = 0;
      virtual void operator()(const Var::TSet *lhs, const Var::TMutable *rhs) const = 0;
      virtual void operator()(const Var::TSet *lhs, const Var::TOpt *rhs) const = 0;
      virtual void operator()(const Var::TSet *lhs, const Var::TObj *rhs) const = 0;
      virtual void operator()(const Var::TSet *lhs, const Var::TReal *rhs) const = 0;
      virtual void operator()(const Var::TSet *lhs, const Var::TSet *rhs) const = 0;
      virtual void operator()(const Var::TSet *lhs, const Var::TStr *rhs) const = 0;
      virtual void operator()(const Var::TSet *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TSet *lhs, const Var::TTimePnt *rhs) const = 0;
      void operator()(const Var::TSet *, const Var::TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const Var::TStr *lhs, const Var::TAddr *rhs) const = 0;
      virtual void operator()(const Var::TStr *lhs, const Var::TBool *rhs) const = 0;
      virtual void operator()(const Var::TStr *lhs, const Var::TDict *rhs) const = 0;
      virtual void operator()(const Var::TStr *lhs, const Var::TErr *rhs) const = 0;
      virtual void operator()(const Var::TStr *lhs, const Var::TFree *rhs) const = 0;
      virtual void operator()(const Var::TStr *lhs, const Var::TId *rhs) const = 0;
      virtual void operator()(const Var::TStr *lhs, const Var::TInt *rhs) const = 0;
      virtual void operator()(const Var::TStr *lhs, const Var::TList *rhs) const = 0;
      virtual void operator()(const Var::TStr *lhs, const Var::TMutable *rhs) const = 0;
      virtual void operator()(const Var::TStr *lhs, const Var::TOpt *rhs) const = 0;
      virtual void operator()(const Var::TStr *lhs, const Var::TObj *rhs) const = 0;
      virtual void operator()(const Var::TStr *lhs, const Var::TReal *rhs) const = 0;
      virtual void operator()(const Var::TStr *lhs, const Var::TSet *rhs) const = 0;
      virtual void operator()(const Var::TStr *lhs, const Var::TStr *rhs) const = 0;
      virtual void operator()(const Var::TStr *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TStr *lhs, const Var::TTimePnt *rhs) const = 0;
      void operator()(const Var::TStr *, const Var::TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TAddr *rhs) const = 0;
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TBool *rhs) const = 0;
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TDict *rhs) const = 0;
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TErr *rhs) const = 0;
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TFree *rhs) const = 0;
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TId *rhs) const = 0;
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TInt *rhs) const = 0;
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TList *rhs) const = 0;
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TMutable *rhs) const = 0;
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TOpt *rhs) const = 0;
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TObj *rhs) const = 0;
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TReal *rhs) const = 0;
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TSet *rhs) const = 0;
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TStr *rhs) const = 0;
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TTimeDiff *lhs, const Var::TTimePnt *rhs) const = 0;
      void operator()(const Var::TTimeDiff *, const Var::TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const Var::TTimePnt *lhs, const Var::TAddr *rhs) const = 0;
      virtual void operator()(const Var::TTimePnt *lhs, const Var::TBool *rhs) const = 0;
      virtual void operator()(const Var::TTimePnt *lhs, const Var::TDict *rhs) const = 0;
      virtual void operator()(const Var::TTimePnt *lhs, const Var::TErr *rhs) const = 0;
      virtual void operator()(const Var::TTimePnt *lhs, const Var::TFree *rhs) const = 0;
      virtual void operator()(const Var::TTimePnt *lhs, const Var::TId *rhs) const = 0;
      virtual void operator()(const Var::TTimePnt *lhs, const Var::TInt *rhs) const = 0;
      virtual void operator()(const Var::TTimePnt *lhs, const Var::TList *rhs) const = 0;
      virtual void operator()(const Var::TTimePnt *lhs, const Var::TMutable *rhs) const = 0;
      virtual void operator()(const Var::TTimePnt *lhs, const Var::TOpt *rhs) const = 0;
      virtual void operator()(const Var::TTimePnt *lhs, const Var::TObj *rhs) const = 0;
      virtual void operator()(const Var::TTimePnt *lhs, const Var::TReal *rhs) const = 0;
      virtual void operator()(const Var::TTimePnt *lhs, const Var::TSet *rhs) const = 0;
      virtual void operator()(const Var::TTimePnt *lhs, const Var::TStr *rhs) const = 0;
      virtual void operator()(const Var::TTimePnt *lhs, const Var::TTimeDiff *rhs) const = 0;
      virtual void operator()(const Var::TTimePnt *lhs, const Var::TTimePnt *rhs) const = 0;
      void operator()(const Var::TTimePnt *, const Var::TUnknown *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const Var::TUnknown *, const Var::TAddr *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const Var::TUnknown *, const Var::TBool *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const Var::TUnknown *, const Var::TDict *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const Var::TUnknown *, const Var::TErr *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const Var::TUnknown *, const Var::TFree *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const Var::TUnknown *, const Var::TId *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const Var::TUnknown *, const Var::TInt *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const Var::TUnknown *, const Var::TList *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const Var::TUnknown *, const Var::TMutable *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const Var::TUnknown *, const Var::TOpt *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const Var::TUnknown *, const Var::TObj *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const Var::TUnknown *, const Var::TReal *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const Var::TUnknown *, const Var::TSet *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const Var::TUnknown *, const Var::TStr *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const Var::TUnknown *, const Var::TTimeDiff *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const Var::TUnknown *, const Var::TTimePnt *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const Var::TUnknown *, const Var::TUnknown *) const {throw Base::TImpossibleError(HERE);}

      protected:

      /* TODO */
      TDoubleVisitor() {}

    };  // TDoubleVisitor


  /* TODO */
  inline std::ostream &operator<<(std::ostream &stream, const TVar &var) {
    var.Write(stream);
    return stream;
  }

  }  // Var

}  // Stig

namespace std {

  /* A standard hasher for Stig::Var::TVar. */
  template <>
  struct hash<Stig::Var::TVar> {

    typedef size_t result_type;
    typedef Stig::Var::TVar argument_type;

    result_type operator()(const argument_type &that) const {
      return that.GetHash();
    }

  };  // hash<Stig::Var::TVar>

}  // std
