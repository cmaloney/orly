/* <orly/data/shakespeare.cc>

   Generates a core-vector file with shakespeare data

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
(0)
(1) {"troop":"Royal Shakespeare Company"}
(2) {"city":"Stratford upon Avon"}
(3) {"country":"United Kingdom"}
(4) {"county":"Tyne and Wear"}
(5) {"city":"Newcastle"}
(6) {"street":"Grey Street"}
(7) {"theatre":"Theatre Royal"}
(8) {"user":"Billy"}
(9) {"rating":5,"review":"This was awesome!"}
(10) {"performance":20061121}
(11) {"performance":20120729}
(12) {"performance":20120729}
(13) {"title":"The Tempest"}
(14) {"title":"Julias Caesar"}
(15) {"firstname":"William","lastname":"Shakespeare"}
(1)-[:APPEARED_IN]->(0)
(1)-[:FROM]->(2)
(2)-[:IN]->(3)
(4)-[:IN]->(3)
(5)-[:IN]->(4)
(6)-[:IN]->(5)
(7)-[:IN]->(6)
(8)-[:WROTE]->(9)
(9)-[:RATED]->(12)
(10)-[:PERFORMED]->(13)
(10)-[:VENUE]->(7)
(11)-[:PERFORMED]->(14)
(11)-[:VENUE]->(7)
(12)-[:PERFORMED]->(14)
(12)-[:VENUE]->(7)
(15)-[:WROTE]->(14) {"date":1599}
(15)-[:WROTE]->(13) {"date":1610}

*/

static const TUuid NodeIndexId(TUuid(TUuid::Twister));
static const TUuid EdgeIndexId(TUuid(TUuid::Twister));

void Node(Atom::TCoreVectorBuilder &builder, int64_t id, const map<string, string> &data) {
  builder.Push(TUuid(TUuid::Twister));
  builder.Push(1L);
  builder.Push(1L);
  builder.Push(NodeIndexId);
  builder.Push(make_tuple(id));
  builder.Push(data);
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

void MakeDataEdge(Atom::TCoreVectorBuilder &builder,
              const std::string &kind,
              int64_t val,
              int64_t from,
              int64_t to) {
  builder.Push(TUuid(TUuid::Twister));
  builder.Push(1L);
  builder.Push(1L);
  builder.Push(EdgeIndexId);
  builder.Push(make_tuple(from, kind, to));
  builder.Push(val);
}

void ApprearedIn(Atom::TCoreVectorBuilder &builder, int64_t from, int64_t to) {
  MakeEdge(builder, "APPEARED_IN", from, to);
}

void From(Atom::TCoreVectorBuilder &builder, int64_t from, int64_t to) {
  MakeEdge(builder, "FROM", from, to);
}

void IsIn(Atom::TCoreVectorBuilder &builder, int64_t from, int64_t to) {
  MakeEdge(builder, "IN", from, to);
}

void Wrote(Atom::TCoreVectorBuilder &builder, int64_t from, int64_t to) {
  MakeEdge(builder, "WROTE", from, to);
}

void WroteOn(Atom::TCoreVectorBuilder &builder, int64_t from, int64_t to, int64_t date) {
  MakeDataEdge(builder, "WROTE", date, from, to);
}

void Rated(Atom::TCoreVectorBuilder &builder, int64_t from, int64_t to) {
  MakeEdge(builder, "RATED", from, to);
}

void Performed(Atom::TCoreVectorBuilder &builder, int64_t from, int64_t to) {
  MakeEdge(builder, "PERFORMED", from, to);
}

void Venue(Atom::TCoreVectorBuilder &builder, int64_t from, int64_t to) {
  MakeEdge(builder, "VENUE", from, to);
}

int main(int /*argc*/, char */*argv*/[]) {
  using TData = map<string, string>;
  const int64_t num_trans = 33L;
  Atom::TCoreVectorBuilder builder;
  builder.Push(num_trans);  // num transactions
  builder.Push(num_trans);  // dummy meta data
  Node(builder, 0, TData{});
  Node(builder, 1, TData{{"troop", "Royal Shakespeare Company"}});
  Node(builder, 2, TData{{"city", "Stratford upon Avon"}});
  Node(builder, 3, TData{{"country", "United Kingdom"}});
  Node(builder, 4, TData{{"county", "Tyne and Wear"}});
  Node(builder, 5, TData{{"city", "Newcastle"}});
  Node(builder, 6, TData{{"street", "Grey Street"}});
  Node(builder, 7, TData{{"theatre", "Theatre Royal"}});
  Node(builder, 8, TData{{"user", "Billy"}});
  Node(builder, 9, TData{{"rating", "5"}, {"review", "This was awesome!"}});
  Node(builder, 10, TData{{"performance", "20061121"}});
  Node(builder, 11, TData{{"performance", "20120729"}});
  Node(builder, 12, TData{{"performance", "20120729"}});
  Node(builder, 13, TData{{"title", "The Tempest"}});
  Node(builder, 14, TData{{"title", "Julias Caesar"}});
  Node(builder, 15, TData{{"firstname", "William"}, {"lastname", "Shakespeare"}});
  ApprearedIn(builder, 1, 0);
  From(builder, 1, 2);
  IsIn(builder, 2, 3);
  IsIn(builder, 4, 3);
  IsIn(builder, 5, 4);
  IsIn(builder, 6, 5);
  IsIn(builder, 7, 6);
  Wrote(builder, 8, 9);
  Rated(builder, 9, 12);
  Performed(builder, 10, 13);
  Venue(builder, 10, 7);
  Performed(builder, 11, 14);
  Venue(builder, 11, 7);
  Performed(builder, 12, 14);
  Venue(builder, 12, 7);
  WroteOn(builder, 15, 14, 1599);
  WroteOn(builder, 15, 13, 1610);
  assert(builder.GetCores().size() == 2UL + num_trans * 6UL);
  Io::TBinaryOutputOnlyStream strm(make_shared<Io::TDevice>(open("shakespeare.bin", O_WRONLY | O_CREAT, 0777)));
  builder.Write(strm);
  return EXIT_SUCCESS;
}

