#include <syslog.h>
#include <base/cmd.h>
#include <base/log.h>
#include <base/opt.h>
#include <orly/csv_to_bin/field.h>
#include <orly/csv_to_bin/json_iter.h>
#include <orly/csv_to_bin/translate.h>
#include <orly/data/core_vector_generator.h>
#include <orly/desc.h>

using namespace std;
using namespace Base::Chrono;
using namespace Orly;
using namespace Orly::CsvToBin;
using namespace Orly::Data;

// {"follower":2209624286,"followed":9627972}
struct TFollowObj :
    public Orly::CsvToBin::TObj {
  int64_t follower;
  int64_t followed;
  virtual const TAnyFields &GetFields() const override {
    static const TFields<TFollowObj> fields {
      NEW_FIELD(TFollowObj, follower),
      NEW_FIELD(TFollowObj, followed)
    };
    return fields;
  }
};

// https://dev.twitter.com/docs/platform-objects/users
struct TUserObj :
    public Orly::CsvToBin::TObj {
  bool contributors_enabled;
  Base::Chrono::TTimePnt created_at;
  bool default_profile;
  bool default_profile_image;
  Base::TOpt<std::string> description;
  // TEntities entities; TODO
  int64_t favourites_count;
  bool follow_request_sent;
  int64_t followers_count;
  int64_t friends_count;
  bool geo_enabled;
  int64_t id;
  //std::string id_str;  We don't really need the string version...
  bool is_translator;
  std::string lang;
  int64_t listed_count;
  Base::TOpt<std::string> location;
  std::string name;
  std::string profile_background_color;
  std::string profile_background_image_url;
  std::string profile_background_image_url_https;
  bool profile_background_tile;
  //std::string profile_banner_url; TODO another "sometimes present field"
  std::string profile_image_url;
  std::string profile_image_url_https;
  std::string profile_link_color;
  std::string profile_sidebar_border_color;
  std::string profile_sidebar_fill_color;
  std::string profile_text_color;
  bool profile_use_background_image;
  bool _protected;
  std::string screen_name;
  // bool show_all_inline_media; TODO: a "not present field"?
  //TStatus status; // TODO: sometimes ommited, empty, null?
  int64_t statuses_count;
  Base::TOpt<std::string> time_zone;
  Base::TOpt<std::string> url;
  Base::TOpt<int64_t> utc_offset;
  bool verified;
  //std::string withheld_in_countries;  // TODO: another "when present" field
  //std::string withheld_scope;  // TODO: another "when present" field
  virtual const TAnyFields &GetFields() const override {
    static const TFields<TUserObj> fields {
      NEW_FIELD(TUserObj, contributors_enabled),
      NEW_FIELD(TUserObj, created_at),
      NEW_FIELD(TUserObj, default_profile),
      NEW_FIELD(TUserObj, default_profile_image),
      NEW_FIELD(TUserObj, description),
      NEW_FIELD(TUserObj, favourites_count),
      NEW_FIELD(TUserObj, follow_request_sent),
      NEW_FIELD(TUserObj, followers_count),
      NEW_FIELD(TUserObj, friends_count),
      NEW_FIELD(TUserObj, geo_enabled),
      NEW_FIELD(TUserObj, id),
      NEW_FIELD(TUserObj, is_translator),
      NEW_FIELD(TUserObj, lang),
      NEW_FIELD(TUserObj, listed_count),
      NEW_FIELD(TUserObj, location),
      NEW_FIELD(TUserObj, name),
      NEW_FIELD(TUserObj, profile_background_color),
      NEW_FIELD(TUserObj, profile_background_image_url),
      NEW_FIELD(TUserObj, profile_background_image_url_https),
      NEW_FIELD(TUserObj, profile_background_tile),
      NEW_FIELD(TUserObj, profile_image_url),
      NEW_FIELD(TUserObj, profile_image_url_https),
      NEW_FIELD(TUserObj, profile_link_color),
      NEW_FIELD(TUserObj, profile_sidebar_border_color),
      NEW_FIELD(TUserObj, profile_sidebar_fill_color),
      NEW_FIELD(TUserObj, profile_text_color),
      NEW_FIELD(TUserObj, profile_use_background_image),
      ::Orly::CsvToBin::NewField("protected", &TUserObj::_protected),
      NEW_FIELD(TUserObj, screen_name),
      NEW_FIELD(TUserObj, statuses_count),
      NEW_FIELD(TUserObj, time_zone),
      NEW_FIELD(TUserObj, url),
      NEW_FIELD(TUserObj, utc_offset),
      NEW_FIELD(TUserObj, verified)
    };
    return fields;
  }
};

struct TStatusObj :
    public Orly::CsvToBin::TObj {
  struct TContributorObj :
      public Orly::CsvToBin::TObj {
    int64_t id;
    std::string screen_name;
    virtual const TAnyFields &GetFields() const override {
      static const TFields<TContributorObj> fields {
        NEW_FIELD(TContributorObj, id),
        NEW_FIELD(TContributorObj, screen_name)
      };
      return fields;
    }
  };
  struct TCoordinateObj :
      public Orly::CsvToBin::TObj {
    std::vector<double> coordinates;
    std::string type;
    virtual const TAnyFields &GetFields() const override {
      static const TFields<TCoordinateObj> fields {
        NEW_FIELD(TCoordinateObj, coordinates),
        NEW_FIELD(TCoordinateObj, type)
      };
      return fields;
    }
  };
  struct TEntitiesObj :
      public Orly::CsvToBin::TObj {
    struct THashTagObj :
        public Orly::CsvToBin::TObj {
      std::vector<int> indices;
      std::string text;
      virtual const TAnyFields &GetFields() const override {
        static const TFields<THashTagObj> fields {
          NEW_FIELD(THashTagObj, indices),
          NEW_FIELD(THashTagObj, text)
        };
        return fields;
      }
    };
    struct TUserMentionObj :
        public Orly::CsvToBin::TObj {
      int64_t id;
      std::vector<int> indices;
      std::string name;
      std::string screen_name;
      virtual const TAnyFields &GetFields() const override {
        static const TFields<TUserMentionObj> fields {
          NEW_FIELD(TUserMentionObj, id),
          NEW_FIELD(TUserMentionObj, indices),
          NEW_FIELD(TUserMentionObj, name),
          NEW_FIELD(TUserMentionObj, screen_name)
        };
        return fields;
      }
    };
    std::vector<THashTagObj> hashtags;
    std::vector<TUserMentionObj> user_mentions;
    virtual const TAnyFields &GetFields() const override {
      static const TFields<TEntitiesObj> fields {
        NEW_FIELD(TEntitiesObj, hashtags),
        NEW_FIELD(TEntitiesObj, user_mentions)
      };
      return fields;
    }
  };

  Base::TOpt<std::vector<TContributorObj>> contributors;
  Base::TOpt<TCoordinateObj> coordinates;
  Base::Chrono::TTimePnt created_at;
  TEntitiesObj entities;
  Base::TOpt<int64_t> favorite_count;
  Base::TOpt<bool> favorited;
  //std::string filter_level;  // this field doesn't seem to exist in the data
  int64_t id;
  Base::TOpt<std::string> in_reply_to_screen_name;
  Base::TOpt<int64_t> in_reply_to_status_id;
  Base::TOpt<int64_t> in_reply_to_user_id;
  Base::TOpt<std::string> lang;
  // Base::TOpt<TPlaceObj> place;  // TODO: place
  //Base::TOpt<bool> possibly_sensitive;  // This field is not always present
  // TScopesObj scopes;  // TODO: scopes
  int64_t retweet_count;
  bool retweeted;
  std::unique_ptr<TStatusObj> retweeted_status;  // TODO: retweeted tweets
  std::string source;
  std::string text;
  bool truncated;
  TUserObj user;
  //bool withheld_copyright;  // this field does not seem to be present
  //std::vector<std::string> withheld_in_countries;  // this field does not seem to be present
  //std::string withheld_scope;  // this field does not seem to be present

  virtual const TAnyFields &GetFields() const override {
    static const TFields<TStatusObj> fields {
      NEW_FIELD(TStatusObj, contributors),
      NEW_FIELD(TStatusObj, coordinates),
      NEW_FIELD(TStatusObj, created_at),
      NEW_FIELD(TStatusObj, entities),
      NEW_FIELD(TStatusObj, favorite_count),
      NEW_FIELD(TStatusObj, favorited),
      NEW_FIELD(TStatusObj, id),
      NEW_FIELD(TStatusObj, in_reply_to_screen_name),
      NEW_FIELD(TStatusObj, in_reply_to_status_id),
      NEW_FIELD(TStatusObj, in_reply_to_user_id),
      NEW_FIELD(TStatusObj, lang),
      NEW_FIELD(TStatusObj, retweet_count),
      NEW_FIELD(TStatusObj, retweeted),
      NEW_FIELD(TStatusObj, retweeted_status),
      NEW_FIELD(TStatusObj, source),
      NEW_FIELD(TStatusObj, text),
      NEW_FIELD(TStatusObj, truncated),
      NEW_FIELD(TStatusObj, user),
    };
    return fields;
  }
};


constexpr int64_t ReplyToTweet = 0;
constexpr int64_t ReplyToUser = 1;
constexpr int64_t DidMentionUser = 2;
constexpr int64_t MentionedByUser = 3;
constexpr int64_t PersonUsedTag = 4;
constexpr int64_t TagUsedByPerson = 5;
constexpr int64_t PersonLocation = 6;
constexpr int64_t Following = 7;
constexpr int64_t FollowedBy = 8;

void TranslateFollow(TJsonIter &input) {
  assert(&input);
  // following
  using t_person_following_key = std::tuple<int64_t, int64_t, int64_t>;  // IndexId, user id, tweet id
  using t_person_following_val = bool;  // unused
  TCoreVectorGenerator<t_person_following_key, t_person_following_val> person_following_cvg("person_following");
  // followed by
  using t_person_followed_by_key = std::tuple<int64_t, int64_t, int64_t>;  // IndexId, user id, tweet id
  using t_person_followed_by_val = bool;  // unused
  TCoreVectorGenerator<t_person_followed_by_key, t_person_followed_by_val> person_followed_by_cvg("person_followed_by");
  for (; input; ++input) {
    const TJson &follow_obj_json = *input;
    TFollowObj follow_obj;
    TranslateJson(follow_obj, follow_obj_json);
    person_following_cvg.Push(t_person_following_key(Following, follow_obj.follower, follow_obj.followed), true);
    person_followed_by_cvg.Push(t_person_followed_by_key(FollowedBy, follow_obj.followed, follow_obj.follower), true);
  }
}

void TranslateUser(TJsonIter &input) {
  assert(&input);
  for (; input; ++input) {
    const TJson &user_obj_json = *input;
    TUserObj user_obj;
    TranslateJson(user_obj, user_obj_json);
    printf("uid=[%ld], name=[%s]\n", user_obj.id, user_obj.screen_name.c_str());
  }
}

struct TPersonTweetedVal {
  using TCoord = Base::TOpt<std::tuple<double, double>>;
  TPersonTweetedVal(int64_t reply_to_status,
                    int64_t reply_to_user,
                    int64_t _retweet_count,
                    const Base::Chrono::TTimePnt &_date,
                    const std::string &_text,
                    const Base::TOpt<TStatusObj::TCoordinateObj> &coord)
      : reply_to_tweet_id(reply_to_status),
        reply_to_user_id(reply_to_user),
        retweet_count(_retweet_count),
        date(_date),
        text(_text),
        coordinates(coord ? TCoord(make_tuple(coord->coordinates[0], coord->coordinates[1])) : TCoord()) {}
  int64_t reply_to_tweet_id;
  int64_t reply_to_user_id;
  int64_t retweet_count;
  Base::Chrono::TTimePnt date;
  std::string text;
  TCoord coordinates;
};
RECORD_ELEM(TPersonTweetedVal, int64_t, reply_to_tweet_id);
RECORD_ELEM(TPersonTweetedVal, int64_t, reply_to_user_id);
RECORD_ELEM(TPersonTweetedVal, Base::Chrono::TTimePnt, date);
RECORD_ELEM(TPersonTweetedVal, std::string, text);
RECORD_ELEM(TPersonTweetedVal, TPersonTweetedVal::TCoord, coordinates);
void TranslateStatus(TJsonIter &input) {
  assert(&input);
  // person tweeted
  using t_person_tweeted_key = std::tuple<int64_t, int64_t>;  // user id, tweet id
  using t_person_tweeted_val = TPersonTweetedVal;  // text
  TCoreVectorGenerator<t_person_tweeted_key, t_person_tweeted_val> person_tweeted_cvg("person_tweeted");
  using t_person_tweeted_tweet_key = std::tuple<int64_t, int64_t, int64_t, int64_t>;  // IndexId, id of replied to tweet, id of user who replied to it, tweet id of reply
  using t_person_tweeted_tweet_val = Base::Chrono::TTimePnt;  // date
  TCoreVectorGenerator<t_person_tweeted_tweet_key, t_person_tweeted_tweet_val> person_tweeted_tweet_cvg("person_tweeted_in_reply_to_tweet");
  using t_person_tweeted_user_key = std::tuple<int64_t, int64_t, int64_t, int64_t>;  // IndexId, id of replied to user, id of user who replied to it, tweet id of reply
  using t_person_tweeted_user_val = Base::Chrono::TTimePnt;  // date
  TCoreVectorGenerator<t_person_tweeted_user_key, t_person_tweeted_user_val> person_tweeted_user_cvg("person_tweeted_in_reply_to_user");
  using t_person_location_key = std::tuple<int64_t, int64_t, Base::Chrono::TTimePnt, double, double>;  // IndexId, id of user, date, longitude, latitude
  using t_person_location_val = int64_t;  // tweet id
  TCoreVectorGenerator<t_person_location_key, t_person_location_val> person_location_cvg("person_location");
  auto person_tweeted = [&](int64_t uid,
                            int64_t tid,
                            const Base::TOpt<int64_t> &reply_to_tweet_id,
                            const Base::TOpt<int64_t> &reply_to_user_id,
                            int64_t retweet_count,
                            const Base::Chrono::TTimePnt &date,
                            const std::string &text,
                            const Base::TOpt<TStatusObj::TCoordinateObj> &coordinates) {
    person_tweeted_cvg.Push(t_person_tweeted_key(uid, tid), t_person_tweeted_val(reply_to_tweet_id ? *reply_to_tweet_id : -1,
                                                                                 reply_to_user_id ? *reply_to_user_id : -1,
                                                                                 retweet_count,
                                                                                 date,
                                                                                 text,
                                                                                 coordinates));
    if (reply_to_tweet_id) {
      person_tweeted_tweet_cvg.Push(t_person_tweeted_tweet_key(ReplyToTweet, *reply_to_tweet_id, uid, tid), date);
    }
    if (reply_to_user_id) {
      person_tweeted_user_cvg.Push(t_person_tweeted_user_key(ReplyToUser, *reply_to_user_id, uid, tid), date);
    }
    if (coordinates) {
      person_location_cvg.Push(t_person_location_key(PersonLocation, uid, date, coordinates->coordinates[0], coordinates->coordinates[1]), tid);
    }
  };
  // person mentioned
  using t_person_did_mention_key = std::tuple<int64_t, int64_t, int64_t, int64_t>;  // IndexId, user id, mentioned user id, tweet id
  using t_person_did_mention_val = Base::Chrono::TTimePnt;  // date
  TCoreVectorGenerator<t_person_did_mention_key, t_person_did_mention_val> person_did_mention_cvg("person_did_mention");
  using t_person_mentioned_by_key = std::tuple<int64_t, int64_t, int64_t, int64_t>;  // IndexId, mentioned user id, user id, tweet id
  using t_person_mentioned_by_val = Base::Chrono::TTimePnt;  // date
  TCoreVectorGenerator<t_person_mentioned_by_key, t_person_mentioned_by_val> person_mentioned_by_cvg("person_mentioned_by");
  auto person_mentioned = [&](int64_t uid, int64_t muid, int64_t tid, const Base::Chrono::TTimePnt &date) {
    person_did_mention_cvg.Push(t_person_did_mention_key(DidMentionUser, uid, muid, tid), date);
    person_mentioned_by_cvg.Push(t_person_mentioned_by_key(MentionedByUser, muid, uid, tid), date);
  };
  // person used hashtag
  using t_person_used_tag_key = std::tuple<int64_t, int64_t, std::string, int64_t>;  // IndexId, user id, hashtag, tweet id
  using t_person_used_tag_val = Base::Chrono::TTimePnt;  // date
  TCoreVectorGenerator<t_person_used_tag_key, t_person_used_tag_val> person_used_tag_cvg("person_used_tag");
  using t_tag_used_by_key = std::tuple<int64_t, std::string, int64_t, int64_t>;  // IndexId, hashtag, user id, tweet id
  using t_tag_used_by_val = Base::Chrono::TTimePnt;  // date
  TCoreVectorGenerator<t_tag_used_by_key, t_tag_used_by_val> tag_used_by_cvg("tag_used_by");
  auto person_tagged = [&](int64_t uid, const std::string &hashtag, int64_t tid, const Base::Chrono::TTimePnt &date) {
    person_used_tag_cvg.Push(t_person_used_tag_key(PersonUsedTag, uid, hashtag, tid), date);
    tag_used_by_cvg.Push(t_tag_used_by_key(TagUsedByPerson, hashtag, uid, tid), date);
  };

  for (; input; ++input) {
    const TJson &status_obj_json = *input;
    TStatusObj obj;
    TranslateJson(obj, status_obj_json);
    person_tweeted(obj.user.id, obj.id, obj.in_reply_to_status_id, obj.in_reply_to_user_id, obj.retweet_count, obj.created_at, obj.text, obj.coordinates);
    stringstream ss;
    ss << obj.created_at;
    for (const auto &mention : obj.entities.user_mentions) {
      person_mentioned(obj.user.id, mention.id, obj.id, obj.created_at);
    }
    for (const auto &tag : obj.entities.hashtags) {
      person_tagged(obj.user.id, tag.text, obj.id, obj.created_at);
    }
  }
}

template <typename TStrm>
static void OpenFile(TStrm &strm, const string &path) {
  assert(&strm);
  assert(&path);
  strm.exceptions(ifstream::failbit);
  try {
    strm.open(path);
  } catch (const ifstream::failure &ex) {
    char temp[256];
    cerr
        << "could not open \"" << path
        << "\"; " << Util::Strerror(errno, temp, sizeof(temp))
        << endl;
    exit(EXIT_FAILURE);
  }
}

class TCmd final
    : public Base::TLog::TCmd {
  public:

  TCmd(int argc, char *argv[])
      : IsUserData(false),
        IsFollowerData(false),
        IsStatusData(false) {
    Parse(argc, argv, TMeta());
  }

  string Json;
  bool IsUserData;
  bool IsFollowerData;
  bool IsStatusData;

  private:

  class TMeta final
      : public Base::TLog::TCmd::TMeta {
    public:

    TMeta()
        : Base::TLog::TCmd::TMeta("CSV to Orly binary converter generator.") {
      Param(
          &TCmd::Json, "json", Required,
          "The path to the json file to read from.");
      Param(
          &TCmd::IsUserData, "user", Optional, "user\0u\0",
          "A flag to indicate this file is user data.");
      Param(
          &TCmd::IsFollowerData, "follower", Optional, "follower\0f\0",
          "A flag to indicate this file is follower data.");
      Param(
          &TCmd::IsStatusData, "status", Optional, "status\0s\0",
          "A flag to indicate this file is status data.");
    }

  };  // TMeta

};  // TCmd

int main(int argc, char *argv[]) {
  TCmd cmd(argc, argv);
  Base::TLog log(cmd);
  ifstream instrm;
  OpenFile(instrm, cmd.Json);
  TJsonIter json_iter(instrm);
  if (cmd.IsUserData) {
    TranslateUser(json_iter);
  } else if (cmd.IsFollowerData) {
    TranslateFollow(json_iter);
  } else if (cmd.IsStatusData) {
    TranslateStatus(json_iter);
  }
}

