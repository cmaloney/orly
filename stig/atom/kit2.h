/* <stig/atom/kit2.h>

   A flyweight which can store any of the types handled by the sabot system.

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

#include <iostream> /* TODO GET RID OF */

#include <cassert>
#include <cstdlib>
#include <mutex>
#include <stdexcept>
#include <string>

#include <base/layout.h>
#include <base/no_copy_semantics.h>
#include <stig/atom/comparison.h>
#include <stig/native/all.h>
#include <stig/sabot/match_prefix_type.h>
#include <stig/sabot/order_states.h>
#include <stig/sabot/state.h>
#include <stig/sabot/type.h>

namespace Stig {

  namespace Atom {

    /* The largest number of bytes which can be stored directly in a core. */
    static const size_t MaxDirectSize = 23;

    /* A flyweight which can store any of the types handled by the sabot system. */
    class PACKED TCore {
      public:

      /* Conveniences. */
      using State         = Sabot::State;
      using Type          = Sabot::Type;
      using TStdDuration  = Sabot::TStdDuration;
      using TStdTimePoint = Sabot::TStdTimePoint;
      using TUuid         = Base::TUuid;

      /* A position within an arena.  Zero is a legit offset. */
      using TOffset = uint64_t;

      /* A remapper of offsets from one arena to another. */
      using TRemap = std::function<TOffset (TOffset)>;

      /* An initializer for an array of cores. */
      using TInit1 = std::function<void (size_t elem_idx, void *elem)>;

      /* An initializer for an array of pairs of cores. */
      using TInit2 = std::function<void (size_t elem_idx, void *lhs, void *rhs)>;

      /* The definitions for these classes appear after TCore, below. */
      class TArena;
      class TExtensibleArena;
      class TNote;

      /* An error thrown attemtping to perform an operation on a core or note of the wrong type. */
      class TBadType
          : public std::runtime_error {
        public:

        /* Do-little. */
        TBadType();

      };  // TBadType

      /* An error thrown when corrupt data is encountered. */
      class TCorrupt
          : public std::runtime_error {
        public:

        /* Do-little. */
        TCorrupt();

      };  // TCorrupt

      /* A comparator for cores that assumes their offsets reference the same *ordered* arena. */
      class TOrderedArenaCompare {
        public:

        TOrderedArenaCompare(TArena *arena) : Arena(arena) {}

        /* TODO */
        bool operator()(const TCore &lhs, const TCore &rhs) const;

        private:

        /* TODO */
        TArena *const Arena;

      };  // TOrderedArenaCompare

      /* Default-construct as a void. */
      TCore();

      /* Construct using the given note as our indirect state. */
      TCore(TOffset offset, const TNote *note);

      /* Construct the default state of a type. */
      TCore(TExtensibleArena *arena, const Type::TAny &type);

      /* Construct a copy of a state. */
      TCore(TExtensibleArena *arena, const State::TAny *state);

      /* Construct from a native object.  Backward, you say?  Sure, but it avoids confusion with the sabot-based constructors. */
      template <typename TVal>
      TCore(const TVal &val, TExtensibleArena *arena, void *state_alloc)
          : TCore(arena, Sabot::State::TAny::TWrapper(Native::State::New(val, state_alloc))) {}

      /* Construct a new core copying the required data from that arena to this arena. */
      inline TCore(TExtensibleArena *arena, void *state_alloc, TArena *that_arena, const TCore &that);

      /* Construct a core with a shallow copy of the given one using the remapper. */
      inline TCore(const TCore &that, const TRemap &remap);

      /* Concatenate the left- and right-hand sides into a single core.  The two cores must be tuples and the result will be a tuple. */
      TCore(TExtensibleArena *arena, const State::TAny *lhs_state, const State::TAny *rhs_state);

      /* True if we're a tombstone. */
      inline bool IsTombstone() const;

      /* True if we're a tuple. */
      inline bool IsTuple() const;

      /* True if we're a free. */
      inline bool IsFree() const;

      /* TODO */
      inline size_t ForceGetIndirectHash() const;

      /* TODO */
      inline bool TryGetQuickHash(size_t &out_hash) const;

      /* Returns true if this core stored the hash value inside it's local storage. If so, out_hash is set to the stored value. */
      inline bool TryGetStoredHash(size_t &out_hash) const;

      /* Returns the indirect hash stored. We assert that this is an indirect core that has a hash value stored in it. */
      inline size_t ForceGetStoredHash() const;

      /* Tries to set the stored hash value of this core. Returns true if the core is an indirect core and the hash value was stored.
         This will return false if the core is a direct core. */
      inline bool TrySetStoredHash(size_t hash_val);

      /* TODO */
      inline bool TryQuickOrderComparison(TArena *this_arena, const TCore &that_core, TArena *that_arena, Atom::TComparison &comp) const;

      /* TODO */
      inline bool MatchType(TArena *this_arena, const TCore &that_core, TArena *that_arena) const;

      /* TODO */
      inline Stig::Sabot::TMatchResult PrefixMatch(TArena *this_arena, const TCore &that_core, TArena *that_arena) const;

      /* TODO */
      inline const uint32_t *TryGetElemCount() const;

      /* Get a sabot around our type. */
      Type::TAny *GetType(TArena *arena, void *type_alloc) const;

      /* Construct a new sabot around our state. */
      State::TAny *NewState(TArena *arena, void *state_alloc) const;

      /* Remap this core's offset from one arena to another.
         If the core contains only direct data, this functions does nothing. */
      void Remap(const TRemap &remap);

      /* If the core is a tuple of size 2 or greater, shorten the tuple by one element and return true.
         If the core is a tuple of size 1 or zero, do nothing and return false.
         It is an error to call this function on a non-tuple. */
      bool TryTruncateTuple();

      /* Split the core into the left- and right-hand sides.  The core must be a tuple. */
      bool TrySplit(
          TArena *arena, size_t lhs_size, TExtensibleArena *lhs_arena, TCore &lhs_core, TExtensibleArena *rhs_arena, TCore &rhs_core) const;

      /* return the address of the indirection offset if there is one. */
      const TOffset *TryGetOffset() const;

      private:

      /* Our type sabots. */
      class ST {
        NO_CONSTRUCTION(ST);
        public:

        /* Base classes. */
        template <typename TBase> class TArrayOfTypes;
        template <typename TBase> class TArrayOfPairsOfTypes;

        /* Empty types. */
        class TTombstone;
        class TVoid;

        /* Scalar types. */
        class TInt8;
        class TInt16;
        class TInt32;
        class TInt64;
        class TUInt8;
        class TUInt16;
        class TUInt32;
        class TUInt64;
        class TBool;
        class TChar;
        class TFloat;
        class TDouble;
        class TDuration;
        class TTimePoint;
        class TUuid;

        /* Arrays of types. */
        class TBlob;
        class TStr;

        /* Arrays of single types. */
        class TDesc;
        class TFree;
        class TOpt;
        class TSet;
        class TVector;

        /* Arrays of pairs of types. */
        class TMap;

        /* Arrays of static size. */
        class TRecord;
        class TTuple;

      };  // ST

      /* Our state sabots. */
      class SS {
        NO_CONSTRUCTION(SS);
        public:

        /* Base classes. */
        template <typename TBase> class TArrayOfStates;
        template <typename TBase> class TArrayOfPairsOfStates;

        /* Empty states. */
        class TFree;
        class TTombstone;
        class TVoid;

        /* Scalar states. */
        class TInt8;
        class TInt16;
        class TInt32;
        class TInt64;
        class TUInt8;
        class TUInt16;
        class TUInt32;
        class TUInt64;
        class TBool;
        class TChar;
        class TFloat;
        class TDouble;
        class TDuration;
        class TTimePoint;
        class TUuid;

        /* Arrays of scalars. */
        class TBlob;
        class TStr;

        /* Arrays of single states. */
        class TDesc;
        class TOpt;
        class TSet;
        class TVector;

        /* Arrays of pairs of states. */
        class TMap;

        /* Arrays of static size. */
        class TRecord;
        class TTuple;

      };  // SS

      /* The numeric type on which TTycon is based. */
      using TTyconNumeric = uint8_t;

      /* The kinds of things a core can store. */
      enum class TTycon : TTyconNumeric {

        /* Lowest range is for direct strings.  The value indicates the number of bytes
           remaining in direct storage and a zero doubles as our null terminator. */
        MinDirectStr = 0,
        MaxDirectStr = MinDirectStr + MaxDirectSize,

        /* Next range is for direct blobs.  Their tycons work just like those of direct strings,
           except we don't worry about null termination. */
        MinDirectBlob = MaxDirectStr + 1,
        MaxDirectBlob = MinDirectBlob  + MaxDirectSize,

        /* And then everybody else. */
        Int8 = MaxDirectBlob + 1, Int16, Int32, Int64, UInt8, UInt16, UInt32, UInt64, Bool, Char,
        Float, Double, Duration, TimePoint, Uuid, Blob, Str, Tombstone, Void,
        Desc, Free, Opt, Set, Vector, Map, Record, Tuple

      };  // TTycon

      /* Verify sizeof(TTycon). */
      static_assert(sizeof(TTycon) == 1, "The size of TTycon must one byte.");

      /* Construct a copy of a c-string.
         This constructor is used only to store the names of elements in records. */
      TCore(TExtensibleArena *arena, const char *c_str);

      /* Copy the string out of the core.  The core must be of a string type.
         This function is used only when accessing the name of elements in records. */
      void CopyOut(TArena *arena, std::string &out) const;

      /* Interpret direct data as the given type, regardless of the tycon. */
      template <typename TVal>
      const TVal &ForceAs() const {
        assert(this);
        return *reinterpret_cast<const TVal *>(DirectBlob);
      }

      /* Interpret direct data as the given type, regardless of the tycon. */
      template <typename TVal>
      TVal &ForceAs() {
        assert(this);
        return *reinterpret_cast<TVal *>(DirectBlob);
      }

      /* Zero the direct storage area and set the tycon. */
      void Init(TTycon tycon);

      /* Init from a scalar. */
      void InitInt8     (int8_t               val = 0              ) { InitDirect(TTycon::Int8,      val); }
      void InitInt16    (int16_t              val = 0              ) { InitDirect(TTycon::Int16,     val); }
      void InitInt32    (int32_t              val = 0              ) { InitDirect(TTycon::Int32,     val); }
      void InitInt64    (int64_t              val = 0              ) { InitDirect(TTycon::Int64,     val); }
      void InitUInt8    (uint8_t              val = 0              ) { InitDirect(TTycon::UInt8,     val); }
      void InitUInt16   (uint16_t             val = 0              ) { InitDirect(TTycon::UInt16,    val); }
      void InitUInt32   (uint32_t             val = 0              ) { InitDirect(TTycon::UInt32,    val); }
      void InitUInt64   (uint64_t             val = 0              ) { InitDirect(TTycon::UInt64,    val); }
      void InitBool     (bool                 val = false          ) { InitDirect(TTycon::Bool,      val); }
      void InitChar     (char                 val = '\0'           ) { InitDirect(TTycon::Char,      val); }
      void InitFloat    (float                val = 0              ) { InitDirect(TTycon::Float,     val); }
      void InitDouble   (double               val = 0              ) { InitDirect(TTycon::Double,    val); }
      void InitDuration (const TStdDuration  &val = TStdDuration() ) { InitDirect(TTycon::Duration,  val); }
      void InitTimePoint(const TStdTimePoint &val = TStdTimePoint()) { InitDirect(TTycon::TimePoint, val); }
      void InitUuid     (const TUuid         &val = TUuid()        ) { InitDirect(TTycon::Uuid,      val); }

      /* If the blob is small enough, store it directly; otherwise, propose an array of bytes and take it as our state. */
      void InitBlob(TExtensibleArena *arena, const uint8_t *start, const uint8_t *limit);

      /* If the small is small enough, store it directly; otherwise, propose a null-terminated array of chars and take it as our state. */
      void InitStr(TExtensibleArena *arena, const char *start, const char *limit);

      /* Set the tycon, then init the direct storage area by copy-constructing a value into it, padding with zeroes as needed. */
      template <typename TVal>
      void InitDirect(TTycon tycon, const TVal &val) {
        static_assert(sizeof(TVal) <= MaxDirectSize, "TVal is value too large to store directly.");
        assert(this);
        assert(&val);
        Init(tycon);
        new (DirectBlob) TVal(val);
      }

      /* Propose a note of an array of cores (copy-constructed from the given state) and take it as our state.
         If the given state has no elements, propose a note of a single core (default-constructed from the given type) and take it as our exemplar.*/
      void InitIndirectCoreArray(TTycon tycon, TExtensibleArena *arena, const Type::TUnary &type, const State::TArrayOfSingleStates &state);

      /* Propose a note of a single core (default-constructed from the given type) and take it either as our state or our exemplar. */
      void InitIndirectCoreArray(TTycon tycon, TExtensibleArena *arena, const Type::TUnary &type, bool is_exemplar);

      /* Propose a note of an array of pairs of cores (copy-constructed from the given state) and take it as our state.
         If the given state has no elements, propose a note of a single pair of cores (default-constructed from the given type) and take it as our
         exemplar.*/
      void InitIndirectCoreArray(TTycon tycon, TExtensibleArena *arena, const Type::TBinary &type, const State::TArrayOfPairsOfStates &state);

      /* Propose a note of a single pair of cores (default-constructed from the given type) and take it either as our state or our exemplar. */
      void InitIndirectCoreArray(TTycon tycon, TExtensibleArena *arena, const Type::TBinary &type, bool is_exemplar);

      /* Propose a note of an array of cores (default-constructed from the given tuple type) and take it either as our state or our exemplar. */
      void InitIndirectCoreArray(TExtensibleArena *arena, const Type::TTuple &type, bool is_exemplar);

      /* Propose a note of an array of pairs of cores (default-constructed from the given record type) and take it either as our state or our
         exemplar. */
      void InitIndirectCoreArray(TExtensibleArena *arena, const Type::TRecord &type, bool is_exemplar);

      /* The offset leads to an array of zero or more cores, of which we will take some number as our state or as our exemplar.
         Note: The array of zero cores can only arise from the empty tuple (or record) type (or state). */
      void InitIndirectCoreArray(TTycon tycon, TOffset offset, size_t elem_count, bool is_exemplar);

      /* The offset ledas to an array of zero or more scalars, which we will take as our state. */
      void InitIndirectScalarArray(TTycon tycon, TOffset offset, size_t size);

      /* If the given state has one or more elements, propose a note of an array of cores (copy-constructed from the given state), take it as our
         state, and return true.  If the given state has no elements, do nothing and return false. */
      bool TryInitIndirectCoreArray(TTycon tycon, TExtensibleArena *arena, const State::TArrayOfSingleStates &state);

      /* If the given states have one or more elements, propose a note of an array of cores (copy-constructed from the given states), take it as our
         state, and return true.  If the given states have no elements, do nothing and return false. */
      bool TryInitIndirectCoreArray(TTycon tycon, TExtensibleArena *arena, const State::TArrayOfSingleStates &lhs_state,
                                    const State::TArrayOfSingleStates &rhs_state);

      /* TODO Splitting a tuple... */
      bool TryInitIndirectCoreArray(TTycon tycon, TExtensibleArena *arena, const State::TArrayOfSingleStates &state, size_t start, size_t limit);

      /* Our storage area is the sum of several types.  Which one we use is determined by our tycon. */
      union {

        /* We are a direct blob and this is our data.  The number of valid bytes is determined by our tycon. */
        uint8_t DirectBlob[MaxDirectSize];

        /* We are a direct string and this is our data.  The number of valid bytes is determined by our tycon. */
        char DirectStr[MaxDirectSize];

        /* We are an indirect blob or string and the offset leads to our data.
           If we're a string, then 'size' does not count our null terminator. */
        struct PACKED {

          /* The position in the arena where our array is stored. */
          TOffset Offset;

          /* The number of bytes in our array. */
          uint32_t Size;

        } IndirectScalarArray;

        /* We're an indirect array of cores and the offset leads to our data.
           The note will contain at least the given number of cores, but it may have more.
           If exemplar is true, then we should interpret the types of the cores in the array, but not their states. */
        struct PACKED {

          /* The position in the arena where our array is stored. */
          TOffset Offset;

          /* The number of element in our array. */
          uint32_t ElemCount;

          /* If true, then we interpret our array only as an example of our type, not as a value.
             This is the case for empty containers and for free variables. */
          bool IsExemplar;

          /* If true, then we are a core representing the whole note referenced by offset. This
             implies we can use the offset for comparison. If false then we have been truncated
             and should not use the offset for comparison. */
          bool UsesFullNote;

          /* TODO */
          bool StoresHash;

          /* TODO */
          size_t HashVal;

        } IndirectCoreArray;

      };  // union

      /* Verify the sizeof(IndirectScalarArray). */
      static_assert(sizeof(IndirectScalarArray) <= MaxDirectSize, "IndirectScalarArray is larger than MaxDirectSize.");
      /* Verify the sizeof(IndirectCoreArray). */
      static_assert(sizeof(IndirectCoreArray) <= MaxDirectSize, "IndirectScalarArray is larger than MaxDirectSize.");

      /* The kind of data we're storing.  This comes after the direct storage area so that, when we store a direct string,
         this byte can double as our null terminator. */
      TTycon Tycon;

      /* size of our pins */
      friend class Stig::Sabot::TSizeChecker;

    };  // TCore

    /* Verify the sizeof(TCore). */
    static_assert(sizeof(TCore) == MaxDirectSize + 1, "The size of TCore must be exactly one byte larger than MaxDirectSize.");
    static_assert((sizeof(TCore) % 8) == 0,           "The size of TCore must be an even mutiple of 8 bytes.");

    /* A storage area for notes. */
    class TCore::TArena {
      NO_COPY_SEMANTICS(TArena);
      public:

      /* Thrown when Pin() cannot locate a note. */
      class TNotFound
          : public std::runtime_error {
        public:

        /* Do-little. */
        TNotFound();

      };  // TNotFound

      /* Holds a note in memory. */
      class TPin {
        NO_COPY_SEMANTICS(TPin);
        public:

        /* Releases the note on its way out. */
        virtual ~TPin();

        /* The note we pin into memory. */
        const TNote *GetNote() const {
          assert(this);
          return Note;
        }

        protected:

        /* Do-nothing */
        TPin() : Arena(nullptr), Note(nullptr), Data1(nullptr), Data2(nullptr), Data3(nullptr) {}

        /* Tries to acquire the note, throwing if it fails. */
        TPin(TArena *arena, TOffset offset);

        /* Tries to acquire the note, throwing if it fails. */
        TPin(TArena *arena, TOffset offset, size_t known_size);

        private:

        /* The arena to which we belong. */
        TArena *Arena;

        /* The offset of the note we pin. */
        TOffset Offset;

        /* See accessor. */
        const TNote *Note;

        /* A value that the arena can use to fulfill its contract upon release. */
        void *Data1;
        void *Data2;
        void *Data3;

      };  // TArena::TPin

      /* TODO */
      class TFinalPin final
          : public TPin {
        public:

        /* TODO */
        class TWrapper {
          public:

          /* TODO */
          TWrapper(TFinalPin *pin) : FinalPin(pin) {}

          /* TODO */
          ~TWrapper() {
            if (FinalPin) {
              FinalPin->~TFinalPin();
            }
          }

          /* TODO */
          TFinalPin *operator->() const {
            return FinalPin;
          }

          /* TODO */
          const TFinalPin &operator*() const {
            return *FinalPin;
          }

          /* TODO */
          operator TFinalPin *() const {
            return FinalPin;
          }

          /* TODO */
          TFinalPin *get() const {
            return FinalPin;
          }

          private:

          /* TODO */
          TFinalPin *FinalPin;

        };  // TWrapper

        /* TODO */
        virtual ~TFinalPin() {}

        private:

        /* TODO */
        TFinalPin(TArena *arena, TOffset offset)
            : TPin(arena, offset) {}

        /* TODO */
        TFinalPin(TArena *arena, TOffset offset, size_t known_size)
            : TPin(arena, offset, known_size) {}

        /* For access to our constructor. */
        friend class TArena;

      };  // TFinalPin

      /* Destroying an arena that still has pins in it is an error. */
      virtual ~TArena() {}

      /* Pin the note with the given offset into memory.  If the note doesn't exist, throw. */
      TFinalPin *Pin(TOffset offset, void *alloc);

      /* Pin the note with the given offset into memory.  If the note doesn't exist, throw. */
      TFinalPin *Pin(TOffset offset, size_t known_size, void *alloc);

      /* TODO */
      bool IsOrdered() const {
        assert(this);
        return IsOrderedFlag;
      }

      protected:

      /* Do-little. */
      TArena(bool is_ordered)
          : IsOrderedFlag(is_ordered) {}

      /* Override to release the note from memory.
         Called by TPin's destructor. */
      virtual void ReleaseNote(const TNote *note, TOffset offset, void *data1, void *data2, void *data3) = 0;

      /* Override to pull the note into memory.  If the offset is not valid, return null.
         Called by TPin's constructor. */
      virtual const TNote *TryAcquireNote(TOffset offset, void *&data1, void *&data2, void *&data3) = 0;

      /* Override to pull the note into memory.  If the offset is not valid, return null.
         Called by TPin's constructor. */
      virtual const TNote *TryAcquireNote(TOffset offset, size_t known_size, void *&data1, void *&data2, void *&data3) = 0;

      private:

      /* TODO */
      const bool IsOrderedFlag;

    };  // TArena

    /* An arena to which we may propose new notes. */
    class TCore::TExtensibleArena
        : public virtual TArena {
      public:

      /* Override to accept a proposal of a note, returning the offset at which is stored.
         The function should take ownership of the note object. */
      virtual TOffset Propose(TNote *note) = 0;

      protected:

      /* Do-little. */
      TExtensibleArena(bool is_ordered)
          : TArena(is_ordered) {}

    };  // TExtensibleArena

    /* A piece of data stored in an arena. */
    class PACKED TCore::TNote {
      NO_COPY_SEMANTICS(TNote);
      public:

      /* Used when getting arrays of pairs of cores. */
      using TPairOfCores = std::pair<TCore, TCore>;

      /* A comparator for notes that assumes their offsets reference the same *ordered* arena, and represent the same exact type. */
      class TOrderedArenaCompare {
        public:

        /* TODO */
        TOrderedArenaCompare(TArena *arena) : OrderedCoreCompare(arena) {
          assert(!arena || arena->IsOrdered());
        }

        /* TODO */
        bool operator()(const TNote &lhs, const TNote &rhs) const;

        private:

        /* TODO */
        const TCore::TOrderedArenaCompare OrderedCoreCompare;

      };  // TOrderedArenaCompare

      /* An STL helper. */
      class THash {
        public:

        /* Return the hash of the note's raw data, ignoring depth and orderedness.
           A null pointer hashes to zero. */
        size_t operator()(const TNote *note) const;

      };  // THash

      /* An STL helper. */
      class TIsEq {
        public:

        /* True iff. lhs is equal to rhs on the basis of raw data comparison, ignoring depth and orderedness.
           Two null pointers compare as equal, null/non-null mixes compare as unequal. */
        bool operator()(const TNote *lhs, const TNote *rhs) const;

      };  // TIsEq

      /* Do-little. */
      ~TNote() {}

      /* Get the contents of the note as a blob.  The note must be a Blob. */
      inline void Get(const uint8_t *&start, const uint8_t *&limit) const;

      /* Get the contents of the note as a blob.  The note must be a Blob. */
      inline void Get(uint8_t *&start, uint8_t *&limit);

      /* Get the contents of the note as a string.  The note must be a Str.  The limit will point at the null terminator. */
      inline void Get(const char *&start, const char *&limit) const;

      /* Get the contents of the note as a string.  The note must be a Str.  The limit will point at the null terminator. */
      inline void Get(char *&start, char *&limit);

      /* Get the contents of the note as an array of cores.  The note must be a Desc, Free, Opt, Set, Vector, or Tuple. */
      inline void Get(const TCore *&start, const TCore *&limit) const;

      /* Get the contents of the note as an array of cores.  The note must be a Desc, Free, Opt, Set, Vector, or Tuple. */
      inline void Get(TCore *&start, TCore *&limit);

      /* Get the contents of the note as an array of pairs of cores.  The note must be a Map or Record. */
      inline void Get(const TPairOfCores *&start, const TPairOfCores *&limit) const;

      /* Get the contents of the note as an array of pairs of cores.  The note must be a Map or Record. */
      inline void Get(TPairOfCores *&start, TPairOfCores *&limit);

      /* The data we store, untyped. */
      inline const void *GetRawData() const;

      /* The number of bytes we store.  In the case of a Str, this will include the null terminator. */
      inline size_t GetRawSize() const;

      /* The number of bytes in the Blob or Str or the number of elements in the Desc, Free, Opt, Set, Vector, Tuple, Map, or Record.
         In the case of a Str, this will not include the null terminator. */
      inline size_t GetSize() const;

      /* True if we're an example of type instead of a true value. */
      inline bool IsExemplar() const;

      /* True if we're not referenced by anyone higher up in the arena. This suggests we are a dead note */
      inline bool IsUnReferenced() const;

      /* True if we're a tuple. */
      inline bool IsTuple() const;

      /* Assumes we're a tuple, get the number of non-free elements. */
      inline size_t GetTupleNumNonFree() const;

      /* Set this note as un-referenced. */
      void SetUnReferenced();

      /* Call back for each offset in this shallow note. */
      void ForOffset(const std::function<void (Atom::TCore::TOffset)> &cb) const;

      /* Apply the remap function to each core in this note.
         If the note is a Blob or Str, this function does nothing. */
      void Remap(const TRemap &remap);

      /* Construct a Blob. */
      static TNote *New(const uint8_t *start, const uint8_t *limit, bool is_exemplar);

      /* Construct a Str. */
      static TNote *New(const char *start, const char *limit, bool is_exemplar);

      /* Construct a Str. */
      static TNote *New(const std::string &str, bool is_exemplar);

      /* Construct a Desc, Free, Opt, Set, Vector, or Tuple. */
      static TNote *New(TTycon tycon, size_t elem_count, bool is_exemplar, const TInit1 &init1);

      /* Construct a Map or Record. */
      static TNote *New(TTycon tycon, size_t elem_count, bool is_exemplar, const TInit2 &init2);

      /* Construct a copy of another note. */
      static TNote *New(const TNote *that);

      /* TODO */
      static void operator delete(void *ptr, size_t);

      private:

      /* Caches the attributes. */
      TNote(TTycon tycon, bool is_exemplar, bool is_un_referenced, size_t raw_size);

      /* Get the start and limit of our data, cast to the appropriate scalar type.
         This operation ignores the tycon but throws if the raw size doesn't divide evenly by the scalar size. */
      template <typename TVal>
      void GetArray(TVal *&start, TVal *&limit) const;

      /* Get the start of our raw data, cast to a particular scalar type, ignoring our tycon.
         Thanks to our private 'new' operator, always immediately follows us in memory. */
      template <typename TVal = char>
      TVal *GetStart() const;

      template <typename TVal = char>
      TVal *GetLimit() const;

      /* Used by New() to allocate space for a new note, plus some extra space. */
      static void *operator new(size_t, size_t extra_size, size_t junk = 0);

      /* Construct a copy of the given raw data. */
      static TNote *NewRawCopy(TTycon tycon, bool is_exemplar, bool is_un_referenced, const void *raw_data, size_t raw_size);

      /* The kind of data we're storing.  A note never stores data which can be stored directly in a core, so
         valid tycons for notes are only Blob, Str, Desc, Free, Opt, Set, Vector, Map, Record, and Tuple. */
      TTycon Tycon;

      /* Flags. */
      unsigned
          Exemplar:1,     // See accessor.
          UnReferenced:1, // See accessor.
          Unused  :6;     // Padding for size and alignment.

      /* Set to zero. */
      uint16_t Pad;

      /* See accessor. */
      uint32_t RawSize;

      /* For access to our tycon. */
      friend class TCore;

    };  // TNote

    /* Verify sizeof(TCore::TNote). */
    static_assert(sizeof(TCore::TNote) == 8, "The size of TCore::TNote must be exactly 8 bytes.");

    template <typename TVal>
    TVal *TCore::TNote::GetStart() const {
      assert(this);
      return const_cast<TVal *>(static_cast<const TVal *>(GetRawData()));
    }

    template <typename TVal>
    TVal *TCore::TNote::GetLimit() const {
      assert(this);
      return const_cast<TVal *>(static_cast<const TVal *>(GetRawData())) + (RawSize / sizeof(TVal));
    }

    /* Inline */

    inline TCore::TCore(TExtensibleArena *arena, void *state_alloc, TArena *that_arena, const TCore &that)
        : TCore(arena, Sabot::State::TAny::TWrapper(that.NewState(that_arena, state_alloc))) {}


    inline TCore::TCore(const TCore &that, const TRemap &remap) : TCore(that) {
      Remap(remap);
    }

    inline bool TCore::IsTombstone() const {
      return Tycon == TTycon::Tombstone;
    }

    inline bool TCore::IsTuple() const {
      return Tycon == TTycon::Tuple;
    }

    inline bool TCore::IsFree() const {
      return Tycon == TTycon::Free;
    }

    template <typename TVal>
    static inline Atom::TComparison QuickCompare(const TVal &lhs, const TVal &rhs) {
      return lhs == rhs ? Atom::TComparison::Eq : (lhs < rhs ? Atom::TComparison::Lt : Atom::TComparison::Gt);
    }

    template <typename TVal>
    static inline Atom::TComparison QuickCompareMem(const TVal *lhs, const TVal *rhs, size_t lhs_len, size_t rhs_len) {
      int comp = memcmp(lhs, rhs, std::min(lhs_len, rhs_len));
      if (comp == 0) {  // min size bytes are equal
        return QuickCompare(lhs_len, rhs_len);
      } else if (comp < 0) { // lhs is less
        return Atom::TComparison::Lt;
      } else { // lhs is more
        assert(comp > 0);
        return Atom::TComparison::Gt;
      }
    }

    inline bool TCore::TryQuickOrderComparison(TArena *this_arena, const TCore &that_core, TArena *that_arena, Atom::TComparison &comp) const {
      assert(this);
      assert(this_arena);
      assert(&that_core);
      assert(that_arena);
      bool success = Tycon == that_core.Tycon &&
        Tycon >= TTycon::Blob &&
        this_arena == that_arena &&
        this_arena->IsOrdered() &&
        IndirectCoreArray.UsesFullNote && that_core.IndirectCoreArray.UsesFullNote;
      if (success) {
        if (IndirectCoreArray.Offset == that_core.IndirectCoreArray.Offset) {
          assert(Tycon == that_core.Tycon);
          comp = Atom::TComparison::Eq;
        } else if (IndirectCoreArray.Offset < that_core.IndirectCoreArray.Offset) {
          comp = Atom::TComparison::Lt;
        } else {
          assert(IndirectCoreArray.Offset > that_core.IndirectCoreArray.Offset);
          comp = Atom::TComparison::Gt;
        }
        return success;
      }
      /* same tycon, easy ones */
      if (Tycon == that_core.Tycon) {
        switch (Tycon) {
          case TTycon::Int8: {
            comp = QuickCompare(ForceAs<int8_t>(), that_core.ForceAs<int8_t>());
            return true;
          }
          case TTycon::Int16: {
            comp = QuickCompare(ForceAs<int16_t>(), that_core.ForceAs<int16_t>());
            return true;
          }
          case TTycon::Int32: {
            comp = QuickCompare(ForceAs<int32_t>(), that_core.ForceAs<int32_t>());
            return true;
          }
          case TTycon::Int64: {
            comp = QuickCompare(ForceAs<int64_t>(), that_core.ForceAs<int64_t>());
            return true;
          }
          case TTycon::UInt8: {
            comp = QuickCompare(ForceAs<uint8_t>(), that_core.ForceAs<uint8_t>());
            return true;
          }
          case TTycon::UInt16: {
            comp = QuickCompare(ForceAs<uint16_t>(), that_core.ForceAs<uint16_t>());
            return true;
          }
          case TTycon::UInt32: {
            comp = QuickCompare(ForceAs<uint32_t>(), that_core.ForceAs<uint32_t>());
            return true;
          }
          case TTycon::UInt64: {
            comp = QuickCompare(ForceAs<uint64_t>(), that_core.ForceAs<uint64_t>());
            return true;
          }
          case TTycon::Bool: {
            comp = QuickCompare(ForceAs<bool>(), that_core.ForceAs<bool>());
            return true;
          }
          case TTycon::Char: {
            comp = QuickCompare(ForceAs<char>(), that_core.ForceAs<char>());
            return true;
          }
          case TTycon::Float: {
            comp = QuickCompare(ForceAs<float>(), that_core.ForceAs<float>());
            return true;
          }
          case TTycon::Double: {
            comp = QuickCompare(ForceAs<double>(), that_core.ForceAs<double>());
            return true;
          }
          case TTycon::Duration: {
            comp = QuickCompare(ForceAs<TStdDuration>().count(), that_core.ForceAs<TStdDuration>().count());
            return true;
          }
          case TTycon::TimePoint: {
            comp = QuickCompare(ForceAs<TStdTimePoint>().time_since_epoch().count(), that_core.ForceAs<TStdTimePoint>().time_since_epoch().count());
            return true;
          }
          case TTycon::Uuid: {
            comp = QuickCompare(ForceAs<Base::TUuid>(), that_core.ForceAs<Base::TUuid>());
            return true;
          }
          case TTycon::Tuple: {
            void *lhs_pin_alloc = alloca(sizeof(TArena::TFinalPin) * 2);
            void *rhs_pin_alloc = reinterpret_cast<uint8_t *>(lhs_pin_alloc) + sizeof(TArena::TFinalPin);
            TArena::TFinalPin::TWrapper lhs_pin(this_arena->Pin(IndirectCoreArray.Offset,
                                                                sizeof(Atom::TCore::TNote) + (sizeof(Atom::TCore) * IndirectCoreArray.ElemCount),
                                                                lhs_pin_alloc));
            TArena::TFinalPin::TWrapper rhs_pin(that_arena->Pin(that_core.IndirectCoreArray.Offset,
                                                                sizeof(Atom::TCore::TNote) + (sizeof(Atom::TCore) * that_core.IndirectCoreArray.ElemCount),
                                                                rhs_pin_alloc));
            const TCore *lhs_start, *lhs_limit,
                        *rhs_start, *rhs_limit;
            lhs_pin->GetNote()->Get(lhs_start, lhs_limit);
            rhs_pin->GetNote()->Get(rhs_start, rhs_limit);
            const size_t lhs_count = IndirectCoreArray.ElemCount;
            assert(lhs_count <= static_cast<size_t>(lhs_limit - lhs_start));
            const size_t rhs_count = that_core.IndirectCoreArray.ElemCount;
            assert(rhs_count <= static_cast<size_t>(rhs_limit - rhs_start));
            lhs_limit = lhs_start + lhs_count;
            rhs_limit = rhs_start + rhs_count;
            for (const TCore *lhs_core = lhs_start, *rhs_core = rhs_start; lhs_core < lhs_limit && rhs_core < rhs_limit; ++lhs_core, ++rhs_core) {
              if (lhs_core->TryQuickOrderComparison(this_arena, *rhs_core, that_arena, comp)) {
                if (Atom::IsLt(comp) || Atom::IsGt(comp)) {
                  return true;
                }
              } else {
                return false;
              }
            }
            comp = QuickCompare(lhs_count, rhs_count);
            return true;
          }
          default: {
            break;
          }
        }
      }
      /* the types are not the same */
      if (Tycon >= TTycon::Int8 && Tycon <= TTycon::Uuid &&
          that_core.Tycon >= TTycon::Int8 && that_core.Tycon <= TTycon::Uuid) {
        assert(Tycon != that_core.Tycon); /* we should have compared these earlier as values */
        comp = QuickCompare(Tycon, that_core.Tycon);
        return true;
      }
      /* lhs is a str */
      if (Tycon == TTycon::Str) { /* lhs indirect str */
        const char *lhs_start, *lhs_limit,
                   *rhs_start, *rhs_limit;
        if (that_core.Tycon == TTycon::Str) { /* rhs indirect str */
          void *lhs_pin_alloc = alloca(sizeof(TArena::TFinalPin) * 2);
          void *rhs_pin_alloc = reinterpret_cast<uint8_t *>(lhs_pin_alloc) + sizeof(TArena::TFinalPin);
          TArena::TFinalPin::TWrapper lhs_pin(this_arena->Pin(IndirectScalarArray.Offset,
                                                              sizeof(Atom::TCore::TNote) + IndirectScalarArray.Size,
                                                              lhs_pin_alloc));
          TArena::TFinalPin::TWrapper rhs_pin(that_arena->Pin(that_core.IndirectScalarArray.Offset,
                                                              sizeof(Atom::TCore::TNote) + that_core.IndirectScalarArray.Size,
                                                              rhs_pin_alloc));
          lhs_pin->GetNote()->Get(lhs_start, lhs_limit);
          rhs_pin->GetNote()->Get(rhs_start, rhs_limit);
          comp = QuickCompareMem(lhs_start, rhs_start, (lhs_limit - lhs_start), (rhs_limit - rhs_start));
          return true;
        } else if (that_core.Tycon >= TTycon::MinDirectStr && that_core.Tycon <= TTycon::MaxDirectStr) { /* rhs direct str */
          void *lhs_pin_alloc = alloca(sizeof(TArena::TFinalPin));
          TArena::TFinalPin::TWrapper lhs_pin(this_arena->Pin(IndirectScalarArray.Offset,
                                                              sizeof(Atom::TCore::TNote) + IndirectScalarArray.Size,
                                                              lhs_pin_alloc));
          lhs_pin->GetNote()->Get(lhs_start, lhs_limit);
          rhs_start = that_core.DirectStr;
          rhs_limit = rhs_start + MaxDirectSize - static_cast<TTyconNumeric>(that_core.Tycon);
          comp = QuickCompareMem(lhs_start, rhs_start, (lhs_limit - lhs_start), (rhs_limit - rhs_start));
          return true;
        }
      } else if (Tycon >= TTycon::MinDirectStr && Tycon <= TTycon::MaxDirectStr) { /* lhs direct str */
        const char *lhs_start, *lhs_limit,
                   *rhs_start, *rhs_limit;
        lhs_start = DirectStr;
        lhs_limit = lhs_start + MaxDirectSize - static_cast<TTyconNumeric>(Tycon);
        if (that_core.Tycon == TTycon::Str) { /* rhs indirect str */
          void *rhs_pin_alloc = alloca(sizeof(TArena::TFinalPin));
          TArena::TFinalPin::TWrapper rhs_pin(that_arena->Pin(that_core.IndirectScalarArray.Offset,
                                                              sizeof(Atom::TCore::TNote) + that_core.IndirectScalarArray.Size,
                                                              rhs_pin_alloc));
          rhs_pin->GetNote()->Get(rhs_start, rhs_limit);
          comp = QuickCompareMem(lhs_start, rhs_start, (lhs_limit - lhs_start), (rhs_limit - rhs_start));
          return true;
        } else if (that_core.Tycon >= TTycon::MinDirectStr && that_core.Tycon <= TTycon::MaxDirectStr) { /* rhs direct str */
          rhs_start = that_core.DirectStr;
          rhs_limit = rhs_start + MaxDirectSize - static_cast<TTyconNumeric>(that_core.Tycon);
          comp = QuickCompareMem(lhs_start, rhs_start, (lhs_limit - lhs_start), (rhs_limit - rhs_start));
          return true;
        }
      }
      /* lhs is a blob */
      if (Tycon == TTycon::Blob) { /* lhs indirect blob */
        const uint8_t *lhs_start, *lhs_limit,
                      *rhs_start, *rhs_limit;
        if (that_core.Tycon == TTycon::Blob) { /* rhs indirect blob */
          void *lhs_pin_alloc = alloca(sizeof(TArena::TFinalPin) * 2);
          void *rhs_pin_alloc = reinterpret_cast<uint8_t *>(lhs_pin_alloc) + sizeof(TArena::TFinalPin);
          TArena::TFinalPin::TWrapper lhs_pin(this_arena->Pin(IndirectScalarArray.Offset,
                                                              sizeof(Atom::TCore::TNote) + IndirectScalarArray.Size,
                                                              lhs_pin_alloc));
          TArena::TFinalPin::TWrapper rhs_pin(that_arena->Pin(that_core.IndirectScalarArray.Offset,
                                                              sizeof(Atom::TCore::TNote) + that_core.IndirectScalarArray.Size,
                                                              rhs_pin_alloc));
          lhs_pin->GetNote()->Get(lhs_start, lhs_limit);
          rhs_pin->GetNote()->Get(rhs_start, rhs_limit);
          comp = QuickCompareMem(lhs_start, rhs_start, (lhs_limit - lhs_start), (rhs_limit - rhs_start));
          return true;
        } else if (that_core.Tycon >= TTycon::MinDirectBlob && that_core.Tycon <= TTycon::MaxDirectBlob) { /* rhs direct blob */
          void *lhs_pin_alloc = alloca(sizeof(TArena::TFinalPin));
          TArena::TFinalPin::TWrapper lhs_pin(this_arena->Pin(IndirectScalarArray.Offset,
                                                              sizeof(Atom::TCore::TNote) + IndirectScalarArray.Size,
                                                              lhs_pin_alloc));
          lhs_pin->GetNote()->Get(lhs_start, lhs_limit);
          rhs_start = that_core.DirectBlob;
          rhs_limit = rhs_start + static_cast<TTyconNumeric>(TTycon::MaxDirectBlob) - static_cast<TTyconNumeric>(that_core.Tycon);
          comp = QuickCompareMem(lhs_start, rhs_start, (lhs_limit - lhs_start), (rhs_limit - rhs_start));
          return true;
        }
      } else if (Tycon >= TTycon::MinDirectBlob && Tycon <= TTycon::MaxDirectBlob) { /* lhs direct blob */
        const uint8_t *lhs_start, *lhs_limit,
                      *rhs_start, *rhs_limit;
        lhs_start = DirectBlob;
        lhs_limit = lhs_start + static_cast<TTyconNumeric>(TTycon::MaxDirectBlob) - static_cast<TTyconNumeric>(Tycon);
        if (that_core.Tycon == TTycon::Blob) { /* rhs indirect blob */
          void *rhs_pin_alloc = alloca(sizeof(TArena::TFinalPin));
          TArena::TFinalPin::TWrapper rhs_pin(that_arena->Pin(that_core.IndirectScalarArray.Offset,
                                                              sizeof(Atom::TCore::TNote) + that_core.IndirectScalarArray.Size,
                                                              rhs_pin_alloc));
          rhs_pin->GetNote()->Get(rhs_start, rhs_limit);
          comp = QuickCompareMem(lhs_start, rhs_start, (lhs_limit - lhs_start), (rhs_limit - rhs_start));
          return true;
        } else if (that_core.Tycon >= TTycon::MinDirectBlob && that_core.Tycon <= TTycon::MaxDirectBlob) { /* rhs direct blob */
          rhs_start = that_core.DirectBlob;
          rhs_limit = rhs_start + static_cast<TTyconNumeric>(TTycon::MaxDirectBlob) - static_cast<TTyconNumeric>(that_core.Tycon);
          comp = QuickCompareMem(lhs_start, rhs_start, (lhs_limit - lhs_start), (rhs_limit - rhs_start));
          return true;
        }
      }
      return success;
    }

    inline bool TCore::MatchType(TArena *this_arena, const TCore &that_core, TArena *that_arena) const {
      assert(this);
      assert(this_arena);
      assert(&that_core);
      assert(that_arena);
      if (Tycon == that_core.Tycon) {
        switch (Tycon) {
          case TTycon::Int8:
          case TTycon::Int16:
          case TTycon::Int32:
          case TTycon::Int64:
          case TTycon::UInt8:
          case TTycon::UInt16:
          case TTycon::UInt32:
          case TTycon::UInt64:
          case TTycon::Bool:
          case TTycon::Char:
          case TTycon::Float:
          case TTycon::Double:
          case TTycon::Duration:
          case TTycon::TimePoint:
          case TTycon::Uuid: {
            return true;
          }
          case TTycon::Tombstone: {
            throw std::logic_error("Tombstone should not be part of a key");
          }
          case TTycon::Void: {
            return true;
          }
          case TTycon::Blob:
          case TTycon::Str: {
            break;
          }
          case TTycon::Free: {
            throw std::logic_error("Free should not be compared to  free");
          }
          case TTycon::Desc:
          case TTycon::Opt:
          case TTycon::Set:
          case TTycon::Vector: {
            void *lhs_pin_alloc = alloca(sizeof(TArena::TFinalPin) * 2);
            void *rhs_pin_alloc = reinterpret_cast<uint8_t *>(lhs_pin_alloc) + sizeof(TArena::TFinalPin);
            TArena::TFinalPin::TWrapper lhs_pin(this_arena->Pin(IndirectCoreArray.Offset,
                                                                sizeof(Atom::TCore::TNote) + (sizeof(Atom::TCore) * IndirectCoreArray.ElemCount),
                                                                lhs_pin_alloc));
            TArena::TFinalPin::TWrapper rhs_pin(that_arena->Pin(that_core.IndirectCoreArray.Offset,
                                                                sizeof(Atom::TCore::TNote) + (sizeof(Atom::TCore) * that_core.IndirectCoreArray.ElemCount),
                                                                rhs_pin_alloc));
            const TCore *lhs_start, *lhs_limit,
                        *rhs_start, *rhs_limit;
            lhs_pin->GetNote()->Get(lhs_start, lhs_limit);
            rhs_pin->GetNote()->Get(rhs_start, rhs_limit);
            assert(lhs_limit - lhs_start >= 1);
            assert(rhs_limit - rhs_start >= 1);
            return lhs_start->MatchType(this_arena, *rhs_start, that_arena);
          }
          case TTycon::Map: {
            void *lhs_pin_alloc = alloca(sizeof(TArena::TFinalPin) * 2);
            void *rhs_pin_alloc = reinterpret_cast<uint8_t *>(lhs_pin_alloc) + sizeof(TArena::TFinalPin);
            TArena::TFinalPin::TWrapper lhs_pin(this_arena->Pin(IndirectCoreArray.Offset,
                                                                sizeof(Atom::TCore::TNote) + (sizeof(Atom::TCore) * 2 * IndirectCoreArray.ElemCount),
                                                                lhs_pin_alloc));
            TArena::TFinalPin::TWrapper rhs_pin(that_arena->Pin(that_core.IndirectCoreArray.Offset,
                                                                sizeof(Atom::TCore::TNote) + (sizeof(Atom::TCore) * 2 * that_core.IndirectCoreArray.ElemCount),
                                                                rhs_pin_alloc));
            const TNote::TPairOfCores *lhs_start, *lhs_limit,
                                      *rhs_start, *rhs_limit;
            lhs_pin->GetNote()->Get(lhs_start, lhs_limit);
            rhs_pin->GetNote()->Get(rhs_start, rhs_limit);
            assert(lhs_limit - lhs_start >= 1);
            assert(rhs_limit - rhs_start >= 1);
            return lhs_start->first.MatchType(this_arena, rhs_start->first, that_arena) && lhs_start->second.MatchType(this_arena, rhs_start->second, that_arena);
          }
          case TTycon::Record: {
            if (IndirectCoreArray.ElemCount == that_core.IndirectCoreArray.ElemCount) {
              void *lhs_pin_alloc = alloca(sizeof(TArena::TFinalPin) * 2);
              void *rhs_pin_alloc = reinterpret_cast<uint8_t *>(lhs_pin_alloc) + sizeof(TArena::TFinalPin);
              TArena::TFinalPin::TWrapper lhs_pin(this_arena->Pin(IndirectCoreArray.Offset,
                                                                  sizeof(Atom::TCore::TNote) + (sizeof(Atom::TCore) * 2 * IndirectCoreArray.ElemCount),
                                                                  lhs_pin_alloc));
              TArena::TFinalPin::TWrapper rhs_pin(that_arena->Pin(that_core.IndirectCoreArray.Offset,
                                                                  sizeof(Atom::TCore::TNote) + (sizeof(Atom::TCore) * 2 * that_core.IndirectCoreArray.ElemCount),
                                                                  rhs_pin_alloc));
              const TNote::TPairOfCores *lhs_start, *lhs_limit,
                                        *rhs_start, *rhs_limit;
              lhs_pin->GetNote()->Get(lhs_start, lhs_limit);
              rhs_pin->GetNote()->Get(rhs_start, rhs_limit);
              assert(lhs_limit - lhs_start >= 2);
              assert(rhs_limit - rhs_start >= 2);
              std::string lhs_name, rhs_name;
              for (; lhs_start < lhs_limit && rhs_start < rhs_limit; ++lhs_start, ++rhs_start) {
                /* check name */
                assert(lhs_start->first.Tycon == TTycon::Str || (lhs_start->first.Tycon >= TTycon::MinDirectStr && lhs_start->first.Tycon <= TTycon::MaxDirectStr));
                assert(rhs_start->first.Tycon == TTycon::Str || (rhs_start->first.Tycon >= TTycon::MinDirectStr && rhs_start->first.Tycon <= TTycon::MaxDirectStr));
                CopyOut(this_arena, lhs_name);
                that_core.CopyOut(that_arena, rhs_name);
                if (lhs_name != rhs_name) {
                  return false;
                }
                /* check value */
                if (!lhs_start->second.MatchType(this_arena, rhs_start->second, that_arena)) {
                  return false;
                }
              }
              return true;
            }
            return false;
          }
          case TTycon::Tuple: {
            if (IndirectCoreArray.ElemCount == IndirectCoreArray.ElemCount) {
              void *lhs_pin_alloc = alloca(sizeof(TArena::TFinalPin) * 2);
              void *rhs_pin_alloc = reinterpret_cast<uint8_t *>(lhs_pin_alloc) + sizeof(TArena::TFinalPin);
              TArena::TFinalPin::TWrapper lhs_pin(this_arena->Pin(IndirectCoreArray.Offset,
                                                                  sizeof(Atom::TCore::TNote) + (sizeof(Atom::TCore) * IndirectCoreArray.ElemCount),
                                                                  lhs_pin_alloc));
              TArena::TFinalPin::TWrapper rhs_pin(that_arena->Pin(that_core.IndirectCoreArray.Offset,
                                                                  sizeof(Atom::TCore::TNote) + (sizeof(Atom::TCore) * that_core.IndirectCoreArray.ElemCount),
                                                                  rhs_pin_alloc));
              const TCore *lhs_start, *lhs_limit,
                          *rhs_start, *rhs_limit;
              lhs_pin->GetNote()->Get(lhs_start, lhs_limit);
              rhs_pin->GetNote()->Get(rhs_start, rhs_limit);
              assert(lhs_limit - lhs_start >= 1);
              assert(rhs_limit - rhs_start >= 1);
              for (; lhs_start < lhs_limit && rhs_start < rhs_limit; ++lhs_start, ++rhs_start) {
                if (!lhs_start->MatchType(this_arena, *rhs_start, that_arena)) {
                  return false;
                }
              }
              return true;
            }
            return false;
          }
          default: {
            if ((Tycon >= TTycon::MinDirectStr && Tycon <= TTycon::MaxDirectStr) ||
                (Tycon >= TTycon::MinDirectBlob && Tycon <= TTycon::MaxDirectBlob)) {
              break;
            }
            throw TCorrupt();
          }
        }
      }
      if (Tycon == TTycon::Str || (Tycon >= TTycon::MinDirectStr && Tycon <= TTycon::MaxDirectStr)) {
        return that_core.Tycon == TTycon::Str || (that_core.Tycon >= TTycon::MinDirectStr && that_core.Tycon <= TTycon::MaxDirectStr);
      } else if (Tycon == TTycon::Blob || (Tycon >= TTycon::MinDirectBlob && Tycon <= TTycon::MaxDirectBlob)) {
        return that_core.Tycon == TTycon::Blob || (that_core.Tycon >= TTycon::MinDirectBlob && that_core.Tycon <= TTycon::MaxDirectBlob);
      }
      return false;
    }

    inline Stig::Sabot::TMatchResult TCore::PrefixMatch(TArena *this_arena, const TCore &that_core, TArena *that_arena) const {
      assert(this);
      assert(this_arena);
      assert(&that_core);
      assert(that_arena);
      assert(Tycon == TTycon::Tuple);
      assert(that_core.Tycon == TTycon::Tuple);
      assert(IndirectCoreArray.UsesFullNote);
      assert(that_core.IndirectCoreArray.UsesFullNote);
      void *lhs_pin_alloc = alloca(sizeof(TArena::TFinalPin) * 2);
      void *rhs_pin_alloc = reinterpret_cast<uint8_t *>(lhs_pin_alloc) + sizeof(TArena::TFinalPin);
      void *lhs_state_alloc = alloca(Sabot::State::GetMaxStateSize() * 2);
      void *rhs_state_alloc = reinterpret_cast<uint8_t *>(lhs_state_alloc) + Sabot::State::GetMaxStateSize();
      TArena::TFinalPin::TWrapper lhs_pin(this_arena->Pin(IndirectCoreArray.Offset,
                                                          sizeof(Atom::TCore::TNote) + (sizeof(Atom::TCore) * IndirectCoreArray.ElemCount),
                                                          lhs_pin_alloc));
      TArena::TFinalPin::TWrapper rhs_pin(that_arena->Pin(that_core.IndirectCoreArray.Offset,
                                                          sizeof(Atom::TCore::TNote) + (sizeof(Atom::TCore) * that_core.IndirectCoreArray.ElemCount),
                                                          rhs_pin_alloc));
      const TCore *lhs_start, *lhs_limit,
                  *rhs_start, *rhs_limit;
      lhs_pin->GetNote()->Get(lhs_start, lhs_limit);
      rhs_pin->GetNote()->Get(rhs_start, rhs_limit);
      const size_t lhs_count = IndirectCoreArray.ElemCount;
      assert(lhs_count == static_cast<size_t>(lhs_limit - lhs_start));
      const size_t rhs_count = that_core.IndirectCoreArray.ElemCount;
      assert(rhs_count == static_cast<size_t>(rhs_limit - rhs_start));
      for (const TCore *lhs_core = lhs_start, *rhs_core = rhs_start; lhs_core < lhs_limit && rhs_core < rhs_limit; ++lhs_core, ++rhs_core) {
        assert(rhs_core->Tycon != TTycon::Free);
        if (lhs_core->Tycon != TTycon::Free) {
          Atom::TComparison comp;
          if (!lhs_core->TryQuickOrderComparison(this_arena, *rhs_core, that_arena, comp)) {
            Sabot::State::TAny::TWrapper
              lhs_state(NewState(this_arena, lhs_state_alloc)),
              rhs_state(that_core.NewState(that_arena, rhs_state_alloc));
            comp = Stig::Sabot::OrderStates(*lhs_state, *rhs_state);
          }
          if (Atom::IsNe(comp)) {
            return Stig::Sabot::TMatchResult::NoMatch;
          }
        } else {
          assert(lhs_core->Tycon == TTycon::Free);
          void *free_pin_alloc = alloca(sizeof(TArena::TFinalPin));
          TArena::TFinalPin::TWrapper free_pin(this_arena->Pin(lhs_core->IndirectCoreArray.Offset,
                                                               sizeof(Atom::TCore::TNote) + (sizeof(Atom::TCore) * lhs_core->IndirectCoreArray.ElemCount),
                                                               free_pin_alloc));
          const TCore *free_start, *free_limit;
          free_pin->GetNote()->Get(free_start, free_limit);
          assert((free_limit - free_start) == 1);
          if (free_start->MatchType(this_arena, *rhs_core, that_arena)) {
          } else {
            return Stig::Sabot::TMatchResult::PrefixMatch;
          }
        }

      }
      return lhs_count == rhs_count ? Stig::Sabot::TMatchResult::Unifies : Stig::Sabot::TMatchResult::PrefixMatch;
    }

    inline const uint32_t *TCore::TryGetElemCount() const {
      assert(this);
      if (Tycon >= TTycon::Desc) {
        return &IndirectCoreArray.ElemCount;
      }
      return nullptr;
    }

    inline size_t TCore::ForceGetIndirectHash() const {
      assert(this);
      assert(Tycon >= TTycon::Blob);
      assert(IndirectCoreArray.StoresHash);
      assert(IndirectCoreArray.UsesFullNote);
      return IndirectCoreArray.HashVal;
    }

    inline bool TCore::TryGetQuickHash(size_t &out_hash) const {
      assert(this);
      assert(&out_hash);
      switch (Tycon) {
        case TTycon::Int8: {
          out_hash = std::_Hash_impl::hash(&ForceAs<int8_t>(), sizeof(int8_t));
          return true;
        }
        case TTycon::Int16: {
          out_hash = std::_Hash_impl::hash(&ForceAs<int16_t>(), sizeof(int16_t));
          return true;
        }
        case TTycon::Int32: {
          out_hash = std::_Hash_impl::hash(&ForceAs<int32_t>(), sizeof(int32_t));
          return true;
        }
        case TTycon::Int64: {
          out_hash = std::_Hash_impl::hash(&ForceAs<int64_t>(), sizeof(int64_t));
          return true;
        }
        case TTycon::UInt8: {
          out_hash = std::_Hash_impl::hash(&ForceAs<uint8_t>(), sizeof(uint8_t));
          return true;
        }
        case TTycon::UInt16: {
          out_hash = std::_Hash_impl::hash(&ForceAs<uint16_t>(), sizeof(uint16_t));
          return true;
        }
        case TTycon::UInt32: {
          out_hash = std::_Hash_impl::hash(&ForceAs<uint32_t>(), sizeof(uint32_t));
          return true;
        }
        case TTycon::UInt64: {
          out_hash = std::_Hash_impl::hash(&ForceAs<uint64_t>(), sizeof(uint64_t));
          return true;
        }
        case TTycon::Bool: {
          out_hash = std::hash<bool>()(ForceAs<bool>());
          return true;
        }
        case TTycon::Char: {
          out_hash = std::hash<char>()(ForceAs<char>());
          return true;
        }
        case TTycon::Float: {
          out_hash = std::hash<float>()(ForceAs<float>());
          return true;
        }
        case TTycon::Double: {
          out_hash = std::hash<double>()(ForceAs<double>());
          return true;
        }
        case TTycon::Duration: {
          out_hash = std::hash<TStdDuration::rep>()(ForceAs<TStdDuration>().count());
          return true;
        }
        case TTycon::TimePoint: {
          out_hash = std::hash<TStdDuration::rep>()(ForceAs<TStdTimePoint>().time_since_epoch().count());
          return true;
        }
        case TTycon::Uuid: {
          out_hash = std::hash<Base::TUuid>()(ForceAs<Base::TUuid>());
          return true;
        }
        default: {
          if (Tycon >= TTycon::MinDirectBlob && Tycon <= TTycon::MaxDirectBlob) {
            const uint8_t *start = DirectBlob;
            const uint8_t *limit = start + static_cast<TTyconNumeric>(TTycon::MaxDirectBlob) - static_cast<TTyconNumeric>(Tycon);
            out_hash = std::_Hash_impl::hash(start, limit - start);
            return true;
          } else if (Tycon >= TTycon::MinDirectStr && Tycon <= TTycon::MaxDirectStr) {
            const char *start = DirectStr;
            const char *limit = start + MaxDirectSize - static_cast<TTyconNumeric>(Tycon);
            out_hash = std::_Hash_impl::hash(start, limit - start);
            return true;
          }
          return TryGetStoredHash(out_hash);
        }
      }
    }

    inline bool TCore::TryGetStoredHash(size_t &out_hash) const {
      assert(this);
      assert(&out_hash);
      bool success = Tycon >= TTycon::Blob &&
        IndirectCoreArray.StoresHash &&
        IndirectCoreArray.UsesFullNote;
      if (success) {
        out_hash = IndirectCoreArray.HashVal;
      }
      return success;
    }

    inline size_t TCore::ForceGetStoredHash() const {
      assert(this);
      assert(Tycon >= TTycon::Blob && IndirectCoreArray.StoresHash);
      return IndirectCoreArray.HashVal;
    }

    inline bool TCore::TrySetStoredHash(size_t hash_val) {
      assert(this);
      bool success = Tycon >= TTycon::Blob;
      if (success) {
        IndirectCoreArray.StoresHash = true;
        IndirectCoreArray.HashVal = hash_val;
      }
      return success;
    }

    /* inline TNote */

    template <typename TVal>
    inline void TCore::TNote::GetArray(TVal *&start, TVal *&limit) const {
      assert(this);
      assert(&start);
      assert(&limit);
      #ifndef NDEBUG
      if (RawSize % sizeof(TVal)) {
        throw TCorrupt();
      }
      #endif
      start = GetStart<TVal>();
      limit = start + (RawSize / sizeof(TVal));
    }

    inline void TCore::TNote::Get(const uint8_t *&start, const uint8_t *&limit) const {
      assert(this);
      #ifndef NDEBUG
      if (Tycon != TTycon::Blob) {
        throw TBadType();
      }
      #endif
      GetArray(start, limit);
    }

    inline void TCore::TNote::Get(uint8_t *&start, uint8_t *&limit) {
      assert(this);
      #ifndef NDEBUG
      if (Tycon != TTycon::Blob) {
        throw TBadType();
      }
      #endif
      GetArray(start, limit);
    }

    inline void TCore::TNote::Get(const char *&start, const char *&limit) const {
      assert(this);
      #ifndef NDEBUG
      if (Tycon != TTycon::Str) {
        throw TBadType();
      }
      #endif
      GetArray(start, limit);
      --limit;
      assert(!*limit);
    }

    inline void TCore::TNote::Get(char *&start, char *&limit) {
      assert(this);
      #ifndef NDEBUG
      if (Tycon != TTycon::Str) {
        throw TBadType();
      }
      #endif
      GetArray(start, limit);
      --limit;
      assert(!*limit);
    }

    inline void TCore::TNote::Get(const TCore *&start, const TCore *&limit) const {
      assert(this);
      #ifndef NDEBUG
      switch (Tycon) {
        case TTycon::Desc:
        case TTycon::Free:
        case TTycon::Opt:
        case TTycon::Set:
        case TTycon::Vector:
        case TTycon::Tuple: {
          GetArray(start, limit);
          break;
        }
        default: {
          throw TBadType();
        }
      }
      #else
      GetArray(start, limit);
      #endif
    }

    inline void TCore::TNote::Get(TCore *&start, TCore *&limit) {
      assert(this);
      #ifndef NDEBUG
      switch (Tycon) {
        case TTycon::Desc:
        case TTycon::Free:
        case TTycon::Opt:
        case TTycon::Set:
        case TTycon::Vector:
        case TTycon::Tuple: {
          GetArray(start, limit);
          break;
        }
        default: {
          throw TBadType();
        }
      }
      #else
      GetArray(start, limit);
      #endif
    }

    inline void TCore::TNote::Get(const TPairOfCores *&start, const TPairOfCores *&limit) const {
      assert(this);
      #ifndef NDEBUG
      switch (Tycon) {
        case TTycon::Map:
        case TTycon::Record: {
          GetArray(start, limit);
          break;
        }
        default: {
          throw TBadType();
        }
      }
      #else
      GetArray(start, limit);
      #endif
    }

    inline void TCore::TNote::Get(TPairOfCores *&start, TPairOfCores *&limit) {
      assert(this);
      #ifndef NDEBUG
      switch (Tycon) {
        case TTycon::Map:
        case TTycon::Record: {
          GetArray(start, limit);
          break;
        }
        default: {
          throw TBadType();
        }
      }
      #else
      GetArray(start, limit);
      #endif
    }

    inline const void *TCore::TNote::GetRawData() const {
      assert(this);
      return reinterpret_cast<const void *>(this + 1);
    }

    inline size_t TCore::TNote::GetRawSize() const {
      assert(this);
      return RawSize;
    }

    inline size_t TCore::TNote::GetSize() const {
      assert(this);
      size_t raw_size = RawSize, elem_size;
      switch (Tycon) {
        case TTycon::Blob: {
          elem_size = 1;
          break;
        }
        case TTycon::Str: {
          elem_size = 1;
          --raw_size;
          break;
        }
        case TTycon::Desc:
        case TTycon::Free:
        case TTycon::Opt:
        case TTycon::Set:
        case TTycon::Vector:
        case TTycon::Tuple: {
          elem_size = sizeof(TCore);
          break;
        }
        case TTycon::Map:
        case TTycon::Record: {
          elem_size = sizeof(TPairOfCores);
          break;
        }
        default: {
          throw TCorrupt();
        }
      }
      #ifndef NDEBUG
      if (raw_size % elem_size) {
        throw TCorrupt();
      }
      #endif
      return raw_size / elem_size;
    }

    inline bool TCore::TNote::IsExemplar() const {
      assert(this);
      return Exemplar;
    }

    inline bool TCore::TNote::IsUnReferenced() const {
      assert(this);
      return UnReferenced;
    }

    inline bool TCore::TNote::IsTuple() const {
      return Tycon == TTycon::Tuple;
    }

    inline size_t TCore::TNote::GetTupleNumNonFree() const {
      assert(Tycon == TTycon::Tuple);
      size_t ret = 0UL;
      const TCore *start, *limit;
      GetArray(start, limit);
      for (; start < limit; ++start, ++ret) {
        if (start->IsFree()) {
          break;
        }
      }
      return ret;
    }

    /*
     *    SS classes.
     */

    #define EMPTY_STATE(name)  \
      class TCore::SS::T##name final  \
          : public State::T##name {  \
        public:  \
        T##name() {}  \
        virtual Type::T##name *Get##name##Type(void *type_alloc) const override {  \
          assert(this);  \
          return new (type_alloc) Type::T##name();  \
        }  \
      };
    EMPTY_STATE(Tombstone);
    EMPTY_STATE(Void);
    #undef EMPTY_STATE

    #define SCALAR_STATE(val_t, name)  \
      class TCore::SS::T##name final  \
          : public State::T##name {  \
        public:  \
        T##name(const TCore *core)  \
            : Core(core) {}  \
        virtual const val_t &Get() const {  \
          return Core->ForceAs<val_t>();  \
        }  \
        virtual Type::T##name *Get##name##Type(void *type_alloc) const override {  \
          assert(this);  \
          return new (type_alloc) Type::T##name();  \
        }  \
        private:  \
        const TCore *Core;  \
      };
    SCALAR_STATE(int8_t, Int8)
    SCALAR_STATE(int16_t, Int16)
    SCALAR_STATE(int32_t, Int32)
    SCALAR_STATE(int64_t, Int64)
    SCALAR_STATE(uint8_t, UInt8)
    SCALAR_STATE(uint16_t, UInt16)
    SCALAR_STATE(uint32_t, UInt32)
    SCALAR_STATE(uint64_t, UInt64)
    SCALAR_STATE(bool, Bool)
    SCALAR_STATE(char, Char)
    SCALAR_STATE(float, Float)
    SCALAR_STATE(double, Double)
    SCALAR_STATE(TStdDuration, Duration)
    SCALAR_STATE(TStdTimePoint, TimePoint)
    SCALAR_STATE(TCore::TUuid, Uuid)
    #undef SCALAR_STATE

    /* State sabot for blob. */
    class TCore::SS::TBlob final
        : public State::TBlob {
      public:

      /* Look up the type and cache the arena and core pointers. */
      TBlob(TArena *arena, const TCore *core)
          : Arena(arena), Core(core) {}

      /* See State::TBlob. */
      virtual size_t GetSize() const override {
        assert(this);
        return Core->IndirectScalarArray.Size;
      }

      /* See State::TBlob. */
      virtual Type::TBlob *GetBlobType(void *type_alloc) const override {
        assert(this);
        return new (type_alloc) Type::TBlob();
      }

      /* See State::TBlob. */
      virtual State::TBlob::TPin *Pin(void *alloc) const override {
        const uint8_t *start, *limit;
        if (Core->Tycon == TTycon::Blob) {
          /* We're a long blob. */
          return new (alloc) TPin(Arena, Core->IndirectScalarArray.Offset, Core->IndirectScalarArray.Size);
        } else if (Core->Tycon >= TTycon::MinDirectBlob && Core->Tycon <= TTycon::MaxDirectBlob) {
          /* We're a short blob. */
          start = Core->DirectBlob;
          limit = start + static_cast<TTyconNumeric>(TTycon::MaxDirectBlob) - static_cast<TTyconNumeric>(Core->Tycon);
        } else {
          /* We're hosed up. */
          std::cerr << HERE << std::endl;
          throw TCorrupt();
        }
        return new (alloc) TPin(start, limit);
      }

      private:

      /* Our finalization of our base's abstract pin. */
      class TPin final
          : public TArena::TPin, public State::TBlob::TPin {
        public:

        /* TODO */
        TPin(TArena *arena, TOffset offset, size_t min_size)
            : TArena::TPin(arena, offset), State::TBlob::TPin(GetNote()->GetStart<TVal>(), GetNote()->GetLimit<TVal>()) {
          size_t size = GetLimit() - GetStart();
          #ifndef NDEBUG
          if (size < min_size) {
            std::cerr << HERE << std::endl;
            throw TCorrupt();
          }
          #endif
        }

        /* TODO */
        TPin(const uint8_t *start, const uint8_t *limit)
            : TArena::TPin(), State::TBlob::TPin(start, limit) {}

      };

      /* Cached at construction. */
      TArena *Arena;

      /* Cached at construction. */
      const TCore *Core;

      /* Our Pin Size */
      friend class Stig::Sabot::TSizeChecker;

    };  // TCore::SS::TBlob

    /* State sabot for str. */
    class TCore::SS::TStr final
        : public State::TStr {
      public:

      /* Look up the type and cache the arena and core pointers. */
      TStr(TArena *arena, const TCore *core)
          : Arena(arena), Core(core) {}

      /* See State::TStr. */
      virtual size_t GetSize() const override {
        assert(this);
        return Core->IndirectScalarArray.Size;
      }

      /* See State::TStr. */
      virtual Type::TStr *GetStrType(void *type_alloc) const override {
        assert(this);
        return new (type_alloc) Sabot::Type::TStr();
      }

      /* See State::TStr. */
      virtual State::TStr::TPin *Pin(void *alloc) const override {
        const char *start, *limit;
        if (Core->Tycon == TTycon::Str) {
          /* We're a long string. */
          return new (alloc) TPin(Arena, Core->IndirectScalarArray.Offset, Core->IndirectScalarArray.Size);
        } else if (Core->Tycon <= TTycon::MaxDirectStr) {
          /* We're a short string. */
          start = Core->DirectStr;
          limit = start + MaxDirectSize - static_cast<TTyconNumeric>(Core->Tycon);
        } else {
          /* We're hosed up. */
          std::cerr << HERE << std::endl;
          throw TCorrupt();
        }
        return new (alloc) TPin(start, limit);
      }

      private:

      /* Our finalization of our base's abstract pin. */
      class TPin final
          : public TArena::TPin, public State::TStr::TPin {
        public:

        /* TODO */
        TPin(TArena *arena, TOffset offset, size_t min_size)
            : TArena::TPin(arena, offset), State::TStr::TPin(GetNote()->GetStart<TVal>(), GetNote()->GetLimit<TVal>() - 1) {
          #ifndef NDEBUG
          size_t size = GetLimit() - GetStart();
          if (size < min_size) {
            std::cerr << HERE << std::endl;
            throw TCorrupt();
          }
          #endif
        }

        /* TODO */
        TPin(const char *start, const char *limit)
            : TArena::TPin(), State::TStr::TPin(start, limit) {}

      };

      /* Cached at construction. */
      TArena *Arena;

      /* Cached at construction. */
      const TCore *Core;

      /* Our Pin Size */
      friend class Stig::Sabot::TSizeChecker;

    };  // TCore::SS::TStr

    template <typename TBase>
    class TCore::ST::TArrayOfTypes
        : public TBase {
      public:

      /* See Type::TBase. */
      virtual typename TBase::TPin *Pin(void *alloc) const override {
        return new (alloc) TPin(this);
      }

      protected:

      /* Cache everything. */
      TArrayOfTypes(TArena *arena, const TCore *core)
          : Arena(arena), Core(core) {}

      /* TODO */
      void PostCtor() {
        assert(this);
        #ifndef NDEBUG
        if (Core->IndirectCoreArray.ElemCount < 1) {
          std::cerr << HERE << std::endl;
          throw TCorrupt();
        }
        void *pin_alloc = alloca(sizeof(TArena::TFinalPin));
        TArena::TFinalPin::TWrapper pin(Arena->Pin(Core->IndirectCoreArray.Offset,
                                                   sizeof(Atom::TCore::TNote) + (sizeof(Atom::TCore) * Core->IndirectCoreArray.ElemCount),
                                                   pin_alloc));
        const TCore *start, *limit;
        pin->GetNote()->Get(start, limit);
        if (start == limit) {
          std::cerr << HERE << std::endl;
          throw TCorrupt();
        }
        #endif
      }

      private:

      /* Our finalization of our base's abstract pin. */
      class TPin final
          : public TArena::TPin, public TBase::TPin {
        public:

        /* TODO */
        TPin(const TArrayOfTypes *owner)
            : TArena::TPin(owner->Arena, owner->Core->IndirectCoreArray.Offset),
              TBase::TPin(), Owner(owner) {
          const TCore *limit;
          GetNote()->Get(Cores, limit);
          #ifndef NDEBUG
          size_t elem_count = limit - Cores;
          if (elem_count < owner->Core->IndirectCoreArray.ElemCount) {
            std::cerr << HERE << std::endl;
            throw TCorrupt();
          }
          #endif
        }

        private:

        /* TODO */
        virtual Type::TAny *NewElem(void *type_alloc) const override {
          return Cores[0].GetType(Owner->Arena, type_alloc);
        }

        /* TODO */
        const TArrayOfTypes *Owner;

        /* TODO */
        const TCore *Cores;

      };  // TCore::ST::TArrayOfTypes::TPin

      /* Cached at construction. */
      TArena *Arena;

      /* Cached at construction. */
      const TCore *Core;

      /* Our Pin Size */
      friend class Stig::Sabot::TSizeChecker;

    };  // TCore::ST::TArrayOfTypes

    #define ARRAY_OF_TYPES_TYPE(name)  \
      class TCore::ST::T##name final  \
          : public TArrayOfTypes<Type::T##name> {  \
        public:  \
        T##name(TArena *arena, const TCore *core)  \
            : TArrayOfTypes<Type::T##name>(arena, core) {  \
          PostCtor();  \
        }  \
      };
    ARRAY_OF_TYPES_TYPE(Desc)
    ARRAY_OF_TYPES_TYPE(Free)
    ARRAY_OF_TYPES_TYPE(Opt)
    ARRAY_OF_TYPES_TYPE(Set)
    ARRAY_OF_TYPES_TYPE(Vector)
    #undef ARRAY_OF_TYPES_TYPE

    template <typename TBase>
    class TCore::SS::TArrayOfStates
        : public TBase {
      public:

      /* See State::TBase. */
      virtual size_t GetElemCount() const override {
        assert(this);
        return !Core->IndirectCoreArray.IsExemplar ? Core->IndirectCoreArray.ElemCount : 0;
      }

      /* See State::TBase. */
      virtual typename TBase::TPin *Pin(void *alloc) const override {
        return new (alloc) TPin(this);
      }

      protected:

      /* Cache everything. */
      TArrayOfStates(TArena *arena, const TCore *core)
          : Arena(arena), Core(core) {}

      /* TODO */
      void PostCtor() {
        assert(this);
        #ifndef NDEBUG
        if (Core->IndirectCoreArray.ElemCount < 1) {
          std::cerr << HERE << std::endl;
          throw TCorrupt();
        }
        void *pin_alloc = alloca(sizeof(TArena::TFinalPin));
        TArena::TFinalPin::TWrapper pin(Arena->Pin(Core->IndirectCoreArray.Offset,
                                                   sizeof(Atom::TCore::TNote) + (sizeof(Atom::TCore) * Core->IndirectCoreArray.ElemCount),
                                                   pin_alloc));
        const TCore *start, *limit;
        pin->GetNote()->Get(start, limit);
        if (start == limit) {
          std::cerr << HERE << std::endl;
          throw TCorrupt();
        }
        #endif
      }

      protected:

      /* Our finalization of our base's abstract pin. */
      class TPin final
          : public TArena::TPin, public TBase::TPin {
        public:

        /* TODO */
        TPin(const TArrayOfStates *owner)
            : TArena::TPin(owner->Arena, owner->Core->IndirectCoreArray.Offset),
              TBase::TPin(owner), Owner(owner) {
          const TCore *limit;
          GetNote()->Get(Cores, limit);
          #ifndef NDEBUG
          size_t elem_count = limit - Cores;
          if (elem_count < owner->Core->IndirectCoreArray.ElemCount) {
            std::cerr << HERE << std::endl;
            throw TCorrupt();
          }
          #endif
        }

        private:

        /* TODO */
        virtual State::TAny *NewElemInRange(size_t elem_idx, void *state_alloc) const override {
          return Cores[elem_idx].NewState(Owner->Arena, state_alloc);
        }

        /* TODO */
        const TArrayOfStates *Owner;

        /* TODO */
        const TCore *Cores;

      };  // TCore::SS::TArrayOfStates::TPin

      /* Cached at construction. */
      TArena *Arena;

      /* Cached at construction. */
      const TCore *Core;

      /* Our Pin Size */
      friend class Stig::Sabot::TSizeChecker;

    };  // TCore::SS::TArrayOfStates

    #define ARRAY_OF_STATES_STATE(name)  \
      class TCore::SS::T##name final  \
          : public TArrayOfStates<State::T##name> {  \
        public:  \
        T##name(TArena *arena, const TCore *core)  \
            : TArrayOfStates<State::T##name>(arena, core) {  \
          PostCtor();  \
        }  \
        virtual Type::T##name *Get##name##Type(void *type_alloc) const override {  \
          assert(this);  \
          return new (type_alloc) TCore::ST::T##name(Arena, Core);  \
        }  \
      };
    ARRAY_OF_STATES_STATE(Desc)
    ARRAY_OF_STATES_STATE(Opt)
    ARRAY_OF_STATES_STATE(Set)
    ARRAY_OF_STATES_STATE(Vector)
    #undef ARRAY_OF_STATES_STATE

    /* State sabot for free. */
    class TCore::SS::TFree final
        : public State::TFree {
      public:

      /* Look up the type. */
      TFree(TArena *arena, const TCore *core)
          : Arena(arena), Core(core) {
        assert(arena);
        assert(core);
        #ifndef NDEBUG
        if (core->IndirectCoreArray.ElemCount < 1) {
          std::cerr << HERE << std::endl;
          throw TCorrupt();
        }
        void *pin_alloc = alloca(sizeof(TArena::TFinalPin));
        TArena::TFinalPin::TWrapper pin(arena->Pin(core->IndirectCoreArray.Offset,
                                                   sizeof(Atom::TCore::TNote) + (sizeof(Atom::TCore) * Core->IndirectCoreArray.ElemCount),
                                                   pin_alloc));
        const TCore *start, *limit;
        pin->GetNote()->Get(start, limit);
        if (start == limit) {
          std::cerr << HERE << std::endl;
          throw TCorrupt();
        }
        #endif
      }

      /* See State::TFree. */
      virtual Type::TFree *GetFreeType(void *type_alloc) const override {
        assert(this);
        return new (type_alloc) TCore::ST::TFree(Arena, Core);
      }

      private:

      /* Cached at construction. */
      TArena *Arena;

      /* Cached at construction. */
      const TCore *Core;

      /* Our Pin Size */
      friend class Stig::Sabot::TSizeChecker;

    };  // TCore::SS::TFree

    template <typename TBase>
    class TCore::ST::TArrayOfPairsOfTypes
        : public TBase {
      public:

      /* See Type::TBase. */
      virtual typename TBase::TPin *Pin(void *alloc) const override {
        return new (alloc) TPin(this);
      }

      protected:

      /* Cache everything. */
      TArrayOfPairsOfTypes(TArena *arena, const TCore *core)
          : Arena(arena), Core(core) {}

      /* TODO */
      void PostCtor() {
        assert(this);
        #ifndef NDEBUG
        if (Core->IndirectCoreArray.ElemCount < 1) {
          std::cerr << HERE << std::endl;
          throw TCorrupt();
        }
        void *pin_alloc = alloca(sizeof(TArena::TFinalPin));
        TArena::TFinalPin::TWrapper pin(Arena->Pin(Core->IndirectCoreArray.Offset,
                                                   sizeof(Atom::TCore::TNote) + (sizeof(Atom::TCore) * 2 * Core->IndirectCoreArray.ElemCount),
                                                   pin_alloc));
        const TNote::TPairOfCores *start, *limit;
        pin->GetNote()->Get(start, limit);
        if (start == limit) {
          std::cerr << HERE << std::endl;
          throw TCorrupt();
        }
        #endif
      }

      private:

      /* Our finalization of our base's abstract pin. */
      class TPin final
          : public TArena::TPin, public TBase::TPin {
        public:

        /* TODO */
        TPin(const TArrayOfPairsOfTypes *owner)
            : TArena::TPin(owner->Arena, owner->Core->IndirectCoreArray.Offset),
              TBase::TPin(), Owner(owner) {
           const TNote::TPairOfCores *limit;
          GetNote()->Get(PairsOfCores, limit);
          #ifndef NDEBUG
          size_t elem_count = limit - PairsOfCores;
          if (elem_count < owner->Core->IndirectCoreArray.ElemCount) {
            std::cerr << HERE << std::endl;
            throw TCorrupt();
          }
          #endif
        }

        private:

        /* TODO */
        virtual Type::TAny *NewLhs(void *type_alloc) const override {
          return PairsOfCores[0].first.GetType(Owner->Arena, type_alloc);
        }

        /* TODO */
        virtual Type::TAny *NewRhs(void *type_alloc) const override {
          return PairsOfCores[0].second.GetType(Owner->Arena, type_alloc);
        }

        /* TODO */
        const TArrayOfPairsOfTypes *Owner;

        /* TODO */
        const TNote::TPairOfCores *PairsOfCores;

      };  // TCore::ST::TArrayOfPairsOfTypes::TPin

      /* Cached at construction. */
      TArena *Arena;

      /* Cached at construction. */
      const TCore *Core;

      /* Our Pin Size */
      friend class Stig::Sabot::TSizeChecker;

    };  // TCore::ST::TArrayOfPairsOfTypes

    #define ARRAY_OF_PAIRS_OF_TYPES_TYPE(name)  \
      class TCore::ST::T##name final  \
          : public TArrayOfPairsOfTypes<Type::T##name> {  \
        public:  \
        T##name(TArena *arena, const TCore *core)  \
            : TArrayOfPairsOfTypes<Type::T##name>(arena, core) {  \
          PostCtor();  \
        }  \
      };
    ARRAY_OF_PAIRS_OF_TYPES_TYPE(Map)
    #undef ARRAY_OF_PAIRS_OF_TYPES_TYPE

    template <typename TBase>
    class TCore::SS::TArrayOfPairsOfStates
        : public TBase {
      public:

      /* See State::TBase. */
      virtual size_t GetElemCount() const override {
        assert(this);
        return !Core->IndirectCoreArray.IsExemplar ? Core->IndirectCoreArray.ElemCount : 0;
      }

      /* See State::TBase. */
      virtual typename TBase::TPin *Pin(void *alloc) const override {
        return new (alloc) TPin(this);
      }

      protected:

      /* Cache everything. */
      TArrayOfPairsOfStates(TArena *arena, const TCore *core)
          : Arena(arena), Core(core) {}

      /* TODO */
      void PostCtor() {
        assert(this);
        #ifndef NDEBUG
        if (Core->IndirectCoreArray.ElemCount < 1) {
          std::cerr << HERE << std::endl;
          throw TCorrupt();
        }
        void *pin_alloc = alloca(sizeof(TArena::TFinalPin));
        TArena::TFinalPin::TWrapper pin(Arena->Pin(Core->IndirectCoreArray.Offset,
                                                   sizeof(Atom::TCore::TNote) + (sizeof(Atom::TCore) * 2 * Core->IndirectCoreArray.ElemCount),
                                                   pin_alloc));
        const TNote::TPairOfCores *start, *limit;
        pin->GetNote()->Get(start, limit);
        if (start == limit) {
          std::cerr << HERE << std::endl;
          throw TCorrupt();
        }
        #endif
      }

      protected:

      /* Our finalization of our base's abstract pin. */
      class TPin final
          : public TArena::TPin, public TBase::TPin {
        public:

        /* TODO */
        TPin(const TArrayOfPairsOfStates *owner)
            : TArena::TPin(owner->Arena, owner->Core->IndirectCoreArray.Offset),
              TBase::TPin(owner), Owner(owner) {
           const TNote::TPairOfCores *limit;
          GetNote()->Get(PairsOfCores, limit);
          #ifndef NDEBUG
          size_t elem_count = limit - PairsOfCores;
          if (elem_count < owner->Core->IndirectCoreArray.ElemCount) {
            std::cerr << HERE << std::endl;
            throw TCorrupt();
          }
          #endif
        }

        private:

        /* TODO */
        virtual State::TAny *NewLhsInRange(size_t elem_idx, void *state_alloc) const override {
          return PairsOfCores[elem_idx].first.NewState(Owner->Arena, state_alloc);
        }

        /* TODO */
        virtual State::TAny *NewRhsInRange(size_t elem_idx, void *state_alloc) const override {
          return PairsOfCores[elem_idx].second.NewState(Owner->Arena, state_alloc);
        }

        /* TODO */
        const TArrayOfPairsOfStates *Owner;

        /* TODO */
        const TNote::TPairOfCores *PairsOfCores;

      };  // TCore::SS::TArrayOfPairsOfStates::TPin

      /* Cached at construction. */
      TArena *Arena;

      /* Cached at construction. */
      const TCore *Core;

      /* Our Pin Size */
      friend class Stig::Sabot::TSizeChecker;

    };  // TCore::SS::TArrayOfPairsOfStates

    #define ARRAY_OF_PAIRS_OF_STATES_STATE(name)  \
      class TCore::SS::T##name final  \
          : public TArrayOfPairsOfStates<State::T##name> {  \
        public:  \
        T##name(TArena *arena, const TCore *core)  \
            : TArrayOfPairsOfStates<State::T##name>(arena, core) {  \
          PostCtor();  \
        }  \
        virtual Type::T##name *Get##name##Type(void *type_alloc) const override {  \
          assert(this);  \
          return new (type_alloc) TCore::ST::T##name(Arena, Core);  \
        }  \
      };
    ARRAY_OF_PAIRS_OF_STATES_STATE(Map)
    #undef ARRAY_OF_PAIRS_OF_STATES_STATE

    /* Type sabot for record. */
    class TCore::ST::TRecord final
        : public Type::TRecord {
      public:

      /* Cache everything and look up the final type. */
      TRecord(TArena *arena, const TCore *core)
          : Arena(arena), Core(core) {
        #ifndef NDEBUG
        void *pin_alloc = alloca(sizeof(TArena::TFinalPin));
        TArena::TFinalPin::TWrapper pin(arena->Pin(core->IndirectCoreArray.Offset,
                                                   sizeof(Atom::TCore::TNote) + (sizeof(Atom::TCore) * 2 * Core->IndirectCoreArray.ElemCount),
                                                   pin_alloc));
        const TNote::TPairOfCores *start, *limit;
        pin->GetNote()->Get(start, limit);
        size_t elem_count = limit - start;
        if (elem_count < core->IndirectCoreArray.ElemCount) {
          std::cerr << HERE << std::endl;
          throw TCorrupt();
        }
        #endif
      }

      /* See Type::TBase. */
      virtual size_t GetElemCount() const override {
        assert(this);
        return Core->IndirectCoreArray.ElemCount;
      }

      /* See Type::TRecord. */
      virtual Type::TRecord::TPin *Pin(void *alloc) const override {
        return new (alloc) TPin(this);
      }

      protected:

      /* Our finalization of our base's abstract pin. */
      class TPin final
          : public TArena::TPin,
            public Type::TRecord::TPin {
        public:

        /* TODO */
        TPin(const TRecord *owner)
            : TArena::TPin(owner->Arena, owner->Core->IndirectCoreArray.Offset),
              Type::TRecord::TPin(owner),
              Owner(owner) {
          const TNote::TPairOfCores *limit;
          GetNote()->Get(PairsOfCores, limit);
          #ifndef NDEBUG
          size_t elem_count = limit - PairsOfCores;
          if (elem_count < owner->Core->IndirectCoreArray.ElemCount) {
            std::cerr << HERE << std::endl;
            throw TCorrupt();
          }
          #endif
        }

        /* TODO */
        virtual Type::TAny *NewElem(size_t elem_idx, std::string &field_name, void *type_alloc) const override {
          PairsOfCores[elem_idx].first.CopyOut(Owner->Arena, field_name);
          return PairsOfCores[elem_idx].second.GetType(Owner->Arena, type_alloc);
        }

        /* TODO */
        virtual TAny *NewElem(size_t elem_idx, void *&out_field_name_state, void *field_name_state_alloc, void *type_alloc) const override {
          out_field_name_state = PairsOfCores[elem_idx].first.NewState(Owner->Arena, field_name_state_alloc);
          return PairsOfCores[elem_idx].second.GetType(Owner->Arena, type_alloc);
        }

        /* TODO */
        virtual Type::TAny *NewElem(size_t elem_idx, void *type_alloc) const override {
          return PairsOfCores[elem_idx].second.GetType(Owner->Arena, type_alloc);
        }

        private:

        /* TODO */
        const TRecord *Owner;

        /* TODO */
        const TNote::TPairOfCores *PairsOfCores;

      };  // TCore::ST::TRecord::TPin

      /* Cached at construction. */
      TArena *Arena;

      /* Cached at construction. */
      const TCore *Core;

      /* Our Pin Size */
      friend class Stig::Sabot::TSizeChecker;

    };  // TCore::ST::TRecord

    /* State sabot for record. */
    class TCore::SS::TRecord final
        : public State::TRecord {
      public:

      /* Cache everything and look up the final type. */
      TRecord(TArena *arena, const TCore *core)
          : Arena(arena), Core(core) {
        #ifndef NDEBUG
        void *pin_alloc = alloca(sizeof(TArena::TFinalPin));
        TArena::TFinalPin::TWrapper pin(arena->Pin(core->IndirectCoreArray.Offset,
                                                   sizeof(Atom::TCore::TNote) + (sizeof(Atom::TCore) * 2 * core->IndirectCoreArray.ElemCount),
                                                   pin_alloc));
        const TNote::TPairOfCores *start, *limit;
        pin->GetNote()->Get(start, limit);
        size_t elem_count = limit - start;
        if (elem_count < core->IndirectCoreArray.ElemCount) {
          std::cerr << HERE << std::endl;
          throw TCorrupt();
        }
        limit = start + core->IndirectCoreArray.ElemCount;
        #endif
      }

      /* See State::TRecord. */
      virtual Type::TRecord *GetRecordType(void *type_alloc) const override {
        assert(this);
        return new (type_alloc) TCore::ST::TRecord(Arena, Core);
      }

      /* See State::TRecord. */
      virtual State::TRecord::TPin *Pin(void *alloc) const override {
        return new (alloc) TPin(this);
      }

      /* TODO */
      virtual size_t GetElemCount() const override {
        assert(this);
        return Core->IndirectCoreArray.ElemCount;
      }

      private:

      /* Our finalization of our base's abstract pin. */
      class TPin final
          : public TArena::TPin,
            public State::TRecord::TPin {
        public:

        /* TODO */
        TPin(const TRecord *owner)
            : TArena::TPin(owner->Arena, owner->Core->IndirectCoreArray.Offset),
              State::TRecord::TPin(owner),
              Owner(owner) {
          const TNote::TPairOfCores *limit;
          GetNote()->Get(PairsOfCores, limit);
          #ifndef NDEBUG
          size_t elem_count = limit - PairsOfCores;
          if (elem_count < owner->Core->IndirectCoreArray.ElemCount) {
            std::cerr << HERE << std::endl;
            throw TCorrupt();
          }
          #endif
        }

        private:

        /* TODO */
        virtual State::TAny *NewElemInRange(size_t elem_idx, void *state_alloc) const override {
          return PairsOfCores[elem_idx].second.NewState(Owner->Arena, state_alloc);
        }

        /* TODO */
        const TRecord *Owner;

        /* TODO */
        const TNote::TPairOfCores *PairsOfCores;

      };  // TCore::SS::TRecord::TPin

      /* Cached at construction. */
      TArena *Arena;

      /* Cached at construction. */
      const TCore *Core;

      /* Our Pin Size */
      friend class Stig::Sabot::TSizeChecker;

    };  // TCore::SS::TRecord

    /* Type sabot for tuple. */
    class TCore::ST::TTuple final
        : public Type::TTuple {
      public:

      /* Cache everything and look up the final type. */
      TTuple(TArena *arena, const TCore *core)
          : Arena(arena), Core(core) {
        #ifndef NDEBUG
        void *pin_alloc = alloca(sizeof(TArena::TFinalPin));
        TArena::TFinalPin::TWrapper pin(arena->Pin(core->IndirectCoreArray.Offset,
                                                   sizeof(Atom::TCore::TNote) + (sizeof(Atom::TCore) * core->IndirectCoreArray.ElemCount),
                                                   pin_alloc));
        const TCore *start, *limit;
        pin->GetNote()->Get(start, limit);
        size_t elem_count = limit - start;
        if (elem_count < core->IndirectCoreArray.ElemCount) {
          std::cerr << HERE << std::endl;
          throw TCorrupt();
        }
        #endif
      }

      /* See Type::TBase. */
      virtual size_t GetElemCount() const override {
        assert(this);
        return Core->IndirectCoreArray.ElemCount;
      }

      /* See Type::TTuple. */
      virtual Type::TTuple::TPin *Pin(void *alloc) const override {
        return new (alloc) TPin(this);
      }

      protected:

      /* Our finalization of our base's abstract pin. */
      class TPin final
          : public TArena::TPin,
            public Type::TTuple::TPin {
        public:

        /* TODO */
        TPin(const TTuple *owner)
            : TArena::TPin(owner->Arena,
                           owner->Core->IndirectCoreArray.Offset,
                           sizeof(Atom::TCore::TNote) + (sizeof(Atom::TCore) * owner->Core->IndirectCoreArray.ElemCount)),
              Type::TTuple::TPin(owner),
              Owner(owner) {
          const TCore *limit;
          GetNote()->Get(Cores, limit);
          #ifndef NDEBUG
          size_t elem_count = limit - Cores;
          if (elem_count < owner->Core->IndirectCoreArray.ElemCount) {
            std::cerr << HERE << std::endl;
            throw TCorrupt();
          }
          #endif
        }

        private:

        /* TODO */
        virtual Type::TAny *NewElem(size_t elem_idx, void *type_alloc) const override {
          return Cores[elem_idx].GetType(Owner->Arena, type_alloc);
        }

        /* TODO */
        const TTuple *Owner;

        /* TODO */
        const TCore *Cores;

      };  // TCore::ST::TTuple::TPin

      /* Cached at construction. */
      TArena *Arena;

      /* Cached at construction. */
      const TCore *Core;

      /* Our Pin Size */
      friend class Stig::Sabot::TSizeChecker;

    };  // TCore::ST::TTuple

    /* State sabot for tuple. */
    class TCore::SS::TTuple final
        : public State::TTuple {
      public:

      /* Cache everything and look up the final type. */
      TTuple(TArena *arena, const TCore *core)
          : Arena(arena), Core(core) {
        #ifndef NDEBUG
        void *pin_alloc = alloca(sizeof(TArena::TFinalPin));
        TArena::TFinalPin::TWrapper pin(arena->Pin(core->IndirectCoreArray.Offset,
                                                   sizeof(Atom::TCore::TNote) + (sizeof(Atom::TCore) * core->IndirectCoreArray.ElemCount),
                                                   pin_alloc));
        const TCore *start, *limit;
        pin->GetNote()->Get(start, limit);
        size_t elem_count = limit - start;
        if (elem_count < core->IndirectCoreArray.ElemCount) {
          std::cerr << HERE << std::endl;
          throw TCorrupt();
        }
        #endif
      }

      /* See State::TTuple. */
      virtual Type::TTuple *GetTupleType(void *type_alloc) const override {
        assert(this);
        return new (type_alloc) TCore::ST::TTuple(Arena, Core);
      }

      /* See State::TTuple. */
      virtual State::TTuple::TPin *Pin(void *alloc) const override {
        assert(this);
        return new (alloc) TPin(this);
      }

      /* TODO */
      virtual size_t GetElemCount() const override {
        assert(this);
        return Core->IndirectCoreArray.ElemCount;
      }

      private:

      /* Our finalization of our base's abstract pin. */
      class TPin final
          : public TArena::TPin,
            public State::TTuple::TPin {
        public:

        /* TODO */
        TPin(const TTuple *owner)
            : TArena::TPin(owner->Arena,
                           owner->Core->IndirectCoreArray.Offset,
                           sizeof(Atom::TCore::TNote) + (sizeof(Atom::TCore) * owner->Core->IndirectCoreArray.ElemCount)),
              State::TTuple::TPin(owner),
              Owner(owner) {
          const TCore *limit;
          GetNote()->Get(Cores, limit);
          #ifndef NDEBUG
          size_t elem_count = limit - Cores;
          if (elem_count < owner->Core->IndirectCoreArray.ElemCount) {
            std::cerr << HERE << std::endl;
            throw TCorrupt();
          }
          #endif
        }

        private:

        /* TODO */
        virtual State::TAny *NewElemInRange(size_t elem_idx, void *state_alloc) const override {
          return Cores[elem_idx].NewState(Owner->Arena, state_alloc);
        }

        /* TODO */
        const TTuple *Owner;

        /* TODO */
        const TCore *Cores;

      };  // TCore::SS::TTuple::TPin

      /* Cached at construction. */
      TArena *Arena;

      /* Cached at construction. */
      const TCore *Core;

      /* Our Pin Size */
      friend class Stig::Sabot::TSizeChecker;

    };  // TCore::SS::TTuple

    /* Binary streamers for Stig::Atom::TCore */
    inline Io::TBinaryOutputStream &operator<<(Io::TBinaryOutputStream &strm, const TCore &core) { strm.WriteExactly(&core, sizeof(TCore)); return strm; }
    inline Io::TBinaryInputStream &operator>>(Io::TBinaryInputStream &strm, TCore &core) { strm.ReadExactly(&core, sizeof(TCore)); return strm; }

  }  // Atom

}  // Stig