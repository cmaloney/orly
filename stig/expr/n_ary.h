/* <stig/expr/n_ary.h>

   TODO

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

#pragma once

#include <array>
#include <map>
#include <unordered_map>
#include <vector>

#include <base/no_copy_semantics.h>
#include <stig/shared_enum.h>
#include <stig/expr/interior.h>
#include <stig/pos_range.h>

namespace Stig {

  namespace Expr {

    template <typename TContainer>
    class TNAry
        : public TInterior {
      NO_COPY_SEMANTICS(TNAry);
      public:

      typedef std::function<void (const TExpr::TPtr &expr)> TCallback;

      /* TList and TSet */
      template <typename TCompatContainer>
      static void ForEachExpr(const TCompatContainer &container, const TCallback &cb) {
        assert(&container);
        assert(&cb);
        assert(cb);
        for (auto elem : container) {
          cb(elem);
        }
      }

      /* TIfElse, TRange */
      static void ForEachExpr(const std::array<TExpr::TPtr, 3U> &exprs, const TCallback &cb) {
        assert(&exprs);
        assert(&cb);
        assert(cb);
        for (auto expr : exprs) {
          if (expr) {
            cb(expr);
          }
        }
      }

      /* TAddr */
      static void ForEachExpr(
          const std::vector<std::pair<TAddrDir, TExpr::TPtr>> &addr_members,
          const TCallback &cb) {
        assert(&addr_members);
        assert(&cb);
        assert(cb);
        for (auto addr_member : addr_members) {
          cb(addr_member.second);
        }
      }

      /* TDict */
      static void ForEachExpr(
          const std::unordered_map<TExpr::TPtr, TExpr::TPtr> &dict_members,
          const TCallback &cb) {
        assert(&dict_members);
        assert(&cb);
        assert(cb);
        for (auto dict_member : dict_members) {
          cb(dict_member.first);
          cb(dict_member.second);
        }
      }

      /* TObj */
      static void ForEachExpr(
          const std::map<std::string, TExpr::TPtr> &obj_members,
          const TCallback &cb) {
        assert(&obj_members);
        assert(&cb);
        assert(cb);
        for (auto obj_member : obj_members) {
          cb(obj_member.second);
        }
      }

      ~TNAry() {
        assert(this);
        ForEachExpr(GetContainer(), [this](const TExpr::TPtr &expr) { expr->UnsetExprParent(this); });
      }

      const TContainer &GetContainer() const {
        assert(this);
        return Container;
      }

      protected:

      /* Empty constructor */
      TNAry(const TPosRange &pos_range)
          : TInterior(pos_range) {}

      /* Non-empty constructor */
      TNAry(const TContainer &container, const TPosRange &pos_range)
          : TInterior(pos_range), Container(container) {
        ForEachExpr(GetContainer(), [this](const TExpr::TPtr &expr) { expr->SetExprParent(this); });
      }

      private:

      TContainer Container;

    };  // TNAry

  }  // Expr

}  // Stig
