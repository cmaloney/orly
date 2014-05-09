/* <orly/data/beer.cc>

   Generates a core-vector file with beer data

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
(0) {"name":"Beer"}
(1) {"name":"Trappist Beer"}
(2) {"name":"Orval"}
(3) {"name":"Westmalle"}
(4) {"name":"La Trappe"}
(5) {"name":"Chimay"}
(6) {"name":"Westvleteren"}
(7) {"name":"Local Beer"}
(8) {"name":"De Koninck"}
(9) {"name":"Seef"}
(10) {"name":"Specialty Beer"}
(11) {"name":"Malheur"}
(12) {"name":"Commodity Beer"}
(13) {"name":"Leffe"}
(14) {"name":"Duvel"}
(15) {"name":"Rodenbach"}
(1)-[:isa]->(0)
(2)-[:isa]->(1)
(3)-[:isa]->(1)
(4)-[:isa]->(1)
(5)-[:isa]->(1)
(6)-[:isa]->(1)
(7)-[:isa]->(0)
(8)-[:isa]->(7)
(9)-[:isa]->(7)
(10)-[:isa]->(0)
(11)-[:isa]->(10)
(12)-[:isa]->(0)
(13)-[:isa]->(12)
(14)-[:isa]->(12)
(15)-[:isa]->(12)

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

void Isa(Atom::TCoreVectorBuilder &builder,
              int64_t from,
              int64_t to) {
  builder.Push(TUuid(TUuid::Twister));
  builder.Push(1L);
  builder.Push(1L);
  builder.Push(EdgeIndexId);
  builder.Push(make_tuple(from, "isa", to));
  builder.Push(true);
}

int main(int /*argc*/, char */*argv*/[]) {
  const int64_t num_trans = 31L;
  Atom::TCoreVectorBuilder builder;
  builder.Push(num_trans);  // num transactions
  builder.Push(num_trans);  // dummy meta data
  Node(builder, 0, "Beer");
  Node(builder, 1, "Trappist Beer");
  Node(builder, 2, "Orval");
  Node(builder, 3, "Westmalle");
  Node(builder, 4, "La Trappe");
  Node(builder, 5, "Chimay");
  Node(builder, 6, "Westvleteren");
  Node(builder, 7, "Local Beer");
  Node(builder, 8, "De Koninck");
  Node(builder, 9, "Seef");
  Node(builder, 10, "Specialty Beer");
  Node(builder, 11, "Malheur");
  Node(builder, 12, "Commodity Beer");
  Node(builder, 13, "Leffe");
  Node(builder, 14, "Duvel");
  Node(builder, 15, "Rodenbach");
  Isa(builder, 1, 0);
  Isa(builder, 2, 1);
  Isa(builder, 3, 1);
  Isa(builder, 4, 1);
  Isa(builder, 5, 1);
  Isa(builder, 6, 1);
  Isa(builder, 7, 0);
  Isa(builder, 8, 7);
  Isa(builder, 9, 7);
  Isa(builder, 10, 0);
  Isa(builder, 11, 10);
  Isa(builder, 12, 0);
  Isa(builder, 13, 12);
  Isa(builder, 14, 12);
  Isa(builder, 15, 12);
  assert(builder.GetCores().size() == 2UL + num_trans * 6UL);
  Io::TBinaryOutputOnlyStream strm(make_shared<Io::TDevice>(open("beer.bin", O_WRONLY | O_CREAT, 0777)));
  builder.Write(strm);
  return EXIT_SUCCESS;
}
