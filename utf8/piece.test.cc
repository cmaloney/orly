/* <utf8/piece.test.cc>

   Unit test for <utf8/piece.h>.

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

#include <utf8/piece.h>

#include <algorithm>
#include <cstring>
#include <vector>

#include <test/kit.h>

using namespace std;
using namespace Utf8;

FIXTURE(Is) {
  char buf_1[4], buf_2[4], buf_3[4];
  strcpy(buf_1, "abc");
  strcpy(buf_2, buf_1);
  strcpy(buf_3, "abd");
  TPiece
      p1 = buf_1,  // 'abc'
      p2 = buf_2,  // a different 'abc' than p1
      p3 = buf_3,  // 'abd'
      p4 = p1,     // the same 'abc' as p1
      p5;          // empty
  /* Every piece 'is' itself. */
  EXPECT_TRUE(p1.Is(p1));
  EXPECT_TRUE(p2.Is(p2));
  EXPECT_TRUE(p3.Is(p3));
  EXPECT_TRUE(p4.Is(p4));
  EXPECT_TRUE(p5.Is(p5));
  /* Pieces p1 and p2, although lexically the same, are different in identity. */
  EXPECT_FALSE(p1.Is(p2));
  EXPECT_FALSE(p2.Is(p1));
  /* Pieces p1 and p3 are different lexically and in identity. */
  EXPECT_FALSE(p1.Is(p3));
  EXPECT_FALSE(p3.Is(p1));
  /* Pieces p1 and p4 are the same lexically and in identity. */
  EXPECT_TRUE(p1.Is(p4));
  EXPECT_TRUE(p4.Is(p1));
  /* Pieces p1 and p5 are different lexically and in identity and p5 happens to be empty. */
  EXPECT_FALSE(p1.Is(p5));
  EXPECT_FALSE(p5.Is(p1));
}

FIXTURE(Assignment) {
  char buf[4];
  strcpy(buf, "abc");
  /* Pieces a and b are different lexically and in identity. */
  TPiece a = buf, b;
  EXPECT_FALSE(a.Is(b));
  /* Assign to piece b the same data as was assigned to piece a. */
  b = buf;
  /* Pieces a and b are now the same lexically and in identity. */
  EXPECT_TRUE(a.Is(b));
}

FIXTURE(EmptyAndReset) {
  TPiece a = "abc", b;
  EXPECT_TRUE(a);
  EXPECT_FALSE(b);
  a.Reset();
  EXPECT_FALSE(a);
}

FIXTURE(Pop) {
  using sample_t = pair<uint32_t, const char *>;
  const sample_t
      s0(0x00024, "\x24"),              // dollar sign
      s1(0x000A2, "\xC2\xA2"),          // cent sign
      s2(0x020AC, "\xE2\x82\xAC"),      // euro sign
      s3(0x24B62, "\xF0\xA4\xAD\xA2"),  // han character
      *all_samples[4] = { &s0, &s1, &s2, &s3 };
  int
      actual_pass_cnt = 0,
      expected_pass_cnt = 0;
  for (int bit_pattern = 0; bit_pattern < 256; ++bit_pattern) {
    /* Use each sample 0, 1, 2, or 3 times to build the selected sample set. */
    vector<const sample_t *> samples;
    for (int sample_idx = 0; sample_idx < 4; ++sample_idx) {
      int repeat_count = (bit_pattern & (3 << (sample_idx << 1))) >> (sample_idx << 1);
      for (int i = 0; i < repeat_count; ++i) {
        samples.push_back(all_samples[sample_idx]);
      }
    }
    /* Build test strings by concatenating the selected samples in all possible orders. */
    do {
      /* The maximum length of a test string is 3 * (1 + 2 + 3 + 4) + 1 = 31 bytes. */
      char buf[31];
      *buf = '\0';
      for (const auto *sample: samples) {
        strcat(buf, sample->second);
      }
      /* Pop the code points from the test string one at a time.
         We should get the samples back in order. */
      TPiece p = buf;
      bool is_pass = true;
      for (const auto *sample: samples) {
        if (p.Pop() != sample->first) {
          is_pass = false;
          break;
        }
      }
      /* The piece should be empty now. */
      if (is_pass && !p) {
        ++actual_pass_cnt;
      }
      ++expected_pass_cnt;
    } while (next_permutation(samples.begin(), samples.end()));
  }
  EXPECT_EQ(actual_pass_cnt, expected_pass_cnt);
}

FIXTURE(GetLength) {
  EXPECT_EQ(TPiece().GetLength(), 0u);
  EXPECT_EQ(TPiece("a").GetLength(), 1u);
  EXPECT_EQ(TPiece("ab").GetLength(), 2u);
  EXPECT_EQ(TPiece("abc").GetLength(), 3u);
  EXPECT_EQ(TPiece("$\xC2\xA2\xE2\x82\xAC\xF0\xA4\xAD\xA2").GetLength(), 4u);
}

FIXTURE(CompareAndHash) {
  TPiece
      a = "abc",
      b = "abx",
      c = "ab",
      d = "abcd",
      e;
  EXPECT_TRUE(a == a);
  EXPECT_TRUE(a <= a);
  EXPECT_TRUE(a >= a);
  EXPECT_TRUE(a != b);
  EXPECT_TRUE(a < b);
  EXPECT_TRUE(a <= b);
  EXPECT_TRUE(b > a);
  EXPECT_TRUE(b >= a);
  EXPECT_TRUE(a > c);
  EXPECT_TRUE(a < d);
  EXPECT_TRUE(a > e);
  EXPECT_TRUE(e == e);
  EXPECT_NE(a.GetHash(), b.GetHash());
  EXPECT_NE(a.GetHash(), c.GetHash());
  EXPECT_NE(a.GetHash(), d.GetHash());
  EXPECT_NE(a.GetHash(), e.GetHash());
  EXPECT_FALSE(e.GetHash());
}

FIXTURE(BadEncoding) {
  bool caught = false;
  try {
    TPiece("\xC2").Pop();
  } catch (const TPiece::TBadEncoding &) {
    caught = true;
  } catch (...) {}
  EXPECT_TRUE(caught);
}

FIXTURE(PopEmpty) {
  bool caught = false;
  try {
    TPiece().Pop();
  } catch (const TPiece::TEmpty &) {
    caught = true;
  } catch (...) {}
  EXPECT_TRUE(caught);
}

FIXTURE(Peek) {
  TPiece a = "ab";
  EXPECT_EQ(a.Peek(), u'a');
  EXPECT_EQ(a.Peek(), u'a');
  EXPECT_EQ(a.Pop(), u'a');
  EXPECT_EQ(a.Peek(), u'b');
}
