/* <orly/data/game_of_thrones.cc>

   Generates a core-vector file with game of thrones data

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
(0) {"name":"Westeros"}
(1) {"house":"Targaryen"}
(2) {"house":"Stark"}
(3) {"house":"Lannister"}
(4) {"house":"Baratheon"}
(5) {"house":"Tully"}
(6) {"name":"Danaerys"}
(7) {"name":"Khal Drogo"}
(8) {"name":"Tywin"}
(9) {"name":"Steffon"}
(10) {"name":"Rickard"}
(11) {"name":"Eddard"}
(12) {"name":"Catelyn"}
(13) {"name":"Jon"}
(14) {"name":"Robb"}
(15) {"name":"Bran"}
(16) {"name":"Arya"}
(17) {"name":"Sansa"}
(18) {"name":"Rickon"}
(19) {"name":"Cersei"}
(20) {"name":"Jamie"}
(21) {"name":"Tyrion"}
(22) {"name":"Robert"}
(23) {"name":"Renly"}
(24) {"name":"Stannis"}
(25) {"name":"Joffrey"}
(26) {"name":"Myrcella"}
(27) {"name":"Tommen"}
(28) {"name":"Tyrion"}
(1)-[:HOUSE]->(0)
(2)-[:HOUSE]->(0)
(3)-[:HOUSE]->(0)
(4)-[:HOUSE]->(0)
(5)-[:HOUSE]->(0)
(6)-[:OF_HOUSE]->(1)
(6)-[:MARRIED_TO]->(7)
(8)-[:OF_HOUSE]->(3)
(9)-[:OF_HOUSE]->(4)
(10)-[:OF_HOUSE]->(2)
(11)-[:CHILD_OF]->(10)
(12)-[:OF_HOUSE]->(5)
(12)-[:MARRIED_TO]->(11)
(13)-[:CHILD_OF]->(11)
(14)-[:CHILD_OF]->(12)
(14)-[:CHILD_OF]->(11)
(15)-[:CHILD_OF]->(12)
(15)-[:CHILD_OF]->(11)
(16)-[:CHILD_OF]->(12)
(16)-[:CHILD_OF]->(11)
(17)-[:MARRIED_TO]->(28)
(17)-[:CHILD_OF]->(12)
(17)-[:CHILD_OF]->(11)
(17)-[:PROMISED_TO]->(25)
(18)-[:CHILD_OF]->(12)
(18)-[:CHILD_OF]->(11)
(19)-[:MARRIED_TO]->(22)
(19)-[:CHILD_OF]->(8)
(20)-[:CHILD_OF]->(8)
(21)-[:CHILD_OF]->(8)
(22)-[:CHILD_OF]->(9)
(23)-[:CHILD_OF]->(9)
(24)-[:CHILD_OF]->(9)
(25)-[:CHILD_OF]->(19)
(25)-[:CHILD_OF]->(20)
(26)-[:CHILD_OF]->(19)
(26)-[:CHILD_OF]->(20)
(27)-[:CHILD_OF]->(19)
(27)-[:CHILD_OF]->(20)
(28)-[:CHILD_OF]->(9)
*/

static const TUuid NodeIndexId(TUuid(TUuid::Twister));
static const TUuid EdgeIndexId(TUuid(TUuid::Twister));

void Node(Atom::TCoreVectorBuilder &builder, int64_t id, const string &kind, const string &val) {
  builder.Push(TUuid(TUuid::Twister));
  builder.Push(1L);
  builder.Push(1L);
  builder.Push(NodeIndexId);
  builder.Push(make_tuple(id, kind));
  builder.Push(val);
}

void Name(Atom::TCoreVectorBuilder &builder, int64_t id, const string &val) {
  Node(builder, id, "name", val);
}

void HouseNode(Atom::TCoreVectorBuilder &builder, int64_t id, const string &val) {
  Node(builder, id, "house", val);
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

void House(Atom::TCoreVectorBuilder &builder, int64_t from, int64_t to) {
  MakeEdge(builder, "HOUSE", from, to);
}

void OfHouse(Atom::TCoreVectorBuilder &builder, int64_t from, int64_t to) {
  MakeEdge(builder, "OF_HOUSE", from, to);
}

void MarriedTo(Atom::TCoreVectorBuilder &builder, int64_t from, int64_t to) {
  MakeEdge(builder, "MARRIED_TO", from, to);
}

void ChildOf(Atom::TCoreVectorBuilder &builder, int64_t from, int64_t to) {
  MakeEdge(builder, "CHILD_OF", from, to);
}

void PromisedTo(Atom::TCoreVectorBuilder &builder, int64_t from, int64_t to) {
  MakeEdge(builder, "PROMISED_TO", from, to);
}

int main(int /*argc*/, char */*argv*/[]) {
  const int64_t num_trans = 69L;
  Atom::TCoreVectorBuilder builder;
  builder.Push(num_trans);  // num transactions
  builder.Push(num_trans);  // dummy meta data
  Name(builder, 0, "Westeros");
  HouseNode(builder, 1, "Targaryen");
  HouseNode(builder, 2, "Stark");
  HouseNode(builder, 3, "Lannister");
  HouseNode(builder, 4, "Baratheon");
  HouseNode(builder, 5, "Tully");
  Name(builder, 6, "Danaerys");
  Name(builder, 7, "Khal Drogo");
  Name(builder, 8, "Tywin");
  Name(builder, 9, "Steffon");
  Name(builder, 10, "Rickard");
  Name(builder, 11, "Eddard");
  Name(builder, 12, "Catelyn");
  Name(builder, 13, "Jon");
  Name(builder, 14, "Robb");
  Name(builder, 15, "Bran");
  Name(builder, 16, "Arya");
  Name(builder, 17, "Sansa");
  Name(builder, 18, "Rickon");
  Name(builder, 19, "Cersei");
  Name(builder, 20, "Jamie");
  Name(builder, 21, "Tyrion");
  Name(builder, 22, "Robert");
  Name(builder, 23, "Renly");
  Name(builder, 24, "Stannis");
  Name(builder, 25, "Joffrey");
  Name(builder, 26, "Myrcella");
  Name(builder, 27, "Tommen");
  Name(builder, 28, "Tyrion");
  House(builder, 1, 0);
  House(builder, 2, 0);
  House(builder, 3, 0);
  House(builder, 4, 0);
  House(builder, 5, 0);
  OfHouse(builder, 6, 1);
  MarriedTo(builder, 6, 7);
  OfHouse(builder, 8, 3);
  OfHouse(builder, 9 ,4);
  OfHouse(builder, 10 ,2);
  ChildOf(builder, 11, 10);
  OfHouse(builder, 12, 5);
  MarriedTo(builder, 12, 11);
  ChildOf(builder, 13, 11);
  ChildOf(builder, 14, 12);
  ChildOf(builder, 14, 11);
  ChildOf(builder, 15, 12);
  ChildOf(builder, 15, 11);
  ChildOf(builder, 16, 12);
  ChildOf(builder, 16, 11);
  MarriedTo(builder, 17, 28);
  ChildOf(builder, 17, 12);
  ChildOf(builder, 17, 11);
  PromisedTo(builder, 17, 25);
  ChildOf(builder, 18, 12);
  ChildOf(builder, 18, 11);
  MarriedTo(builder, 19, 22);
  ChildOf(builder, 19, 8);
  ChildOf(builder, 20, 8);
  ChildOf(builder, 21, 8);
  ChildOf(builder, 22, 9);
  ChildOf(builder, 23, 9);
  ChildOf(builder, 24, 9);
  ChildOf(builder, 25, 19);
  ChildOf(builder, 25, 20);
  ChildOf(builder, 26, 19);
  ChildOf(builder, 26, 20);
  ChildOf(builder, 27, 19);
  ChildOf(builder, 27, 20);
  ChildOf(builder, 28, 9);
  assert(builder.GetCores().size() == 2UL + num_trans * 6UL);
  Io::TBinaryOutputOnlyStream strm(make_shared<Io::TDevice>(open("game_of_thrones.bin", O_WRONLY | O_CREAT, 0777)));
  builder.Write(strm);
  return EXIT_SUCCESS;
}
