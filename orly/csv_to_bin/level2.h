/* <orly/csv_to_bin/level2.h>

   A level-2 parser for data in CSV format.  This level converts the
   output of a level-1 parser into a sequence of start and stop markers
   for files, records, and fields, and presents the bytes of each field
   as buffers,

   See RFC-4180 for details. (http://tools.ietf.org/html/rfc4180)

   Copyright 2010-2014 OrlyAtomics, Inc.

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
#include <ostream>

#include <base/class_traits.h>
#include <orly/csv_to_bin/level1.h>

namespace Orly {

  namespace CsvToBin {

    /* An input consumer that translates CSV level-1 syntax. */
    class TLevel2 final {
      NO_COPY(TLevel2);
      public:

      /* Describes our position in the parse. */
      enum TState {

        /* The start of the file.  The first state reported for any file, even
           if the file is empty.  Next state is StartOfRecord (if the file is
           non-empty) or EndOfFile (if the file is empty). */
        StartOfFile,

        /* The start of a record, which may or may not be empty.  We reach
           this state at least one for every non-empty file.  The next state
           is StartOfField. */
        StartOfRecord,

        /* The start of a field, which may or may not be empty.  We reach
           this state at least once for every record.  (We consider an empty
           record to be a record of one empty field, not a record of no
           fields.)  The next state is Bytes (if the field is non-empty) or
           EndOfField (if the field is empty). */
        StartOfField,

        /* We have bytes to report for a field and the Start and Limit
           pointers are set to point to them.  We will repeat this state
           until all bytes for the field have been reported, then we switch
           to EndOfField. */
        Bytes,

        /* The end of a field, after its bytes, if any, have been reported.
           If there is another field in the record, the next state will be
           StartOfField; otherwise, the next state will be EndOfRecord. */
        EndOfField,


        /* The end of a record, after its fields have been reported.  If there
           is another record in the file, the next state will be
           StartOfRecord; otherwise, the next state will be EndOfFile. */
        EndOfRecord,

        /* The end of the file.  This is always the last state reported. */
        EndOfFile

      };  // TLevel2::TState

      /* The type of our cache, which we expose via the dereferencing
         operators. */
      struct TCache {

        /* See TState. */
        TState State;

        /* If State is Bytes, these point to the bytes; otherwise, these are
           junk. */
        const uint8_t *Start, *Limit;

      };  // TLevel2::TCache

      /* TODO */
      static const size_t MinBufferSize, DefBufferSize;

      /* TODO */
      explicit TLevel2(TLevel1 &level1, size_t buffer_size = DefBufferSize);

      /* TODO */
      ~TLevel2();

      /* Our cached state. */
      const TCache &operator*() const {
        assert(this);
        Refresh();
        return Cache;
      }

      /* Our cached state. */
      const TCache *operator->() const {
        assert(this);
        Refresh();
        return &Cache;
      }

      /* Dump our cached state and advance. */
      TLevel2 &operator++() {
        assert(this);
        Refresh();
        IsCached = false;
        return *this;
      }

      /* The human-readable name for a state. */
      static const char *GetName(TState state);

      private:

      /* If our cache is fresh, do nothing; otherwise, update it. */
      void Refresh() const {
        assert(this);
        if (!IsCached) {
          const_cast<TLevel2 *>(this)->Update();
          IsCached = true;
        }
      }

      /* Update our cache. */
      void Update();

      /* Our level-1 parser. */
      TLevel1 &Level1;

      /* See TCache.  Valid only if IsCached is true. */
      TCache Cache;

      /* If true, then Cache contains valid data. */
      mutable bool IsCached;

      /* TODO */
      TState NextState;

      /* TODO */
      uint8_t *Start, *Limit;

    };  // TLevel2

  }  // Csv2Bin

}  // Orly

/* Inserts human-readable names for the level-2 parser states. */
std::ostream &operator<<(
    std::ostream &strm, Orly::CsvToBin::TLevel2::TState that);
