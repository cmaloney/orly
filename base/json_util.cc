#include <base/json_util.h>

using namespace Base;


bool TJsonReader<bool>::Read(const Base::TJson &entry) {
  ThrowIfWrongKind(Base::TJson::Bool, entry);
  return entry.GetBool();
}

int64_t TJsonReader<int64_t>::Read(const Base::TJson &entry) {
ThrowIfWrongKind(Base::TJson::Number, entry);
return entry.GetNumber();
}

std::string TJsonReader<std::string>::Read(const Base::TJson &entry) {
  ThrowIfWrongKind(Base::TJson::String, entry);
  return entry.GetString();
}

void Base::ThrowIfWrongKind(const TJson::TKind expected, const TJson &json, const char *extra_msg) {
  if (extra_msg == nullptr) {
    extra_msg = "";
  }
  if(json.GetKind() != expected) {
    THROWER(TInvalidValue) << "Expected a " << expected << " but found a "
                                        << json.GetKind() << extra_msg;
  }
}
