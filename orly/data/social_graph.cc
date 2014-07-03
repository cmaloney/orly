/* <orly/data/social_graph.cc>

   Generates a core-vector file with a large sample social graph

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

#include <iomanip>
#include <fcntl.h>
#include <unistd.h>

#include <base/chrono.h>
#include <base/log.h>
#include <base/tmp_file.h>
#include <gz/output_consumer.h>
#include <io/binary_input_only_stream.h>
#include <io/binary_output_only_stream.h>
#include <io/device.h>
#include <orly/atom/core_vector_builder.h>
#include <orly/atom/suprena.h>
#include <orly/desc.h>

using namespace std;
using namespace Base;
using namespace Base::Chrono;
using namespace Orly;

static int64_t NumRounds;
static int64_t NumUsers;
static int64_t NumPlaces;
static int64_t Phase1ViewPct;
static int64_t Phase2FriendPct;
static int64_t Phase2MovePct;
static int64_t Phase3FollowPct;
static int64_t MaxTrans;
static const char *TmpFileTemplate = "social_graph_XXXXXX.tmp";

/* friendship:
  Apply bi-directionaly. is_initiator should be flipped
  <[friendship_ID (enum int), from_uid (int), to_uid (int)]> <- since (time_pnt)
  <[friendship_ID (enum int), to_uid (int), from_uid (int)]> <- since (time_pnt)
*/

/* lived in:
  <[lived_in_ID (enum int), uid (int), since (desc<time_pnt>), place_id (int)]> <- unused (bool)
  <[population_ID (enum int), place_id (int), since (time_point), uid (int)]> <- unused (bool)
*/

/* played game against:
  <[played_game_ID (enum int), from_uid (int), to_uid (int), game_id (int), date (desc<time_pnt>)]> <- unused (bool)
  <[games_ID (enum int), game_id (int), date (time_pnt)]> <- <[from_uid (int), to_uid (int)]>
*/

/* comment:
   <[commented_to_ID (enum_int), from_uid (int), to_uid (int), date (time_pnt)]> <- comment (str)
   <[commented_time_ID (enum_int), from_uid (int), date (Desc<time_pnt>), to_uid (int)]> <- comment (str)
   <[received_comment_from_ID (enum_int), to_uid (int), from_uid (int), date (time_pnt)]> <- comment (str)
   <[received_comment_time_ID (enum_int), to_uid (int), date (Desc<time_pnt>), from_uid (int)]> <- comment (str)
*/

/* viewed:
   <[viewed_to_ID (enum_int), from_uid (int), to_uid (int), date (time_pnt)]> <- unused (bool)
   <[viewed_time_ID (enum_int), from_uid (int), date (Desc<time_pnt>), to_uid (int)]> <- unused (bool)
   <[received_view_from_ID (enum_int), to_uid (int), from_uid (int), date (time_pnt)]> <- unused (bool)
   <[received_view_time_ID (enum_int), to_uid (int), date (Desc<time_pnt>), from_uid (int)]> <- unused (bool)
*/

static const TUuid FriendshipIndexId{TUuid::Twister};
static const TUuid LivedInIndexId{TUuid::Twister};
static const TUuid PopulationIndexId{TUuid::Twister};
static const TUuid PlayedAgainstIndexId{TUuid::Twister};
static const TUuid GamesIndexId{TUuid::Twister};
static const TUuid CommentToIndexId{TUuid::Twister};
static const TUuid CommentTimeIndexId{TUuid::Twister};
static const TUuid ReceivedCommentFromIndexId = CommentToIndexId;
static const TUuid ReceivedCommentTimeIndexId = CommentTimeIndexId;
static const TUuid ViewedToIndexId{TUuid::Twister};
static const TUuid ViewedTimeIndexId{TUuid::Twister};
static const TUuid ReceivedViewFromIndexId = ViewedToIndexId;
static const TUuid ReceivedViewTimeIndexId = ViewedTimeIndexId;

static const int64_t FriendshipId = 3;
static const int64_t LivedInId = 5;
static const int64_t PopulationId = 7;
static const int64_t PlayedGameId = 11;
static const int64_t GamesId = 13;
static const int64_t CommentToId = 17;
static const int64_t CommentTimeId = 19;
static const int64_t ReceivedCommentFromId = 23;
static const int64_t ReceivedCommentTimeId = 29;
static const int64_t ViewedToId = 31;
static const int64_t ViewedTimeId = 37;
static const int64_t ReceivedViewFromId = 41;
static const int64_t ReceivedViewTimeId = 43;

static Base::Chrono::TTimePnt LastTime;
inline const Base::Chrono::TTimePnt &NextTime() {
  LastTime += std::chrono::seconds(1);
  return LastTime;
}

static std::unique_ptr<Atom::TCoreVectorBuilder> Builder;
static int64_t TransCounter;
static int64_t FileNum = 1L;
void InitBuilder() {
  TransCounter = 0L;
  Builder = make_unique<Atom::TCoreVectorBuilder>();
  Builder->Push(0UL);  // dummy num transactions
  Builder->Push(0UL);  // dummy meta data
}

void Check() {
  assert(TransCounter > 0);
  std::stringstream ss;
  ss << "social_graph_" << FileNum++ << ".bin.gz";
  const std::string fname = ss.str();
  Io::TBinaryOutputOnlyStream strm(std::make_shared<Gz::TOutputConsumer>(fname.c_str(), "w"));
  Atom::TCore &tc_core = const_cast<Atom::TCore &>(Builder->GetCores().front());
  Atom::TSuprena suprena;
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  tc_core = Atom::TCore(TransCounter, &suprena, state_alloc);
  Builder->Write(strm);
  InitBuilder();
}

void MakeFriendship(int64_t from_id, int64_t to_id, const Base::Chrono::TTimePnt &since) {
  assert(Builder);
  Builder->Push(TUuid(TUuid::Twister));
  Builder->Push(1L);
  Builder->Push(2L);
  Builder->Push(FriendshipIndexId);
  Builder->Push(make_tuple(FriendshipId, from_id, to_id));
  Builder->Push(since);
  Builder->Push(FriendshipIndexId);
  Builder->Push(make_tuple(FriendshipId, from_id, to_id));
  Builder->Push(since);
  ++TransCounter;
  if (TransCounter >= MaxTrans) {
    Check();
  }
}

void LivedIn(int64_t uid, int64_t place_id, const Base::Chrono::TTimePnt &since) {
  assert(Builder);
  Builder->Push(TUuid(TUuid::Twister));
  Builder->Push(1L);
  Builder->Push(2L);
  Builder->Push(LivedInIndexId);
  Builder->Push(make_tuple(LivedInId, uid, Orly::TDesc<Base::Chrono::TTimePnt>(since), place_id));
  Builder->Push(true);
  Builder->Push(PopulationIndexId);
  Builder->Push(make_tuple(PopulationId, place_id, since, uid));
  Builder->Push(true);
  ++TransCounter;
  if (TransCounter >= MaxTrans) {
    Check();
  }
}

void PlayedGameAgainst(int64_t from_uid, int64_t to_uid, int64_t game_id, const Base::Chrono::TTimePnt &date) {
  assert(Builder);
  Builder->Push(TUuid(TUuid::Twister));
  Builder->Push(1L);
  Builder->Push(2L);
  Builder->Push(PlayedAgainstIndexId);
  Builder->Push(make_tuple(PlayedGameId, from_uid, to_uid, game_id, Orly::TDesc<Base::Chrono::TTimePnt>(date)));
  Builder->Push(true);
  Builder->Push(GamesIndexId);
  Builder->Push(make_tuple(GamesId, game_id, date));
  Builder->Push(make_tuple(from_uid, to_uid));
  ++TransCounter;
  if (TransCounter >= MaxTrans) {
    Check();
  }
}

void CommentedOn(int64_t from_uid, int64_t to_uid, const Base::Chrono::TTimePnt &date, const std::string &comment) {
  assert(Builder);
  Builder->Push(TUuid(TUuid::Twister));
  Builder->Push(1L);
  Builder->Push(4L);
  Builder->Push(CommentToIndexId);
  Builder->Push(make_tuple(CommentToId, from_uid, to_uid, date));
  Builder->Push(comment);
  Builder->Push(CommentTimeIndexId);
  Builder->Push(make_tuple(CommentTimeId, from_uid, Orly::TDesc<Base::Chrono::TTimePnt>(date), to_uid));
  Builder->Push(ReceivedCommentFromIndexId);
  Builder->Push(make_tuple(ReceivedCommentFromId, to_uid, from_uid, date));
  Builder->Push(comment);
  Builder->Push(ReceivedCommentTimeIndexId);
  Builder->Push(make_tuple(ReceivedCommentTimeId, to_uid, Orly::TDesc<Base::Chrono::TTimePnt>(date), from_uid));
  Builder->Push(comment);
  ++TransCounter;
  if (TransCounter >= MaxTrans) {
    Check();
  }
}

void ViewedBy(int64_t from_uid, int64_t to_uid, const Base::Chrono::TTimePnt &date) {
  assert(Builder);
  Builder->Push(TUuid(TUuid::Twister));
  Builder->Push(1L);
  Builder->Push(4L);
  Builder->Push(ViewedToIndexId);
  Builder->Push(make_tuple(ViewedToId, from_uid, to_uid, date));
  Builder->Push(true);
  Builder->Push(ViewedTimeIndexId);
  Builder->Push(make_tuple(ViewedTimeId, from_uid, Orly::TDesc<Base::Chrono::TTimePnt>(date), to_uid));
  Builder->Push(true);
  Builder->Push(ReceivedViewFromIndexId);
  Builder->Push(make_tuple(ReceivedViewFromId, to_uid, from_uid, date));
  Builder->Push(true);
  Builder->Push(ReceivedViewTimeIndexId);
  Builder->Push(make_tuple(ReceivedViewTimeId, to_uid, Orly::TDesc<Base::Chrono::TTimePnt>(date), from_uid));
  Builder->Push(true);
  ++TransCounter;
  if (TransCounter >= MaxTrans) {
    Check();
  }
}

/* people start looking around */
int64_t Phase1(Io::TBinaryOutputOnlyStream &view_strm, size_t &num_viewed, Io::TBinaryOutputOnlyStream &lived_strm, size_t &num_lived, int64_t seed) {
  assert(Builder);
  std::mt19937 engine(seed);
  int mean = NumPlaces / 2;
  std::normal_distribution<> place_dist(mean, mean / 3);
  for (int64_t i = 0; i < NumUsers; ++i) {
    // everyone should get assigned a place to live
    int64_t place = -1;
    for (; place < 0 || place >= NumPlaces;) {
      place = std::round(place_dist(engine));
    }
    LivedIn(i, place, NextTime());
    ++num_lived;
    lived_strm << i << place;
    if (static_cast<int64_t>(engine() % 100L) < Phase1ViewPct) {
      const int64_t to_user = engine() % NumUsers;
      ViewedBy(i, to_user, NextTime());
      view_strm << i << to_user;
      ++num_viewed;
    }
  }
  return engine();
}

struct TFriendship {
  int64_t from_id;
  int64_t to_id;
  TFriendship(int64_t from, int64_t to) : from_id(from), to_id(to) {}
  inline size_t GetHash() const {
    return from_id ^ to_id;
  }
  inline bool operator==(const TFriendship &that) const {
    return from_id == that.from_id && to_id == that.to_id;
  }
};

namespace std {

  /* A standard hasher for TFriendship. */
  template <>
  struct hash<TFriendship> {
    typedef size_t result_type;
    typedef TFriendship argument_type;
    size_t operator()(const TFriendship &that) const {
      return that.GetHash();
    }
  };

}

/* some people who received a view will be the requestor of a new friendship, some of those will move to the city that their new friend is from */
int64_t Phase2(const TTmpFile &view_file, const size_t num_viewed,
               const TTmpFile &lived_file,
               size_t num_lived_in,
               const TTmpFile &friend_file, size_t &num_friended,
               Io::TBinaryOutputOnlyStream &lived_out_strm, size_t &num_lived_out, int64_t seed) {
  assert(Builder);
  const Base::TFd &fd = view_file.GetFd();
  lseek(fd, 0, SEEK_SET);
  Io::TBinaryInputOnlyStream in_strm(make_shared<Io::TDevice>(fd));
  std::mt19937 engine(seed);
  int64_t from_id, to_id;
  std::unordered_multimap<int64_t, int64_t> search_map;
  std::unordered_set<TFriendship> suggested_friendship;
  for (size_t i = 0; i < num_viewed; ++i) {
    in_strm.Read(from_id);
    in_strm.Read(to_id);
    if (static_cast<int64_t>(engine() % 100L) < Phase2FriendPct) {
      if (to_id < from_id) {
        suggested_friendship.emplace(to_id, from_id);
      } else {
        suggested_friendship.emplace(from_id, to_id);
      }
      if (static_cast<int64_t>(engine() % 100L) < Phase2MovePct) {
        // create a search object for X is friend of Y such that "when we find out where Y lives, then X moves to Y"
        search_map.emplace(from_id, to_id);
      }
    }
  }
  /* scan over the friendships and if they exist in the suggest_friendship set, then remove them. We don't want to make a friendship a second time */ {
    const Base::TFd &fd = friend_file.GetFd();
    lseek(fd, 0, SEEK_SET);
    Io::TBinaryInputOnlyStream in_strm(make_shared<Io::TDevice>(fd));
    for (size_t i = 0; i < num_friended; ++i) {
      in_strm.Read(from_id);
      in_strm.Read(to_id);
      auto pos = suggested_friendship.find(from_id < to_id ? TFriendship(from_id, to_id) : TFriendship(to_id, from_id));
      if (pos != suggested_friendship.end()) {
        suggested_friendship.erase(pos);
      }
    }
  }
  /* go over the suggested friendships and make them / write them out */ {
    const Base::TFd &friend_fd = friend_file.GetFd();
    lseek(friend_fd, 0, SEEK_END);
    Io::TBinaryOutputOnlyStream friend_out_strm(make_shared<Io::TDevice>(friend_fd));
    for (const auto &f : suggested_friendship) {
      MakeFriendship(f.from_id, f.to_id, NextTime());
      friend_out_strm << f.from_id << f.to_id;
      ++num_friended;
    }
  }
  /* scan over the lived places and move X to Y's city if Y exists in the search map */ {
    const Base::TFd &fd = lived_file.GetFd();
    lseek(fd, 0, SEEK_SET);
    Io::TBinaryInputOnlyStream in_strm(make_shared<Io::TDevice>(fd));
    int64_t uid, place_id;
    for (size_t i = 0; i < num_lived_in; ++i) {
      in_strm.Read(uid);
      in_strm.Read(place_id);
      auto range = search_map.equal_range(uid);
      for_each(range.first, range.second, [&](const std::pair<int64_t, int64_t> &so) {
        const int64_t x = so.second;
        LivedIn(x, place_id, NextTime());
        lived_out_strm << x << to_id;
        ++num_lived_out;
      });
    }
  }
  return engine();
}

/* for some friendships that were made from X to Y, and Y to Z, let X view Z */
int64_t Phase3(const TTmpFile &friend_file, const size_t num_friended, Io::TBinaryOutputOnlyStream &view_out_strm, size_t &num_viewed, int64_t seed) {
  assert(Builder);
  std::mt19937 engine(seed);
  int64_t from_id, to_id;
  std::unordered_multimap<int64_t, int64_t> search_map;
  /* scan over the friendships (X is friend of Y) and create search objects for X such that "if Y is friends of Z where Z != X, then view from X to Z" */ {
    const Base::TFd &fd = friend_file.GetFd();
    lseek(fd, 0, SEEK_SET);
    Io::TBinaryInputOnlyStream in_strm(make_shared<Io::TDevice>(fd));
    for (size_t i = 0; i < num_friended; ++i) {
      in_strm.Read(from_id);
      in_strm.Read(to_id);
      if (static_cast<int64_t>(engine() % 100L) < Phase3FollowPct) {
        search_map.emplace(to_id, from_id);
      }
    }
  }
  /* scan over the friendships again (X is friend of Y) and try to match X with the keys in the search map. If it matches, have the value in the search map view Y of the friendship. */ {
    const Base::TFd &fd = friend_file.GetFd();
    lseek(fd, 0, SEEK_SET);
    Io::TBinaryInputOnlyStream in_strm(make_shared<Io::TDevice>(fd));
    for (size_t i = 0; i < num_friended; ++i) {
      in_strm.Read(from_id);
      in_strm.Read(to_id);
      auto range = search_map.equal_range(from_id);
      for_each(range.first, range.second, [&](const std::pair<int64_t, int64_t> &so) {
        const int64_t x = so.second;
        ViewedBy(x, to_id, NextTime());
        view_out_strm << x << to_id;
        ++num_viewed;
      });
    }
  }

  return engine();
}

/* Command-line arguments. */
class TCmd final
    : public Base::TLog::TCmd {
  public:

  /* Construct with defaults. */
  TCmd()
      : MaxTrans(250000),
        NumRounds(20),
        NumUsers(10000),
        NumPlaces(26),
        ViewPct(75),
        FriendPct(33),
        MovePct(1),
        FollowPct(30) {}

  /* Construct from argc/argv. */
  TCmd(int argc, char *argv[])
      : TCmd() {
    Parse(argc, argv, TMeta());
  }

  /* The userbase size */
  int64_t MaxTrans;
  int64_t NumRounds;
  int64_t NumUsers;
  int64_t NumPlaces;
  int64_t ViewPct;
  int64_t FriendPct;
  int64_t MovePct;
  int64_t FollowPct;

  private:

  /* Our meta-type. */
  class TMeta final
      : public Base::TLog::TCmd::TMeta {
    public:

    /* Registers our fields. */
    TMeta()
        : Base::TLog::TCmd::TMeta("Orly social graph synthetic data generator") {
      Param(
          &TCmd::MaxTrans, "max-trans-per-file", Optional, "max-trans-per-file\0mtpf\0",
          "The maximum number of transactions per output file. This is dependent on how many slots you have in the Orly server."
      );
      Param(
          &TCmd::NumRounds, "num-rounds", Optional, "num-rounds\0nr\0",
          "The number of rounds of friendship -> follow happen."
      );
      Param(
          &TCmd::NumUsers, "num-users", Optional, "num-users\0nu\0",
          "The number of users that will be part of this data-set."
      );
      Param(
          &TCmd::NumPlaces, "num-places", Optional, "num-places\0np\0",
          "The number of places (like cities) that will be part of this data-set."
      );
      Param(
          &TCmd::ViewPct, "view-pct", Optional, "view-pct\0vp\0",
          "The percentage of users that start off with a view."
      );
      Param(
          &TCmd::FriendPct, "friend-pct", Optional, "friend-pct\0fp\0",
          "(Per Round) The percentage of users that initiate a friendship when viewed."
      );
      Param(
          &TCmd::MovePct, "move-pct", Optional, "move-pct\0mp\0",
          "(Per Round) The percentage of users that move because of a friendship."
      );
      Param(
          &TCmd::FollowPct, "follow-pct", Optional, "follow-pct\0folp\0",
          "(Per Round) The percenatage of users that will follow a friend's activity and possibly view a 2nd degree friendship."
      );
    }

  };  // TCmd::TMeta

};  // TCmd

int main(int argc, char *argv[]) {
  ::TCmd cmd(argc, argv);
  Base::TLog log(cmd);
  MaxTrans = cmd.MaxTrans;
  NumRounds = cmd.NumRounds;
  NumUsers = cmd.NumUsers;
  NumPlaces = cmd.NumPlaces;
  Phase1ViewPct = cmd.ViewPct;
  Phase2FriendPct = cmd.FriendPct;
  Phase2MovePct = cmd.MovePct;
  Phase3FollowPct = cmd.FollowPct;
  LastTime = CreateTimePnt(2000, 1, 1, 0, 0, 0, 0, 0);
  std::cout << "Start Time [" << LastTime << "]" << std::endl;
  InitBuilder();
  int64_t seed = 0;
  /* phase 1: */
  size_t num_viewed_p1 = 0UL;
  size_t num_lived_p1 = 0UL;
  std::unique_ptr<TTmpFile> p1_tmp_view_f(make_unique<TTmpFile>(TmpFileTemplate, true));
  std::unique_ptr<TTmpFile> p1_tmp_lived_f(make_unique<TTmpFile>(TmpFileTemplate, true));
  /* flush output */ {
    Io::TBinaryOutputOnlyStream p1_view_strm(make_shared<Io::TDevice>(p1_tmp_view_f->GetFd()));
    Io::TBinaryOutputOnlyStream p1_lived_strm(make_shared<Io::TDevice>(p1_tmp_lived_f->GetFd()));
    seed = Phase1(p1_view_strm, num_viewed_p1, p1_lived_strm, num_lived_p1, seed);
  }
  std::cout << "Time after first views [" << LastTime << "]" << std::endl;
  /* phase 2: */
  size_t num_friended_p2 = 0UL;
  size_t num_lived_p2 = 0UL;
  std::unique_ptr<TTmpFile> p2_tmp_friend_f(make_unique<TTmpFile>(TmpFileTemplate, true));
  std::unique_ptr<TTmpFile> p2_tmp_lived_f(make_unique<TTmpFile>(TmpFileTemplate, true));
  /* flush output */ {
    Io::TBinaryOutputOnlyStream p2_lived_strm(make_shared<Io::TDevice>(p2_tmp_lived_f->GetFd()));
    seed = Phase2(*p1_tmp_view_f, num_viewed_p1, *p1_tmp_lived_f, num_lived_p1, *p2_tmp_friend_f, num_friended_p2, p2_lived_strm, num_lived_p2, seed);
  }
  std::cout << "Time after first friendships [" << LastTime << "]" << std::endl;
  auto view_and_friend_round = [&](const std::unique_ptr<TTmpFile> &tmp_friend_in, size_t tmp_friend_in_cnt,
                                   const std::unique_ptr<TTmpFile> &tmp_lived_in, size_t tmp_lived_in_cnt,
                                   size_t &tmp_friend_out_cnt,
                                   std::unique_ptr<TTmpFile> &tmp_lived_out, size_t &tmp_lived_out_cnt,
                                   int64_t seed) -> int64_t {
    /* phase 3: */
    size_t num_viewed_p3 = 0UL;
    std::unique_ptr<TTmpFile> p3_tmp_view_f(make_unique<TTmpFile>(TmpFileTemplate, true));
    /* flush output */ {
      Io::TBinaryOutputOnlyStream p3_view_out_strm(make_shared<Io::TDevice>(p3_tmp_view_f->GetFd()));
      seed = Phase3(*tmp_friend_in, tmp_friend_in_cnt, p3_view_out_strm, num_viewed_p3, seed);
    }
    /* phase 4: */
    /* flush output */ {
      Io::TBinaryOutputOnlyStream p4_lived_strm(make_shared<Io::TDevice>(tmp_lived_out->GetFd()));
      seed = Phase2(*p3_tmp_view_f, num_viewed_p3, *tmp_lived_in, tmp_lived_in_cnt, *tmp_friend_in, tmp_friend_out_cnt, p4_lived_strm, tmp_lived_out_cnt, seed);
    }
    return seed;
  };
  std::unique_ptr<TTmpFile> &tmp_friend_in = p2_tmp_friend_f;
  size_t tmp_friend_in_cnt = num_friended_p2;
  std::unique_ptr<TTmpFile> &tmp_lived_in = p2_tmp_lived_f;
  size_t tmp_lived_in_cnt = num_lived_p2;
  size_t tmp_friend_out_cnt = num_friended_p2;
  std::unique_ptr<TTmpFile> tmp_lived_out(make_unique<TTmpFile>(TmpFileTemplate, true));
  size_t tmp_lived_out_cnt = 0UL;
  for (int64_t i = 0; i < NumRounds; ++i) {
    seed = view_and_friend_round(tmp_friend_in, tmp_friend_in_cnt, tmp_lived_in, tmp_lived_in_cnt, tmp_friend_out_cnt, tmp_lived_out, tmp_lived_out_cnt, seed);
    std::cout << "Time after round [" << (i + 1) << "] [" << LastTime << "]" << std::endl;
    tmp_friend_in_cnt = tmp_friend_out_cnt;
    tmp_lived_in = std::move(tmp_lived_out);
    tmp_lived_in_cnt = tmp_lived_out_cnt;
    tmp_lived_out = make_unique<TTmpFile>(TmpFileTemplate, true);
    tmp_lived_out_cnt = 0UL;
  }
  if (TransCounter > 0) {
    Check();
  }
  return EXIT_SUCCESS;
}
