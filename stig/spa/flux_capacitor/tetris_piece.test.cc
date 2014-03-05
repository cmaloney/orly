/* <stig/spa/flux_capacitor/tetris_piece.test.cc>

   Unit test for <stig/spa/flux_capacitor/tetris_piece.h>.

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

#include <stig/spa/flux_capacitor/tetris_piece.h>

#include <initializer_list>
#include <unordered_map>

#include <stig/type.h>

#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Stig;
using namespace Stig::Spa::FluxCapacitor;

typedef unordered_map<Var::TVar, Var::TVar> TKvMap;

const TKvMap EmptyKvMap;

const Var::TVar DefaultVar;

namespace Stig { namespace Spa { namespace FluxCapacitor {

  class TContext {
    NO_COPY_SEMANTICS(TContext);
    public:

    TContext(TKvMap &kv_map)
        : KvMap(kv_map) {}

    const Var::TVar &operator[](const Var::TVar &key) const {
      assert(this);
      auto iter = KvMap.find(key);
      return (iter != KvMap.end()) ? iter->second : DefaultVar;
    }

    TKvMap &KvMap;

  };

} } }

typedef function<bool (TContext &)> TAssertFunc;

typedef function<void ()> TUpdateFunc;

bool NoAssert(TContext &) {
  return true;
}

class TMyPiece
    : public TTetrisPiece {
  NO_COPY_SEMANTICS(TMyPiece);
  public:

  enum TStatus { Undecided, Promoted, Failed };

  TMyPiece(initializer_list<TKey> keys, const TAssertFunc &assert_func, const TUpdateFunc &update_func, size_t max_age)
      : TTetrisPiece(max_age), Status(Undecided), Keys(keys), AssertFunc(assert_func), UpdateFunc(update_func) {}

  virtual bool Assert(TContext &ctxt) const {
    return AssertFunc(ctxt);
  }

  virtual void Fail() {
    assert(this);
    Status = Failed;
  }

  virtual bool ForEachKey(const function<bool (const TKey &)> &cb) const {
    assert(this);
    assert(cb);
    for (auto key: Keys) {
      if (!cb(key)) {
        return false;
      }
    }
    return true;
  }

  virtual size_t GetKeyCount() const {
    assert(this);
    return Keys.size();
  }

  TStatus GetStatus() const {
    assert(this);
    return Status;
  }

  virtual bool Promote() {
    assert(this);
    UpdateFunc();
    Status = Promoted;
    return true;
  }

  private:

  TStatus Status;

  unordered_set<TKey> Keys;

  TAssertFunc AssertFunc;

  TUpdateFunc UpdateFunc;

};

class TMyPlayer {
  NO_COPY_SEMANTICS(TMyPlayer);
  public:

  TMyPlayer() {}

  TMyPlayer(initializer_list<pair<const Var::TVar, Var::TVar>> db)
     : Db(db) {}

  ~TMyPlayer() {
    assert(this);
    for (pair<const int, TMyPiece *> &item: PieceById) {
      delete item.second;
    }
  }

  TMyPiece *GetPiece(int id) const {
    assert(this);
    auto iter = PieceById.find(id);
    assert(iter != PieceById.end());
    return iter->second;
  }

  void NewPiece(
      int id, initializer_list<TMyPiece::TKey> keys,
      const TAssertFunc &assert_func = NoAssert,
      const TKvMap &changes = EmptyKvMap,
      size_t max_age = 0) {
    assert(this);
    TMyPiece *&piece = PieceById.insert(make_pair(id, static_cast<TMyPiece *>(0))).first->second;
    if (piece) {
      delete piece;
      piece = 0;
    }
    piece = new TMyPiece(keys, assert_func, bind(&TMyPlayer::ApplyChanges, this, changes), max_age);
  }

  void PlayTetris() {
    assert(this);
    TContext ctxt(Db);
    TMyPiece::PlayTetris(ctxt, [this](const function<void (TTetrisPiece *)> &cb) {
      for (pair<const int, TMyPiece *> &item: PieceById) {
        auto piece = item.second;
        if (piece && piece->GetStatus() == TMyPiece::Undecided) {
          cb(piece);
        }
      }
    });
  }

  private:

  void ApplyChanges(const TKvMap &changes) {
    assert(this);
    assert(&changes);
    for (auto change: changes) {
      Db[change.first] = change.second;
    }
  }

  unordered_map<int, TMyPiece *> PieceById;

  TKvMap Db;

};

const Var::TVar
    A(string("a")), B(string("b")), C(string("c")),
    X(string("x")), Y(string("y")), Z(string("z")),
    N101(101), N102(102), N103(103);

FIXTURE(NoConflicts) {
  TMyPlayer player;
  player.NewPiece(101, { A });
  player.NewPiece(102, { B });
  player.NewPiece(103, { C });
  player.PlayTetris();
  for (int id = 101; id <= 103; ++id) {
    EXPECT_TRUE(player.GetPiece(id)->GetStatus() == TMyPiece::Promoted);
  }
}

FIXTURE(AllTooOld) {
  TMyPlayer player;
  player.NewPiece(101, { A }, NoAssert, EmptyKvMap, 1);
  player.NewPiece(102, { B }, NoAssert, EmptyKvMap, 1);
  player.NewPiece(103, { C }, NoAssert, EmptyKvMap, 1);
  for (int id = 101; id <= 103; ++id) {
    player.GetPiece(id)->IncrAge();
  }
  player.PlayTetris();
  for (int id = 101; id <= 103; ++id) {
    EXPECT_TRUE(player.GetPiece(id)->GetStatus() == TMyPiece::Failed);
  }
}

FIXTURE(Conflicts) {
  TMyPlayer player({ { A, N101 } });
  player.NewPiece(101, { A }, [](TContext &ctxt){ return ctxt[A] == N101; }, { { A, N102 } });
  player.NewPiece(102, { A }, [](TContext &ctxt){ return ctxt[A] == N101; }, { { A, N103 } });
  player.GetPiece(101)->IncrAge();
  /* Based on assertions and key cross-section, either 101 and 102 could promote.
     However, 101 is older than 102 and so will go first.  It will cause A to change
     such that 102's assertions no longer pass. */
  player.PlayTetris();
  EXPECT_TRUE(player.GetPiece(101)->GetStatus() == TMyPiece::Undecided);
  EXPECT_TRUE(player.GetPiece(102)->GetStatus() == TMyPiece::Promoted);
  player.NewPiece(103, { A }, [](TContext &ctxt){ return ctxt[A] == N102; }, { { A, N101 } });
  /* Based on age, 102 should go before 103; however, 102's assertions do not pass,
     so 103 will promote and 102 will not. */
  player.PlayTetris();
  EXPECT_TRUE(player.GetPiece(103)->GetStatus() == TMyPiece::Undecided);
  EXPECT_TRUE(player.GetPiece(102)->GetStatus() == TMyPiece::Promoted);
  /* The effect of 103 made 102's assertions pass, so 103 will now promote. */
  player.PlayTetris();
  EXPECT_TRUE(player.GetPiece(102)->GetStatus() == TMyPiece::Promoted);
}
