/* <stig/var/new_sabot.h>

   Given a var, construct a state sabot.  The sabot is yours to delete.

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
#include <stdexcept>

#include <stig/native/state.h>
#include <stig/sabot/state.h>
#include <stig/sabot/type.h>
#include <stig/type/new_sabot.h>
#include <stig/var.h>

namespace Stig {

  namespace Var {

    /* Thrown by NewSabot() when the state has no translation in the sabot system. */
    class TStateTranslationError
        : public std::logic_error {
      public:

      /* Do-little. */
      TStateTranslationError();

    };  // TStateTranslationError

    /* Given a var, construct a state sabot at the given address.
       If the var has no translation into the sabot system, this function throws. */
    Sabot::State::TAny *NewSabot(void *buf, const TVar &var, TAddrDir addr_dir = TAddrDir::Asc);

    /* Given a var, construct a state sabot at the given address.
       If the var has no translation into the sabot system, this function returns null. */
    Sabot::State::TAny *TryNewSabot(void *buf, const TVar &var, TAddrDir addr_dir = TAddrDir::Asc);

    namespace SS {

      /* TODO */
      class TDescMod final
          : public Sabot::Type::TDesc {
        public:

        /* TODO */
        using TFactory = std::function<Sabot::Type::TAny *(void *)>;

        /* TODO */
        using TPinBase = Sabot::Type::TDesc::TPin;

        /* TODO */
        class TPin final
            : public TPinBase {
          public:

          /* TODO */
          TPin(const TDescMod *desc_mod)
              : DescMod(desc_mod) {}

          /* TODO */
          virtual TAny *NewElem(void *type_alloc) const override {
            assert(this);
            return DescMod->Factory(type_alloc);
          }

          private:

          /* TODO */
          const TDescMod *DescMod;

        };  // TDescNullary<TFoo>::TPin

        /* TODO */
        TDescMod(TFactory &&factory)
            : Factory(std::move(factory)) {}

        /* TODO */
        virtual TPinBase *Pin(void *alloc) const override {
          assert(this);
          return new (alloc) TPin(this);
        }

        private:

        /* TODO */
        TFactory Factory;

      };  // TDescMod

      /* TODO */
      class TFreeDesc final
          : public Sabot::Type::TFree {
        public:

        /* TODO */
        class TPin final
            : public Sabot::Type::TFree::TPin {
          public:

          /* Cache the element type. */
          TPin(const Type::TType &elem)
              : Elem(elem) {}

          /* See Sabot::Type::TFreeDesc. */
          virtual Sabot::Type::TAny *NewElem(void *buf) const override {
            assert(this);
            return new (buf) Type::ST::TDesc(Elem);
          }

          private:

          /* Our element type. */
          Type::TType Elem;

        };  // TFreeDesc::TPin

        /* TODO */
        TFreeDesc(const Type::TType &elem)
            : Elem(elem) {}

        /* Pin the array into memory. */
        virtual Sabot::Type::TFree::TPin *Pin(void *buf) const override {
          return new (buf) TPin(Elem);
        }

        private:

        /* Our element type. */
        Type::TType Elem;

      };  // TFreeDesc

      /* Sabot for Var::TFree. */
      class TFree final
          : public Sabot::State::TFree {
        public:

        /* Cache a reference to the elements. */
        TFree(const Var::TFree *var, TAddrDir addr_dir)
            : Type(var->GetType()), AddrDir(addr_dir) {}

        /* See Sabot::State::TFree. */
        virtual Sabot::Type::TFree *GetFreeType(void *buf) const override {
          if (AddrDir == TAddrDir::Desc) {
            return new (buf) TFreeDesc(Type);
          }
          return new (buf) Stig::Type::ST::TFree(Type);
        }

        private:

        /* The type on which we are free. */
        Type::TType Type;

        /* TODO */
        TAddrDir AddrDir;

      };  // TFree

      /* Sabot for Var::TBool. */
      class TBool final
          : public Sabot::State::TBool {
        public:

        /* Cache the value. */
        TBool(const Var::TBool *var)
            : Val(var->GetVal()) {}

        /* See Sabot::State::TScalar<TVal>. */
        virtual const bool &Get() const override {
          assert(this);
          return Val;
        }

        /* See Sabot::State::TBool. */
        virtual Sabot::Type::TBool *GetBoolType(void *buf) const override {
          return new (buf) Sabot::Type::TBool();
        }

        private:

        /* Our cached value. */
        bool Val;

      };  // TBool

      /* Sabot for Var::TId. */
      class TId final
          : public Sabot::State::TUuid {
        public:

        /* Cache the value. */
        TId(const Var::TId *var)
            : Val(var->GetVal().GetRaw()) {}

        /* See Sabot::State::TScalar<TVal>. */
        virtual const Base::TUuid &Get() const override {
          assert(this);
          return Val;
        }

        /* See Sabot::State::TUuid. */
        virtual Sabot::Type::TUuid *GetUuidType(void *buf) const override {
          return new (buf) Sabot::Type::TUuid();
        }

        private:

        /* Our cached value. */
        Base::TUuid Val;

      };  // TId

      /* TODO */
      class TDesc final
          : public Sabot::State::TDesc {
        public:

        /* TODO */
        using TPinBase = Sabot::State::TDesc::TPin;

        /* TODO */
        class TPin final
            : public TPinBase {
          public:

          /* TODO */
          TPin(const TDesc *desc)
              : TPinBase(desc), Desc(desc) {}

          private:

          /* TODO */
          virtual Sabot::State::TAny *NewElemInRange(size_t, void *state_alloc) const override {
            assert(this);
            return NewSabot(state_alloc, Desc->Var);
          }

          /* TODO */
          const TDesc *Desc;

        };  // TDesc::TPin

        /* TODO */
        TDesc(const Var::TVar &var)
            : Var(var) {}

        /* TODO */
        virtual Sabot::Type::TDesc *GetDescType(void *buf) const override {
          return new (buf) Type::ST::TDesc(Var.GetType());
        }

        /* TODO */
        virtual size_t GetElemCount() const override {
          return 1;
        }

        /* TODO */
        virtual TPinBase *Pin(void *alloc) const override {
          return new (alloc) TPin(this);
        }

        private:

        /* TODO */
        Var::TVar Var;

      };  // TDesc

      /* Sabot for Var::TInt. */
      class TInt final
          : public Sabot::State::TInt64 {
        public:

        /* Cache the value. */
        TInt(const Var::TInt *var)
            : Val(var->GetVal()) {}

        /* See Sabot::State::TScalar<TVal>. */
        virtual const int64_t &Get() const override {
          assert(this);
          return Val;
        }

        /* See Sabot::State::TInt. */
        virtual Sabot::Type::TInt64 *GetInt64Type(void *buf) const override {
          return new (buf) Sabot::Type::TInt64();
        }

        private:

        /* Our cached value. */
        int64_t Val;

      };  // TInt

      /* Sabot for Var::TReal. */
      class TReal final
          : public Sabot::State::TDouble {
        public:

        /* Cache the value. */
        TReal(const Var::TReal *var)
            : Val(var->GetVal()) {}

        /* See Sabot::State::TScalar<TVal>. */
        virtual const double &Get() const override {
          assert(this);
          return Val;
        }

        /* See Sabot::State::TDouble. */
        virtual Sabot::Type::TDouble *GetDoubleType(void *buf) const override {
          assert(this);
          return new (buf) Sabot::Type::TDouble();
        }

        private:

        /* Our cached value. */
        double Val;

      };  // TReal

      /* Sabot for Var::TTimeDiff. */
      class TTimeDiff final
          : public Sabot::State::TDuration {
        public:

        /* Cache the value. */
        TTimeDiff(const Var::TTimeDiff *var)
            : Val(std::chrono::duration_cast<Sabot::TStdDuration>(var->GetVal())) {}

        /* See Sabot::State::TScalar<TVal>. */
        virtual const Sabot::TStdDuration &Get() const override {
          assert(this);
          return Val;
        }

        /* See Sabot::State::TDuration. */
        virtual Sabot::Type::TDuration *GetDurationType(void *buf) const override {
          assert(this);
          return new (buf) Sabot::Type::TDuration();
        }

        private:

        /* Our cached value. */
        Sabot::TStdDuration Val;

      };  // TTimeDiff

      /* Sabot for Var::TTimePnt. */
      class TTimePnt final
          : public Sabot::State::TTimePoint {
        public:

        /* Cache the value. */
        TTimePnt(const Var::TTimePnt *var)
            : Val(var->GetVal()) {}

        /* See Sabot::State::TScalar<TVal>. */
        virtual const Sabot::TStdTimePoint &Get() const override {
          assert(this);
          return Val;
        }

        /* See Sabot::State::TTimePoint. */
        virtual Sabot::Type::TTimePoint *GetTimePointType(void *buf) const override {
          assert(this);
          return new (buf) Sabot::Type::TTimePoint();
        }

        private:

        /* Our cached value. */
        Sabot::TStdTimePoint Val;

      };  // TTimePnt

      /* Sabot for Var::TStr. */
      class TStr final
          : public Sabot::State::TStr {
        public:

        /* The base class which our pin finalizes. */
        using TPinBase = Sabot::State::TStr::TPin;

        /* Our pin. */
        class TPin final
            : public TPinBase {
          public:

          /* Point at our state's array. */
          TPin(const TStr *state)
              : TPinBase(state->Start, state->Limit) {}

        };  // TStr::TPin

        /* Cache the value. */
        TStr(const Var::TStr *var) {
          assert(var);
          const auto &val = var->GetVal();
          Start = val.c_str();
          Limit = Start + val.size();
        }

        /* See Sabot::State::TArrayOfScalars<>. */
        virtual size_t GetSize() const override {
          assert(this);
          return Limit - Start;
        }

        /* See Sabot::State::TStr. */
        virtual Sabot::Type::TStr *GetStrType(void *buf) const override {
          return new (buf) Sabot::Type::TStr();
        }

        /* See Sabot::State::TArrayOfScalars<>. */
        virtual TPinBase *Pin(void *buf) const override {
          return new (buf) TPin(this);
        }

        private:

        /* Our array of scalars. */
        const char *Start, *Limit;

      };  // TStr

      /* Sabot for Var::TList. */
      class TList final
          : public Sabot::State::TVector {
        public:

        /* The base class which our pin finalizes. */
        using TPinBase = Sabot::State::TVector::TPin;

        /* Our pin. */
        class TPin final
            : public TPinBase {
          public:

          /* Point at our state's array. */
          TPin(const TList *state)
              : TPinBase(state), State(state) {
            assert(state);
          }

          private:

          /* See TPinBase. */
          virtual Sabot::State::TAny *NewElemInRange(size_t elem_idx, void *buf) const override {
            assert(this);
            return NewSabot(buf, State->Elems[elem_idx]);
          }

          /* The state which we pin. */
          const TList *State;

        };  // TList::TPin

        /* Cache a reference to the elements. */
        TList(const Var::TList *var)
            : Elems(var->GetVal()), Type(var->GetType()) {}

        /* See Sabot::State::TArrayOfScalars<>. */
        virtual size_t GetElemCount() const override {
          assert(this);
          return Elems.size();
        }

        /* See Sabot::State::TVector. */
        virtual Sabot::Type::TVector *GetVectorType(void *buf) const override {
          return dynamic_cast<Sabot::Type::TVector *>(Type::NewSabot(buf, Type));
        }

        /* See Sabot::State::TArrayOfScalars<>. */
        virtual TPinBase *Pin(void *buf) const override {
          return new (buf) TPin(this);
        }

        private:

        /* Our elements. */
        const std::vector<Var::TVar> &Elems;

        /* Our type. */
        Type::TType Type;

      };  // TList

      /* Sabot for Var::TSet. */
      class TSet final
          : public Sabot::State::TSet {
        public:

        /* The base class which our pin finalizes. */
        using TPinBase = Sabot::State::TSet::TPin;

        /* Our pin. */
        class TPin final
            : public TPinBase {
          public:

          /* Point at our state's array. */
          TPin(const TSet *state)
              : TPinBase(state), State(state) {
            assert(state);
          }

          private:

          /* See TPinBase. */
          virtual Sabot::State::TAny *NewElemInRange(size_t elem_idx, void *buf) const override {
            assert(this);
            return NewSabot(buf, State->Elems[elem_idx]);
          }

          /* The state which we pin. */
          const TSet *State;

        };  // TSet::TPin

        /* Cache a reference to the elements. */
        TSet(const Var::TSet *var)
            : Type(var->GetType()) {
          for (const auto &elem: var->GetVal()) {
            Elems.push_back(elem);
          }
          std::sort(Elems.begin(), Elems.end());
        }

        /* See Sabot::State::TArrayOfScalars<>. */
        virtual size_t GetElemCount() const override {
          assert(this);
          return Elems.size();
        }

        /* See Sabot::State::TSet. */
        virtual Sabot::Type::TSet *GetSetType(void *buf) const override {
          return dynamic_cast<Sabot::Type::TSet *>(Type::NewSabot(buf, Type));
        }

        /* See Sabot::State::TArrayOfScalars<>. */
        virtual TPinBase *Pin(void *buf) const override {
          return new (buf) TPin(this);
        }

        private:

        /* Our type. */
        Type::TType Type;

        /* Our elements. */
        std::vector<Var::TVar> Elems;

      };  // TSet

      /* Sabot for Var::TOpt. */
      class TOpt final
          : public Sabot::State::TOpt {
        public:

        /* The base class which our pin finalizes. */
        using TPinBase = Sabot::State::TOpt::TPin;

        /* Our pin. */
        class TPin final
            : public TPinBase {
          public:

          /* Point at our state's array. */
          TPin(const TOpt *state)
              : TPinBase(state), State(state) {
            assert(state);
          }

          private:

          /* See TPinBase. */
          virtual Sabot::State::TAny *NewElemInRange(size_t, void *buf) const override {
            assert(this);
            return NewSabot(buf, State->Elem.GetVal());
          }

          /* The state which we pin. */
          const TOpt *State;

        };  // TOpt::TPin

        /* Cache a reference to the elements. */
        TOpt(const Var::TOpt *var)
            : Elem(var->GetVal()), Type(var->GetType()) {}

        /* See Sabot::State::TArrayOfScalars<>. */
        virtual size_t GetElemCount() const override {
          assert(this);
          return Elem.IsKnown() ? 1 : 0;
        }

        /* See Sabot::State::TOpt. */
        virtual Sabot::Type::TOpt *GetOptType(void *buf) const override {
          return dynamic_cast<Sabot::Type::TOpt *>(Type::NewSabot(buf, Type));
        }

        /* See Sabot::State::TArrayOfScalars<>. */
        virtual TPinBase *Pin(void *buf) const override {
          return new (buf) TPin(this);
        }

        private:

        /* Our element. */
        const Rt::TOpt<Var::TVar> &Elem;

        /* Our type. */
        Type::TType Type;

      };  // TOpt

      /* Sabot for Var::TDict. */
      class TDict final
          : public Sabot::State::TMap {
        public:

        /* The base class which our pin finalizes. */
        using TPinBase = Sabot::State::TMap::TPin;

        /* Our pin. */
        class TPin final
            : public TPinBase {
          public:

          /* Point at our state's array. */
          TPin(const TDict *state)
              : TPinBase(state), State(state) {
            assert(state);
          }

          private:

          /* See TPinBase. */
          virtual Sabot::State::TAny *NewLhsInRange(size_t elem_idx, void *buf) const override {
            assert(this);
            return NewSabot(buf, State->Elems[elem_idx].first);
          }

          /* See TPinBase. */
          virtual Sabot::State::TAny *NewRhsInRange(size_t elem_idx, void *buf) const override {
            assert(this);
            return NewSabot(buf, State->Elems[elem_idx].second);
          }

          /* The state which we pin. */
          const TDict *State;

        };  // TDict::TPin

        /* Cache a reference to the elements. */
        TDict(const Var::TDict *var)
            : Type(var->GetType()) {
          for (const auto &elem: var->GetVal()) {
            Elems.push_back(elem);
          }
          std::sort(Elems.begin(), Elems.end());
        }

        /* See Sabot::State::TArrayOfScalars<>. */
        virtual size_t GetElemCount() const override {
          assert(this);
          return Elems.size();
        }

        /* See Sabot::State::TMap. */
        virtual Sabot::Type::TMap *GetMapType(void *buf) const override {
          return dynamic_cast<Sabot::Type::TMap *>(Type::NewSabot(buf, Type));
        }

        /* See Sabot::State::TArrayOfScalars<>. */
        virtual TPinBase *Pin(void *buf) const override {
          return new (buf) TPin(this);
        }

        private:

        /* Our type. */
        Type::TType Type;

        /* Our elements. */
        std::vector<std::pair<Var::TVar, Var::TVar>> Elems;

      };  // TDict

      /* Sabot for Var::TAddr. */
      class TAddr final
          : public Sabot::State::TTuple {
        public:

        /* The base class which our pin finalizes. */
        using TPinBase = Sabot::State::TTuple::TPin;

        /* Our pin. */
        class TPin final
            : public TPinBase {
          public:

          /* Point at our state's array. */
          TPin(const TAddr *state)
              : TPinBase(state), State(state) {
            assert(state);
          }

          private:

          /* See TPinBase. */
          virtual Sabot::State::TAny *NewElemInRange(size_t elem_idx, void *buf) const override {
            assert(this);
            const auto &field = State->Elems[elem_idx];
            return NewSabot(buf, field.second, field.first);
          }

          /* The state which we pin. */
          const TAddr *State;

        };  // TAddr::TPin

        /* Cache a reference to the elements. */
        TAddr(const Var::TAddr *var)
            : Elems(var->GetVal()), Type(var->GetType()) {}

        /* See Sabot::State::TTuple. */
        virtual Sabot::Type::TTuple *GetTupleType(void *buf) const override {
          return dynamic_cast<Sabot::Type::TTuple *>(Type::NewSabot(buf, Type));
        }

        /* See Sabot::State::TArrayOfScalars<>. */
        virtual TPinBase *Pin(void *buf) const override {
          return new (buf) TPin(this);
        }

        private:

        /* Our elements. */
        const Var::TAddr::TAddrType &Elems;

        /* Our type. */
        Type::TType Type;

      };  // TAddr

      /* Sabot for Var::TObj. */
      class TObj final
          : public Sabot::State::TRecord {
        public:

        /* The base class which our pin finalizes. */
        using TPinBase = Sabot::State::TRecord::TPin;

        /* Our pin. */
        class TPin final
            : public TPinBase {
          public:

          /* Point at our state's array. */
          TPin(const TObj *state)
              : TPinBase(state), State(state) {
            assert(state);
          }

          private:

          /* See TPinBase. */
          virtual Sabot::State::TAny *NewElemInRange(size_t elem_idx, void *buf) const override {
            assert(this);
            return NewSabot(buf, State->Elems[elem_idx].second);
          }

          /* The state which we pin. */
          const TObj *State;

        };  // TObj::TPin

        /* Cache a reference to the elements. */
        TObj(const Var::TObj *var)
            : Type(var->GetType()) {
          for (const auto &item: var->GetVal()) {
            Elems.push_back(item);
          }
          std::sort(Elems.begin(), Elems.end());
        }

        /* See Sabot::State::TRecord. */
        virtual Sabot::Type::TRecord *GetRecordType(void *buf) const override {
          return dynamic_cast<Sabot::Type::TRecord *>(Type::NewSabot(buf, Type));
        }

        /* See Sabot::State::TArrayOfScalars<>. */
        virtual TPinBase *Pin(void *buf) const override {
          return new (buf) TPin(this);
        }

        private:

        /* Our type. */
        Type::TType Type;

        /* Our elements. */
        std::vector<std::pair<std::string, Var::TVar>> Elems;

      };  // TObj

    }  // SS

  }  // Var

  namespace Native {

    /* Explicit specialization of native state sabot factory for vars. */
    template <>
    class State::Factory<Var::TVar> final {
      NO_CONSTRUCTION(Factory);
      public:

      /* Construct a new state sabot around the value. */
      static Sabot::State::TAny *New(const Var::TVar &val, void *state_alloc) {
        return Var::NewSabot(state_alloc, val);
      }

    };  // State::Factory<Var::TVar>

  }  // Native

}  // Stig
