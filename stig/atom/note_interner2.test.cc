/* <stig/atom/note_interner2.test.cc>

   Unit test for <stig/atom/note_interner2.h>.

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

#include <stig/atom/note_interner2.h>

#include <memory>

#include <test/kit.h>

using namespace std;
using namespace Stig::Atom;

static const TCore::TNote *Propose(TNoteInterner &note_interner, const char *c_str) {
  return note_interner.Propose(TCore::TNote::New(c_str, c_str + strlen(c_str), false));
}

FIXTURE(Typical) {
  const char
      *str_a = "hello",
      *str_b = "doctor";
  TNoteInterner
      interner_a,
      interner_b;
  auto
      note_a1 = Propose(interner_a, str_a),
      note_a2 = Propose(interner_a, str_a),
      note_b1 = Propose(interner_a, str_b),
      note_b2 = Propose(interner_b, str_b);
  EXPECT_EQ(note_a1, note_a2);
  EXPECT_NE(note_a1, note_b1);
  EXPECT_NE(note_a1, note_b2);
  EXPECT_NE(note_b1, note_b2);
  EXPECT_TRUE(interner_a.IsKnown(note_a1));
  EXPECT_TRUE(interner_a.IsKnown(note_b1));
  EXPECT_TRUE(interner_b.IsKnown(note_b2));
  EXPECT_TRUE(interner_a.IsKnown(note_b2));
  EXPECT_TRUE(interner_b.IsKnown(note_b1));
  EXPECT_FALSE(interner_a.IsKnown(nullptr));
  EXPECT_FALSE(interner_b.IsKnown(nullptr));
  EXPECT_TRUE(interner_a.IsOwned(note_a1));
  EXPECT_TRUE(interner_a.IsOwned(note_b1));
  EXPECT_TRUE(interner_b.IsOwned(note_b2));
  EXPECT_FALSE(interner_a.IsOwned(note_b2));
  EXPECT_FALSE(interner_b.IsOwned(note_b1));
  EXPECT_FALSE(interner_a.IsOwned(nullptr));
  EXPECT_FALSE(interner_b.IsOwned(nullptr));
  EXPECT_EQ(interner_a.GetSize(), 2U);
  EXPECT_EQ(interner_b.GetSize(), 1U);
  unordered_set<const TCore::TNote *> notes;
  for (const auto &iter : interner_a.GetNotes()) {
    notes.insert(iter);
  }
  EXPECT_EQ(notes.size(), 2U);
  EXPECT_TRUE(notes.find(note_a1) != notes.end());
  EXPECT_TRUE(notes.find(note_b1) != notes.end());
  EXPECT_TRUE(notes.find(note_b2) == notes.end());
}
