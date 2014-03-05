/* <stig/rt/collated_by.h>

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

#include <memory>

#include <base/iter.h>
#include <base/no_copy_semantics.h>
#include <stig/rt/generator.h>
#include <stig/rt/opt.h>
#include <stig/rt/runtime_error.h>

namespace Stig {

  namespace Rt {

    template <typename TRes, typename TSrc, typename TReduceResult, typename THavingResult>
    class TCollatedByGenerator
        : public TGenerator<TRes>,
          public std::enable_shared_from_this<TCollatedByGenerator<TRes, TSrc, TReduceResult, THavingResult>> {
      NO_COPY_SEMANTICS(TCollatedByGenerator);
      public:

      using TPtr = std::shared_ptr<const TCollatedByGenerator>;

      using TValGenPtr = typename TGenerator<TSrc>::TPtr;

      // using TRes = std::tuple<THavingResult, TReduceResult>;

      using TItem = const TRes;

      using TReduceFunc = std::function<TReduceResult (const TReduceResult &carry, const TSrc &that)>;

      using THavingFunc = std::function<THavingResult (const TSrc &that)>;

      static TPtr New(
            const TReduceFunc &reduce_func,
            const THavingFunc &having_func,
            const TValGenPtr &generator,
            const TReduceResult &start) {
        return TPtr(new TCollatedByGenerator(reduce_func, having_func, generator, start));
      }

      class TCursor
          : public Base::TIter<TItem> {
        public:

        TCursor(const TPtr &ptr)
            : Iter(ptr->GetGenerator()->NewCursor()), Ptr(ptr) {}

        TCursor(const TCursor &that)
            : Iter(that.Iter), Ptr(that.Ptr) {}

        TCursor(TCursor &&that)
            : Iter(std::move(that.Iter)), Ptr(std::move(that.Ptr)) {}

        explicit operator bool() const {
          assert(this);
          return Iter || ResultCache.IsKnown();
        }

        TItem &operator*() const {
          assert(this);
          if (ResultCache.IsKnown()) {
            return ResultCache.GetVal();
          }  // if
          if (!*this) {
            throw TPastEndError(HERE);
          }  // if
          if (HavingResultCache.IsUnknown()) {
            HavingResultCache = Ptr->GetHavingFunc()(*Iter);
          }  // if
          assert(HavingResultCache.IsKnown());
          TReduceResult carry = Ptr->GetStart();
          carry = Ptr->GetReduceFunc()(carry, *Iter);
          for (++Iter; Iter; ++Iter) {
            const auto &previous = HavingResultCache.GetVal();
            const auto &current = Ptr->GetHavingFunc()(*Iter);
            if (previous != current) {
              break;
            }  // if
            carry = Ptr->GetReduceFunc()(carry, *Iter);
            HavingResultCache = current;
          }  // for
          ResultCache = TItem(HavingResultCache.GetVal(), carry);
          assert(ResultCache.IsKnown());
          return ResultCache.GetVal();
        }

        Base::TIter<TItem> &operator++() {
          assert(this);
          ResultCache.Reset();
          HavingResultCache.Reset();
          return *this;
        }

        private:

        mutable TOpt<TRes> ResultCache;

        mutable TOpt<THavingResult> HavingResultCache;

        mutable Base::TIterHolder<const TSrc> Iter;

        TPtr Ptr;

      };  // TCursor

      virtual Base::TIterHolder<TItem> NewCursor() const {
        return MakeHolder(new TCursor(this->shared_from_this()));
      }

      const TReduceFunc &GetReduceFunc() const {
        assert(this);
        return ReduceFunc;
      }

      const THavingFunc &GetHavingFunc() const {
        assert(this);
        return HavingFunc;
      }

      const TValGenPtr &GetGenerator() const {
        assert(this);
        return Generator;
      }

      const TReduceResult &GetStart() const {
        assert(this);
        return Start;
      }

      private:

      TCollatedByGenerator(
          const TReduceFunc &reduce_func,
          const THavingFunc &having_func,
          const TValGenPtr &generator,
          const TReduceResult &start)
            : ReduceFunc(reduce_func),
              HavingFunc(having_func),
              Generator(generator),
              Start(start) {}

      TReduceFunc ReduceFunc;

      THavingFunc HavingFunc;

      TValGenPtr Generator;

      TReduceResult Start;

    };  // TCollatedByGenerator

  }  // namespace Rt

}  // namespace Stig
