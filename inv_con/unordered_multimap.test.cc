/* <inv_con/unordered_multimap.test.cc>

   Unit test for <inv_con/unordered_multimap.h>.

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

#include <inv_con/unordered_multimap.h>

#include <base/no_copy_semantics.h>
#include <test/kit.h>

class TTeam;
class TPlayer;

class TTeam {
  NO_COPY_SEMANTICS(TTeam);
  public:

  typedef InvCon::UnorderedMultimap::TCollection<TTeam, TPlayer, int> TPlayerCollection;

  TTeam()
      : PlayerCollection(this) {}

  ~TTeam() {
    assert(this);
    PlayerCollection.DeleteEachMember();
  }

  TPlayerCollection *GetPlayerCollection() const {
    assert(this);
    return &PlayerCollection;
  }

  private:

  mutable TPlayerCollection::TImpl PlayerCollection;

};  // TTeam

class TPlayer {
  NO_COPY_SEMANTICS(TPlayer);
  public:

  typedef InvCon::UnorderedMultimap::TMembership<TPlayer, TTeam, int> TTeamMembership;

  TPlayer(TTeam *team, int number)
      : TeamMembership(this, number, team->GetPlayerCollection()) {}

  int GetNumber() const {
    assert(this);
    return TeamMembership.GetKey();
  }

  TTeamMembership *GetTeamMembership() {
    assert(this);
    return &TeamMembership;
  }

  private:

  TTeamMembership::TImpl TeamMembership;

};  // TPlayer

FIXTURE(Typical) {
  TTeam team;
  TPlayer *p101  = new TPlayer(&team, 101);
  TPlayer *p102a = new TPlayer(&team, 102);
  TPlayer *p102b = new TPlayer(&team, 102);
  TPlayer *p103  = new TPlayer(&team, 103);
  TPlayer *p = team.GetPlayerCollection()->TryGetFirstMember();
  EXPECT_EQ(p, p101);
  p = p->GetTeamMembership()->TryGetNextMember();
  EXPECT_EQ(p, p102a);
  p = p->GetTeamMembership()->TryGetNextMember();
  EXPECT_EQ(p, p102b);
  p = p->GetTeamMembership()->TryGetNextMember();
  EXPECT_EQ(p, p103);
  p = p->GetTeamMembership()->TryGetNextMember();
  EXPECT_FALSE(p);
  p = team.GetPlayerCollection()->TryGetLastMember();
  EXPECT_EQ(p, p103);
  p = p->GetTeamMembership()->TryGetPrevMember();
  EXPECT_EQ(p, p102b);
  p = p->GetTeamMembership()->TryGetPrevMember();
  EXPECT_EQ(p, p102a);
  p = p->GetTeamMembership()->TryGetPrevMember();
  EXPECT_EQ(p, p101);
  p = p->GetTeamMembership()->TryGetPrevMember();
  EXPECT_FALSE(p);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(101), p101);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember (101), p101);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(102), p102a);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember (102), p102b);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(103), p103);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember (103), p103);
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMember(999));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetLastMember (999));
  int expected = 101;
  bool flag = false;
  for (TTeam::TPlayerCollection::TCursor csr(team.GetPlayerCollection()); csr; ++csr) {
    EXPECT_EQ(csr->GetNumber(), expected);
    if (expected == 102) {
      if (flag) {
        ++expected;
      } else {
        flag = true;
      }
    } else {
      ++expected;
    }
  }
  EXPECT_EQ(expected, 104);
  expected = 103;
  flag = false;
  for (TTeam::TPlayerCollection::TCursor csr(team.GetPlayerCollection(), InvCon::Rev); csr; ++csr) {
    EXPECT_EQ(csr->GetNumber(), expected);
    if (expected == 102) {
      if (flag) {
        --expected;
      } else {
        flag = true;
      }
    } else {
      --expected;
    }
  }
  EXPECT_EQ(expected, 100);
}

FIXTURE(ForEachUnqiueMember) {
  TTeam team;
  TPlayer *p101  = new TPlayer(&team, 101);
  TPlayer *p102a = new TPlayer(&team, 102);
  TPlayer *p102b = new TPlayer(&team, 102);
  TPlayer *p103  = new TPlayer(&team, 103);
  TPlayer *unique_players[3] = { p101, p102a, p103 };
  TPlayer **csr = unique_players, **end = csr + 3;
  team.GetPlayerCollection()->ForEachUniqueMember([p102a, p102b, &csr, end](TPlayer *p){
    if (EXPECT_TRUE(*csr == p)) {
      ++csr;
      auto next = p->GetTeamMembership()->TryGetNextMemberWithSameKey();
      if (p == p102a) {
        EXPECT_EQ(next, p102b);
      } else {
        EXPECT_FALSE(next);
      }
    }
    return true;
  });
  EXPECT_EQ(csr, end);
}
