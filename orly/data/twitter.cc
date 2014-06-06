/* <orly/data/twitter.cc>

   Generates a core-vector file with twitter data. Takes as an input a
   twitter data drop file

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

#include <fstream>
#include <functional>
#include <iostream>

#include <fcntl.h>
#include <unistd.h>

#include <base/glob.h>
#include <base/regex_matcher.h>
#include <io/binary_output_only_stream.h>
#include <io/device.h>
#include <orly/atom/core_vector_builder.h>
#include <orly/data/twitter.import.cst.h>
#include <tools/nycr/pos_range.h>
#include <tools/nycr/error.h>

using namespace std;
using namespace Base;
using namespace Orly;
using namespace Orly::Data::Twitter;
using namespace Orly::Data::Twitter::Import::Syntax;

static const TUuid TweetUserIndexId(TUuid(TUuid::Best));
static const TUuid UserReplyStatusIndexId(TUuid(TUuid::Best));
static const TUuid UserReplyUserIndexId(TUuid(TUuid::Best));
static const TUuid ReplyUserToUserIndexId(TUuid(TUuid::Best));
static const int64_t UserTweetInt = 13;
static const int64_t TweetUserInt = 17;

class TUserObj {
  public:
  int64_t Id;
  std::string Name;
};

class TTweetObj {
  public:
  int64_t Id;
  int64_t UserId;
  std::string UserName;
  std::string Text;
  int64_t ReplyToUserId;
  int64_t ReplyToStatusId;
  int64_t FavoriteCount;
  int64_t RetweetCount;
};

void GetStr(string &val, const TExpr *expr) {
  class TExprVisitor : public TExpr::TVisitor {
    NO_COPY(TExprVisitor);
    public:
    TExprVisitor(string &val) : Val(val) {}
    virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TRefExpr */*that*/) const override {assert(false);}
    virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TParenExpr */*that*/) const override {assert(false);}
    virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TLiteralExpr *that) const override {
      class TVisitor : public TLiteral::TVisitor {
        NO_COPY(TVisitor);
        public:
        TVisitor(string &val) : Val(val) {}
        virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TTimeDiffLiteral */*that*/) const override {assert(false);}
        virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TTimePntLiteral */*that*/) const override {assert(false);}
        virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TRealLiteral */*that*/) const override {assert(false);}
        virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TIntLiteral */*that*/) const override {assert(false);}
        virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TIdLiteral */*that*/) const override {assert(false);}
        virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TDoubleQuotedRawStrLiteral *that) const override {
          Val = that->GetLexeme().GetText().substr(1, that->GetLexeme().GetText().size() - 2);
        }
        virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TDoubleQuotedStrLiteral *that) const override {
          Val = that->GetLexeme().GetText().substr(1, that->GetLexeme().GetText().size() - 2);
        }
        virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TTrueKwd */*that*/) const override {assert(false);}
        virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TFalseKwd */*that*/) const override {assert(false);}
        virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TNullLiteral */*that*/) const override {assert(false);}
        private:
        string &Val;
      };  // TVisitor
      that->GetLiteral()->Accept(TVisitor(Val));
    }
    virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TSetCtor */*that*/) const override {assert(false);}
    virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TListCtor */*that*/) const override {assert(false);}
    virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TObjCtor */*that*/) const override {assert(false);}
    virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TDictCtor */*that*/) const override {assert(false);}
    private:
    string &Val;
  };  // TExprVisitor
  TExprVisitor vis(val);
    expr->Accept(vis);
}

void GetOptInt(int64_t &val, const TExpr *expr) {
  class TExprVisitor : public TExpr::TVisitor {
    NO_COPY(TExprVisitor);
    public:
    TExprVisitor(int64_t &val) : Val(val) {}
    virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TRefExpr */*that*/) const override {assert(false);}
    virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TParenExpr */*that*/) const override {assert(false);}
    virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TLiteralExpr *that) const override {
      class TVisitor : public TLiteral::TVisitor {
        NO_COPY(TVisitor);
        public:
        TVisitor(int64_t &val) : Val(val) {}
        virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TTimeDiffLiteral */*that*/) const override {assert(false);}
        virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TTimePntLiteral */*that*/) const override {assert(false);}
        virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TRealLiteral */*that*/) const override {assert(false);}
        virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TIntLiteral *that) const override {
          Val = that->GetLexeme().AsInt();
        }
        virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TIdLiteral */*that*/) const override {assert(false);}
        virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TDoubleQuotedRawStrLiteral */*that*/) const override {assert(false);}
        virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TDoubleQuotedStrLiteral */*that*/) const override {assert(false);}
        virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TTrueKwd */*that*/) const override {assert(false);}
        virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TFalseKwd */*that*/) const override {assert(false);}
        virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TNullLiteral */*that*/) const override {
          Val = -1;
        }
        private:
        int64_t &Val;
      };  // TVisitor
      that->GetLiteral()->Accept(TVisitor(Val));
    }
    virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TSetCtor */*that*/) const override {assert(false);}
    virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TListCtor */*that*/) const override {assert(false);}
    virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TObjCtor */*that*/) const override {assert(false);}
    virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TDictCtor */*that*/) const override {assert(false);}
    private:
    int64_t &Val;
  };  // TExprVisitor
  TExprVisitor vis(val);
  expr->Accept(vis);
}

int64_t GetInt(const TExpr *expr) {
  int64_t val;
  GetOptInt(val, expr);
  assert(val != -1);
  return val;
}

void GetUserObj(TUserObj &val, const TExpr *expr) {
  class TExprVisitor : public TExpr::TVisitor {
    NO_COPY(TExprVisitor);
    public:
    TExprVisitor(TUserObj &val) : Val(val) {}
    virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TRefExpr */*that*/) const override {assert(false);}
    virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TParenExpr */*that*/) const override {assert(false);}
    virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TLiteralExpr */*that*/) const override {assert(false);}
    virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TSetCtor */*that*/) const override {assert(false);}
    virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TListCtor */*that*/) const override {assert(false);}
    virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TObjCtor */*that*/) const override {assert(false);}
    virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TDictCtor *that) const override {
      class TTailVisitor : public TOptDictMemberListTail::TVisitor {
        NO_COPY(TTailVisitor);
        public:
        TTailVisitor(const std::function<void (const TDictMember *, TUserObj *)> &cb, TUserObj *user) : Cb(cb), User(user) {}
        virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TNoDictMemberListTail */*that*/) const override {}
        virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TDictMemberListTail *that) const override {
          auto member_list = that->GetDictMemberList();
          Cb(member_list->GetDictMember(), User);
          member_list->GetOptDictMemberListTail()->Accept(*this);
        }
      private:
      const std::function<void (const TDictMember *, TUserObj *)> &Cb;
      TUserObj *User;
      };  // TTailVisitor
      auto member_list = that->GetDictMemberList();
      auto cb = [](const TDictMember *member, TUserObj *user) {
        static const std::string id_str("id");
        static const std::string name_str("name");
        std::string name;
        GetStr(name, member->GetKey());
        const TExpr *val = member->GetValue();
        if (name == id_str) {
          user->Id = GetInt(val);
        } else if (name == name_str) {
          GetStr(user->Name, val);
        }
      };
      cb(member_list->GetDictMember(), &Val);
      TTailVisitor vis(cb, &Val);
      member_list->GetOptDictMemberListTail()->Accept(vis);
    }
    private:
    TUserObj &Val;
  };  // TExprVisitor
  TExprVisitor vis(val);
  expr->Accept(vis);
}

class TExprVisitor : public TExpr::TVisitor {
  NO_COPY(TExprVisitor);
  public:

  TExprVisitor(TTweetObj *tweet) : Tweet(tweet) {}

  virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TRefExpr */*that*/) const override {assert(false);}
  virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TParenExpr */*that*/) const override {assert(false);}
  virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TLiteralExpr */*that*/) const override {assert(false);}
  virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TSetCtor */*that*/) const override {assert(false);}
  virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TListCtor */*that*/) const override {assert(false);}
  virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TObjCtor */*that*/) const override {assert(false);}
  virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TDictCtor *that) const override {
    class TTailVisitor : public TOptDictMemberListTail::TVisitor {
      NO_COPY(TTailVisitor);
      public:
      TTailVisitor(const std::function<void (const TDictMember *, TTweetObj *)> &cb, TTweetObj *tweet) : Cb(cb), Tweet(tweet) {}
      virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TNoDictMemberListTail */*that*/) const override {
      }
      virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TDictMemberListTail *that) const override {
        auto member_list = that->GetDictMemberList();
        Cb(member_list->GetDictMember(), Tweet);
        member_list->GetOptDictMemberListTail()->Accept(*this);
      }
    private:
    const std::function<void (const TDictMember *, TTweetObj *)> &Cb;
    TTweetObj *Tweet;
    };  // TTailVisitor
    auto member_list = that->GetDictMemberList();
    auto cb = [](const TDictMember *member, TTweetObj *tweet) {
      static const std::string id_str("id");
      static const std::string text_str("text");
      static const std::string in_reply_to_status_id_str("in_reply_to_status_id");
      static const std::string in_reply_to_user_id_str("in_reply_to_user_id");
      static const std::string retweet_count_str("retweet_count");
      static const std::string favorite_count_str("favorite_count");
      static const std::string user_str("user");
      std::string name;
      GetStr(name, member->GetKey());
      const TExpr *val = member->GetValue();
      if (name == id_str) {
        tweet->Id = GetInt(val);
      } else if (name == text_str) {
        GetStr(tweet->Text, val);
      } else if (name == in_reply_to_status_id_str) {
        GetOptInt(tweet->ReplyToStatusId, val);
      } else if (name == in_reply_to_user_id_str) {
        GetOptInt(tweet->ReplyToUserId, val);
      } else if (name == retweet_count_str) {
        tweet->RetweetCount = GetInt(val);
      } else if (name == favorite_count_str) {
        tweet->FavoriteCount = GetInt(val);
      } else if (name == user_str) {
        TUserObj user;
        GetUserObj(user, val);
        tweet->UserId = user.Id;
        tweet->UserName = user.Name;
      }
    };
    cb(member_list->GetDictMember(), Tweet);
    TTailVisitor vis(cb, Tweet);
    member_list->GetOptDictMemberListTail()->Accept(vis);
  }

  private:

  TTweetObj *Tweet;

};  // TExprVisitor

class TImportStmtVisitor : public TImportStmt::TVisitor {
  NO_COPY(TImportStmtVisitor);
  public:

  TImportStmtVisitor(Atom::TCoreVectorBuilder & builder) : Builder(builder) {}

  virtual void operator()(const Orly::Data::Twitter::Import::Syntax::TDataRow *that) const {
    auto expr = that->GetExpr();
    TTweetObj tweet;
    TExprVisitor vis(&tweet);
    expr->Accept(vis);
    /*
    printf("[%p] Tweet Id=[%ld], user=[%ld, %s], in_reply_to_status_id=[%ld], in_reply_to_user_id=[%ld], retweet_count=[%ld], favorite_count=[%ld], text=[%s]\n",
           &tweet,
           tweet.Id,
           tweet.UserId,
           tweet.UserName.c_str(),
           (tweet.ReplyToStatusId ? *tweet.ReplyToStatusId : -1),
           (tweet.ReplyToUserId ? *tweet.ReplyToUserId : -1),
           tweet.RetweetCount,
           tweet.FavoriteCount,
           tweet.Text.c_str());
           */
    const int64_t num_kv = 2 + ((tweet.ReplyToStatusId != -1) ? 1 : 0) + ((tweet.ReplyToUserId != -1) ? 2 : 0);
    Builder.Push(TUuid(TUuid::Twister));  // transaction id
    Builder.Push(1L);  // meta id
    Builder.Push(num_kv);  // num KV
    Builder.Push(TweetUserIndexId);  // kv idx id
    Builder.Push(make_tuple(TweetUserInt, tweet.Id, tweet.UserId));  // key
    Builder.Push(make_tuple(tweet.Id, tweet.Text, tweet.UserName, tweet.RetweetCount, tweet.FavoriteCount, tweet.ReplyToStatusId, tweet.ReplyToUserId));  // val
    Builder.Push(TweetUserIndexId);  // kv idx id
    Builder.Push(make_tuple(UserTweetInt, tweet.UserId, tweet.Id));  // key
    Builder.Push(make_tuple(tweet.Id, tweet.Text, tweet.UserName, tweet.RetweetCount, tweet.FavoriteCount, tweet.ReplyToStatusId, tweet.ReplyToUserId));  // val
    //printf("UserId [%ld][%s]\n", tweet.UserId, tweet.Text.c_str());
    if (tweet.ReplyToStatusId != -1) {
      Builder.Push(UserReplyStatusIndexId);  // kv idx id
      Builder.Push(make_tuple(tweet.UserId, tweet.ReplyToStatusId, tweet.Id));  // key
      Builder.Push(true);  // val (different type used to specify different index)
    }
    if (tweet.ReplyToUserId != -1) {
      Builder.Push(UserReplyUserIndexId);  // kv idx id
      Builder.Push(make_tuple(tweet.UserId, tweet.ReplyToUserId, tweet.Id));  // key
      Builder.Push(0L);  // val (different type used to specify different index)
      Builder.Push(ReplyUserToUserIndexId);  // kv idx id
      Builder.Push(make_tuple(tweet.ReplyToUserId, tweet.UserId, tweet.Id));  // key
      Builder.Push("");  // val (different type used to specify different index)
    }
  }

  private:

  Atom::TCoreVectorBuilder &Builder;

};  // TImportStmtVisitor

class TOptImportStmtSeqVisitor : public TOptImportStmtSeq::TVisitor {
  NO_COPY(TOptImportStmtSeqVisitor);
  public:

  TOptImportStmtSeqVisitor(std::list<const TImportStmt *> &check_stmt_list) : CheckStmtList(check_stmt_list) {}

  virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TNoImportStmtSeq */*that*/) const {}
  virtual void operator()(const ::Orly::Data::Twitter::Import::Syntax::TImportStmtSeq *that) const {
    CheckStmtList.push_back(that->GetImportStmt());
    auto opt_check_stmt = that->GetOptImportStmtSeq();
    opt_check_stmt->Accept(*this);
  }

  private:

  std::list<const TImportStmt *> &CheckStmtList;

};

int main(int argc, char *argv[]) {
  printf("use 'split -l50000 -d --additional-suffix=.json INPUT OUTPUT' if the input file is large.\n");
  if (argc != 2) {
    throw std::runtime_error("Invalid number of arguments");
  }
  const std::string file_pattern(argv[1]);
  std::vector<string> file_vec;
  Base::Glob(file_pattern.c_str(), [&file_vec](const char *file) {
    file_vec.push_back(string(file));
    return true;
  });
  for (const auto &file : file_vec) {
    printf("Parsing File [%s]\n", file.c_str());

    auto Import = Orly::Data::Twitter::Import::Syntax::TImport::ParseFile(file.c_str());
    if (Tools::Nycr::TError::GetFirstError()) {
      Tools::Nycr::TError::PrintSortedErrors(std::cerr);
      throw Rt::TSystemError(HERE, "Error loading checkpoint file");
    }
    auto opt_checkpoint_stmt_seq = Import->GetOptImportStmtSeq();
    std::list<const TImportStmt *> check_stmt_list;
    TOptImportStmtSeqVisitor opt_check_vis(check_stmt_list);
    opt_checkpoint_stmt_seq->Accept(opt_check_vis);
    printf("Found [%ld] records\n", check_stmt_list.size());
    Atom::TCoreVectorBuilder builder;
    int64_t num_trans = check_stmt_list.size();
    builder.Push(num_trans);  // num transactions
    builder.Push(num_trans);  // dummy meta data
    TImportStmtVisitor stmt_vis(builder);
    for (auto stmt : check_stmt_list) {
      stmt->Accept(stmt_vis);
    }
    std::string::size_type slash_pos = file.rfind('/');
    std::string new_file_name;
    if (slash_pos != std::string::npos) {
      new_file_name = file.substr(slash_pos + 1, file.size());
    } else {
      new_file_name = file;
    }
    std::string::size_type dot_pos = new_file_name.rfind('.');
    if (dot_pos != std::string::npos) {
      new_file_name = new_file_name.substr(0, dot_pos) + ".bin";
    } else {
      new_file_name = new_file_name + ".bin";
    }
    printf("generating output file [%s]\n", new_file_name.c_str());
    Io::TBinaryOutputOnlyStream strm(make_shared<Io::TDevice>(open(new_file_name.c_str(), O_WRONLY | O_CREAT, 0777)));
    builder.Write(strm);
  }
  return EXIT_SUCCESS;
}
