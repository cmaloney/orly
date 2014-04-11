/* <orly/type/part.cc>

   Implements <orly/type/part.h> */

#include <orly/type/part.h>

using namespace Stig;
using namespace Stig::Type;

namespace Stig {

  namespace Type {

    TListIndex::TPtr *GetNewListIndex() {
      return new TListIndex::TPtr(new TListIndex());
    }

  }  // Type

}  // Stig

Base::TSafeGlobal<Base::TInterner<TAddrMember, size_t>> TAddrMember::Interner(
    []() -> Base::TInterner<TAddrMember, size_t> * {
      return new Base::TInterner<TAddrMember, size_t>();
    });

const Base::TSafeGlobal<TListIndex::TPtr> TListIndex::ListIndex(GetNewListIndex);

Base::TSafeGlobal<Base::TInterner<TDictMember, TType>> TDictMember::Interner(
    []() -> Base::TInterner<TDictMember, TType> * {
      return new Base::TInterner<TDictMember, TType>();
    });

Base::TSafeGlobal<Base::TInterner<TObjMember, std::string>> TObjMember::Interner(
    []() -> Base::TInterner<TObjMember, std::string> * {
      return new Base::TInterner<TObjMember, std::string>();
    });

