/* <inv_con/cursor.test.cc>

   Unit test for <inv_con/cursor.h>.

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

#include <inv_con/cursor.h>

#include <base/no_copy_semantics.h>
#include <inv_con/ordered_list.h>
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

  int GetNumber() const {
    assert(this);
    return TeamMembership.GetKey();
  }

  void Insert(TTeam *team) {
    assert(this);
    TeamMembership.Insert(team->GetPlayerCollection());
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
  TTeam team, team2;
  TPlayer *player1 = new TPlayer(&team, 101);
  TPlayer *player2 = new TPlayer(&team, 102);
  TPlayer *player3 = new TPlayer(&team, 103);
  TPlayer *player4 = new TPlayer(&team2, 101);
  TPlayer *player5 = new TPlayer(&team2, 102);
  TPlayer *player6 = new TPlayer(&team2, 103);
  { // forward team 1
    std::vector<TPlayer *> vec({player1, player2, player3});
    auto iter = vec.begin();
    TTeam::TPlayerCollection::TCursor cursor(team.GetPlayerCollection());
    for (; cursor && iter != vec.end(); ++cursor, ++iter) {
      EXPECT_EQ(*iter, &(*cursor));
    }
    auto iter2 = vec.rbegin();
    for (TTeam::TPlayerCollection::TCursor cursor2(player3->GetTeamMembership()); cursor2 && iter2 != vec.rend(); --cursor2, ++iter2) {
      EXPECT_EQ(*iter2, &(*cursor2));
    }
  }
  { // reverse team 1
    std::vector<TPlayer *> vec({player3, player2, player1});
    auto iter = vec.begin();
    for (TTeam::TPlayerCollection::TCursor cursor(team.GetPlayerCollection(), InvCon::Rev); cursor && iter != vec.end(); ++cursor, ++iter) {
      EXPECT_EQ(*iter, &(*cursor));
    }
    auto iter2 = vec.rbegin();
    for (TTeam::TPlayerCollection::TCursor cursor2(player1->GetTeamMembership(), InvCon::Rev); cursor2 && iter2 != vec.rend(); --cursor2, ++iter2) {
      EXPECT_EQ(*iter2, &(*cursor2));
    }
  }
  { // forward team 2
    std::vector<TPlayer *> vec({player4, player5, player6});
    auto iter = vec.begin();
    for (TTeam::TPlayerCollection::TCursor cursor(team2.GetPlayerCollection()); cursor && iter != vec.end(); ++cursor, ++iter) {
      EXPECT_EQ(*iter, &(*cursor));
    }
  }
  { // reverse team 2
    std::vector<TPlayer *> vec({player6, player5, player4});
    auto iter = vec.begin();
    for (TTeam::TPlayerCollection::TCursor cursor(team2.GetPlayerCollection(), InvCon::Rev); cursor && iter != vec.end(); ++cursor, ++iter) {
      EXPECT_EQ(*iter, &(*cursor));
    }
  }
  { // forward team 1 & 2
    TTeam::TPlayerCollection::TCursor cursor1(team.GetPlayerCollection());
    for (TTeam::TPlayerCollection::TCursor cursor2(team2.GetPlayerCollection()); cursor1 && cursor2; ++cursor1, ++cursor2) {
      EXPECT_EQ(cursor1->GetNumber(), cursor2->GetNumber());
    }
  }
  { // reverse team 1 & 2
    TTeam::TPlayerCollection::TCursor cursor1(team.GetPlayerCollection(), InvCon::Rev);
    for (TTeam::TPlayerCollection::TCursor cursor2(team2.GetPlayerCollection(), InvCon::Rev); cursor1 && cursor2; ++cursor1, ++cursor2) {
      EXPECT_EQ(cursor1->GetNumber(), cursor2->GetNumber());
    }
  }
}