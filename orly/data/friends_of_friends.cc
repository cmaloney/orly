/* <orly/data/friends_of_friends.cc>

   Generates a core-vector file with friends_of_friends data

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

#include <fcntl.h>
#include <unistd.h>

#include <io/binary_output_only_stream.h>
#include <io/device.h>
#include <orly/atom/core_vector_builder.h>

using namespace std;
using namespace Base;
using namespace Orly;

/*
(0) {"name":"Animals"}
(1) {"name":"Hobby"}
(2) {"name":"Surfing"}
(3) {"name":"Sara"}
(4) {"name":"Cats"}
(5) {"name":"Derrick"}
(6) {"name":"Horses"}
(7) {"name":"Bikes"}
(8) {"name":"Joe"}
(2)-[:tagged]->(1)
(3)-[:favorite]->(7)
(3)-[:favorite]->(6)
(4)-[:tagged]->(0)
(5)-[:favorite]->(7)
(6)-[:tagged]->(0)
(7)-[:tagged]->(1)
(8)-[:favorite]->(4)
(8)-[:favorite]->(6)
(8)-[:favorite]->(7)
(8)-[:favorite]->(2)
*/

static const TUuid NodeIndexId(TUuid(TUuid::Twister));
static const TUuid EdgeIndexId(TUuid(TUuid::Twister));

void Node(Atom::TCoreVectorBuilder &builder, int64_t id, const string &name) {
  builder.Push(TUuid(TUuid::Twister));
  builder.Push(1L);
  builder.Push(1L);
  builder.Push(NodeIndexId);
  builder.Push(make_tuple(id));
  builder.Push(name);
}

void MakeEdge(Atom::TCoreVectorBuilder &builder,
              const std::string &kind,
              int64_t from,
              int64_t to) {
  builder.Push(TUuid(TUuid::Twister));
  builder.Push(1L);
  builder.Push(1L);
  builder.Push(EdgeIndexId);
  builder.Push(make_tuple(from, kind, to));
  builder.Push(true);
}

void Tagged(Atom::TCoreVectorBuilder &builder, int64_t from, int64_t to) {
  MakeEdge(builder, "Tagged", from, to);
}

void Favorite(Atom::TCoreVectorBuilder &builder, int64_t from, int64_t to) {
  MakeEdge(builder, "Favorite", from, to);
}

int main(int /*argc*/, char */*argv*/[]) {
  const int64_t num_trans = 20L;
  Atom::TCoreVectorBuilder builder;
  builder.Push(num_trans);  // num transactions
  builder.Push(num_trans);  // dummy meta data
  Node(builder, 0, "Animals");
  Node(builder, 1, "Hobby");
  Node(builder, 2, "Surfing");
  Node(builder, 3, "Sara");
  Node(builder, 4, "Cats");
  Node(builder, 5, "Derrick");
  Node(builder, 6, "Horses");
  Node(builder, 7, "Bikes");
  Node(builder, 8, "Joe");
  Tagged(builder, 2, 1);
  Favorite(builder, 3, 7);
  Favorite(builder, 3, 6);
  Tagged(builder, 4, 0);
  Favorite(builder, 5, 7);
  Tagged(builder, 6, 0);
  Tagged(builder, 7, 1);
  Favorite(builder, 8, 4);
  Favorite(builder, 8, 6);
  Favorite(builder, 8, 7);
  Favorite(builder, 8, 2);
  assert(builder.GetCores().size() == 2UL + num_trans * 6UL);
  Io::TBinaryOutputOnlyStream strm(make_shared<Io::TDevice>(open("friends_of_friends.bin", O_WRONLY | O_CREAT, 0777)));
  builder.Write(strm);
  return EXIT_SUCCESS;
}
