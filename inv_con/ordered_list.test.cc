/* <inv_con/ordered_list.test.cc>

   Unit test for <inv_con/ordered_list.h>.

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

#include <inv_con/ordered_list.h>

#include <base/no_copy_semantics.h>
#include <test/kit.h>

class TTeam;
class TPlayer;

class TTeam {
  NO_COPY_SEMANTICS(TTeam);
  public:

  typedef InvCon::OrderedList::TCollection<TTeam, TPlayer, int> TPlayerCollection;

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

  void Insert(TPlayerCollection::TImpl::TTypedMembership *membership) {
    PlayerCollection.Insert(membership);
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

  typedef InvCon::OrderedList::TMembership<TPlayer, TTeam, int> TTeamMembership;

  TPlayer(TTeam *team, int number)
      : TeamMembership(this, number, team->GetPlayerCollection()) {}

  TPlayer(TTeam *team) : TeamMembership(this, 0, team->GetPlayerCollection()) {}

  TPlayer(int number) : TeamMembership(this, number) {}

  int GetNumber() const {
    assert(this);
    return TeamMembership.GetKey();
  }

  void Insert(TTeam *team) {
    assert(this);
    TeamMembership.Insert(team->GetPlayerCollection());
  }

  void ReverseInsert(TTeam *team) {
    assert(this);
    TeamMembership.ReverseInsert(team->GetPlayerCollection());
  }

  void Remove() {
    assert(this);
    TeamMembership.Remove();
  }

  void SetKey(int key) {
    assert(this);
    TeamMembership.SetKey(key);
  }

  TTeamMembership *GetTeamMembership() {
    assert(this);
    return &TeamMembership;
  }

  private:

  TTeamMembership::TImpl TeamMembership;

};  // TPlayer

FIXTURE(Typical) {
  static const size_t player_count = 5;
  static int number_array[player_count] = { 103, 101, 105, 104, 102 };
  TTeam team;
  for (size_t i = 0; i < player_count; ++i) {
    new TPlayer(&team, number_array[i]);
  }
  int expected = 101;
  for (TTeam::TPlayerCollection::TCursor csr(team.GetPlayerCollection()); csr; ++csr) {
    EXPECT_EQ(csr->GetNumber(), expected);
    ++expected;
  }
  EXPECT_EQ(expected, 106);
  expected = 105;
  for (TTeam::TPlayerCollection::TCursor csr(team.GetPlayerCollection(), InvCon::Rev); csr; ++csr) {
    EXPECT_EQ(csr->GetNumber(), expected);
    --expected;
  }
  EXPECT_EQ(expected, 100);
  for (int i = 101; i <= 105; ++i) {
    TPlayer *p = team.GetPlayerCollection()->TryGetFirstMember(i);
    EXPECT_EQ(p->GetNumber(), i);
    p = team.GetPlayerCollection()->TryGetLastMember(i);
    EXPECT_EQ(p->GetNumber(), i);
  }
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMember(106));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetLastMember (106));
}

FIXTURE(GetCollector) {
  TTeam team, team2;
  EXPECT_EQ(team.GetPlayerCollection()->GetCollector(), &team);
  EXPECT_EQ(team2.GetPlayerCollection()->GetCollector(), &team2);
  EXPECT_NE(team.GetPlayerCollection()->GetCollector(), &team2);
}

FIXTURE(CollectionImpl) {
  TTeam team;
  TPlayer *player1 = new TPlayer(&team, 101);
  TPlayer *player2 = new TPlayer(&team, 102);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player1);
  team.RemoveEachMember();
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMember());
  team.Insert(player2->GetTeamMembership());
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player2);
  team.Insert(player1->GetTeamMembership());
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player1);
  team.DeleteEachMember();
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMember());
}

FIXTURE(TryGetFirstMember) {
  TTeam team, team2;
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMember());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMember());
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMembership());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMembership());
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMember(101));
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMember(101));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMember(102));
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMember(102));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMember(103));
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMember(103));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMembership(101));
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMembership(101));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMembership(102));
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMembership(102));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMembership(103));
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMembership(103));
  TPlayer *player1 = new TPlayer(&team, 101);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player1);
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMember());
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMembership(), player1->GetTeamMembership());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMembership());
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(101), player1);
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMember(101));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMember(102));
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMember(102));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMember(103));
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMember(103));
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMembership(101), player1->GetTeamMembership());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMembership(101));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMembership(102));
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMembership(102));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMembership(103));
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMembership(103));
  TPlayer *player2 = new TPlayer(&team, 102);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player1);
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMember());
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMembership(), player1->GetTeamMembership());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMembership());
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(101), player1);
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMember(101));
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(102), player2);
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMember(102));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMember(103));
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMember(103));
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMembership(101), player1->GetTeamMembership());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMembership(101));
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMembership(102), player2->GetTeamMembership());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMembership(102));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMembership(103));
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMembership(103));
  TPlayer *player3 = new TPlayer(&team2, 103);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player1);
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetFirstMember(), player3);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMembership(), player1->GetTeamMembership());
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetFirstMembership(), player3->GetTeamMembership());
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(101), player1);
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMember(101));
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(102), player2);
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMember(102));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMember(103));
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetFirstMember(103), player3);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMembership(101), player1->GetTeamMembership());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMembership(101));
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMembership(102), player2->GetTeamMembership());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMembership(102));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMembership(103));
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetFirstMembership(103), player3->GetTeamMembership());
  TPlayer *player0 = new TPlayer(&team, 100);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player0);
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetFirstMember(), player3);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMembership(), player0->GetTeamMembership());
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetFirstMembership(), player3->GetTeamMembership());
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(101), player1);
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMember(101));
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(102), player2);
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMember(102));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMember(103));
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetFirstMember(103), player3);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMembership(101), player1->GetTeamMembership());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMembership(101));
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMembership(102), player2->GetTeamMembership());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMembership(102));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMembership(103));
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetFirstMembership(103), player3->GetTeamMembership());
  team2.Insert(player0->GetTeamMembership());
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player1);
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetFirstMember(), player0);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMembership(), player1->GetTeamMembership());
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetFirstMembership(), player0->GetTeamMembership());
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMember(100));
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetFirstMember(100), player0);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(101), player1);
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMember(101));
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(102), player2);
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMember(102));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMember(103));
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetFirstMember(103), player3);
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMembership(100));
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetFirstMembership(100), player0->GetTeamMembership());
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMembership(101), player1->GetTeamMembership());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMembership(101));
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMembership(102), player2->GetTeamMembership());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetFirstMembership(102));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMembership(103));
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetFirstMembership(103), player3->GetTeamMembership());
}

FIXTURE(TryGetLastMember) {
  TTeam team, team2;
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetLastMember());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMember());
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetLastMembership());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMembership());
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetLastMember(101));
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMember(101));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetLastMember(102));
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMember(102));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetLastMember(103));
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMember(103));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetLastMembership(101));
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMembership(101));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetLastMembership(102));
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMembership(102));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetLastMembership(103));
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMembership(103));
  TPlayer *player1 = new TPlayer(&team, 101);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player1);
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMember());
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMembership(), player1->GetTeamMembership());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMembership());
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(101), player1);
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMember(101));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetLastMember(102));
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMember(102));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetLastMember(103));
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMember(103));
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMembership(101), player1->GetTeamMembership());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMembership(101));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetLastMembership(102));
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMembership(102));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetLastMembership(103));
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMembership(103));
  TPlayer *player2 = new TPlayer(&team, 102);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player2);
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMember());
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMembership(), player2->GetTeamMembership());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMembership());
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(101), player1);
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMember(101));
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(102), player2);
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMember(102));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetLastMember(103));
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMember(103));
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMembership(101), player1->GetTeamMembership());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMembership(101));
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMembership(102), player2->GetTeamMembership());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMembership(102));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetLastMembership(103));
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMembership(103));
  TPlayer *player3 = new TPlayer(&team2, 103);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player2);
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetLastMember(), player3);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMembership(), player2->GetTeamMembership());
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetLastMembership(), player3->GetTeamMembership());
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(101), player1);
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMember(101));
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(102), player2);
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMember(102));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetLastMember(103));
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetLastMember(103), player3);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMembership(101), player1->GetTeamMembership());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMembership(101));
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMembership(102), player2->GetTeamMembership());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMembership(102));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetLastMembership(103));
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetLastMembership(103), player3->GetTeamMembership());
  new TPlayer(&team, 100);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player2);
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetLastMember(), player3);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMembership(), player2->GetTeamMembership());
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetLastMembership(), player3->GetTeamMembership());
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(101), player1);
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMember(101));
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(102), player2);
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMember(102));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetLastMember(103));
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetLastMember(103), player3);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMembership(101), player1->GetTeamMembership());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMembership(101));
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMembership(102), player2->GetTeamMembership());
  EXPECT_FALSE(team2.GetPlayerCollection()->TryGetLastMembership(102));
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetLastMembership(103));
  EXPECT_EQ(team2.GetPlayerCollection()->TryGetLastMembership(103), player3->GetTeamMembership());
}

FIXTURE(GetKey) {
  TTeam team;
  TPlayer *player1 = new TPlayer(&team, 101);
  TPlayer *player2 = new TPlayer(&team, 102);
  EXPECT_EQ(player1->GetTeamMembership()->GetKey(), 101);
  EXPECT_EQ(player2->GetTeamMembership()->GetKey(), 102);
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
}

FIXTURE(PlayerImpl) {
  TTeam team;
  TPlayer *player1 = new TPlayer(&team);
  TPlayer *player2 = new TPlayer(&team);
  TPlayer *player3 = new TPlayer(&team);
  EXPECT_TRUE(team.GetPlayerCollection()->TryGetFirstMember());
  EXPECT_TRUE(team.GetPlayerCollection()->TryGetLastMember());
  EXPECT_NE(team.GetPlayerCollection()->TryGetFirstMember(), team.GetPlayerCollection()->TryGetLastMember());
  player1->SetKey(101);
  player2->SetKey(102);
  player3->SetKey(103);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player1);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player3);
  player1->SetKey(103);
  player2->SetKey(101);
  player3->SetKey(102);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player2);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player1);
  player1->Remove();
  player2->Remove();
  player3->Remove();
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMember());
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetLastMember());
  EXPECT_FALSE(player1->GetTeamMembership()->TryGetCollection());
  EXPECT_FALSE(player2->GetTeamMembership()->TryGetCollection());
  EXPECT_FALSE(player3->GetTeamMembership()->TryGetCollection());
  player1->SetKey(101);
  player2->SetKey(102);
  player3->SetKey(103);
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetFirstMember());
  EXPECT_FALSE(team.GetPlayerCollection()->TryGetLastMember());
  player3->Insert(&team);
  player2->Insert(&team);
  player1->Insert(&team);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetFirstMember(), player1);
  EXPECT_EQ(team.GetPlayerCollection()->TryGetLastMember(), player3);
}

FIXTURE(ReverseInsert) {
  int num_iter = 1000000;
  TTeam team;
  for (int i = 0; i < num_iter; ++i) {
    TPlayer *player = new TPlayer(i);
    player->ReverseInsert(&team);
  }
  int seen = 0UL;
  for (TTeam::TPlayerCollection::TCursor csr(team.GetPlayerCollection()); csr; ++csr) {
    EXPECT_EQ(csr->GetNumber(), seen++);
  }
  EXPECT_EQ(seen, num_iter);
}