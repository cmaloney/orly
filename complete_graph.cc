/* <orly/data/complete_graph.cc>

   Generates a core-vector file with complete graph data

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
(1) {"count":1}
(2) {"count":2}
(3) {"count":3}
(4) {"count":4}
(5) {"count":5}
(6) {"count":6}
(1)-[:X]->(6)
(1)-[:X]->(5)
(1)-[:X]->(4)
(1)-[:X]->(3)
(1)-[:X]->(2)
(2)-[:X]->(6)
(2)-[:X]->(5)
(2)-[:X]->(4)
(2)-[:X]->(3)
(3)-[:X]->(6)
(3)-[:X]->(5)
(3)-[:X]->(4)
(4)-[:X]->(6)
(4)-[:X]->(5)
(5)-[:X]->(6)
*/

static const TUuid NodeIndexId(TUuid(TUuid::Twister));
static const TUuid EdgeIndexId(TUuid(TUuid::Twister));

void Node(Atom::TCoreVectorBuilder &builder, int64_t id, int64_t count) {
  builder.Push(TUuid(TUuid::Twister));
  builder.Push(1L);
  builder.Push(1L);
  builder.Push(NodeIndexId);
  builder.Push(make_tuple(id));
  builder.Push(count);
}

void Edge(Atom::TCoreVectorBuilder &builder,
              int64_t from,
              int64_t to) {
  builder.Push(TUuid(TUuid::Twister));
  builder.Push(1L);
  builder.Push(1L);
  builder.Push(EdgeIndexId);
  builder.Push(make_tuple(from, to));
  builder.Push(true);
}

int main(int /*argc*/, char */*argv*/[]) {
  const int64_t num_trans = 21L;
  Atom::TCoreVectorBuilder builder;
  builder.Push(num_trans);  // num transactions
  builder.Push(num_trans);  // dummy meta data
  Node(builder, 1, 1);
  Node(builder, 2, 2);
  Node(builder, 3, 3);
  Node(builder, 4, 4);
  Node(builder, 5, 5);
  Node(builder, 6, 6);
  Edge(builder, 1, 6);
  Edge(builder, 1, 5);
  Edge(builder, 1, 4);
  Edge(builder, 1, 3);
  Edge(builder, 1, 2);
  Edge(builder, 2, 6);
  Edge(builder, 2, 5);
  Edge(builder, 2, 4);
  Edge(builder, 2, 3);
  Edge(builder, 3, 6);
  Edge(builder, 3, 5);
  Edge(builder, 3, 4);
  Edge(builder, 4, 6);
  Edge(builder, 4, 5);
  Edge(builder, 5, 6);
  assert(builder.GetCores().size() == 2UL + num_trans * 6UL);
  Io::TBinaryOutputOnlyStream strm(make_shared<Io::TDevice>(open("complete_graph.bin", O_WRONLY | O_CREAT, 0777)));
  builder.Write(strm);
  return EXIT_SUCCESS;
}
