/* <stig/type/impl.h>

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

#include <cassert>
#include <memory>
#include <ostream>

#include <base/impossible_error.h>
#include <base/no_copy_semantics.h>
#include <stig/pos_range.h>

namespace Stig {

  namespace Type {

    /* TODO */
    class TAddr;
    class TAny;
    class TBool;
    class TDict;
    class TErr;
    class TFunc;
    class TId;
    class TInt;
    class TList;
    class TMutable;
    class TObj;
    class TOpt;
    class TReal;
    class TSeq;
    class TSet;
    class TStr;
    class TTimeDiff;
    class TTimePnt;
    class TUnknown;

    /* TOOD */
    class TType {
      public:

      /* TODO */
      class TVisitor;

      /* TODO */
      class TDoubleVisitor;

      /* TODO */
      class TImpl : public std::enable_shared_from_this<TImpl> {
        NO_COPY_SEMANTICS(TImpl);


        public:

        /* TODO */
        TType AsType() const {
          assert(this);
          return TType(shared_from_this());
        }

        protected:

        /* TODO */
        TImpl() {}

        /* TODO */
        virtual ~TImpl() {}

        /* TODO */
        virtual void Accept(const TVisitor &visitor) const = 0;

        /* TODO */
        virtual void Write(std::ostream &) const = 0;

        /* TODO */
        friend class TType;

      };  // TImpl

      /* TODO */
      TType() {
        Init();
      }

      /* TODO */
      TType(TType &&that) {
        assert(&that);
        Init();
        std::swap(Impl, that.Impl);
      }

      /* TODO */
      TType(const TType &that) {
        assert(&that);
        Impl = that.Impl;
      }

      /* TODO */
      ~TType() {}

      /* TODO */
      TType &operator=(TType &&that) {
        assert(this);
        assert(&that);
        std::swap(Impl, that.Impl);
        return *this;
      }

      /* TODO */
      TType &operator=(const TType &that) {
        assert(this);
        assert(&that);
        return *this = TType(that);
      }

      /* TODO */
      bool operator==(const TType &that) const {
        assert(this);
        assert(&that);
        assert(Impl);
        assert(that.Impl);
        return Impl.get() == that.Impl.get();
      }

      /* TODO */
      bool operator!=(const TType &that) const {
        assert(this);
        assert(&that);
        assert(Impl);
        assert(that.Impl);
        return Impl.get() != that.Impl.get();
      }

      /* TODO */
      operator bool() const;

      /* TODO */
      void Accept(const TVisitor &visitor) const {
        assert(this);
        assert(&visitor);
        assert(Impl);
        Impl->Accept(visitor);
      }

      /* TODO */
      static void Accept(const TType &lhs, const TType &rhs, const TDoubleVisitor &double_visitor);

      template <typename TTarget>
      const TTarget *As() const;

      /* TODO */
      size_t GetHash() const {
        assert(this);
        assert(Impl);
        return reinterpret_cast<size_t>(Impl.get());
      }

      /* TODO: Write a demangler */
      /* Mangling rules
         NOTES:
           - All compound types have a capital letter.
           - All compounds which contain more than one type have a count of types following the compound identifier.
      Addr -> 'A' size (('a'|'d') type)*
      Any  -> 'a'
      Bool -> 'b'
      Dict -> 'D' key_type val_type
      Err  -> 'E' err_type
      Func -> 'F' ret_type num_args MangleElemMap(args)
      Id   -> 'I'
      Int  -> 'i'
      List -> 'L' elem_type
      Mutable -> "M2"addr_type value_type
      Obj  -> 'O' MangleElemMap(members)
      Opt  -> 'P' inner_type
      Real -> 'r'
      seq  -> 'Q' inner_type
      set  -> 'S' inner_type
      str  -> 's'
      time_diff -> 't'
      time_pnt  -> 'T'
      */
      std::string GetMangledName() const;

      template <typename TTarget>
      bool Is() const;

      template <typename TTarget>
      const TTarget *TryAs() const;

      /* TODO */
      void Write(std::ostream &stream) const {
        Impl->Write(stream);
      }

      private:

      /* TODO */
      TType(const std::shared_ptr<const TImpl> &impl) : Impl(impl) {
        assert(impl);
      }

      /* TODO */
      void Init();

      /* TODO */
      std::shared_ptr<const TImpl> Impl;

    };  // TType

    /* TODO */
    template <typename TVal>
    struct TDt;

    /* TODO */
    class TType::TVisitor {
      public:

      /* TODO */
      virtual ~TVisitor();

      virtual void operator()(const TAddr     *that) const = 0;
      virtual void operator()(const TAny      *that) const = 0;
      virtual void operator()(const TBool     *that) const = 0;
      virtual void operator()(const TDict     *that) const = 0;
      virtual void operator()(const TErr      *that) const = 0;
      virtual void operator()(const TFunc     *that) const = 0;
      virtual void operator()(const TId       *that) const = 0;
      virtual void operator()(const TInt      *that) const = 0;
      virtual void operator()(const TList     *that) const = 0;
      virtual void operator()(const TMutable  *that) const = 0;
      virtual void operator()(const TObj      *that) const = 0;
      virtual void operator()(const TOpt      *that) const = 0;
      virtual void operator()(const TReal     *that) const = 0;
      virtual void operator()(const TSeq      *that) const = 0;
      virtual void operator()(const TSet      *that) const = 0;
      virtual void operator()(const TStr      *that) const = 0;
      virtual void operator()(const TTimeDiff *that) const = 0;
      virtual void operator()(const TTimePnt  *that) const = 0;
      void operator()(const TUnknown *) const {throw Base::TImpossibleError(HERE);}

      protected:

      /* TODO */
      TVisitor();

    };  // TType::TVisitor;

    /* TODO */
    class TType::TDoubleVisitor {
      public:

      /* TODO */
      virtual ~TDoubleVisitor() {}

      /* TODO */
      virtual void operator()(const TAddr *lhs, const TAddr *rhs) const = 0;
      virtual void operator()(const TAddr *lhs, const TAny *rhs) const = 0;
      virtual void operator()(const TAddr *lhs, const TBool *rhs) const = 0;
      virtual void operator()(const TAddr *lhs, const TDict *rhs) const = 0;
      virtual void operator()(const TAddr *lhs, const TErr *rhs) const = 0;
      virtual void operator()(const TAddr *lhs, const TFunc *rhs) const = 0;
      virtual void operator()(const TAddr *lhs, const TId *rhs) const = 0;
      virtual void operator()(const TAddr *lhs, const TInt *rhs) const = 0;
      virtual void operator()(const TAddr *lhs, const TList *rhs) const = 0;
      virtual void operator()(const TAddr *lhs, const TMutable *rhs) const = 0;
      virtual void operator()(const TAddr *lhs, const TOpt *rhs) const = 0;
      virtual void operator()(const TAddr *lhs, const TObj *rhs) const = 0;
      virtual void operator()(const TAddr *lhs, const TReal *rhs) const = 0;
      virtual void operator()(const TAddr *lhs, const TSet *rhs) const = 0;
      virtual void operator()(const TAddr *lhs, const TSeq *rhs) const = 0;
      virtual void operator()(const TAddr *lhs, const TStr *rhs) const = 0;
      virtual void operator()(const TAddr *lhs, const TTimeDiff *rhs) const = 0;
      virtual void operator()(const TAddr *lhs, const TTimePnt *rhs) const = 0;
      void operator()(const TAddr *, const TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const TAny *lhs, const TAddr *rhs) const = 0;
      virtual void operator()(const TAny *lhs, const TAny *rhs) const = 0;
      virtual void operator()(const TAny *lhs, const TBool *rhs) const = 0;
      virtual void operator()(const TAny *lhs, const TDict *rhs) const = 0;
      virtual void operator()(const TAny *lhs, const TErr *rhs) const = 0;
      virtual void operator()(const TAny *lhs, const TFunc *rhs) const = 0;
      virtual void operator()(const TAny *lhs, const TId *rhs) const = 0;
      virtual void operator()(const TAny *lhs, const TInt *rhs) const = 0;
      virtual void operator()(const TAny *lhs, const TList *rhs) const = 0;
      virtual void operator()(const TAny *lhs, const TMutable *rhs) const = 0;
      virtual void operator()(const TAny *lhs, const TOpt *rhs) const = 0;
      virtual void operator()(const TAny *lhs, const TObj *rhs) const = 0;
      virtual void operator()(const TAny *lhs, const TReal *rhs) const = 0;
      virtual void operator()(const TAny *lhs, const TSet *rhs) const = 0;
      virtual void operator()(const TAny *lhs, const TSeq *rhs) const = 0;
      virtual void operator()(const TAny *lhs, const TStr *rhs) const = 0;
      virtual void operator()(const TAny *lhs, const TTimeDiff *rhs) const = 0;
      virtual void operator()(const TAny *lhs, const TTimePnt *rhs) const = 0;
      void operator()(const TAny *, const TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const TBool *lhs, const TAddr *rhs) const = 0;
      virtual void operator()(const TBool *lhs, const TAny *rhs) const = 0;
      virtual void operator()(const TBool *lhs, const TBool *rhs) const = 0;
      virtual void operator()(const TBool *lhs, const TDict *rhs) const = 0;
      virtual void operator()(const TBool *lhs, const TErr *rhs) const = 0;
      virtual void operator()(const TBool *lhs, const TFunc *rhs) const = 0;
      virtual void operator()(const TBool *lhs, const TId *rhs) const = 0;
      virtual void operator()(const TBool *lhs, const TInt *rhs) const = 0;
      virtual void operator()(const TBool *lhs, const TList *rhs) const = 0;
      virtual void operator()(const TBool *lhs, const TMutable *rhs) const = 0;
      virtual void operator()(const TBool *lhs, const TOpt *rhs) const = 0;
      virtual void operator()(const TBool *lhs, const TObj *rhs) const = 0;
      virtual void operator()(const TBool *lhs, const TReal *rhs) const = 0;
      virtual void operator()(const TBool *lhs, const TSet *rhs) const = 0;
      virtual void operator()(const TBool *lhs, const TSeq *rhs) const = 0;
      virtual void operator()(const TBool *lhs, const TStr *rhs) const = 0;
      virtual void operator()(const TBool *lhs, const TTimeDiff *rhs) const = 0;
      virtual void operator()(const TBool *lhs, const TTimePnt *rhs) const = 0;
      void operator()(const TBool *, const TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const TDict *lhs, const TAddr *rhs) const = 0;
      virtual void operator()(const TDict *lhs, const TAny *rhs) const = 0;
      virtual void operator()(const TDict *lhs, const TBool *rhs) const = 0;
      virtual void operator()(const TDict *lhs, const TDict *rhs) const = 0;
      virtual void operator()(const TDict *lhs, const TErr *rhs) const = 0;
      virtual void operator()(const TDict *lhs, const TFunc *rhs) const = 0;
      virtual void operator()(const TDict *lhs, const TId *rhs) const = 0;
      virtual void operator()(const TDict *lhs, const TInt *rhs) const = 0;
      virtual void operator()(const TDict *lhs, const TList *rhs) const = 0;
      virtual void operator()(const TDict *lhs, const TMutable *rhs) const = 0;
      virtual void operator()(const TDict *lhs, const TOpt *rhs) const = 0;
      virtual void operator()(const TDict *lhs, const TObj *rhs) const = 0;
      virtual void operator()(const TDict *lhs, const TReal *rhs) const = 0;
      virtual void operator()(const TDict *lhs, const TSet *rhs) const = 0;
      virtual void operator()(const TDict *lhs, const TSeq *rhs) const = 0;
      virtual void operator()(const TDict *lhs, const TStr *rhs) const = 0;
      virtual void operator()(const TDict *lhs, const TTimeDiff *rhs) const = 0;
      virtual void operator()(const TDict *lhs, const TTimePnt *rhs) const = 0;
      void operator()(const TDict *, const TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const TErr *lhs, const TAddr *rhs) const = 0;
      virtual void operator()(const TErr *lhs, const TAny *rhs) const = 0;
      virtual void operator()(const TErr *lhs, const TBool *rhs) const = 0;
      virtual void operator()(const TErr *lhs, const TDict *rhs) const = 0;
      virtual void operator()(const TErr *lhs, const TErr *rhs) const = 0;
      virtual void operator()(const TErr *lhs, const TFunc *rhs) const = 0;
      virtual void operator()(const TErr *lhs, const TId *rhs) const = 0;
      virtual void operator()(const TErr *lhs, const TInt *rhs) const = 0;
      virtual void operator()(const TErr *lhs, const TList *rhs) const = 0;
      virtual void operator()(const TErr *lhs, const TMutable *rhs) const = 0;
      virtual void operator()(const TErr *lhs, const TOpt *rhs) const = 0;
      virtual void operator()(const TErr *lhs, const TObj *rhs) const = 0;
      virtual void operator()(const TErr *lhs, const TReal *rhs) const = 0;
      virtual void operator()(const TErr *lhs, const TSet *rhs) const = 0;
      virtual void operator()(const TErr *lhs, const TSeq *rhs) const = 0;
      virtual void operator()(const TErr *lhs, const TStr *rhs) const = 0;
      virtual void operator()(const TErr *lhs, const TTimeDiff *rhs) const = 0;
      virtual void operator()(const TErr *lhs, const TTimePnt *rhs) const = 0;
      void operator()(const TErr *, const TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const TFunc *lhs, const TAddr *rhs) const = 0;
      virtual void operator()(const TFunc *lhs, const TAny *rhs) const = 0;
      virtual void operator()(const TFunc *lhs, const TBool *rhs) const = 0;
      virtual void operator()(const TFunc *lhs, const TDict *rhs) const = 0;
      virtual void operator()(const TFunc *lhs, const TErr *rhs) const = 0;
      virtual void operator()(const TFunc *lhs, const TFunc *rhs) const = 0;
      virtual void operator()(const TFunc *lhs, const TId *rhs) const = 0;
      virtual void operator()(const TFunc *lhs, const TInt *rhs) const = 0;
      virtual void operator()(const TFunc *lhs, const TList *rhs) const = 0;
      virtual void operator()(const TFunc *lhs, const TMutable *rhs) const = 0;
      virtual void operator()(const TFunc *lhs, const TOpt *rhs) const = 0;
      virtual void operator()(const TFunc *lhs, const TObj *rhs) const = 0;
      virtual void operator()(const TFunc *lhs, const TReal *rhs) const = 0;
      virtual void operator()(const TFunc *lhs, const TSet *rhs) const = 0;
      virtual void operator()(const TFunc *lhs, const TSeq *rhs) const = 0;
      virtual void operator()(const TFunc *lhs, const TStr *rhs) const = 0;
      virtual void operator()(const TFunc *lhs, const TTimeDiff *rhs) const = 0;
      virtual void operator()(const TFunc *lhs, const TTimePnt *rhs) const = 0;
      void operator()(const TFunc *, const TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const TId *lhs, const TAddr *rhs) const = 0;
      virtual void operator()(const TId *lhs, const TAny *rhs) const = 0;
      virtual void operator()(const TId *lhs, const TBool *rhs) const = 0;
      virtual void operator()(const TId *lhs, const TDict *rhs) const = 0;
      virtual void operator()(const TId *lhs, const TErr *rhs) const = 0;
      virtual void operator()(const TId *lhs, const TFunc *rhs) const = 0;
      virtual void operator()(const TId *lhs, const TId *rhs) const = 0;
      virtual void operator()(const TId *lhs, const TInt *rhs) const = 0;
      virtual void operator()(const TId *lhs, const TList *rhs) const = 0;
      virtual void operator()(const TId *lhs, const TMutable *rhs) const = 0;
      virtual void operator()(const TId *lhs, const TOpt *rhs) const = 0;
      virtual void operator()(const TId *lhs, const TObj *rhs) const = 0;
      virtual void operator()(const TId *lhs, const TReal *rhs) const = 0;
      virtual void operator()(const TId *lhs, const TSet *rhs) const = 0;
      virtual void operator()(const TId *lhs, const TSeq *rhs) const = 0;
      virtual void operator()(const TId *lhs, const TStr *rhs) const = 0;
      virtual void operator()(const TId *lhs, const TTimeDiff *rhs) const = 0;
      virtual void operator()(const TId *lhs, const TTimePnt *rhs) const = 0;
      void operator()(const TId *, const TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const TInt *lhs, const TAddr *rhs) const = 0;
      virtual void operator()(const TInt *lhs, const TAny *rhs) const = 0;
      virtual void operator()(const TInt *lhs, const TBool *rhs) const = 0;
      virtual void operator()(const TInt *lhs, const TDict *rhs) const = 0;
      virtual void operator()(const TInt *lhs, const TErr *rhs) const = 0;
      virtual void operator()(const TInt *lhs, const TFunc *rhs) const = 0;
      virtual void operator()(const TInt *lhs, const TId *rhs) const = 0;
      virtual void operator()(const TInt *lhs, const TInt *rhs) const = 0;
      virtual void operator()(const TInt *lhs, const TList *rhs) const = 0;
      virtual void operator()(const TInt *lhs, const TMutable *rhs) const = 0;
      virtual void operator()(const TInt *lhs, const TOpt *rhs) const = 0;
      virtual void operator()(const TInt *lhs, const TObj *rhs) const = 0;
      virtual void operator()(const TInt *lhs, const TReal *rhs) const = 0;
      virtual void operator()(const TInt *lhs, const TSet *rhs) const = 0;
      virtual void operator()(const TInt *lhs, const TSeq *rhs) const = 0;
      virtual void operator()(const TInt *lhs, const TStr *rhs) const = 0;
      virtual void operator()(const TInt *lhs, const TTimeDiff *rhs) const = 0;
      virtual void operator()(const TInt *lhs, const TTimePnt *rhs) const = 0;
      void operator()(const TInt *, const TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const TList *lhs, const TAddr *rhs) const = 0;
      virtual void operator()(const TList *lhs, const TAny *rhs) const = 0;
      virtual void operator()(const TList *lhs, const TBool *rhs) const = 0;
      virtual void operator()(const TList *lhs, const TDict *rhs) const = 0;
      virtual void operator()(const TList *lhs, const TErr *rhs) const = 0;
      virtual void operator()(const TList *lhs, const TFunc *rhs) const = 0;
      virtual void operator()(const TList *lhs, const TId *rhs) const = 0;
      virtual void operator()(const TList *lhs, const TInt *rhs) const = 0;
      virtual void operator()(const TList *lhs, const TList *rhs) const = 0;
      virtual void operator()(const TList *lhs, const TMutable *rhs) const = 0;
      virtual void operator()(const TList *lhs, const TOpt *rhs) const = 0;
      virtual void operator()(const TList *lhs, const TObj *rhs) const = 0;
      virtual void operator()(const TList *lhs, const TReal *rhs) const = 0;
      virtual void operator()(const TList *lhs, const TSet *rhs) const = 0;
      virtual void operator()(const TList *lhs, const TSeq *rhs) const = 0;
      virtual void operator()(const TList *lhs, const TStr *rhs) const = 0;
      virtual void operator()(const TList *lhs, const TTimeDiff *rhs) const = 0;
      virtual void operator()(const TList *lhs, const TTimePnt *rhs) const = 0;
      void operator()(const TList *, const TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const TMutable *lhs, const TAddr *rhs) const = 0;
      virtual void operator()(const TMutable *lhs, const TAny *rhs) const = 0;
      virtual void operator()(const TMutable *lhs, const TBool *rhs) const = 0;
      virtual void operator()(const TMutable *lhs, const TDict *rhs) const = 0;
      virtual void operator()(const TMutable *lhs, const TErr *rhs) const = 0;
      virtual void operator()(const TMutable *lhs, const TFunc *rhs) const = 0;
      virtual void operator()(const TMutable *lhs, const TId *rhs) const = 0;
      virtual void operator()(const TMutable *lhs, const TInt *rhs) const = 0;
      virtual void operator()(const TMutable *lhs, const TList *rhs) const = 0;
      virtual void operator()(const TMutable *lhs, const TMutable *rhs) const = 0;
      virtual void operator()(const TMutable *lhs, const TOpt *rhs) const = 0;
      virtual void operator()(const TMutable *lhs, const TObj *rhs) const = 0;
      virtual void operator()(const TMutable *lhs, const TReal *rhs) const = 0;
      virtual void operator()(const TMutable *lhs, const TSet *rhs) const = 0;
      virtual void operator()(const TMutable *lhs, const TSeq *rhs) const = 0;
      virtual void operator()(const TMutable *lhs, const TStr *rhs) const = 0;
      virtual void operator()(const TMutable *lhs, const TTimeDiff *rhs) const = 0;
      virtual void operator()(const TMutable *lhs, const TTimePnt *rhs) const = 0;
      void operator()(const TMutable *, const TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const TOpt *lhs, const TAddr *rhs) const = 0;
      virtual void operator()(const TOpt *lhs, const TAny *rhs) const = 0;
      virtual void operator()(const TOpt *lhs, const TBool *rhs) const = 0;
      virtual void operator()(const TOpt *lhs, const TDict *rhs) const = 0;
      virtual void operator()(const TOpt *lhs, const TErr *rhs) const = 0;
      virtual void operator()(const TOpt *lhs, const TFunc *rhs) const = 0;
      virtual void operator()(const TOpt *lhs, const TId *rhs) const = 0;
      virtual void operator()(const TOpt *lhs, const TInt *rhs) const = 0;
      virtual void operator()(const TOpt *lhs, const TList *rhs) const = 0;
      virtual void operator()(const TOpt *lhs, const TMutable *rhs) const = 0;
      virtual void operator()(const TOpt *lhs, const TOpt *rhs) const = 0;
      virtual void operator()(const TOpt *lhs, const TObj *rhs) const = 0;
      virtual void operator()(const TOpt *lhs, const TReal *rhs) const = 0;
      virtual void operator()(const TOpt *lhs, const TSet *rhs) const = 0;
      virtual void operator()(const TOpt *lhs, const TSeq *rhs) const = 0;
      virtual void operator()(const TOpt *lhs, const TStr *rhs) const = 0;
      virtual void operator()(const TOpt *lhs, const TTimeDiff *rhs) const = 0;
      virtual void operator()(const TOpt *lhs, const TTimePnt *rhs) const = 0;
      void operator()(const TOpt *, const TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const TObj *lhs, const TAddr *rhs) const = 0;
      virtual void operator()(const TObj *lhs, const TAny *rhs) const = 0;
      virtual void operator()(const TObj *lhs, const TBool *rhs) const = 0;
      virtual void operator()(const TObj *lhs, const TDict *rhs) const = 0;
      virtual void operator()(const TObj *lhs, const TErr *rhs) const = 0;
      virtual void operator()(const TObj *lhs, const TFunc *rhs) const = 0;
      virtual void operator()(const TObj *lhs, const TId *rhs) const = 0;
      virtual void operator()(const TObj *lhs, const TInt *rhs) const = 0;
      virtual void operator()(const TObj *lhs, const TList *rhs) const = 0;
      virtual void operator()(const TObj *lhs, const TMutable *rhs) const = 0;
      virtual void operator()(const TObj *lhs, const TOpt *rhs) const = 0;
      virtual void operator()(const TObj *lhs, const TObj *rhs) const = 0;
      virtual void operator()(const TObj *lhs, const TReal *rhs) const = 0;
      virtual void operator()(const TObj *lhs, const TSet *rhs) const = 0;
      virtual void operator()(const TObj *lhs, const TSeq *rhs) const = 0;
      virtual void operator()(const TObj *lhs, const TStr *rhs) const = 0;
      virtual void operator()(const TObj *lhs, const TTimeDiff *rhs) const = 0;
      virtual void operator()(const TObj *lhs, const TTimePnt *rhs) const = 0;
      void operator()(const TObj *, const TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const TReal *lhs, const TAddr *rhs) const = 0;
      virtual void operator()(const TReal *lhs, const TAny *rhs) const = 0;
      virtual void operator()(const TReal *lhs, const TBool *rhs) const = 0;
      virtual void operator()(const TReal *lhs, const TDict *rhs) const = 0;
      virtual void operator()(const TReal *lhs, const TErr *rhs) const = 0;
      virtual void operator()(const TReal *lhs, const TFunc *rhs) const = 0;
      virtual void operator()(const TReal *lhs, const TId *rhs) const = 0;
      virtual void operator()(const TReal *lhs, const TInt *rhs) const = 0;
      virtual void operator()(const TReal *lhs, const TList *rhs) const = 0;
      virtual void operator()(const TReal *lhs, const TMutable *rhs) const = 0;
      virtual void operator()(const TReal *lhs, const TOpt *rhs) const = 0;
      virtual void operator()(const TReal *lhs, const TObj *rhs) const = 0;
      virtual void operator()(const TReal *lhs, const TReal *rhs) const = 0;
      virtual void operator()(const TReal *lhs, const TSet *rhs) const = 0;
      virtual void operator()(const TReal *lhs, const TSeq *rhs) const = 0;
      virtual void operator()(const TReal *lhs, const TStr *rhs) const = 0;
      virtual void operator()(const TReal *lhs, const TTimeDiff *rhs) const = 0;
      virtual void operator()(const TReal *lhs, const TTimePnt *rhs) const = 0;
      void operator()(const TReal *, const TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const TSet *lhs, const TAddr *rhs) const = 0;
      virtual void operator()(const TSet *lhs, const TAny *rhs) const = 0;
      virtual void operator()(const TSet *lhs, const TBool *rhs) const = 0;
      virtual void operator()(const TSet *lhs, const TDict *rhs) const = 0;
      virtual void operator()(const TSet *lhs, const TErr *rhs) const = 0;
      virtual void operator()(const TSet *lhs, const TFunc *rhs) const = 0;
      virtual void operator()(const TSet *lhs, const TId *rhs) const = 0;
      virtual void operator()(const TSet *lhs, const TInt *rhs) const = 0;
      virtual void operator()(const TSet *lhs, const TList *rhs) const = 0;
      virtual void operator()(const TSet *lhs, const TMutable *rhs) const = 0;
      virtual void operator()(const TSet *lhs, const TOpt *rhs) const = 0;
      virtual void operator()(const TSet *lhs, const TObj *rhs) const = 0;
      virtual void operator()(const TSet *lhs, const TReal *rhs) const = 0;
      virtual void operator()(const TSet *lhs, const TSet *rhs) const = 0;
      virtual void operator()(const TSet *lhs, const TSeq *rhs) const = 0;
      virtual void operator()(const TSet *lhs, const TStr *rhs) const = 0;
      virtual void operator()(const TSet *lhs, const TTimeDiff *rhs) const = 0;
      virtual void operator()(const TSet *lhs, const TTimePnt *rhs) const = 0;
      void operator()(const TSet *, const TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const TSeq *lhs, const TAddr *rhs) const = 0;
      virtual void operator()(const TSeq *lhs, const TAny *rhs) const = 0;
      virtual void operator()(const TSeq *lhs, const TBool *rhs) const = 0;
      virtual void operator()(const TSeq *lhs, const TDict *rhs) const = 0;
      virtual void operator()(const TSeq *lhs, const TErr *rhs) const = 0;
      virtual void operator()(const TSeq *lhs, const TFunc *rhs) const = 0;
      virtual void operator()(const TSeq *lhs, const TId *rhs) const = 0;
      virtual void operator()(const TSeq *lhs, const TInt *rhs) const = 0;
      virtual void operator()(const TSeq *lhs, const TList *rhs) const = 0;
      virtual void operator()(const TSeq *lhs, const TMutable *rhs) const = 0;
      virtual void operator()(const TSeq *lhs, const TOpt *rhs) const = 0;
      virtual void operator()(const TSeq *lhs, const TObj *rhs) const = 0;
      virtual void operator()(const TSeq *lhs, const TReal *rhs) const = 0;
      virtual void operator()(const TSeq *lhs, const TSet *rhs) const = 0;
      virtual void operator()(const TSeq *lhs, const TSeq *rhs) const = 0;
      virtual void operator()(const TSeq *lhs, const TStr *rhs) const = 0;
      virtual void operator()(const TSeq *lhs, const TTimeDiff *rhs) const = 0;
      virtual void operator()(const TSeq *lhs, const TTimePnt *rhs) const = 0;
      void operator()(const TSeq *, const TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const TStr *lhs, const TAddr *rhs) const = 0;
      virtual void operator()(const TStr *lhs, const TAny *rhs) const = 0;
      virtual void operator()(const TStr *lhs, const TBool *rhs) const = 0;
      virtual void operator()(const TStr *lhs, const TDict *rhs) const = 0;
      virtual void operator()(const TStr *lhs, const TErr *rhs) const = 0;
      virtual void operator()(const TStr *lhs, const TFunc *rhs) const = 0;
      virtual void operator()(const TStr *lhs, const TId *rhs) const = 0;
      virtual void operator()(const TStr *lhs, const TInt *rhs) const = 0;
      virtual void operator()(const TStr *lhs, const TList *rhs) const = 0;
      virtual void operator()(const TStr *lhs, const TMutable *rhs) const = 0;
      virtual void operator()(const TStr *lhs, const TOpt *rhs) const = 0;
      virtual void operator()(const TStr *lhs, const TObj *rhs) const = 0;
      virtual void operator()(const TStr *lhs, const TReal *rhs) const = 0;
      virtual void operator()(const TStr *lhs, const TSet *rhs) const = 0;
      virtual void operator()(const TStr *lhs, const TSeq *rhs) const = 0;
      virtual void operator()(const TStr *lhs, const TStr *rhs) const = 0;
      virtual void operator()(const TStr *lhs, const TTimeDiff *rhs) const = 0;
      virtual void operator()(const TStr *lhs, const TTimePnt *rhs) const = 0;
      void operator()(const TStr *, const TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const TTimeDiff *lhs, const TAddr *rhs) const = 0;
      virtual void operator()(const TTimeDiff *lhs, const TAny *rhs) const = 0;
      virtual void operator()(const TTimeDiff *lhs, const TBool *rhs) const = 0;
      virtual void operator()(const TTimeDiff *lhs, const TDict *rhs) const = 0;
      virtual void operator()(const TTimeDiff *lhs, const TErr *rhs) const = 0;
      virtual void operator()(const TTimeDiff *lhs, const TFunc *rhs) const = 0;
      virtual void operator()(const TTimeDiff *lhs, const TId *rhs) const = 0;
      virtual void operator()(const TTimeDiff *lhs, const TInt *rhs) const = 0;
      virtual void operator()(const TTimeDiff *lhs, const TList *rhs) const = 0;
      virtual void operator()(const TTimeDiff *lhs, const TMutable *rhs) const = 0;
      virtual void operator()(const TTimeDiff *lhs, const TOpt *rhs) const = 0;
      virtual void operator()(const TTimeDiff *lhs, const TObj *rhs) const = 0;
      virtual void operator()(const TTimeDiff *lhs, const TReal *rhs) const = 0;
      virtual void operator()(const TTimeDiff *lhs, const TSet *rhs) const = 0;
      virtual void operator()(const TTimeDiff *lhs, const TSeq *rhs) const = 0;
      virtual void operator()(const TTimeDiff *lhs, const TStr *rhs) const = 0;
      virtual void operator()(const TTimeDiff *lhs, const TTimeDiff *rhs) const = 0;
      virtual void operator()(const TTimeDiff *lhs, const TTimePnt *rhs) const = 0;
      void operator()(const TTimeDiff *, const TUnknown *) const {throw Base::TImpossibleError(HERE);}
      virtual void operator()(const TTimePnt *lhs, const TAddr *rhs) const = 0;
      virtual void operator()(const TTimePnt *lhs, const TAny *rhs) const = 0;
      virtual void operator()(const TTimePnt *lhs, const TBool *rhs) const = 0;
      virtual void operator()(const TTimePnt *lhs, const TDict *rhs) const = 0;
      virtual void operator()(const TTimePnt *lhs, const TErr *rhs) const = 0;
      virtual void operator()(const TTimePnt *lhs, const TFunc *rhs) const = 0;
      virtual void operator()(const TTimePnt *lhs, const TId *rhs) const = 0;
      virtual void operator()(const TTimePnt *lhs, const TInt *rhs) const = 0;
      virtual void operator()(const TTimePnt *lhs, const TList *rhs) const = 0;
      virtual void operator()(const TTimePnt *lhs, const TMutable *rhs) const = 0;
      virtual void operator()(const TTimePnt *lhs, const TOpt *rhs) const = 0;
      virtual void operator()(const TTimePnt *lhs, const TObj *rhs) const = 0;
      virtual void operator()(const TTimePnt *lhs, const TReal *rhs) const = 0;
      virtual void operator()(const TTimePnt *lhs, const TSet *rhs) const = 0;
      virtual void operator()(const TTimePnt *lhs, const TSeq *rhs) const = 0;
      virtual void operator()(const TTimePnt *lhs, const TStr *rhs) const = 0;
      virtual void operator()(const TTimePnt *lhs, const TTimeDiff *rhs) const = 0;
      virtual void operator()(const TTimePnt *lhs, const TTimePnt *rhs) const = 0;
      void operator()(const TTimePnt *, const TUnknown *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const TUnknown *, const TAddr *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const TUnknown *, const TAny *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const TUnknown *, const TBool *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const TUnknown *, const TDict *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const TUnknown *, const TErr *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const TUnknown *, const TFunc *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const TUnknown *, const TId *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const TUnknown *, const TInt *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const TUnknown *, const TList *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const TUnknown *, const TMutable *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const TUnknown *, const TOpt *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const TUnknown *, const TObj *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const TUnknown *, const TReal *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const TUnknown *, const TSet *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const TUnknown *, const TSeq *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const TUnknown *, const TStr *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const TUnknown *, const TTimeDiff *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const TUnknown *, const TTimePnt *) const {throw Base::TImpossibleError(HERE);}
      void operator()(const TUnknown *, const TUnknown *) const {throw Base::TImpossibleError(HERE);}

      protected:

      /* TODO */
      TDoubleVisitor() {}

    };  // TDoubleVisitor

  }  // Type

}  // Stig

namespace std {

  /* TODO */
  inline ostream &operator<<(ostream &stream, const Stig::Type::TType &that) {
    that.Write(stream);
    return stream;
  }

  /* A standard hasher for Stig::Type::TType. */
  template <>
  struct hash<Stig::Type::TType> {

    typedef size_t result_type;
    typedef Stig::Type::TType argument_type;

    result_type operator()(const argument_type &that) const {
      assert(&that);
      return that.GetHash();
    }

  };  // hash<Stig::Type::TType>

}  // std
