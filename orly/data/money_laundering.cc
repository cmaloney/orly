/* <orly/data/money_laundering.cc>

   Generates a core-vector file with money laundering data

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
(0) {"name":"ATM1"}                                        }
(1) {"name":"ATM2"}
(2) {"name":"Crook nr 1"}
(3) {"name":"Crook nr 2"}
(4) {"name":"Crook nr 3"}
(5) {"name":"NormalPerson"}
(6) {"name":"Restaurant"}
(7) {"name":"Bank"}
(2)-[:spendsmoneyon]->(0) {"money":800}
(2)-[:spendsmoneyon]->(1) {"money":1200}
(3)-[:wiresmoney]->(2) {"money":23000}
(4)-[:wiresmoney]->(6) {"money":12000}
(5)-[:spendsmoneyon]->(0) {"money":43}
(5)-[:spendsmoneyon]->(1) {"money":43}
(6)-[:spendsmoneyon]->(1) {"money":45000}
(7)-[:withdrawsmoney]->(3) {"money":23000}
(7)-[:withdrawsmoney]->(4) {"money":12000}
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

void MakeDataEdge(Atom::TCoreVectorBuilder &builder,
              const std::string &kind,
              int64_t amt,
              int64_t from,
              int64_t to) {
  builder.Push(TUuid(TUuid::Twister));
  builder.Push(1L);
  builder.Push(1L);
  builder.Push(EdgeIndexId);
  builder.Push(make_tuple(from, kind, to));
  builder.Push(amt);
}

int main(int /*argc*/, char */*argv*/[]) {
  const int64_t num_trans = 17L;
  Atom::TCoreVectorBuilder builder;
  builder.Push(num_trans);  // num transactions
  builder.Push(num_trans);  // dummy meta data
  Node(builder, 0, "ATM1");
  Node(builder, 1, "ATM2");
  Node(builder, 2, "Crook nr 1");
  Node(builder, 3, "Crook nr 2");
  Node(builder, 4, "Crook nr 3");
  Node(builder, 5, "NormalPerson");
  Node(builder, 6, "Restaurant");
  Node(builder, 7, "Bank");
  MakeDataEdge(builder, "spendsmoneyon", 800, 2, 0);
  MakeDataEdge(builder, "spendsmoneyon", 1200, 2, 1);
  MakeDataEdge(builder, "wiresmoney", 23000, 3, 2);
  MakeDataEdge(builder, "wiresmoney", 12000, 4, 6);
  MakeDataEdge(builder, "spendsmoneyon", 43, 5, 0);
  MakeDataEdge(builder, "spendsmoneyon", 43, 5, 1);
  MakeDataEdge(builder, "spendsmoneyon", 45000, 6, 1);
  MakeDataEdge(builder, "withdrawsmoney", 23000, 7, 3);
  MakeDataEdge(builder, "withdrawsmoney", 12000, 7, 4);
  assert(builder.GetCores().size() == 2UL + num_trans * 6UL);
  Io::TBinaryOutputOnlyStream strm(make_shared<Io::TDevice>(open("money_laundering.bin", O_WRONLY | O_CREAT, 0777)));
  builder.Write(strm);
  return EXIT_SUCCESS;
}
