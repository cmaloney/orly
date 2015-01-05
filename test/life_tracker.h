/* <test/life_tracker.h>

   An object that tracks its own construction, destruction, movement, and
   copying.  Useful when testing the copy- and/or move-semantics of
   containers as they act upon their elements.

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
#include <memory>
#include <ostream>
#include <utility>

namespace Test {

/* An object that tracks its own construction, destruction, movement, and
   copying. */
class TLifeTracker final {
  public:
  /* Keeps counts of our life events. */
  struct TCounts {
    /* Construct with all counts zero. */
    TCounts() : MoveCtor(0), CopyCtor(0), MoveAssign(0), CopyAssign(0), Swap(0) {}

    /* Construct with the given counts. */
    TCounts(int move_ctor, int copy_ctor, int move_assign, int copy_assign, int swap)
        : MoveCtor(move_ctor),
          CopyCtor(copy_ctor),
          MoveAssign(move_assign),
          CopyAssign(copy_assign),
          Swap(swap) {}

    /* True iff. all counts equal. */
    bool operator==(const TCounts &that) const noexcept {
      assert(this);
      assert(&that);
      return MoveCtor == that.MoveCtor && CopyCtor == that.CopyCtor &&
             MoveAssign == that.MoveAssign && CopyAssign == that.CopyAssign && Swap == that.Swap;
    }

    /* True iff. any counts non-equal. */
    bool operator!=(const TCounts &that) const noexcept { return !(*this == that); }

    /* Human-readable dump for us in EXPECT clauses. */
    friend std::ostream &operator<<(std::ostream &strm, const TCounts &that) {
      return strm << "{ MoveCtor: " << that.MoveCtor << ", CopyCtor: " << that.CopyCtor
                  << ", MoveAssign: " << that.MoveAssign << ", CopyAssign: " << that.CopyAssign
                  << ", Swap: " << that.Swap << " }";
    }

    /* The counts themselves. */
    int MoveCtor, CopyCtor, MoveAssign, CopyAssign, Swap;

  };  // TLifeTracker::TCounts

  /* A new object with a distinct identity and all counts set to zero. */
  TLifeTracker() : Counts(std::make_shared<TCounts>()) {}

  /* Share the identity of our donor and count a move-construction. */
  TLifeTracker(TLifeTracker &&that) noexcept : Counts(that.Counts) { ++(Counts->MoveCtor); }

  /* Share the identity of our example and count a copy-construction. */
  TLifeTracker(const TLifeTracker &that) : Counts(that.Counts) { ++(Counts->CopyCtor); }

  /* Take on the identity of our donor and count a move-assignment. */
  TLifeTracker &operator=(TLifeTracker &&that) noexcept {
    assert(this);
    assert(&that);
    Counts = that.Counts;
    ++(Counts->MoveAssign);
    return *this;
  }

  /* Take on the identity of our example and count a copy-assignment. */
  TLifeTracker &operator=(const TLifeTracker &that) {
    assert(this);
    assert(&that);
    Counts = that.Counts;
    ++(Counts->CopyAssign);
    return *this;
  }

  /* Get our counts. */
  const TCounts &operator*() const noexcept {
    assert(this);
    return *Counts;
  }

  /* Get our counts. */
  const TCounts *operator->() const noexcept {
    assert(this);
    return Counts.get();
  }

  /* Count a swap against both objects. */
  friend void swap(TLifeTracker &lhs, TLifeTracker &rhs) noexcept {
    assert(&rhs);
    assert(&lhs);
    using std::swap;
    swap(lhs.Counts, rhs.Counts);
    ++(lhs.Counts->Swap);
    ++(rhs.Counts->Swap);
  }

  private:
  /* Keeps track of our identity and counts. */
  std::shared_ptr<TCounts> Counts;

};  // TLifeTracker

}  // Test
