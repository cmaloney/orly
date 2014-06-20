/* <orly/type/part.cc>

   Implements <orly/type/part.h> */

#include <orly/type/part.h>

using namespace Base;
using namespace Orly;
using namespace Orly::Type;

TAddrMember::TPtr TAddrMember::Get(size_t index) {
  static TInterner<TAddrMember, size_t> interner;
  return interner.Get(index);
}

TListIndex::TPtr TListIndex::Get() {
  static TListIndex::TPtr list_idx(new TListIndex());
  return list_idx;
}


TDictMember::TPtr TDictMember::Get(const Type::TType &key_type) {
  static TInterner<TDictMember, TType> interner;
  return interner.Get(key_type);
}

TObjMember::TPtr TObjMember::Get(const std::string &name) {
  static TInterner<TObjMember, std::string> interner;
  return interner.Get(name);
}
