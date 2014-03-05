/* <tools/nycr/node.cc>

   Implements <tools/nycr/node.h>

   Copyright 2010-2014 Stig LLC

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <tools/nycr/node.h>

#include <cassert>

using namespace Tools::Nycr;

TNode::~TNode() {
  assert(this);
  (Prev ? Prev->Next : First) = Next;
  (Next ? Next->Prev : Last ) = Prev;
}

void TNode::DeleteEach() {
  while (First) {
    delete First;
  }
}

TNode::TNode() {
  Next = nullptr;
  Prev = Last;
  (Prev ? Prev->Next : First) = this;
  Last = this;
}

TNode
    *TNode::First = nullptr,
    *TNode::Last  = nullptr;
