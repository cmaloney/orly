/* <stig/closure.h>

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
#include <map>
#include <memory>
#include <string>

#include <base/thrower.h>
#include <io/binary_input_stream.h>
#include <io/binary_output_stream.h>
#include <stig/atom/kit2.h>
#include <stig/atom/suprena.h>
#include <stig/sabot/to_native.h>

namespace Stig {

  /* TODO */
  class TClosure {
    public:

    /* TODO */
    DEFINE_ERROR(TUnknownArgName, std::invalid_argument, "the closure contains no argument by the given name");

    /* Convenience. */
    using TCoreByName = std::map<std::string, Atom::TCore>;

    /* Optimizes access to a map so that sequential access (which is the norm) is very efficient. */
    class TWalker final {
      public:

      /* Caches the pointer. */
      TWalker(const TClosure *closure)
          : Closure(closure) {
        assert(closure);
        Rewind();
      }

      /* Return an iterator to the nth element in the map. */
      TCoreByName::const_iterator operator[](size_t elem_idx) const {
        assert(this);
        assert(elem_idx < Closure->CoreByName.size());
        if (ElemIdx > elem_idx) {
          Rewind();
        }
        while (ElemIdx < elem_idx) {
          ++Iter;
          ++ElemIdx;
        }
        return Iter;
      }

      /* The closure whose map we're walking. */
      const TClosure *GetClosure() const {
        assert(this);
        return Closure;
      }

      private:

      /* Send Iter and ElemIdx back to the beginning of the map. */
      void Rewind() const {
        assert(this);
        Iter = Closure->CoreByName.begin();
        ElemIdx = 0;
      }

      /* See accessor. */
      const TClosure *Closure;

      /* Our current position in the map. */
      mutable TCoreByName::const_iterator Iter;

      /* Our current index into the map. */
      mutable size_t ElemIdx;

    };  // TWalker

    /* The type sabot we use when constructing our fake record object. */
    class TType final
        : public Sabot::Type::TRecord {
      public:

      /* The base for record-type pins. */
      using TPinBase = Sabot::Type::TRecord::TPin;

      /* Our final version of TPinBase. */
      class TPin final
          : public TPinBase {
        public:

        /* Initialzies the base and the walker. */
        TPin(const TType *type)
            : TPinBase(type), Walker(type->Closure) {}

        /* See TPinBase. */
        virtual TAny *NewElem(size_t elem_idx, std::string &name, void *type_alloc) const override;

        /* See TPinBase. */
        virtual TAny *NewElem(size_t elem_idx, void *&out_field_name_state, void *field_name_state_alloc, void *type_alloc) const override;

        /* See TPinBase. */
        virtual TAny *NewElem(size_t elem_idx, void *type_alloc) const override;

        private:

        /* Walks the map for us. */
        TWalker Walker;

      };  // TPin

      /* Caches the pointer. */
      TType(const TClosure *closure)
          : Closure(closure) {
        assert(closure);
      }

      /* See Sabot::Type::TNAry. */
      virtual size_t GetElemCount() const override;

      /* See Sabot::Type::TRecord. */
      virtual TPinBase *Pin(void *alloc) const override;

      private:

      /* The closure we wrap. */
      const TClosure *Closure;

    };  // TType

    /* The state sabot we use when constructing our fake record object. */
    class TState final
        : public Sabot::State::TRecord {
      public:

      /* The base for record-state pins. */
      using TPinBase = Sabot::State::TRecord::TPin;

      /* Our final version of TPinBase. */
      class TPin final
          : public TPinBase {
        public:

        /* Initialzies the base and the walker. */
        TPin(const TState *state)
            : TPinBase(state), Walker(state->Closure) {}

        private:

        /* See TPinBase. */
        virtual TAny *NewElemInRange(size_t elem_idx, void *state_alloc) const override;

        /* Walks the map for us. */
        TWalker Walker;

      };  // TPin

      /* Caches the pointer. */
      TState(const TClosure *closure)
          : Closure(closure) {}

      /* See Sabot::State::TRecord. */
      virtual Sabot::Type::TRecord *GetRecordType(void *type_alloc) const override;

      /* See Sabot::State::TArrayOfSingleStates. */
      virtual TPinBase *Pin(void *alloc) const override;

      private:

      /* The closure we wrap. */
      const TClosure *Closure;

    };  // TState

    /* TODO */
    TClosure() {
      Reset();
    }

    /* TODO */
    explicit TClosure(const std::string &method_name);

    /* TODO */
    template <typename... TPairs>
    TClosure(const std::string &method_name, const TPairs &... pairs)
        : TClosure(method_name) {
      ArgsAdder<TPairs...>::AddArgs(this, pairs...);
    }

    /* TODO */
    void AddArgBySabot(const std::string &name, const Sabot::State::TAny *state) {
      assert(this);
      AddCore(name, Atom::TCore(Arena.get(), state));
    }

    /* TODO */
    template <typename TVal>
    TVal &GetArg(const std::string &name, TVal &out) const {
      assert(this);
      void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
      Sabot::State::TAny::TWrapper state(GetCore(name)->NewState(Arena.get(), state_alloc));
      Sabot::ToNative(*state, out);
      return out;
    }

    /* TODO */
    const std::shared_ptr<Atom::TSuprena> &GetArena() const {
      assert(this);
      return Arena;
    }

    /* TODO */
    size_t GetArgCount() const {
      assert(this);
      return CoreByName.size();
    }

    /* TODO */
    const TCoreByName &GetCoreByName() const {
      assert(this);
      return CoreByName;
    }

    /* TODO */
    const std::string &GetMethodName() const {
      assert(this);
      return MethodName;
    }

    /* TODO */
    void Read(Io::TBinaryInputStream &strm);

    /* TODO */
    void Reset();

    /* TODO */
    void Write(Io::TBinaryOutputStream &strm) const;

    private:

    /* TODO */
    template <typename... TArgs>
    class ArgsAdder;

    /* TODO */
    template <typename TVal>
    bool AddArg(const std::string &name, const TVal &val) {
      assert(this);
      void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
      Sabot::State::TAny::TWrapper state(Native::State::New(val, state_alloc));
      return AddCore(name, Atom::TCore(Arena.get(), state));
    }

    /* TODO */
    bool AddCore(const std::string &name, const Atom::TCore &core);

    /* TODO */
    const Atom::TCore *GetCore(const std::string &name) const;

    /* TODO */
    const Atom::TCore *TryGetCore(const std::string &name) const;

    /* TODO */
    std::shared_ptr<Atom::TSuprena> Arena;

    /* TODO */
    std::string MethodName;

    /* TODO */
    TCoreByName CoreByName;

  };  // TClosure

  /* TODO */
  template <>
  class TClosure::ArgsAdder<> {
    public:

    /* TODO */
    static void AddArgs(TClosure *) {}

  };  // TClosure::ArgsAdder<>

  /* TODO */
  template <typename TVal, typename... TMorePairs>
  class TClosure::ArgsAdder<std::string, TVal, TMorePairs...> {
    public:

    /* TODO */
    static void AddArgs(TClosure *closure, const std::string &name, const TVal &val, const TMorePairs &... more_pairs) {
      closure->AddArg(name, val);
      ArgsAdder<TMorePairs...>::AddArgs(closure, more_pairs...);
    }

  };  // TClosure::ArgsAdder<std::string, TVal, TMorePairs...>

  /* Binary stream extractor for Stig::TClosure. */
  inline Io::TBinaryInputStream &operator>>(Io::TBinaryInputStream &strm, TClosure &that) {
    assert(&that);
    that.Read(strm);
    return strm;
  }

  /* Binary stream inserter for Stig::TClosure. */
  inline Io::TBinaryOutputStream &operator<<(Io::TBinaryOutputStream &strm, const TClosure &that) {
    assert(&that);
    that.Write(strm);
    return strm;
  }

}  // Stig