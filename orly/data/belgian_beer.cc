/* <orly/data/belgian_beer.cc>

   Generates a core-vector file with belgian beer data

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
(0) {"id":"1","name":"Orval","type":"BeerBrand"}
(1) {"id":"2","name":"Duvel","type":"BeerBrand"}
(2) {"id":"3","name":"Heksemiebier","type":"BeerBrand"}
(3) {"id":"4","name":"Wilderen Goud","type":"BeerBrand"}
(4) {"id":"10","name":"Abdij Notre-Dame d Orval","type":"Brewery"}
(5) {"id":"11","name":"Brouwerij Duvel Moortgat","type":"Brewery"}
(6) {"id":"12","name":"Brouwerij Wilderen","type":"Brewery"}
(7) {"id":"13","name":"Microbrouwerij Achilles","type":"Brewery"}
(8) {"id":"20","name":"Hoge gisting","type":"Beertype"}
(9) {"id":"21","name":"Trappist","type":"Beertype"}
(10) {"id":"30","name":"6.2","type":"AlcoholPercentage"}
(11) {"id":"31","name":"8.5","type":"AlcoholPercentage"}
(0)-[:IS_A]->(9)
(0)-[:ALCOHOL_PERCENTAGE]->(10)
(0)-[:BREWED_BY]->(4)
(1)-[:IS_A]->(8)
(1)-[:ALCOHOL_PERCENTAGE]->(11)
(1)-[:BREWED_BY]->(5)
(2)-[:IS_A]->(8)
(2)-[:ALCOHOL_PERCENTAGE]->(10)
(2)-[:BREWED_BY]->(7)
(3)-[:IS_A]->(8)
(3)-[:ALCOHOL_PERCENTAGE]->(10)
(3)-[:BREWED_BY]->(6)


*/

static const TUuid NodeIndexId(TUuid(TUuid::Twister));
static const TUuid EdgeIndexId(TUuid(TUuid::Twister));

void Node(Atom::TCoreVectorBuilder &builder,
          int64_t id,
          int64_t other_id,
          const string &name,
          const string &type) {
  builder.Push(TUuid(TUuid::Twister));
  builder.Push(1L);
  builder.Push(3L);
  builder.Push(NodeIndexId);
  builder.Push(std::make_tuple(id, "id"));
  builder.Push(other_id);
  builder.Push(NodeIndexId);
  builder.Push(std::make_tuple(id, "name"));
  builder.Push(name);
  builder.Push(NodeIndexId);
  builder.Push(std::make_tuple(id, "type"));
  builder.Push(type);
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

void Isa(Atom::TCoreVectorBuilder &builder, int64_t from, int64_t to) {
  MakeEdge(builder, "IS_A", from, to);
}

void BrewedBy(Atom::TCoreVectorBuilder &builder, int64_t from, int64_t to) {
  MakeEdge(builder, "BREWED_BY", from, to);
}
void AlcoholPercentage(Atom::TCoreVectorBuilder &builder, int64_t from, int64_t to) {
  MakeEdge(builder, "ALCOHOL_PERCENTAGE", from, to);
}

int main(int /*argc*/, char */*argv*/[]) {
  const int64_t node_count = 12L;
  const int64_t edge_count = 12L;
  const int64_t num_trans = node_count + edge_count;
  Atom::TCoreVectorBuilder builder;
  builder.Push(num_trans);  // num transactions
  builder.Push(num_trans);  // dummy meta data
  Node(builder, 0, 1, "Orval", "BeerBrand");
  Node(builder, 1, 2, "Duvel", "BeerBrand");
  Node(builder, 2, 3, "Heksemiebier", "BeerBrand");
  Node(builder, 3, 4, "Wilderen Goud", "BeerBrand");
  Node(builder, 4, 10, "Abdij Notre-Dame d Orval", "Brewery");
  Node(builder, 5, 11, "Brouwerij Duvel Moortgat", "Brewery");
  Node(builder, 6, 12, "Brouwerij Wilderen", "Brewery");
  Node(builder, 7, 13, "Microbrouwerij Achilles", "Brewery");
  Node(builder, 8, 20, "Hoge gisting", "Beertype");
  Node(builder, 9, 21, "Trappist", "Beertype");
  Node(builder, 10, 30, "6.2", "AlcoholPercentage");
  Node(builder, 11, 31, "8.5", "AlcoholPercentage");
  Isa(builder, 0, 9);
  AlcoholPercentage(builder, 0, 10);
  BrewedBy(builder, 0, 4);
  Isa(builder, 1, 8);
  AlcoholPercentage(builder, 1, 11);
  BrewedBy(builder, 1, 5);
  Isa(builder, 2, 8);
  AlcoholPercentage(builder, 2, 10);
  BrewedBy(builder, 2, 7);
  Isa(builder, 3, 8);
  AlcoholPercentage(builder, 3, 10);
  BrewedBy(builder, 3, 6);
  assert(builder.GetCores().size() == 2UL + edge_count * 6UL + node_count * 12UL);
  Io::TBinaryOutputOnlyStream strm(make_shared<Io::TDevice>(open("belgian_beer.bin", O_WRONLY | O_CREAT, 0777)));
  builder.Write(strm);
  return EXIT_SUCCESS;
}
