/* <stig/rt/generator.h>

   Generator base classes.

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

#include <c14/lang.h>
#include <memory>

#include <base/iter.h>
#include <base/no_copy_semantics.h>
#include <stig/rt/containers.h>
#include <stig/rt/opt.h>
#include <stig/rt/regex_splitter.h>
#include <stig/rt/runtime_error.h>

namespace Stig {

  namespace Rt {

    class TPastEndError : public TRuntimeError, public Base::TFinalError<TPastEndError> {
      public:

      /* Constructor. */
      TPastEndError(const Base::TCodeLocation &loc) {
        PostCtor(loc, "Sequence iterator advanced past end");
      }
    };  // TPastEndError

    /* A generator of TItem_ */
    template <typename TItem_>
    class TGenerator {
      NO_COPY_SEMANTICS(TGenerator);
      public:

      typedef std::shared_ptr<const TGenerator> TPtr;

      virtual ~TGenerator() {}

      typedef const TItem_ TItem;

      //TODO: This one func causes a lot of inefficiency / ugh.
      virtual Base::TIterHolder<TItem> NewCursor() const = 0;

      protected:
      TGenerator() {}
    };

    /* TODO
       NOTE: The dependence on the value generator's type here is not strictly necessary, but it allows us to keep away
       from the buisiness of managing weak_ptr to self for now. It is likely this is the sign of impending doom. */
    template <typename TRes>
    class TFilterGenerator
        : public TGenerator<TRes>, public std::enable_shared_from_this<TFilterGenerator<TRes>> {
      NO_COPY_SEMANTICS(TFilterGenerator);
      public:

      typedef std::shared_ptr<const TFilterGenerator> TPtr;
      typedef typename TGenerator<TRes>::TPtr TValGenPtr;
      typedef const TRes TItem;

      //TODO: Capture meta_ctx, ctx, predicate_binding_map, effect_binding_map
      typedef std::function<bool (const TRes&)> TFilterFunc;

      static TFilterGenerator::TPtr New(const TFilterFunc &filter, const TValGenPtr &ptr) {
        return TPtr(new TFilterGenerator(filter, ptr));
      }

      class TCursor : public Base::TIter<TItem> {
        public:

        TCursor(const TPtr &ptr) : Iter(ptr->GetGenerator()->NewCursor()), Ptr(ptr), State(NotAcquired) {}
        TCursor(const TCursor &that) : Iter(that.Iter), Ptr(that.Ptr), State(NotAcquired) {}
        TCursor(TCursor &&that) : Iter(std::move(that.Iter)), Ptr(std::move(that.Ptr)), State(NotAcquired) {}

        operator bool() const {
          assert(this);
          Verify();

          return State != AcquiredEnd;
        }

        TItem &operator*() const {
          assert(this);

          Verify();

          return *Iter;
        }

        Base::TIter<TItem> &operator++() {
          if (State == AcquiredEnd) {
            throw TPastEndError(HERE);
          }
          Advance();

          return *this;
        }

        private:

        void Advance() const {
          switch(State) {
            case NotAcquired:
              State = Acquired;
              if (!Iter) {
                State = AcquiredEnd;
                break;
              }
              if (Ptr->GetFilter()(*Iter)) {
                State = Acquired;
                break;
              }
              //Fall Through
            case Acquired: {
              while (Iter) {
                ++Iter;
                if(Iter && Ptr->GetFilter()(*Iter)) {
                  break;
                }
              }
              if(!Iter){
                State = AcquiredEnd;
              }
              break;
            }
            case AcquiredEnd:
              break;
          }
        }

        void Verify() const {
          switch(State) {
            case NotAcquired:
              Advance();
              break;
            case Acquired:
            case AcquiredEnd:
              break;
          }
        }

        mutable Base::TIterHolder<const TItem> Iter;
        TPtr Ptr;
        mutable enum TState { NotAcquired, Acquired, AcquiredEnd } State;
      };

      virtual Base::TIterHolder<const TRes> NewCursor() const {
        return MakeHolder(new TCursor(this->shared_from_this()));
      }

      const TFilterFunc &GetFilter() const {
        assert(this);

        return Filter;
      }

      const TValGenPtr &GetGenerator() const {
        assert(this);

        return Generator;
      }

      private:

      TFilterGenerator(const TFilterFunc &filter, const TValGenPtr &ptr)
          : Filter(filter), Generator(ptr) {}

      TFilterFunc Filter;
      TValGenPtr Generator;
    }; // TFilterGenerator<TRes>

    template <typename TRes, typename TSrc>
    class TMapGenerator
        : public TGenerator<TRes>, public std::enable_shared_from_this<TMapGenerator<TRes, TSrc>> {
      NO_COPY_SEMANTICS(TMapGenerator);
      public:

      typedef std::shared_ptr<const TMapGenerator> TPtr;
      typedef typename TGenerator<TSrc>::TPtr TValGenPtr;
      typedef std::function<TRes (const TSrc&)> TFunc;
      typedef const TRes TItem;


      static TMapGenerator::TPtr New(const TFunc &func, const TValGenPtr &val_gen) {
        assert(&func);
        assert(func);
        assert(&val_gen);

        return TPtr(new TMapGenerator(func, val_gen));
      }

      class TCursor : public Base::TIter<TItem> {
        public:
        TCursor(const TPtr &ptr) : Iter(ptr->GetGenerator()->NewCursor()), Ptr(ptr), State(NotAcquired) {}
        TCursor(const TCursor &that) : Iter(that.Iter), Ptr(that.Ptr), State(NotAcquired) {}
        TCursor(TCursor &&that)
            : Iter(std::move(that.Iter)), Ptr(std::move(that.Ptr)), State(NotAcquired) {}

        operator bool() const {
          assert(this);
          Verify();

          return State != AcquiredEnd;
        }

        TItem &operator*() const {
          assert(this);

          Verify();
          assert(State == Acquired);
          return Item.GetVal();
        }

        Base::TIter<TItem> &operator++() {
          if (State == AcquiredEnd) {
            throw TPastEndError(HERE);
          }
          Advance();

          return *this;
        }

        private:

        void Advance() const {
          switch(State) {
            case NotAcquired:
              if (!Iter) {
                State = AcquiredEnd;
                break;
              }
              Item = Ptr->GetFunc()(*Iter);
              State = Acquired;
              break;
            case Acquired: {
              ++Iter;
              if(!Iter) {
                State = AcquiredEnd;
                break;
              }
              Item = Ptr->GetFunc()(*Iter);
              break;
            }
            case AcquiredEnd:
              break;
          }
        }

        void Verify() const {
          switch(State) {
            case NotAcquired:
              Advance();
              break;
            case Acquired:
            case AcquiredEnd:
              break;
          }
        }

        /* TODO: The use of an optional here to cache the result is less than ideal but necessary in order to handle
           that TRes may not have a default constructor. */
        mutable TOpt<TRes> Item;
        mutable Base::TIterHolder<const TSrc> Iter;
        TPtr Ptr;
        mutable enum TState { NotAcquired, Acquired, AcquiredEnd } State;
      };

      virtual Base::TIterHolder<TItem> NewCursor() const {
        return MakeHolder(new TCursor(this->shared_from_this()));
      }

      const TFunc &GetFunc() const {
        assert(this);

        return Func;
      }

      const TValGenPtr &GetGenerator() const {
        assert(this);
        return ValGen;
      }

      private:
      TMapGenerator(const TFunc &func, const TValGenPtr &val_gen) : ValGen(val_gen), Func(func) {}

      TValGenPtr ValGen;
      TFunc Func;
    };

    //TODO: Zip, Product, Chain, Mix?


  /* A generator wrapping TRegexMatcher. */
    class TMatchGenerator final
      : public TGenerator<std::string>,
        public std::enable_shared_from_this<TMatchGenerator> {
      NO_COPY_SEMANTICS(TMatchGenerator);
      public:

      /* Convenience. */
      typedef std::shared_ptr<const TMatchGenerator> TPtr;

      /* TODO */
      class TCursor final
        : public Base::TIter<TItem> {
        public:

        /* TODO */
        TCursor(const TPtr &ptr)
            : Ptr(ptr), RegexMatcher(&(ptr->Regex), ptr->Text.c_str()) {}

        /* TODO */
        virtual operator bool() const override {
          assert(this);
          return RegexMatcher;
        }

        /* TODO */
        virtual const std::string &operator*() const override {
          assert(this);
          return *RegexMatcher;
        }

        /* Iterates over the split text as long as we still find delimiters */
        virtual Base::TIter<TItem> &operator++() override {
          assert(this);
          if (!RegexMatcher) {
            throw TPastEndError(HERE);
          } else {
            ++RegexMatcher;
          }
          return *this;
        }

        private:

        /* TODO */
        TPtr Ptr;

        /* TODO */
        TRegexMatcher RegexMatcher;

      };  // TMatchGenerator::TCursor

      /* Cache a copy of the text and construct the regex. */
      TMatchGenerator(const std::string &text, const std::string &delim)
          : Text(text), Regex(delim.c_str()) {
      }

      /* See base class. */
      virtual Base::TIterHolder<TItem> NewCursor() const {
        return MakeHolder(new TCursor(shared_from_this()));
      }

      /* Factory. */
      static TPtr New(const std::string &text, const std::string &delim) {
        return TPtr(new TMatchGenerator(text, delim));
      }

      private:

      /* The text we're matching on  */
      std::string Text;

      /* The regex we're matching */
      TRegexMatcher::TRegex Regex;

    };  // TMatchGenerator


    class TRangeGenerator
        : public TGenerator<int64_t>, public std::enable_shared_from_this<TRangeGenerator> {
      NO_COPY_SEMANTICS(TRangeGenerator);
      public:

      typedef std::shared_ptr<const TRangeGenerator> TPtr;

      static TPtr NewWithSecond(int64_t start, int64_t limit, bool include_limit, int64_t second) {
        return TPtr(new TRangeGenerator(start, limit, second - start, include_limit));
      }
      static TPtr NewWithSecond(int64_t start, int64_t second) {
        return TPtr(new TRangeGenerator(start, second - start));
      }
      static TPtr New(int64_t start, int64_t limit, bool include_limit) {
        return TPtr(new TRangeGenerator(start, limit, limit - start > 0 ? 1 : -1, include_limit));
      }
      static TPtr New(int64_t start) {
        return TPtr(new TRangeGenerator(start, 1));
      }

      //TODO: It would probably be better just to coppy in stride/limt....
      class TCursor : public Base::TIter<const int64_t> {
        public:
        TCursor(const TPtr &ptr) : Cur(ptr->GetStart()), Ptr(ptr) {}
        TCursor(const TCursor &that) : Cur(that.Cur), Ptr(that.Ptr) {}
        TCursor(TCursor &&that) : Cur(std::move(that.Cur)), Ptr(std::move(that.Ptr)) {}

        operator bool() const {
          assert(this);

          if (Ptr->HasEnd()) {
            bool res = (Ptr->GetStride() > 0 ? Cur < Ptr->GetLimit() : Cur > Ptr->GetLimit());
            if(Ptr->GetIncludeLimit()) {
              res |= Ptr->GetLimit() == Cur;
            }
            return res;
          }
          return true;
        }

        const int64_t &operator*() const {
          assert(this);
          if(!*this) {
            throw Rt::TSystemError(HERE, "Tried reading past end of Range Generator");
          }
          return Cur;
        }

        Base::TIter<const int64_t> &operator++() {
          assert(this);
          if(!*this) {
            throw TPastEndError(HERE);
          }
          Cur += Ptr->GetStride();

          return *this;
        }

        private:
        int64_t Cur;
        TPtr Ptr;
      };

      virtual Base::TIterHolder<const int64_t> NewCursor() const {
        return MakeHolder(new TCursor(shared_from_this()));
      }

      int64_t GetStart() const {
        assert(this);
        return Start;
      }

      int64_t GetStride() const {
        assert(this);
        return Stride;
      }

      int64_t GetLimit() const {
        assert(this);
        assert(Limit.IsKnown());
        return Limit.GetVal();
      }

      bool GetIncludeLimit() const {
        assert(this);
        assert(Limit.IsKnown());
        return IncludeLimit;
      }

      bool HasEnd() const {
        assert(this);
        return Limit.IsKnown();
      }

      private:
      /* Builds a new range generator starting at start, ending at limit, and stepping with stride. Throws a runtime
         error if the stride is not in the direction from start to limit. */
      TRangeGenerator(int64_t start, int64_t limit, int64_t stride, bool include_limit)
            : IncludeLimit(include_limit), Start(start), Stride(stride),  Limit(limit) {
        if (limit - start > 0 ? stride < 0 : stride > 0) {
          throw TSystemError(HERE, "Range stride must be in the direction from start to limit");
        }
      }
      TRangeGenerator(int64_t start, int64_t stride) : IncludeLimit(true), Start(start), Stride(stride) {}

      bool IncludeLimit;
      int64_t Start, Stride;
      TOpt<int64_t> Limit;
    }; // TRangeGenerator

  /* A generator wrapping TRegexSplitter. */
    class TSplitGenerator final
      : public TGenerator<TRegexSplitter::TItem>,
        public std::enable_shared_from_this<TSplitGenerator> {
      NO_COPY_SEMANTICS(TSplitGenerator);
      public:

      /* Convenience. */
      typedef std::shared_ptr<const TSplitGenerator> TPtr;

      /* TODO */
      class TCursor final
          : public Base::TIter<TItem> {
        public:

        /* TODO */
        TCursor(const TPtr &ptr)
            : Ptr(ptr), RegexSplitter(&(ptr->Regex), ptr->Text.c_str()), HasData(true) {}

        /* TODO */
        virtual operator bool() const override {
          assert(this);
          return HasData;
        }

        /* TODO */
        virtual const TRegexSplitter::TItem &operator*() const override {
          assert(this);
          assert(HasData);
          return *RegexSplitter;
        }

        /* Iterates over the split text as long as we still find delimiters */
        virtual Base::TIter<TItem> &operator++() override {
          assert(this);
          assert(HasData);
          if (std::get<1>(*RegexSplitter).IsUnknown()) {
            HasData = false;
          } else {
            ++RegexSplitter;
          }
          return *this;
        }

        private:

        /* TODO */
        TPtr Ptr;

        /* TODO */
        TRegexSplitter RegexSplitter;

        /* TODO */
        bool HasData;

      };  // TSplitGenerator::TCursor

      /* Cache a copy of the text and construct the regex. */
      TSplitGenerator(const std::string &text, const std::string &delim)
          : Text(text), Regex(delim.c_str()) {
      }

      /* See base class. */
      virtual Base::TIterHolder<TItem> NewCursor() const {
        return MakeHolder(new TCursor(shared_from_this()));
      }

      /* Factory. */
      static TPtr New(const std::string &text, const std::string &delim) {
        return TPtr(new TSplitGenerator(text, delim));
      }

      private:

      /* The text we split. */
      std::string Text;

      /* The regex we use to recognize delimiters. */
      TRegexSplitter::TRegex Regex;

    };  // TSplitGenerator

    template <typename TContainer>
    class TStlGenerator
        : public TGenerator<typename TContainer::value_type>,
          public std::enable_shared_from_this<TStlGenerator<TContainer>> {
      NO_COPY_SEMANTICS(TStlGenerator);
      public:

      typedef std::shared_ptr<const TStlGenerator> TPtr;

      static TStlGenerator::TPtr New(const TContainer &data) {
        return TPtr(new TStlGenerator(data));
      }

      static TStlGenerator::TPtr New(TContainer &&data) {
        return TPtr(new TStlGenerator(std::move(data)));
      }

      class TCursor : public Base::TIter<const typename TContainer::value_type> {
        public:
        typedef const typename TContainer::value_type TVal;

        TCursor(const TPtr &ptr) : Ptr(ptr), Cur(ptr->GetContainer().begin()), End(ptr->GetContainer().end()) {}
        TCursor(const TCursor &that) : Ptr(that.Ptr), Cur(that.Cur), End(that.End) {}
        TCursor(TCursor &&that) : Ptr(std::move(that.Ptr)), Cur(std::move(that.Cur)), End(std::move(that.End)) {}

        operator bool() const {
          assert(this);

          return Cur != End;
        }

        TVal &operator*() const {
          assert(this);
          return *Cur;
        }

        Base::TIter<TVal> &operator++() {
          assert(this);
          if(!*this) {
            throw TPastEndError(HERE);
          }

          ++Cur;

          return *this;
        }

        private:
        /* Pointer to the generator because it holds what we are iterating over. */
        TPtr Ptr;
        typename TContainer::const_iterator Cur, End;
      };

      const TContainer &GetContainer() const {
        assert(this);
        return Data;
      }

      virtual Base::TIterHolder<const typename TContainer::value_type> NewCursor() const {
        return MakeHolder(new TCursor(this->shared_from_this()));
      }

      private:

      TStlGenerator(const TContainer &data) : Data(data) {}
      TStlGenerator(TContainer &&data) : Data(std::move(data)) {}

      const TContainer Data;
    }; // TStlGenerator<TContainer>

    /* An explicit specialization to deal with the weirdness that is std::vector<bool>. */
    template <>
    class TStlGenerator<std::vector<bool>>
        : public TGenerator<bool>,
          public std::enable_shared_from_this<TStlGenerator<std::vector<bool>>> {
      NO_COPY_SEMANTICS(TStlGenerator);
      public:

      typedef std::shared_ptr<const TStlGenerator> TPtr;

      static TStlGenerator::TPtr New(const std::vector<bool> &data) {
        return TPtr(new TStlGenerator(data));
      }

      static TStlGenerator::TPtr New(std::vector<bool> &&data) {
        return TPtr(new TStlGenerator(std::move(data)));
      }

      class TCursor : public Base::TIter<const typename std::vector<bool>::value_type> {
        public:
        typedef const typename std::vector<bool>::value_type TVal;

        TCursor(const TPtr &ptr) : Ptr(ptr), Cur(ptr->GetContainer().begin()), End(ptr->GetContainer().end()) {
          UpdateCachedVal();
        }

        TCursor(const TCursor &that) : Ptr(that.Ptr), Cur(that.Cur), End(that.End) {
          UpdateCachedVal();
        }

        TCursor(TCursor &&that) : Ptr(std::move(that.Ptr)), Cur(std::move(that.Cur)), End(std::move(that.End)) {
          UpdateCachedVal();
        }

        operator bool() const {
          assert(this);
          return Cur != End;
        }

        TVal &operator*() const {
          assert(this);
          if (Cur == End) {
            throw TPastEndError(HERE);
          }
          return CachedVal;
        }

        Base::TIter<TVal> &operator++() {
          assert(this);
          assert(this);
          if (Cur == End) {
            throw TPastEndError(HERE);
          }
          ++Cur;
          UpdateCachedVal();
          return *this;
        }

        private:

        void UpdateCachedVal() {
          assert(this);
          if (Cur != End) {
            CachedVal = *Cur;
          }
        }

        /* Pointer to the generator because it holds what we are iterating over. */
        TPtr Ptr;

        typename std::vector<bool>::const_iterator Cur, End;

        mutable bool CachedVal;

      };

      const std::vector<bool> &GetContainer() const {
        assert(this);
        return Data;
      }

      virtual Base::TIterHolder<const typename std::vector<bool>::value_type> NewCursor() const {
        return MakeHolder(new TCursor(this->shared_from_this()));
      }

      private:

      TStlGenerator(const std::vector<bool> &data) : Data(data) {}
      TStlGenerator(std::vector<bool> &&data) : Data(std::move(data)) {}

      const std::vector<bool> Data;

    }; // TStlGenerator<std::vector<bool>>

    template <typename TRes>
    class TTakeGenerator
        : public TGenerator<TRes>, public std::enable_shared_from_this<TTakeGenerator<TRes>> {
      NO_COPY_SEMANTICS(TTakeGenerator);
      public:

      typedef std::shared_ptr<const TTakeGenerator> TPtr;

      typedef typename TGenerator<TRes>::TPtr TValGenPtr;

      typedef const TRes TItem;

      static TTakeGenerator::TPtr New(int64_t count, const TValGenPtr &ptr) {
        return TPtr(new TTakeGenerator(count, ptr));
      }

      class TCursor : public Base::TIter<TItem> {
        public:

        TCursor(const TPtr &ptr)
            : CurCount(0),
              Iter(ptr->GetGenerator()->NewCursor()),
              Ptr(ptr) {}

        TCursor(const TCursor &that)
            : CurCount(that.CurCount),
              Iter(that.Iter),
              Ptr(that.Ptr) {}

        TCursor(TCursor &&that)
            : CurCount(std::move(that.CurCount)),
              Iter(std::move(that.Iter)),
              Ptr(std::move(that.Ptr)) {}

        operator bool() const {
          assert(this);
          return Iter && CurCount < Ptr->GetCount();
        }

        TItem &operator*() const {
          assert(this);
          if (!*this) {
            throw TPastEndError(HERE);
          }
          return *Iter;
        }

        Base::TIter<TItem> &operator++() {
          assert(this);
          if (!*this) {
            throw TPastEndError(HERE);
          }
          ++CurCount;
          ++Iter;
          return *this;
        }

        private:

        int64_t CurCount;

        Base::TIterHolder<const TItem> Iter;

        TPtr Ptr;

      };  // TCursor

      virtual Base::TIterHolder<const TRes> NewCursor() const {
        return MakeHolder(new TCursor(this->shared_from_this()));
      }

      int64_t GetCount() const {
        assert(this);
        return Count;
      }

      const TValGenPtr &GetGenerator() const {
        assert(this);
        return Generator;
      }

      private:

      TTakeGenerator(int64_t count, const TValGenPtr &generator)
          : Count(count), Generator(generator) {}

      int64_t Count;

      TValGenPtr Generator;

    };  // TTakeGenerator<TRes>

    template <typename TRes>
    class TSkipGenerator
        : public TGenerator<TRes>, public std::enable_shared_from_this<TSkipGenerator<TRes>> {
      NO_COPY_SEMANTICS(TSkipGenerator);
      public:

      typedef std::shared_ptr<const TSkipGenerator> TPtr;

      typedef typename TGenerator<TRes>::TPtr TValGenPtr;

      typedef const TRes TItem;

      static TSkipGenerator::TPtr New(int64_t count, const TValGenPtr &ptr) {
        return TPtr(new TSkipGenerator(count, ptr));
      }

      class TCursor : public Base::TIter<TItem> {
        public:

        TCursor(const TPtr &ptr)
            : CurCount(0),
              Iter(ptr->GetGenerator()->NewCursor()),
              Ptr(ptr) {
          for (; Iter && CurCount < Ptr->GetCount(); ++CurCount, ++Iter) {}
        }

        TCursor(const TCursor &that)
            : CurCount(that.CurCount),
              Iter(that.Iter),
              Ptr(that.Ptr) {
          for (; Iter && CurCount < Ptr->GetCount(); ++CurCount, ++Iter) {}
        }

        TCursor(TCursor &&that)
            : CurCount(std::move(that.CurCount)),
              Iter(std::move(that.Iter)),
              Ptr(std::move(that.Ptr)) {
          for (; Iter && CurCount < Ptr->GetCount(); ++CurCount, ++Iter) {}
        }

        operator bool() const {
          assert(this);
          return Iter;
        }

        TItem &operator*() const {
          assert(this);
          if (!*this) {
            throw TPastEndError(HERE);
          }
          return *Iter;
        }

        Base::TIter<TItem> &operator++() {
          assert(this);
          if (!*this) {
            throw TPastEndError(HERE);
          }
          ++CurCount;
          ++Iter;
          return *this;
        }

        private:

        int64_t CurCount;

        Base::TIterHolder<const TItem> Iter;

        TPtr Ptr;

      };  // TCursor

      virtual Base::TIterHolder<const TRes> NewCursor() const {
        return MakeHolder(new TCursor(this->shared_from_this()));
      }

      int64_t GetCount() const {
        assert(this);
        return Count;
      }

      const TValGenPtr &GetGenerator() const {
        assert(this);
        return Generator;
      }

      private:

      TSkipGenerator(int64_t count, const TValGenPtr &generator)
          : Count(count), Generator(generator) {}

      int64_t Count;

      TValGenPtr Generator;

    };  // TSkipGenerator<TRes>


    template <typename TRes>
    class TWhileGenerator
        : public TGenerator<TRes>, public std::enable_shared_from_this<TWhileGenerator<TRes>> {
      NO_COPY_SEMANTICS(TWhileGenerator);
      public:

      typedef std::shared_ptr<const TWhileGenerator> TPtr;

      typedef typename TGenerator<TRes>::TPtr TValGenPtr;

      typedef const TRes TItem;

      // drubin: putting this in from filter because I need a function for my while
      //TODO: Capture meta_ctx, ctx, predicate_binding_map, effect_binding_map
      typedef std::function<bool (const TRes&)> TWhileFunc;

      static TWhileGenerator::TPtr New(const TWhileFunc &while_func, const TValGenPtr &ptr) {
        return TPtr(new TWhileGenerator(while_func, ptr));
      }

      class TCursor : public Base::TIter<TItem> {
        public:

        TCursor(const TPtr &ptr)
            : Iter(ptr->GetGenerator()->NewCursor()),
              Ptr(ptr) {}

        TCursor(const TCursor &that)
            : Iter(that.Iter),
              Ptr(that.Ptr) {}

        TCursor(TCursor &&that)
            : Iter(std::move(that.Iter)),
              Ptr(std::move(that.Ptr)) {}

        operator bool() const {
          assert(this);
          return Iter && Ptr->GetWhileFunc()(*Iter);
        }

        TItem &operator*() const {
          assert(this);
          if (!*this) {
            throw TPastEndError(HERE);
          }
          return *Iter;
        }

        Base::TIter<TItem> &operator++() {
          assert(this);
          if (!*this) {
            throw TPastEndError(HERE);
          }
          ++Iter;
          return *this;
        }

        private:

        Base::TIterHolder<const TItem> Iter;

        TPtr Ptr;

      };  // TCursor

      virtual Base::TIterHolder<const TRes> NewCursor() const {
        return MakeHolder(new TCursor(this->shared_from_this()));
      }

      const TWhileFunc &GetWhileFunc() const {
        assert(this);
        return WhileFunc;
      }

      const TValGenPtr &GetGenerator() const {
        assert(this);
        return Generator;
      }

      private:

      TWhileGenerator(const TWhileFunc &while_func, const TValGenPtr &generator)
        : WhileFunc(while_func), Generator(generator) {}

      TWhileFunc WhileFunc;

      TValGenPtr Generator;

    };  // TWhileGenerator<TRes>

    template <typename TGeneratorType>
    auto MakeCursor(const std::shared_ptr<const TGeneratorType> &generator)
      DECLTYPE_AUTO(typename TGeneratorType::TCursor(generator));

    template <typename TVal>
    auto MakeCursor(const typename TGenerator<TVal>::TPtr &generator)
      DECLTYPE_AUTO(generator->NewCursor());

    template <typename TVal>
    auto MakeGenerator(const std::vector<TVal> &val)
      DECLTYPE_AUTO(TStlGenerator<std::vector<TVal>>::New(val));

    template <typename TVal>
    auto MakeGenerator(const TSet<TVal> &set)
      DECLTYPE_AUTO(TStlGenerator<TSet<TVal>>::New(set));

    template <typename TKey, typename TVal>
    auto MakeGenerator(const TDict<TKey, TVal> &dict)
      DECLTYPE_AUTO((TStlGenerator<TDict<TKey, TVal>>::New(dict)));

  } // Rt

} // Stig