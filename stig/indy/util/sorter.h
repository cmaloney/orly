/* <stig/indy/util/sorter.h>

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

#include <syslog.h>

#include <cassert>
#include <iterator>

#include <base/no_copy_semantics.h>

namespace Stig {

  namespace Indy {

    namespace Util {

      /* TODO */
      template<typename TVal>
      class TRandomIterator
            : public std::iterator<std::random_access_iterator_tag, TVal> {
        protected:

        /* TODO */
        TVal *Data;

        public:

        /* TODO */
        typedef std::random_access_iterator_tag iterator_category;
        typedef typename std::iterator<std::random_access_iterator_tag, TVal>::value_type value_type;
        typedef typename std::iterator<std::random_access_iterator_tag, TVal>::difference_type difference_type;
        typedef typename std::iterator<std::random_access_iterator_tag, TVal>::reference reference;
        typedef typename std::iterator<std::random_access_iterator_tag, TVal>::pointer pointer;

        /* TODO */
        TRandomIterator()
            : Data(0) {}

        /* TODO */
        template<typename T2>
        TRandomIterator(const TRandomIterator<T2>& r)
            : Data(&(*r)) {}

        /* TODO */
        TRandomIterator(pointer data)
            : Data(data) {}

        /* TODO */
        template<typename T2>
        TRandomIterator& operator=(const TRandomIterator<T2>& r) {
          Data = &(*r);
          return *this;
        }

        /* TODO */
        TRandomIterator& operator++() {
          ++Data;
          return *this;
        }

        /* TODO */
        TRandomIterator& operator--() {
          --Data;
          return *this;
        }

        /* TODO */
        TRandomIterator operator++(int) {
          return TRandomIterator(Data++);
        }

        /* TODO */
        TRandomIterator operator--(int) {
          return TRandomIterator(Data--);
        }

        /* TODO */
        TRandomIterator operator+(const difference_type& n) const {
          return TRandomIterator(Data + n);
        }

        /* TODO */
        TRandomIterator& operator+=(const difference_type& n) {
          Data += n;
          return *this;
        }

        /* TODO */
        TRandomIterator operator-(const difference_type& n) const {
          return TRandomIterator(pointer(Data - n));
        }

        /* TODO */
        TRandomIterator& operator-=(const difference_type& n) {
          Data -= n;
          return *this;
        }

        /* TODO */
        reference operator*() const {
          return *Data;
        }

        /* TODO */
        pointer operator->() const {
          return Data;
        }

        /* TODO */
        reference operator[](const difference_type& n) const {
          return Data[n];
        }

        /* TODO */
        template<typename T>
        friend bool operator==(const TRandomIterator<T>& r1, const TRandomIterator<T>& r2);

        /* TODO */
        template<typename T>
        friend bool operator!=(const TRandomIterator<T>& r1, const TRandomIterator<T>& r2);

        /* TODO */
        template<typename T>
        friend bool operator<(const TRandomIterator<T>& r1, const TRandomIterator<T>& r2);

        /* TODO */
        template<typename T>
        friend bool operator>(const TRandomIterator<T>& r1, const TRandomIterator<T>& r2);

        /* TODO */
        template<typename T>
        friend bool operator<=(const TRandomIterator<T>& r1, const TRandomIterator<T>& r2);

        /* TODO */
        template<typename T>
        friend bool operator>=(const TRandomIterator<T>& r1, const TRandomIterator<T>& r2);

        /* TODO */
        template<typename T>
        friend typename TRandomIterator<T>::difference_type operator+(const TRandomIterator<T>& r1, const TRandomIterator<T>& r2);

        /* TODO */
        template<typename T>
        friend typename TRandomIterator<T>::difference_type operator-(const TRandomIterator<T>& r1, const TRandomIterator<T>& r2);

      };  // TRandomIterator

      /* TODO */
      template<typename T>
      bool operator==(const TRandomIterator<T>& r1, const TRandomIterator<T>& r2) {
        return r1.Data == r2.Data;
      }

      /* TODO */
      template<typename T>
      bool operator!=(const TRandomIterator<T>& r1, const TRandomIterator<T>& r2) {
        return r1.Data != r2.Data;
      }

      /* TODO */
      template<typename T>
      bool operator<(const TRandomIterator<T>& r1, const TRandomIterator<T>& r2) {
        return r1.Data < r2.Data;
      }

      /* TODO */
      template<typename T>
      bool operator>(const TRandomIterator<T>& r1, const TRandomIterator<T>& r2) {
        return r1.Data > r2.Data;
      }

      /* TODO */
      template<typename T>
      bool operator<=(const TRandomIterator<T>& r1, const TRandomIterator<T>& r2) {
        return r1.Data <= r2.Data;
      }

      /* TODO */
      template<typename T>
      bool operator>=(const TRandomIterator<T>& r1, const TRandomIterator<T>& r2) {
        return r1.Data >= r2.Data;
      }

      /* TODO */
      template<typename T>
      typename TRandomIterator<T>::difference_type operator+(const TRandomIterator<T>& r1, const TRandomIterator<T>& r2) {
        return TRandomIterator<T>(r1.Data + r2.Data);
      }

      /* TODO */
      template<typename T>
      typename TRandomIterator<T>::difference_type operator-(const TRandomIterator<T>& r1, const TRandomIterator<T>& r2) {
        return r1.Data - r2.Data;
      }

      /* TODO */
      template <typename TVal, size_t MaxSize>
      class TSorter {
        NO_COPY_SEMANTICS(TSorter);
        public:

        /* TODO */
        class TCursor {
          NO_COPY_SEMANTICS(TCursor);
          public:

          /* TODO */
          virtual operator bool() const = 0;

          /* TODO */
          virtual const TVal &operator*() const = 0;

          /* TODO */
          virtual TCursor &operator++() = 0;

          /* TODO */
          virtual ~TCursor() {}

          protected:

          /* TODO */
          TCursor() {}

        };  // TCursor

        /* TODO */
        class TMemCursor
            : public TCursor {
          NO_COPY_SEMANTICS(TMemCursor);
          public:

          /* TODO */
          TMemCursor(TSorter *sorter)
              : Iter(sorter->begin()),
                End(sorter->end()) {}

          /* TODO */
          virtual ~TMemCursor() {}

          /* TODO */
          virtual operator bool() const {
            assert(this);
            return Iter != End;
          }

          /* TODO */
          virtual const TVal &operator*() const {
            assert(this);
            assert(Iter != End);
            return *Iter;
          }

          /* TODO */
          virtual TCursor &operator++() {
            assert(this);
            assert(Iter != End);
            ++Iter;
            return *this;
          }

          private:

          /* TODO */
          TRandomIterator<TVal> Iter;

          /* TODO */
          TRandomIterator<TVal> End;

        };  // TMemCursor

        /* TODO */
        TSorter()
            : Size(0U) {
          Data = reinterpret_cast<TVal *>(malloc(sizeof(TVal) * MaxSize));
          if (Data == 0) {
            syslog(LOG_EMERG, "bad alloc in Util::Sorter [%ld]", sizeof(TVal) * MaxSize);
            throw std::bad_alloc();
          }
        }

        /* TODO */
        virtual ~TSorter() {
          assert(this);
          free(Data);
        }

        /* TODO */
        const TVal &operator[](size_t pos) const {
          assert(this);
          assert(pos < Size);
          return *(Data + pos);
        }

        /* TODO */
        template <class... Args>
        void Emplace(Args &&... args) {
          assert(Size < MaxSize);
          new (Data + Size) TVal(std::forward<Args>(args)...);
          ++Size;
        }

        /* TODO */
        size_t GetSize() const {
          assert(this);
          return Size;
        }

        /* TODO */
        bool IsFull() const {
          assert(this);
          return Size == MaxSize;
        }

        /* TODO */
        TRandomIterator<TVal> begin() const {
          assert(this);
          return TRandomIterator<TVal>(Data);
        }

        /* TODO */
        TRandomIterator<TVal> end() const {
          assert(this);
          return TRandomIterator<TVal>(Data + Size);
        }

        /* TODO */
        void Clear() {
          assert(this);
          Size = 0U;
        }

        /* TODO */
        static size_t GetMaxSize() {
          return MaxSize;
        }

        private:

        /* TODO */
        size_t Size;

        /* TODO */
        TVal *Data;

      };  // TSorter

    }  // Util

  }  // Indy

}  // Stig