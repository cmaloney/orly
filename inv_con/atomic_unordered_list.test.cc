/* <inv_con/atomic_unordered_list.test.cc>

   Unit test for <inv_con/atomic_unordered_list.h>.

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

#include <inv_con/atomic_unordered_list.h>

#include <thread>

#include <base/no_copy_semantics.h>
#include <test/kit.h>

class TTeam;
class TPlayer;

class TTeam {
  NO_COPY_SEMANTICS(TTeam);
  public:

  typedef InvCon::AtomicUnorderedList::TCollection<TTeam, TPlayer> TPlayerCollection;

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

  void Insert(TPlayerCollection::TImpl::TTypedMembership *membership) {
    PlayerCollection.Insert(membership);
  }

  private:

  mutable TPlayerCollection::TImpl PlayerCollection;

};  // TTeam

class TPlayer {
  NO_COPY_SEMANTICS(TPlayer);
  public:

  typedef InvCon::AtomicUnorderedList::TMembership<TPlayer, TTeam> TTeamMembership;

  TPlayer(TTeam *team, int number)
      : TeamMembership(this, team->GetPlayerCollection()), Number(number) {}

  TPlayer(int number)
      : TeamMembership(this), Number(number) {}

  int GetNumber() const {
    assert(this);
    return Number;
  }

  void Insert(TTeam *team) {
    assert(this);
    TeamMembership.Insert(team->GetPlayerCollection());
  }

  void Remove() {
    assert(this);
    TeamMembership.Remove();
  }

  TTeamMembership *GetTeamMembership() {
    assert(this);
    return &TeamMembership;
  }

  private:

  TTeamMembership::TImpl TeamMembership;

  int Number;

};  // TPlayer

FIXTURE(Typical) {
  TTeam team;
  for (size_t i = 0; i < 5; ++i) {
    new TPlayer(&team, 5 - i);
  }
  std::vector<int> ret;
  std::vector<int> expected{5, 4, 3, 2, 1};
  auto cb = [&ret](const TPlayer &player) {
    ret.push_back(player.GetNumber());
    return true;
  };
  team.GetPlayerCollection()->ForEach(cb);
  EXPECT_EQ(ret.size(), expected.size());
  for (size_t i = 0; i < ret.size(); ++i) {
    EXPECT_EQ(ret[i], expected[i]);
  }
}

FIXTURE(Tough) {
  for (size_t i = 0; i < 5000L; ++i) {
    TTeam team;
    auto func = [&team](size_t num_iter) {
      std::vector<TPlayer *> vec;
      for (size_t i = 0; i < num_iter; ++i) {
        vec.push_back(new TPlayer(rand()));
      }
      size_t found = 0;
      int look_for;
      auto find_func = [&found, &look_for](const TPlayer &player) {
        if (player.GetNumber() == look_for) {
          ++found;
          return false;
        }
        return true;
      };
      for (auto iter : vec) {
        iter->Insert(&team);
      }
      for (auto iter : vec) {
        look_for = iter->GetNumber();
        team.GetPlayerCollection()->ForEach(find_func);
        delete iter;
      }
      EXPECT_EQ(found, num_iter);
    };
    std::thread t1(func, rand() % 1000);
    std::thread t2(func, rand() % 1000);
    std::thread t3(func, rand() % 1000);
    std::thread t4(func, rand() % 1000);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
  }
}
