/* <orly/rt/shortest_path.h>

   TODO

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

#pragma once

#include <base/class_traits.h>
#include <base/uuid.h>
#include <orly/indy/fiber/extern_fiber.h>
#include <orly/package/api.h>
#include <orly/package/rt.h>

namespace Orly {

  namespace Rt {

    /* TODO */
    template <typename TKeyType, typename TSearchType, size_t SrcPos, size_t TargetPos>
    std::vector<std::vector<TKeyType>> BidirShortestPath(Orly::Package::TContext &ctx,
                                                         const size_t num_parallel,
                                                         const Base::TUuid &idx_id,
                                                         const TSearchType &val_args,
                                                         const typename std::tuple_element<SrcPos, TKeyType>::type &src,
                                                         const typename std::tuple_element<TargetPos, TKeyType>::type &target) {
      using match_t = typename std::tuple_element<SrcPos, TKeyType>::type;
      static_assert(std::is_same<match_t, typename std::tuple_element<TargetPos, TKeyType>::type>::value, "Cannot follow links of differing types");
      using edge_vec_t = std::vector<TKeyType>;
      std::vector<edge_vec_t> ret;
      Atom::TSuprena arena;
      void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
      TSearchType src_search_key_tuple = val_args;
      std::get<SrcPos>(src_search_key_tuple) = src;
      const Indy::TIndexKey src_search_key(idx_id, Indy::TKey(src_search_key_tuple,
                                                              &arena,
                                                              state_alloc));
      std::unique_ptr<TKeyCursor> src_kcp(ctx.NewKeyCursor(&ctx.GetFlux(), src_search_key));
      TKeyType key;
      // for each edge from our src
      std::unordered_set<match_t> seen_set;
      using sp_map_t = std::unordered_map<match_t, edge_vec_t>;
      sp_map_t sp_to, sp_from;
      seen_set.emplace(src);
      sp_to.emplace(src, edge_vec_t{});
      for (auto &kc = *src_kcp; kc; ++kc) {
        Sabot::ToNative(*Sabot::State::TAny::TWrapper(kc->GetState(state_alloc)), key);
        const match_t &to = std::get<TargetPos>(key);
        // if the edge is our target, we have a winner
        if (to == target) {
          ret.emplace_back(edge_vec_t{key});
        } else {
          sp_to.emplace(to, edge_vec_t{key});
        }
      }
      if (ret.empty()) {
        TSearchType target_search_key_tuple = val_args;
        std::get<SrcPos>(target_search_key_tuple) = target;
        const Indy::TIndexKey target_search_key(idx_id, Indy::TKey(target_search_key_tuple,
                                                                   &arena,
                                                                   state_alloc));
        std::unique_ptr<TKeyCursor> target_kcp(ctx.NewKeyCursor(&ctx.GetFlux(), target_search_key));
        // for each edge to our target
        seen_set.emplace(target);
        sp_from.emplace(target, edge_vec_t{});
        for (auto &kc = *target_kcp; kc; ++kc) {
          Sabot::ToNative(*Sabot::State::TAny::TWrapper(kc->GetState(state_alloc)), key);
          const match_t from = std::get<TargetPos>(key); // don't take by reference as we're going to swap src / target
          // swap the positions since we're searching in reverse
          std::swap(std::get<SrcPos>(key), std::get<TargetPos>(key));
          auto p = sp_to.find(from);
          if (p != sp_to.end()) {
            // we discovered the path src -> from <- target
            ret.emplace_back(edge_vec_t{sp_to.find(from)->second.front(), key});
          }
          sp_from.emplace(from, edge_vec_t{key});
        }
      }
      if (ret.empty()) {
        // if we still don't have a result, we need to start looking at depth > 2
        bool keep_going = true;
        size_t dep = 1UL;
        for (;keep_going; ++dep) {
          size_t num_explored = 0UL;
          /* choose the for-loop path if there are only a few nodes to explore (as opposed to table scan) */
          sp_map_t tmp_sp_map;
          /* remove entries in sp_to that exist in seen_set. We are only interested in the ones we have not explored yet. */ {
            for (auto iter = sp_to.begin(); iter != sp_to.end();) {
              if (seen_set.find(iter->first) != seen_set.end()) {
                auto cp_iter = iter;
                ++iter;
                sp_to.erase(cp_iter);
              } else {
                ++iter;
              }
            }
          }  // done cleaning sp_to
          const size_t expected_to_see = sp_to.size();
          if (expected_to_see == 0) {
            keep_going = false;
            break;
          }
          /* Sub Scan Path */
          size_t hits = 0UL;
          Indy::ExternFiber::TSync extern_sync(num_parallel);
          auto iter = sp_to.begin();
          auto sub_scanner_func = [&]() {
            TKeyType k;
            for (;iter != sp_to.end();) {
              const match_t &match = iter->first;
              assert(seen_set.find(match) == seen_set.end());
              edge_vec_t edge_vec(iter->second.begin(), iter->second.end());
              ++iter; // increment the iterator before we have a chance to block
              ++num_explored;
              std::get<SrcPos>(src_search_key_tuple) = match;
              const Indy::TIndexKey search_key(idx_id, Indy::TKey(src_search_key_tuple,
                                                                  &arena,
                                                                  state_alloc));
              std::unique_ptr<TKeyCursor> kcp(ctx.NewKeyCursor(&ctx.GetFlux(), search_key));
              for (auto &kc = *kcp; kc; ++kc) {
                ++hits;
                Sabot::ToNative(*Sabot::State::TAny::TWrapper(kc->GetState(state_alloc)), k);
                const match_t &to = std::get<TargetPos>(k);
                edge_vec.emplace_back(k);
                auto p = sp_from.find(to);
                if (p != sp_from.end()) {
                  // we've discovered an intersection between sp_to and sp_from, report it
                  edge_vec_t result(edge_vec.begin(), edge_vec.end());
                  auto riter = p->second.rbegin();
                  result.insert(result.end(), riter, p->second.rend());
                  ret.emplace_back(result);
                  keep_going = false;
                }
                tmp_sp_map.emplace(to, edge_vec);
                edge_vec.pop_back();
              }
              seen_set.emplace(match);
            }
            extern_sync.Complete();
          };
          for (size_t i = 0; i < num_parallel; ++i) {
            Indy::ExternFiber::SchedTaskLocally(sub_scanner_func);
          }
          extern_sync.Sync();
          sp_to.insert(tmp_sp_map.begin(), tmp_sp_map.end());
          if (num_explored == 0) {
            // if every node to explore has already been seen, we've explored the whole connected subgraph from the src!
            return ret;
          }
        }
      }
      return ret;
    }

  }  // Rt

}  // Orly
