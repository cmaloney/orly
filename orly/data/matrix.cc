/* <orly/data/matrix.cc>

   Generates a core-vector file with matrix data

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
(0) {"name":"Neo"}
(1) {"name":"Morpheus"}
(2) {"name":"Trinity"}
(3) {"name":"Cypher"}
(4) {"name":"Agent Smith"}
(5) {"name":"The Architect"}
(0)-[:KNOWS]->(1)
(0)-[:LOVES]->(2)
(1)-[:KNOWS]->(2)
(1)-[:KNOWS]->(3)
(3)-[:KNOWS]->(4)
(4)-[:CODED_BY]->(5)
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

void Knows(Atom::TCoreVectorBuilder &builder, int64_t from, int64_t to) {
  MakeEdge(builder, "Knows", from, to);
}

void Loves(Atom::TCoreVectorBuilder &builder, int64_t from, int64_t to) {
  MakeEdge(builder, "Loves", from, to);
}

void CodedBy(Atom::TCoreVectorBuilder &builder, int64_t from, int64_t to) {
  MakeEdge(builder, "CodedBy", from, to);
}

int main(int /*argc*/, char */*argv*/[]) {
  const int64_t num_trans = 12L;
  Atom::TCoreVectorBuilder builder;
  builder.Push(num_trans);  // num transactions
  builder.Push(num_trans);  // dummy meta data
  Node(builder, 0, "Neo");
  Node(builder, 1, "Morpheus");
  Node(builder, 2, "Trinity");
  Node(builder, 3, "Cypher");
  Node(builder, 4, "Agent Smith");
  Node(builder, 5, "The Architect");
  Knows(builder, 0, 1);
  Loves(builder, 0, 2);
  Knows(builder, 1, 2);
  Knows(builder, 1, 3);
  Knows(builder, 3, 4);
  CodedBy(builder, 4, 5);
  assert(builder.GetCores().size() == 2UL + num_trans * 6UL);
  Io::TBinaryOutputOnlyStream strm(make_shared<Io::TDevice>(open("matrix.bin", O_WRONLY | O_CREAT, 0777)));
  builder.Write(strm);
  return EXIT_SUCCESS;
}
