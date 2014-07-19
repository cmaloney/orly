#include <syslog.h>
#include <base/cmd.h>
#include <base/log.h>
#include <base/opt.h>
#include <orly/csv_to_bin/field.h>
#include <orly/csv_to_bin/json_iter.h>
#include <orly/csv_to_bin/translate.h>
#include <orly/data/core_vector_generator.h>
#include <orly/desc.h>
#include <util/io.h>

using namespace std;
using namespace Base::Chrono;
using namespace Orly;
using namespace Orly::CsvToBin;
using namespace Orly::Data;

// {"follower":2209624286,"followed":9627972}
struct TFollowObj :
    public Orly::CsvToBin::TObj {
  int64_t follower;
  std::string follower_screen_name;
  int64_t followed;
  std::string followed_screen_name;
  virtual const TAnyFields &GetFields() const override {
    static const TFields<TFollowObj> fields {
      NEW_FIELD(TFollowObj, follower),
      NEW_FIELD(TFollowObj, follower_screen_name),
      NEW_FIELD(TFollowObj, followed),
      NEW_FIELD(TFollowObj, followed_screen_name)
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
constexpr int64_t ReplyFromUser = 2;
constexpr int64_t DidMentionUser = 3;
constexpr int64_t MentionedByUser = 4;
constexpr int64_t PersonUsedTag = 5;
constexpr int64_t TagUsedByPerson = 6;
constexpr int64_t PersonLocation = 7;
constexpr int64_t Following = 8;
constexpr int64_t FollowedBy = 9;
constexpr int64_t PersonRetweeted = 10;
constexpr int64_t PersonRetweetedTweet = 11;
constexpr int64_t PersonWasRetweeted = 12;
constexpr int64_t TweetWasRetweeted = 13;

void TranslateFollow(TJsonIter &input) {
  assert(&input);
  // following
  using t_person_following_key = std::tuple<int64_t, int64_t, int64_t>;  // IndexId, follower user id, followed user id
  using t_person_following_val = std::tuple<std::string, std::string>;  // user handle (following), user handle (followed)
  TCoreVectorGenerator<t_person_following_key, t_person_following_val> person_following_cvg("person_following");
  // followed by
  using t_person_followed_by_key = std::tuple<int64_t, int64_t, int64_t>;  // IndexId, followed user id, follower user id
  using t_person_followed_by_val = std::tuple<std::string, std::string>;  // user handle (followed), user handle (following)
  TCoreVectorGenerator<t_person_followed_by_key, t_person_followed_by_val> person_followed_by_cvg("person_followed_by");
  for (; input; ++input) {
    const TJson &follow_obj_json = *input;
    TFollowObj follow_obj;
    TranslateJson(follow_obj, follow_obj_json);
    person_following_cvg.Push(t_person_following_key(Following, follow_obj.follower, follow_obj.followed), t_person_following_val(follow_obj.follower_screen_name, follow_obj.followed_screen_name));
    person_followed_by_cvg.Push(t_person_followed_by_key(FollowedBy, follow_obj.followed, follow_obj.follower), t_person_followed_by_val(follow_obj.followed_screen_name, follow_obj.follower_screen_name));
  }
}

void TranslateUser(TJsonIter &input) {
  assert(&input);
  // user screen name -> id
  using t_user_by_screen_key = std::tuple<std::string>;  // screen name
  using t_user_by_screen_val = int64_t;  // user_id
  TCoreVectorGenerator<t_user_by_screen_key, t_user_by_screen_val> user_by_screen_cvg("user_by_screen");
  // user id -> screen name
  using t_screen_by_user_key = std::tuple<int64_t>;  // user id
  using t_screen_by_user_val = std::string;  // screen name
  TCoreVectorGenerator<t_screen_by_user_key, t_screen_by_user_val> screen_by_user_cvg("screen_by_user");
  for (; input; ++input) {
    const TJson &user_obj_json = *input;
    TUserObj user_obj;
    TranslateJson(user_obj, user_obj_json);
    user_by_screen_cvg.Push(t_user_by_screen_key(user_obj.screen_name), user_obj.id);
    screen_by_user_cvg.Push(t_screen_by_user_key(user_obj.id), user_obj.screen_name);
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
  using t_person_tweeted_tweet_val = std::tuple<std::string, Base::Chrono::TTimePnt>;  // user handle, date
  TCoreVectorGenerator<t_person_tweeted_tweet_key, t_person_tweeted_tweet_val> person_tweeted_tweet_cvg("person_tweeted_in_reply_to_tweet");
  using t_person_tweeted_user_key = std::tuple<int64_t, int64_t, int64_t, int64_t>;  // IndexId, id of user who replied to it, id of replied to user, tweet id of reply
  using t_person_tweeted_user_val = std::tuple<std::string, std::string, Base::Chrono::TTimePnt>; // user handle (of person that made reply) user handle (of replied), date
  TCoreVectorGenerator<t_person_tweeted_user_key, t_person_tweeted_user_val> person_tweeted_user_cvg("person_tweeted_in_reply_to_user");
  using t_user_received_reply_key = std::tuple<int64_t, int64_t, int64_t, int64_t>;  // IndexId, id of replied to user, id of user who replied, tweet id of reply
  using t_user_received_reply_val = std::tuple<std::string, std::string, Base::Chrono::TTimePnt>; // user handle (of replied) user handle (of person that made reply), date
  TCoreVectorGenerator<t_user_received_reply_key, t_user_received_reply_val> user_received_reply_cvg("user_received_reply");
  using t_person_location_key = std::tuple<int64_t, int64_t, Base::Chrono::TTimePnt, double, double>;  // IndexId, id of user, user handle, date, longitude, latitude
  using t_person_location_val = std::tuple<std::string, int64_t>;  // user handle, tweet id
  TCoreVectorGenerator<t_person_location_key, t_person_location_val> person_location_cvg("person_location");
  auto person_tweeted = [&](int64_t uid,
                            const std::string &handle,
                            int64_t tid,
                            const Base::TOpt<int64_t> &reply_to_tweet_id,
                            const Base::TOpt<int64_t> &reply_to_user_id,
                            const Base::TOpt<std::string> &reply_to_screen_name,
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
      person_tweeted_tweet_cvg.Push(t_person_tweeted_tweet_key(ReplyToTweet, *reply_to_tweet_id, uid, tid), t_person_tweeted_tweet_val(handle, date));
    }
    if (reply_to_user_id) {
      assert(reply_to_screen_name);
      person_tweeted_user_cvg.Push(t_person_tweeted_user_key(ReplyToUser, uid, *reply_to_user_id, tid), t_person_tweeted_user_val(handle, *reply_to_screen_name, date));
      user_received_reply_cvg.Push(t_user_received_reply_key(ReplyFromUser, *reply_to_user_id, uid, tid), t_user_received_reply_val(*reply_to_screen_name, handle, date));
    }
    if (coordinates) {
      person_location_cvg.Push(t_person_location_key(PersonLocation, uid, date, coordinates->coordinates[0], coordinates->coordinates[1]), t_person_location_val(handle, tid));
    }
  };
  // person mentioned
  using t_person_did_mention_key = std::tuple<int64_t, int64_t, int64_t, int64_t>;  // IndexId, user id, mentioned user id, tweet id
  using t_person_did_mention_val = std::tuple<std::string, std::string, Base::Chrono::TTimePnt>;  // user handle (did mention), user handle (mentioned), date
  TCoreVectorGenerator<t_person_did_mention_key, t_person_did_mention_val> person_did_mention_cvg("person_did_mention");
  using t_person_mentioned_by_key = std::tuple<int64_t, int64_t, int64_t, int64_t>;  // IndexId, mentioned user id, user id, tweet id
  using t_person_mentioned_by_val = std::tuple<std::string, std::string, Base::Chrono::TTimePnt>;  // user handle (mentioned), user handle (did mention), date
  TCoreVectorGenerator<t_person_mentioned_by_key, t_person_mentioned_by_val> person_mentioned_by_cvg("person_mentioned_by");
  auto person_mentioned = [&](int64_t uid,
                              const std::string &handle,
                              int64_t muid,
                              const std::string &mhandle,
                              int64_t tid,
                              const Base::Chrono::TTimePnt &date) {
    person_did_mention_cvg.Push(t_person_did_mention_key(DidMentionUser, uid, muid, tid), t_person_did_mention_val(handle, mhandle, date));
    person_mentioned_by_cvg.Push(t_person_mentioned_by_key(MentionedByUser, muid, uid, tid), t_person_mentioned_by_val(mhandle, handle, date));
  };
  // person used hashtag
  using t_person_used_tag_key = std::tuple<int64_t, int64_t, std::string, int64_t>;  // IndexId, user id, hashtag, tweet id
  using t_person_used_tag_val = std::tuple<std::string, Base::Chrono::TTimePnt>;  // user handle, date
  TCoreVectorGenerator<t_person_used_tag_key, t_person_used_tag_val> person_used_tag_cvg("person_used_tag");
  using t_tag_used_by_key = std::tuple<int64_t, std::string, int64_t, int64_t>;  // IndexId, hashtag, user id, user handle, tweet id
  using t_tag_used_by_val = std::tuple<std::string, Base::Chrono::TTimePnt>;  // user handle, date
  TCoreVectorGenerator<t_tag_used_by_key, t_tag_used_by_val> tag_used_by_cvg("tag_used_by");
  auto person_tagged = [&](int64_t uid,
                           const std::string &handle,
                           const std::string &hashtag,
                           int64_t tid,
                           const Base::Chrono::TTimePnt &date) {
    person_used_tag_cvg.Push(t_person_used_tag_key(PersonUsedTag, uid, hashtag, tid), t_person_used_tag_val(handle, date));
    tag_used_by_cvg.Push(t_tag_used_by_key(TagUsedByPerson, hashtag, uid, tid), t_tag_used_by_val(handle, date));
  };
  // person retweeted
  using t_person_retweet_key = std::tuple<int64_t, int64_t, int64_t, int64_t>;  // IndexId, user id, id of original tweet user , tweet id
  using t_person_retweet_val = std::tuple<std::string, std::string, int64_t, Base::Chrono::TTimePnt>;  // user handle, user handle of originator, original tweet id, date
  TCoreVectorGenerator<t_person_retweet_key, t_person_retweet_val> person_retweet_cvg("person_retweet_user");
  using t_person_retweet_tweet_key = std::tuple<int64_t, int64_t, int64_t, int64_t>;  // IndexId, user id, original tweet id, tweet id
  using t_person_retweet_tweet_val = std::tuple<std::string, std::string, int64_t, Base::Chrono::TTimePnt>;  // user handle, user handle of originator, user id of originator, date
  TCoreVectorGenerator<t_person_retweet_tweet_key, t_person_retweet_tweet_val> person_retweet_tweet_cvg("person_retweet_tweet");
  using t_person_was_retweeted_key = std::tuple<int64_t, int64_t, int64_t, int64_t>;  // IndexId, user id of original user, user who retweeted , tweet id
  using t_person_was_retweeted_val = std::tuple<std::string, std::string, int64_t, Base::Chrono::TTimePnt>;  // user handle of original user, user handle of retweeter, original tweet id, date
  TCoreVectorGenerator<t_person_was_retweeted_key, t_person_was_retweeted_val> person_was_retweeted_cvg("person_was_retweeted");
  using t_tweet_was_retweeted_key = std::tuple<int64_t, int64_t, int64_t, int64_t>;  // IndexId, original tweet id, user id who retweeted , tweet id
  using t_tweet_was_retweeted_val = std::tuple<std::string, std::string, int64_t, Base::Chrono::TTimePnt>;  // user handle of original user, user handle of retweeter, user id of original user, date
  TCoreVectorGenerator<t_tweet_was_retweeted_key, t_tweet_was_retweeted_val> tweet_was_retweeted_cvg("tweet_was_retweeted");
  auto person_retweeted = [&](int64_t uid,
                              const std::string &handle,
                              int64_t ouid,
                              const std::string &ohandle,
                              int64_t tid,
                              int64_t otid,
                              const Base::Chrono::TTimePnt &date) {
    person_retweet_cvg.Push(t_person_retweet_key(PersonRetweeted, uid, ouid, tid), t_person_retweet_val(handle, ohandle, otid, date));
    person_retweet_tweet_cvg.Push(t_person_retweet_tweet_key(PersonRetweetedTweet, uid, otid, tid), t_person_retweet_tweet_val(handle, ohandle, ouid, date));
    person_was_retweeted_cvg.Push(t_person_was_retweeted_key(PersonWasRetweeted, ouid, uid, tid), t_person_was_retweeted_val(ohandle, handle, otid, date));
    tweet_was_retweeted_cvg.Push(t_tweet_was_retweeted_key(TweetWasRetweeted, otid, uid, tid), t_tweet_was_retweeted_val(ohandle, handle, ouid, date));
  };

  for (; input; ++input) {
    const TJson &status_obj_json = *input;
    TStatusObj obj;
    TranslateJson(obj, status_obj_json);
    person_tweeted(obj.user.id, obj.user.screen_name, obj.id, obj.in_reply_to_status_id, obj.in_reply_to_user_id, obj.in_reply_to_screen_name, obj.retweet_count, obj.created_at, obj.text, obj.coordinates);
    stringstream ss;
    ss << obj.created_at;
    for (const auto &mention : obj.entities.user_mentions) {
      person_mentioned(obj.user.id, obj.user.screen_name, mention.id, mention.screen_name, obj.id, obj.created_at);
    }
    for (const auto &tag : obj.entities.hashtags) {
      person_tagged(obj.user.id, obj.user.screen_name, tag.text, obj.id, obj.created_at);
    }
    if (obj.retweeted_status) {
      person_retweeted(obj.user.id,
                       obj.user.screen_name,
                       obj.retweeted_status->user.id,
                       obj.retweeted_status->user.screen_name,
                       obj.id,
                       obj.retweeted_status->id,
                       obj.created_at);
    }
  }
}


class TCmd final
    : public Base::TLog::TCmd {
  public:

  TCmd(int argc, char *argv[]) {
    Parse(argc, argv, TMeta());
  }

  string Username;

  private:

  class TMeta final
      : public Base::TLog::TCmd::TMeta {
    public:

    TMeta()
        : Base::TLog::TCmd::TMeta("CSV to Orly binary converter generator.") {
      Param(&TCmd::Username, "username", Required, "The user whose json we should read");
    }

  };  // TMeta

};  // TCmd

int main(int argc, char *argv[]) {
  TCmd cmd(argc, argv);
  Base::TLog log(cmd);

  const vector<string> datasets = {"users", "follows", "statuses"};
  for (const string &dataset: datasets) {
    try {
      ifstream in;
      Util::OpenFile(in, cmd.Username + '.' + dataset + ".json");
      TJsonIter json_iter(in);
      if (dataset == "users") {
        TranslateUser(json_iter);
      } else if (dataset == "follows") {
        //TranslateFollow(json_iter);
      } else if (dataset == "statuses") {
        TranslateStatus(json_iter);
      }
    } catch (const Util::TOpenFileError &ex) {
      cout << "ERROR: Processing dataset " << quoted(dataset) << " for user " << quoted(cmd.Username) << endl;
      cout << ex.what() << endl;
    }
  }
}

