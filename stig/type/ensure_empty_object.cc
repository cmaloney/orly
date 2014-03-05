/* <stig/type/ensure_empty_object.cc>

   Implements <stig/type/ensure_empty_object.h>.

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

#include <stig/type/ensure_empty_object.h>

#include <stig/error.h>
#include <stig/type/obj.h>

using namespace Stig;
using namespace Stig::Type;

class TIsEmptyObjVisitor
    : public TType::TVisitor {
  NO_COPY_SEMANTICS(TIsEmptyObjVisitor);
  public:

  TIsEmptyObjVisitor(bool &is_empty, const TPosRange &pos_range)
      : IsEmpty(is_empty), PosRange(pos_range) {}

  virtual void operator()(const TAddr     *) const { throw TImpossibleError(HERE, PosRange); }
  virtual void operator()(const TAny      *) const { throw TImpossibleError(HERE, PosRange); }
  virtual void operator()(const TBool     *) const { throw TImpossibleError(HERE, PosRange); }
  virtual void operator()(const TDict     *) const { throw TImpossibleError(HERE, PosRange); }
  virtual void operator()(const TErr      *) const { throw TImpossibleError(HERE, PosRange); }
  virtual void operator()(const TFunc     *) const { throw TImpossibleError(HERE, PosRange); }
  virtual void operator()(const TId       *) const { throw TImpossibleError(HERE, PosRange); }
  virtual void operator()(const TInt      *) const { throw TImpossibleError(HERE, PosRange); }
  virtual void operator()(const TList     *) const { throw TImpossibleError(HERE, PosRange); }
  virtual void operator()(const TMutable  *) const { throw TImpossibleError(HERE, PosRange); }
  virtual void operator()(const TObj      *that) const {
    IsEmpty = that->GetElems().empty();
  }
  virtual void operator()(const TOpt      *) const { throw TImpossibleError(HERE, PosRange); }
  virtual void operator()(const TReal     *) const { throw TImpossibleError(HERE, PosRange); }
  virtual void operator()(const TSeq      *) const { throw TImpossibleError(HERE, PosRange); }
  virtual void operator()(const TSet      *) const { throw TImpossibleError(HERE, PosRange); }
  virtual void operator()(const TStr      *) const { throw TImpossibleError(HERE, PosRange); }
  virtual void operator()(const TTimeDiff *) const { throw TImpossibleError(HERE, PosRange); }
  virtual void operator()(const TTimePnt  *) const { throw TImpossibleError(HERE, PosRange); }

  private:

  bool &IsEmpty;

  const TPosRange PosRange;

};  // TIsEmptyObjVisitor

void Stig::Type::EnsureEmptyObject(const Type::TType &type, const TPosRange &pos_range) {
  bool is_empty = false;
  type.Accept(TIsEmptyObjVisitor(is_empty, pos_range));
  if (!is_empty) {
    throw TCompileError(HERE, pos_range, "Attempted to implicitly call a non-zero parameter function.");
  }
}
