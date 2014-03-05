/* <stig/rt/mutate.cc>

   Implements <stig/rt/mutate.h>

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

#include <stig/rt/mutate.h>
#include <stig/var.h>

using namespace Stig;
using namespace Stig::Rt;

Var::TVar Stig::Rt::Mutate(const Var::TVar &lhs, TMutator mutator, const Var::TVar &rhs) {
  Var::TVar result = lhs.Copy();
  switch (mutator) {
    case TMutator::Add: {
      result.Add(rhs);
      break;
    }
    case TMutator::And: {
      result.And(rhs);
      break;
    }
    case TMutator::Assign: {
      // NOTE: We copy rhs to be safe here, it may or may not be strictly necessary.
      result = rhs.Copy();
      break;
    }
    case TMutator::Div: {
      result.Div(rhs);
      break;
    }
    case TMutator::Exp: {
      result.Exp(rhs);
      break;
    }
    case TMutator::Intersection: {
      result.Intersection(rhs);
      break;
    }
    case TMutator::Mod: {
      result.Mod(rhs);
      break;
    }
    case TMutator::Mult: {
      result.Mult(rhs);
      break;
    }
    case TMutator::Or: {
      result.Or(rhs);
      break;
    }
    case TMutator::Sub: {
      result.Sub(rhs);
      break;
    }
    case TMutator::SymmetricDiff: {
      result.SymmetricDiff(rhs);
      break;
    }
    case TMutator::Union: {
      result.Union(rhs);
      break;
    }
    case TMutator::Xor: {
      result.Xor(rhs);
      break;
    }
  }  // switch (mutator)
  return result;
}