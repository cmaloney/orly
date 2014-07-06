/* <orly/csv_to_bin/synth.test.cc>

   Unit test for <orly/csv_to_bin/synth.h>.

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

#include <orly/csv_to_bin/synth.h>

#include <sstream>

#include <test/kit.h>

using namespace std;
using namespace Orly::CsvToBin;

FIXTURE(Typical) {
  ostringstream strm;
  auto table = NewTable(
      strm,
      "/* c-style comments /* even nested */ are ok */\n"
      "user_id id not null;\n"
      "last_name str;\n"
      "cash real;\n"
      "birthday time_pnt;\n"
      "code int null;\n"
      "primary key (user_id);\n"
      "last_name key (last_name, user_id);\n"
      "birthday key (birthday desc, user_id asc);\n");
  if (EXPECT_TRUE(table)) {
    EXPECT_EQ(table->GetColCount(), 5u);
    auto *col = table->TryFindCol("user_id");
    if (EXPECT_TRUE(col)) {
      EXPECT_TRUE(col->GetType() == Symbol::TType::Id);
      EXPECT_FALSE(col->IsNull());
    }
    col = table->TryFindCol("last_name");
    if (EXPECT_TRUE(col)) {
      EXPECT_TRUE(col->GetType() == Symbol::TType::Str);
      EXPECT_FALSE(col->IsNull());
    }
    col = table->TryFindCol("cash");
    if (EXPECT_TRUE(col)) {
      EXPECT_TRUE(col->GetType() == Symbol::TType::Real);
      EXPECT_FALSE(col->IsNull());
    }
    col = table->TryFindCol("birthday");
    if (EXPECT_TRUE(col)) {
      EXPECT_TRUE(col->GetType() == Symbol::TType::TimePnt);
      EXPECT_FALSE(col->IsNull());
    }
    col = table->TryFindCol("code");
    if (EXPECT_TRUE(col)) {
      EXPECT_TRUE(col->GetType() == Symbol::TType::Int);
      EXPECT_TRUE(col->IsNull());
    }
    const auto &fields = table->GetPrimaryKey()->GetFields();
    if (EXPECT_EQ(fields.size(), 1u)) {
      EXPECT_EQ(fields[0].Col->GetName(), "user_id");
      EXPECT_TRUE(fields[0].Order == Symbol::TKey::Asc);
    }
    EXPECT_EQ(table->GetSecondaryKeyCount(), 2u);
    const auto *key = table->TryFindSecondaryKey("last_name");
    if (EXPECT_TRUE(key)) {
      const auto &fields = key->GetFields();
      if (EXPECT_EQ(fields.size(), 2u)) {
        EXPECT_EQ(fields[0].Col->GetName(), "last_name");
        EXPECT_TRUE(fields[0].Order == Symbol::TKey::Asc);
        EXPECT_EQ(fields[1].Col->GetName(), "user_id");
        EXPECT_TRUE(fields[1].Order == Symbol::TKey::Asc);
      }
    }
    key = table->TryFindSecondaryKey("birthday");
    if (EXPECT_TRUE(key)) {
      const auto &fields = key->GetFields();
      if (EXPECT_EQ(fields.size(), 2u)) {
        EXPECT_EQ(fields[0].Col->GetName(), "birthday");
        EXPECT_TRUE(fields[0].Order == Symbol::TKey::Desc);
        EXPECT_EQ(fields[1].Col->GetName(), "user_id");
        EXPECT_TRUE(fields[1].Order == Symbol::TKey::Asc);
      }
    }
  }
}
