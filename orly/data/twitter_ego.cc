/* <orly/data/twitter_ego.cc>

   Generates a core-vector file for twitter EGO-network.

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

#include <algorithm>
#include <cctype>
#include <fstream>
#include <memory>
#include <tuple>
#include <vector>

#include <sys/stat.h>
#include <fcntl.h>

#include <base/as_str.h>
#include <base/glob.h>
#include <base/hash.h>
#include <base/path.h>
#include <base/split.h>
#include <base/uuid.h>
#include <io/binary_output_only_stream.h>
#include <io/device.h>
#include <orly/atom/core_vector_builder.h>

using TFollows = std::vector<std::pair<int64_t, int64_t>>;
using THashtags = std::vector<std::pair<int64_t, std::string>>;
using TMentions = std::vector<std::pair<int64_t, std::string>>;

static const auto FollowsIndexId(Base::TUuid(Base::TUuid::Twister));
static const auto HashtagsIndexId(Base::TUuid(Base::TUuid::Twister));
static const auto MentionsIndexId(Base::TUuid(Base::TUuid::Twister));

std::tuple<TFollows, THashtags, TMentions> Parse() {
  bool has_feature;
  int64_t from, to, idx, uid;
  std::string feature;
  TFollows follows;
  THashtags hashtags;
  TMentions mentions;
  Base::Glob("twitter-ego.in/*.edges",
             [&](const char *filename) {
               const Base::TPath path(filename);
               int64_t ego = std::stoll(path.Name);
               // Parse '*.edges' file.
               std::ifstream edges(filename);
               while (edges >> from >> to) {
                 follows.emplace_back(from, to);
                 follows.emplace_back(ego, from);
                 follows.emplace_back(ego, to);
               }  // while
               auto basename =
                   "/" + Base::AsStr(Base::Join(path.ToNamespaceIncludingName(), '/'));
               // Parse the '*.featnames' file.
               std::vector<std::string> features;
               std::ifstream featnames(Base::AsStr(basename, ".featnames"));
               while (featnames >> idx >> feature) {
                 features.push_back(std::move(feature));
               }  // while
               auto parse_features = [&](std::istream &strm, int64_t uid) {
                 for (const auto &feature : features) {
                   strm >> has_feature;
                   if (!has_feature) {
                     continue;
                   }  // if
                   auto begin = std::begin(feature) + 1;
                   auto end = std::find_if_not(
                       begin,
                       std::end(feature),
                       [](char ch) {
                         return std::isalnum(ch) || ch == '_';
                       });
                   switch (feature.at(0)) {
                     case '#': {
                       hashtags.emplace_back(uid, std::string(begin, end));
                       break;
                     }  // case
                     case '@': {
                       mentions.emplace_back(uid, std::string(begin, end));
                       break;
                     }  // case
                   }  // switch
                 }  // for
               };
               // Parse the '*.egofeat' file.
               std::ifstream egofeat(Base::AsStr(basename, ".egofeat"));
               parse_features(egofeat, ego);
               // Parse the '*.feat' file.
               std::ifstream feat(Base::AsStr(basename, ".feat"));
               while (feat >> uid) {
                 parse_features(feat, uid);
               }  // while
               return true;
             });
  auto unique = [](auto &container) {
    std::sort(std::begin(container), std::end(container));
    auto last = std::unique(std::begin(container), std::end(container));
    container.erase(last, std::end(container));
  };
  unique(follows);
  unique(hashtags);
  unique(mentions);
  return std::make_tuple(
      std::move(follows), std::move(hashtags), std::move(mentions));
}

int main() {
  const auto result = Parse();
  const auto &follows = std::get<0>(result);
  const auto &hashtags = std::get<1>(result);
  const auto &mentions = std::get<2>(result);
  std::cout << "Found " << follows.size() << " edges" << std::endl;
  std::cout << "Found " << hashtags.size() << " hashtags" << std::endl;
  std::cout << "Found " << mentions.size() << " mentions" << std::endl;
  constexpr const std::size_t chunk_size = 300000;
  std::size_t file_count = 0;
  auto write = [&](
      const auto &edges, const std::string &kind, const Base::TUuid &index_id) {
    std::size_t chunk_count = 0;
    for (auto iter = std::begin(edges);
         iter < std::end(edges);
         ++chunk_count, ++file_count) {
      int64_t num_transactions =
          std::min(chunk_size, edges.size() - chunk_count * chunk_size);
      Orly::Atom::TCoreVectorBuilder builder;
      builder.Push(num_transactions);  // Number of transactions
      builder.Push(num_transactions);  // Dummy meta-data.
      for (int64_t j = 0; j < num_transactions; ++j, ++iter) {
        const auto &edge = *iter;
        builder.Push(Base::TUuid(Base::TUuid::Twister));  // Transaction id.
        builder.Push(1L);  // Dummy meta-id.
        builder.Push(1L);  // Number of kv-pairs.
        builder.Push(index_id);  // index-id.
        builder.Push(
            std::make_tuple(kind, edge.first, edge.second));  // Key
        builder.Push(true);  // Val
      }  // for
      auto outfile =
          Base::AsStr("twitter-ego.out/twitter-ego", file_count, ".bin");
      Io::TBinaryOutputOnlyStream strm(std::make_shared<Io::TDevice>(
              open(outfile.data(), O_WRONLY | O_CREAT, 0777)));
      builder.Write(strm);
      std::cout << "Wrote " << outfile << std::endl;
    }  // for
  };
  write(follows, "follow", FollowsIndexId);
  write(hashtags, "#", HashtagsIndexId);
  write(mentions, "@", MentionsIndexId);
}
