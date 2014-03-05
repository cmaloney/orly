/* <stig/indy/util/merge_sorter.h>

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

#include <stig/atom/kit2.h>
#include <stig/indy/key.h>
#include <stig/indy/sequence_number.h>

namespace Stig {

  namespace Indy {

    namespace Util {

      /* TODO */
      template <typename TRef>
      class TKeySorter {
        NO_COPY_SEMANTICS(TKeySorter);
        public:

          /* TODO */
        class TMergeElement {
          NO_COPY_SEMANTICS(TMergeElement);
          public:

          /* TODO */
          TMergeElement(TKeySorter *sorter, const TKey &key, TSequenceNumber seq_num, TRef ref)
              : Next(0), Key(key), SeqNum(seq_num), Ref(ref) {
            TMergeElement *first = sorter->First;
            TMergeElement *prev = 0;
            for (;;) {
              if (!first) break;
              Atom::TComparison comp = first->Key.Compare(Key);
              if (Atom::IsLt(comp) || (Atom::IsEq(comp) && first->SeqNum >= SeqNum)) {
                prev = first;
                first = first->Next;
              } else {
                break;
              }
            }
            if (prev) {
              prev->Next = this;
            } else {
              sorter->First = this;
            }
            Next = first;
          }

          /* TODO */
          ~TMergeElement() {}

          private:

          /* TODO */
          TMergeElement *Next;

          /* TODO */
          const TKey &Key;

          /* TODO */
          TSequenceNumber SeqNum;

          /* TODO */
          TRef Ref;

          /* TODO */
          friend class TKeySorter;

        };  // TMergeElement

        /* TODO */
        TKeySorter() : First(0) {}

        /* TODO */
        ~TKeySorter() {
          while (First) {
            TMergeElement *first = First;
            First = First->Next;
            first->~TMergeElement();
          }
        }

        /* TODO */
        TRef Pop() {
          assert(this);
          assert(!IsEmpty());
          TRef ref = First->Ref;
          TMergeElement *first = First;
          First = First->Next;
          first->~TMergeElement();
          return ref;
        }

        /* TODO */
        bool IsEmpty() const {
          assert(this);
          return First == 0;
        }

        private:

        /* TODO */
        TMergeElement *First;

      };  // TKeySorter

      /* TODO */
      template <typename TRef>
      class TKeyCopySorter {
        NO_COPY_SEMANTICS(TKeyCopySorter);
        public:

          /* TODO */
        class TMergeElement {
          NO_COPY_SEMANTICS(TMergeElement);
          public:

          /* TODO */
          TMergeElement(TKeyCopySorter *sorter, const TKey &key, TSequenceNumber seq_num, TRef ref)
              : Next(0), Key(key), SeqNum(seq_num), Ref(ref) {
            TMergeElement *first = sorter->First;
            TMergeElement *prev = 0;
            for (;;) {
              if (!first) break;
              Atom::TComparison comp = first->Key.Compare(Key);
              if (Atom::IsLt(comp) || (Atom::IsEq(comp) && first->SeqNum >= SeqNum)) {
                prev = first;
                first = first->Next;
              } else {
                break;
              }
            }
            if (prev) {
              prev->Next = this;
            } else {
              sorter->First = this;
            }
            Next = first;
          }

          /* TODO */
          ~TMergeElement() {}

          private:

          /* TODO */
          TMergeElement *Next;

          /* TODO */
          TKey Key;

          /* TODO */
          TSequenceNumber SeqNum;

          /* TODO */
          TRef Ref;

          /* TODO */
          friend class TKeyCopySorter;

        };  // TMergeElement

        /* TODO */
        TKeyCopySorter() : First(0) {}

        /* TODO */
        ~TKeyCopySorter() {
          while (First) {
            TMergeElement *first = First;
            First = First->Next;
            first->~TMergeElement();
          }
        }

        /* TODO */
        const TKey &Peek() const {
          assert(this);
          assert(!IsEmpty());
          return First->Key;
        }

        /* TODO */
        TKey &Peek() {
          assert(this);
          assert(!IsEmpty());
          return First->Key;
        }

        /* TODO */
        TRef Pop(TSequenceNumber &seq_num) {
          assert(this);
          assert(!IsEmpty());
          TRef ref = First->Ref;
          seq_num = First->SeqNum;
          TMergeElement *first = First;
          First = First->Next;
          first->~TMergeElement();
          return ref;
        }

        /* TODO */
        bool IsEmpty() const {
          assert(this);
          return First == 0;
        }

        private:

        /* TODO */
        TMergeElement *First;

      };  // TKeyCopySorter

      /* TODO */
      template <typename TRef>
      class TCoreSorter {
        NO_COPY_SEMANTICS(TCoreSorter);
        public:

          /* TODO */
        class TMergeElement {
          NO_COPY_SEMANTICS(TMergeElement);
          public:

          /* TODO */
          TMergeElement(TCoreSorter *sorter, const Atom::TCore *core, Atom::TCore::TArena *arena, TSequenceNumber seq_num, TRef ref)
              : Next(0), Core(core), Arena(arena), SeqNum(seq_num), Ref(ref) {
            TMergeElement *first = sorter->First;
            TMergeElement *prev = 0;
            void *lhs_state_alloc = alloca(Sabot::State::GetMaxStateSize() * 2);
            void *rhs_state_alloc = reinterpret_cast<uint8_t *>(lhs_state_alloc) + Sabot::State::GetMaxStateSize();
            for (;;) {
              if (!first) break;
              Atom::TComparison comp;
              if (!first->Core->TryQuickOrderComparison(first->Arena, *Core, Arena, comp)) {
                comp = Sabot::OrderStates(*Sabot::State::TAny::TWrapper(first->Core->NewState(first->Arena, lhs_state_alloc)),
                                          *Sabot::State::TAny::TWrapper(Core->NewState(Arena, rhs_state_alloc)));
              }
              if (Atom::IsLt(comp) || (Atom::IsEq(comp) && first->SeqNum >= SeqNum)) {
                prev = first;
                first = first->Next;
              } else {
                break;
              }
            }
            if (prev) {
              prev->Next = this;
            } else {
              sorter->First = this;
            }
            Next = first;
          }

          /* TODO */
          ~TMergeElement() {}

          private:

          /* TODO */
          TMergeElement *Next;

          /* TODO */
          const Atom::TCore *Core;

          /* TODO */
          Atom::TCore::TArena *Arena;

          /* TODO */
          TSequenceNumber SeqNum;

          /* TODO */
          TRef Ref;

          /* TODO */
          friend class TCoreSorter;

        };  // TMergeElement

        /* TODO */
        TCoreSorter() : First(0) {}

        /* TODO */
        ~TCoreSorter() {
          while (First) {
            TMergeElement *first = First;
            First = First->Next;
            first->~TMergeElement();
          }
        }

        /* TODO */
        void Clear() {
          while (First) {
            TMergeElement *first = First;
            First = First->Next;
            first->~TMergeElement();
          }
        }

        /* TODO */
        TRef Pop(TSequenceNumber &seq_num) {
          assert(this);
          assert(!IsEmpty());
          TRef ref = First->Ref;
          seq_num = First->SeqNum;
          TMergeElement *first = First;
          First = First->Next;
          first->~TMergeElement();
          return ref;
        }

        /* TODO */
        bool IsEmpty() const {
          assert(this);
          return First == 0;
        }

        private:

        /* TODO */
        TMergeElement *First;

      };  // TCoreSorter

      /* TODO */
      template <typename TRef>
      class TDurableSorter {
        NO_COPY_SEMANTICS(TDurableSorter);
        public:

          /* TODO */
        class TMergeElement {
          NO_COPY_SEMANTICS(TMergeElement);
          public:

          /* TODO */
          TMergeElement(TDurableSorter *sorter, const uuid_t &id, TSequenceNumber seq_num, TRef ref)
              : Next(0), SeqNum(seq_num), Ref(ref) {
            uuid_copy(Id, id);
            TMergeElement *first = sorter->First;
            TMergeElement *prev = 0;
            for (;;) {
              if (!first) break;
              int comp = uuid_compare(first->Id, Id);
              if (comp < 0 || (comp == 0 && first->SeqNum >= SeqNum)) {
                prev = first;
                first = first->Next;
              } else {
                break;
              }
            }
            if (prev) {
              prev->Next = this;
            } else {
              sorter->First = this;
            }
            Next = first;
          }

          /* TODO */
          ~TMergeElement() {}

          private:

          /* TODO */
          TMergeElement *Next;

          /* TODO */
          uuid_t Id;

          /* TODO */
          const TSequenceNumber SeqNum;

          /* TODO */
          const TRef Ref;

          /* TODO */
          friend class TDurableSorter;

        };  // TMergeElement

        /* TODO */
        TDurableSorter() : First(0) {}

        /* TODO */
        ~TDurableSorter() {
          while (First) {
            TMergeElement *first = First;
            First = First->Next;
            first->~TMergeElement();
          }
        }

        /* TODO */
        TRef Pop(TSequenceNumber &seq_num, uuid_t &id) {
          assert(this);
          assert(!IsEmpty());
          TRef ref = First->Ref;
          seq_num = First->SeqNum;
          uuid_copy(id, First->Id);
          TMergeElement *first = First;
          First = First->Next;
          first->~TMergeElement();
          return ref;
        }

        /* TODO */
        bool IsEmpty() const {
          assert(this);
          return First == 0;
        }

        private:

        /* TODO */
        TMergeElement *First;

      };  // TDurableSorter

      /* TODO */
      template <typename TVal, typename TRef, class TComparator = std::less<TVal>>
      class TMergeSorter {
        NO_COPY_SEMANTICS(TMergeSorter);
        public:

          /* TODO */
        class TMergeElement {
          NO_COPY_SEMANTICS(TMergeElement);
          public:

          /* TODO */
          TMergeElement(TMergeSorter *sorter, const TVal &val, TRef ref)
              : Next(0), Val(val), Ref(ref) {
            TMergeElement *first = sorter->First;
            TMergeElement *prev = 0;
            for (;;) {
              if (!first) break;
              if (sorter->Comp(first->Val, Val)) {
                prev = first;
                first = first->Next;
              } else {
                break;
              }
            }
            if (prev) {
              prev->Next = this;
            } else {
              sorter->First = this;
            }
            Next = first;
          }

          /* TODO */
          ~TMergeElement() {}

          private:

          /* TODO */
          TMergeElement *Next;

          /* TODO */
          const TVal &Val;

          /* TODO */
          TRef Ref;

          /* TODO */
          friend class TMergeSorter;

        };  // TMergeElement

        /* TODO */
        TMergeSorter(const TComparator &comp = std::less<TVal>())
            : Comp(comp), First(0) {}

        /* TODO */
        ~TMergeSorter() {
          while (First) {
            TMergeElement *first = First;
            First = First->Next;
            first->~TMergeElement();
          }
        }

        /* TODO */
        void Clear() {
          while (First) {
            TMergeElement *first = First;
            First = First->Next;
            first->~TMergeElement();
          }
        }

        /* TODO */
        const TVal &Peek() const {
          assert(this);
          assert(!IsEmpty());
          return First->Val;
        }

        /* TODO */
        const TVal &Pop(TRef &ref) {
          assert(this);
          assert(!IsEmpty());
          ref = First->Ref;
          const TVal &val = First->Val;
          TMergeElement *first = First;
          First = First->Next;
          first->~TMergeElement();
          return val;
        }

        /* TODO */
        bool IsEmpty() const {
          assert(this);
          return First == 0;
        }

        private:

        /* TODO */
        TComparator Comp;

        /* TODO */
        TMergeElement *First;

      };  // TMergeSorter

      /* TODO */
      template <typename TVal, typename TRef, class TComparator = std::less<TVal>>
      class TCopyMergeSorter {
        NO_COPY_SEMANTICS(TCopyMergeSorter);
        public:

          /* TODO */
        class TMergeElement {
          NO_COPY_SEMANTICS(TMergeElement);
          public:

          /* TODO */
          TMergeElement(TCopyMergeSorter *sorter, const TVal &val, TRef ref)
              : Next(0), Val(val), Ref(ref) {
            TMergeElement *first = sorter->First;
            TMergeElement *prev = 0;
            for (;;) {
              if (!first) break;
              if (sorter->Comp(first->Val, Val)) {
                prev = first;
                first = first->Next;
              } else {
                break;
              }
            }
            if (prev) {
              prev->Next = this;
            } else {
              sorter->First = this;
            }
            Next = first;
          }

          /* TODO */
          ~TMergeElement() {}

          private:

          /* TODO */
          TMergeElement *Next;

          /* TODO */
          const TVal Val;

          /* TODO */
          TRef Ref;

          /* TODO */
          friend class TCopyMergeSorter;

        };  // TMergeElement

        /* TODO */
        TCopyMergeSorter(const TComparator &comp = std::less<TVal>())
            : Comp(comp), First(0) {}

        /* TODO */
        ~TCopyMergeSorter() {
          while (First) {
            TMergeElement *first = First;
            First = First->Next;
            first->~TMergeElement();
          }
        }

        /* TODO */
        const TVal &Peek() const {
          assert(this);
          assert(!IsEmpty());
          return First->Val;
        }

        /* TODO */
        TVal Pop(TRef &ref) {
          assert(this);
          assert(!IsEmpty());
          ref = First->Ref;
          const TVal val = First->Val;
          TMergeElement *first = First;
          First = First->Next;
          first->~TMergeElement();
          return val;
        }

        /* TODO */
        bool IsEmpty() const {
          assert(this);
          return First == 0;
        }

        private:

        /* TODO */
        TComparator Comp;

        /* TODO */
        TMergeElement *First;

      };  // TCopyMergeSorter

    }  // Util

  }  // Indy

}  // Stig