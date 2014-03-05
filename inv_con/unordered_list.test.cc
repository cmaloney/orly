/* <inv_con/unordered_list.test.cc>

   Unit test for <inv_con/unordered_list.h>.

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

#include <inv_con/unordered_list.h>

#include <base/no_copy_semantics.h>
#include <test/kit.h>

class TTeam;
class TPlayer;

class TTeam {
  NO_COPY_SEMANTICS(TTeam);
  public:

  typedef InvCon::UnorderedList::TCollection<TTeam, TPlayer> TPlayerCollection;

  TTeam()
      : PlayerCollection(this) {}

  ~TTeam() {
    assert(this);
    PlayerCollection.DeleteEachMember();
  }

  void DeleteEachMember() {
    assert(this);
    PlayerCollection.DeleteEachMember();
  }

  TPlayerCollection *GetPlayerCollection() const {
    assert(this);
    return &PlayerCollection;
  }

  void Insert(TPlayerCollection::TImpl::TTypedMembership *membership, InvCon::TOrient orient = InvCon::Fwd) {
    PlayerCollection.Insert(membership, orient);
  }

  void RemoveEachMember() {
    PlayerCollection.RemoveEachMember();
  }

  private:

  mutable TPlayerCollection::TImpl PlayerCollection;

};  // TTeam

class TPlayer {
  NO_COPY_SEMANTICS(TPlayer);
  public:

  typedef InvCon::UnorderedList::TMembership<TPlayer, TTeam> TTeamMembership;

  TPlayer(int number)
      : Number(number), TeamMembership(this) {}

  TPlayer(TTeam *team, int number, InvCon::TOrient orient = InvCon::Fwd)
      : Number(number), TeamMembership(this, team->GetPlayerCollection(), orient) {}

  TPlayer(TPlayer *sibling, int number, InvCon::TOrient orient = InvCon::Fwd)
      : Number(number), TeamMembership(this, sibling->GetTeamMembership(), orient) {}

  int GetNumber() const {
    assert(this);
    return Number;
  }

  TTeamMembership *GetTeamMembership() {
    assert(this);
    return &TeamMembership;
  }

  void Insert(TTeam *team, InvCon::TOrient orient = InvCon::Fwd) {
    assert(this);
    TeamMembership.Insert(team->GetPlayerCollection(), orient);
  }

  void Insert(TPlayer *sibling, InvCon::TOrient orient = InvCon::Fwd) {
    assert(this);
    TeamMembership.Insert(sibling->GetTeamMembership(), orient);
  }

  void Remove() {
    assert(this);
    TeamMembership.Remove();
  }

  private:

  int Number;

  TTeamMembership::TImpl TeamMembership;

};  // TPlayer

FIXTURE(Typical) {
  TTeam team;
  new TPlayer(&team, 101);
  new TPlayer(&team, 102);
  new TPlayer(&team, 103);
  int expected = 101;
  for (TTeam::TPlayerCollection::TCursor csr(team.GetPlayerCollection()); csr; ++csr) {
    EXPECT_EQ(csr->GetNumber(), expected);
    ++expected;
  }
  EXPECT_EQ(expected, 104);
  expected = 103;
  for (TTeam::TPlayerCollection::TCursor csr(team.GetPlayerCollection(), InvCon::Rev); csr; ++csr) {
    EXPECT_EQ(csr->GetNumber(), expected);
    --expected;
  }
  EXPECT_EQ(expected, 100);
}

FIXTURE(GetCollector) {
  TTeam team, team2;
  EXPECT_EQ(team.GetPlayerCollection()->GetCollector(), &team);
  EXPECT_EQ(team2.GetPlayerCollection()->GetCollector(), &team2);
  EXPECT_NE(team.GetPlayerCollection()->GetCollector(), &team2);
}

FIXTURE(CollectionImplFwd) {
  TTeam team;
  TPlayer *player1 = new TPlayer(&team, 101);
  TPlayer *player2 = new TPlayer(&team, 102);
  TPlayer *player3 = new TPlayer(&team, 103);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player1);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player3);
  team.RemoveEachMember();
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMember());
  team.Insert(player3->GetTeamMembership());
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player3);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player3);
  team.Insert(player2->GetTeamMembership());
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player3);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player2);
  team.Insert(player1->GetTeamMembership());
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player3);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player1);
  team.DeleteEachMember();
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMember());
}

FIXTURE(CollectionImplRev) {
  TTeam team;
  TPlayer *player1 = new TPlayer(&team, 101, InvCon::Rev);
  TPlayer *player2 = new TPlayer(&team, 102, InvCon::Rev);
  TPlayer *player3 = new TPlayer(&team, 103, InvCon::Rev);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player3);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player1);
  team.RemoveEachMember();
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMember());
  team.Insert(player3->GetTeamMembership(), InvCon::Rev);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player3);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player3);
  team.Insert(player2->GetTeamMembership(), InvCon::Rev);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player2);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player3);
  team.Insert(player1->GetTeamMembership(), InvCon::Rev);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player1);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player3);
  team.DeleteEachMember();
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMember());
}

FIXTURE(TryGetFirstMember) {
  TTeam team, team2;
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMember());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMember());
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMembership());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMembership());
  TPlayer *player1 = new TPlayer(&team, 101);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player1);
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMember());
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMembership(), player1->GetTeamMembership());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMembership());
  TPlayer *player2 = new TPlayer(&team, 102);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player1);
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMember());
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMembership(), player1->GetTeamMembership());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMembership());
  TPlayer *player3 = new TPlayer(&team2, 103);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player1);
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetFirstMember(), player3);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMembership(), player1->GetTeamMembership());
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetFirstMembership(), player3->GetTeamMembership());
  TPlayer *player0 = new TPlayer(&team, 100);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player1);
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetFirstMember(), player3);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMembership(), player1->GetTeamMembership());
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetFirstMembership(), player3->GetTeamMembership());
  team2.Insert(player0->GetTeamMembership());
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player1);
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetFirstMember(), player3);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMembership(), player1->GetTeamMembership());
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetFirstMembership(), player3->GetTeamMembership());
  team2.Insert(player1->GetTeamMembership(), InvCon::Rev);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player2);
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetFirstMember(), player1);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMembership(), player2->GetTeamMembership());
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetFirstMembership(), player1->GetTeamMembership());
}

FIXTURE(TryGetLastMember) {
  TTeam team, team2;
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetLastMember());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMember());
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetLastMembership());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMembership());
  TPlayer *player1 = new TPlayer(&team, 101);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player1);
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMember());
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMembership(), player1->GetTeamMembership());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMembership());
  TPlayer *player2 = new TPlayer(&team, 102);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player2);
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMember());
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMembership(), player2->GetTeamMembership());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMembership());
  TPlayer *player3 = new TPlayer(&team2, 103);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player2);
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetLastMember(), player3);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMembership(), player2->GetTeamMembership());
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetLastMembership(), player3->GetTeamMembership());
  TPlayer *player0 = new TPlayer(&team, 100);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player0);
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetLastMember(), player3);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMembership(), player0->GetTeamMembership());
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetLastMembership(), player3->GetTeamMembership());
  team2.Insert(player0->GetTeamMembership());
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player2);
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetLastMember(), player0);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMembership(), player2->GetTeamMembership());
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetLastMembership(), player0->GetTeamMembership());
  team2.Insert(player1->GetTeamMembership(), InvCon::Rev);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player2);
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetLastMember(), player0);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMembership(), player2->GetTeamMembership());
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetLastMembership(), player0->GetTeamMembership());
}

FIXTURE(GetMember) {
  TTeam team;
  TPlayer *player1 = new TPlayer(&team, 101);
  TPlayer *player2 = new TPlayer(&team, 102);
  EXPECT_EQ(player1->GetTeamMembership()->GetMember(), player1);
  EXPECT_EQ(player2->GetTeamMembership()->GetMember(), player2);
}

FIXTURE(TryGetCollect) {
  TTeam team, team2;
  TPlayer *player1 = new TPlayer(&team, 101);
  TPlayer *player2 = new TPlayer(&team, 102);
  TPlayer *player3 = new TPlayer(&team2, 103);
  EXPECT_EQ(player1->GetTeamMembership()->TryGetCollection(), team.GetPlayerCollection());
  EXPECT_EQ(player2->GetTeamMembership()->TryGetCollection(), team.GetPlayerCollection());
  EXPECT_EQ(player3->GetTeamMembership()->TryGetCollection(), team2.GetPlayerCollection());
  EXPECT_EQ(player1->GetTeamMembership()->TryGetCollector(), &team);
  EXPECT_EQ(player2->GetTeamMembership()->TryGetCollector(), &team);
  EXPECT_EQ(player3->GetTeamMembership()->TryGetCollector(), &team2);
}

FIXTURE(TryGetMember) {
  TTeam team, team2;
  TPlayer *player1 = new TPlayer(&team, 101);
  TPlayer *player2 = new TPlayer(&team, 102);
  TPlayer *player3 = new TPlayer(&team2, 103);
  EXPECT_FALSE(player1->GetTeamMembership()->TryGetPrevMember());
  EXPECT_EQ(player2->GetTeamMembership()->TryGetPrevMember(), player1);
  EXPECT_FALSE(player3->GetTeamMembership()->TryGetPrevMember());
  EXPECT_FALSE(player1->GetTeamMembership()->TryGetPrevMembership());
  EXPECT_EQ(player2->GetTeamMembership()->TryGetPrevMembership(), player1->GetTeamMembership());
  EXPECT_FALSE(player3->GetTeamMembership()->TryGetPrevMembership());
  EXPECT_EQ(player1->GetTeamMembership()->TryGetNextMember(), player2);
  EXPECT_FALSE(player2->GetTeamMembership()->TryGetNextMember());
  EXPECT_FALSE(player3->GetTeamMembership()->TryGetNextMember());
  EXPECT_EQ(player1->GetTeamMembership()->TryGetNextMembership(), player2->GetTeamMembership());
  EXPECT_FALSE(player2->GetTeamMembership()->TryGetNextMembership());
  EXPECT_FALSE(player3->GetTeamMembership()->TryGetNextMembership());
  team.RemoveEachMember();
  EXPECT_FALSE(player1->GetTeamMembership()->TryGetPrevMember());
  EXPECT_FALSE(player2->GetTeamMembership()->TryGetPrevMember());
  EXPECT_FALSE(player3->GetTeamMembership()->TryGetPrevMember());
  EXPECT_FALSE(player1->GetTeamMembership()->TryGetPrevMembership());
  EXPECT_FALSE(player2->GetTeamMembership()->TryGetPrevMembership());
  EXPECT_FALSE(player3->GetTeamMembership()->TryGetPrevMembership());
  EXPECT_FALSE(player1->GetTeamMembership()->TryGetNextMember());
  EXPECT_FALSE(player2->GetTeamMembership()->TryGetNextMember());
  EXPECT_FALSE(player3->GetTeamMembership()->TryGetNextMember());
  EXPECT_FALSE(player1->GetTeamMembership()->TryGetNextMembership());
  EXPECT_FALSE(player2->GetTeamMembership()->TryGetNextMembership());
  EXPECT_FALSE(player3->GetTeamMembership()->TryGetNextMembership());
  team.Insert(player1->GetTeamMembership(), InvCon::Rev);
  team.Insert(player2->GetTeamMembership(), InvCon::Rev);
  team.Insert(player3->GetTeamMembership(), InvCon::Rev);
  EXPECT_EQ(player1->GetTeamMembership()->TryGetPrevMember(), player2);
  EXPECT_EQ(player2->GetTeamMembership()->TryGetPrevMember(), player3);
  EXPECT_FALSE(player3->GetTeamMembership()->TryGetPrevMember());
  EXPECT_EQ(player1->GetTeamMembership()->TryGetPrevMembership(), player2->GetTeamMembership());
  EXPECT_EQ(player2->GetTeamMembership()->TryGetPrevMembership(), player3->GetTeamMembership());
  EXPECT_FALSE(player3->GetTeamMembership()->TryGetPrevMembership());
  EXPECT_FALSE(player1->GetTeamMembership()->TryGetNextMember());
  EXPECT_EQ(player2->GetTeamMembership()->TryGetNextMember(), player1);
  EXPECT_EQ(player3->GetTeamMembership()->TryGetNextMember(), player2);
  EXPECT_FALSE(player1->GetTeamMembership()->TryGetNextMembership());
  EXPECT_EQ(player2->GetTeamMembership()->TryGetNextMembership(), player1->GetTeamMembership());
  EXPECT_EQ(player3->GetTeamMembership()->TryGetNextMembership(), player2->GetTeamMembership());
  TPlayer *player4 = new TPlayer(player3, 104);
  TPlayer *player5 = new TPlayer(player4, 105, InvCon::Rev);
  EXPECT_EQ(player1->GetTeamMembership()->TryGetPrevMember(), player2);
  EXPECT_EQ(player2->GetTeamMembership()->TryGetPrevMember(), player4);
  EXPECT_EQ(player4->GetTeamMembership()->TryGetPrevMember(), player5);
  EXPECT_EQ(player5->GetTeamMembership()->TryGetPrevMember(), player3);
  EXPECT_FALSE(player3->GetTeamMembership()->TryGetPrevMember());
  EXPECT_EQ(player1->GetTeamMembership()->TryGetPrevMembership(), player2->GetTeamMembership());
  EXPECT_EQ(player2->GetTeamMembership()->TryGetPrevMembership(), player4->GetTeamMembership());
  EXPECT_EQ(player4->GetTeamMembership()->TryGetPrevMembership(), player5->GetTeamMembership());
  EXPECT_EQ(player5->GetTeamMembership()->TryGetPrevMembership(), player3->GetTeamMembership());
  EXPECT_FALSE(player3->GetTeamMembership()->TryGetPrevMembership());
  EXPECT_FALSE(player1->GetTeamMembership()->TryGetNextMember());
  EXPECT_EQ(player2->GetTeamMembership()->TryGetNextMember(), player1);
  EXPECT_EQ(player4->GetTeamMembership()->TryGetNextMember(), player2);
  EXPECT_EQ(player5->GetTeamMembership()->TryGetNextMember(), player4);
  EXPECT_EQ(player3->GetTeamMembership()->TryGetNextMember(), player5);
  EXPECT_FALSE(player1->GetTeamMembership()->TryGetNextMembership());
  EXPECT_EQ(player2->GetTeamMembership()->TryGetNextMembership(), player1->GetTeamMembership());
  EXPECT_EQ(player4->GetTeamMembership()->TryGetNextMembership(), player2->GetTeamMembership());
  EXPECT_EQ(player5->GetTeamMembership()->TryGetNextMembership(), player4->GetTeamMembership());
  EXPECT_EQ(player3->GetTeamMembership()->TryGetNextMembership(), player5->GetTeamMembership());
}

FIXTURE(PlayerImpl) {
  TTeam team;
  TPlayer *player1 = new TPlayer(101);
  TPlayer *player2 = new TPlayer(102);
  TPlayer *player3 = new TPlayer(103);
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMember());
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetLastMember());
  player1->Insert(&team);
  player2->Insert(&team);
  player3->Insert(&team);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player1);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player3);
  player3->Insert(player1);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player1);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player2);
  player3->Insert(player1, InvCon::Rev);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player3);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player2);
  player2->Insert(player3, InvCon::Rev);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player2);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player1);
  player2->Insert(player3, InvCon::Rev);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player2);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player1);
  player1->Insert(player3);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player2);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player1);
  player3->Insert(player1, InvCon::Rev);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player2);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player1);
  player3->Insert(player2);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player2);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player1);
  player3->Remove();
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player2);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player1);
  player2->Remove();
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player1);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player1);
  player2->Insert(&team, InvCon::Rev);
  player3->Insert(&team, InvCon::Rev);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player3);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player1);
}