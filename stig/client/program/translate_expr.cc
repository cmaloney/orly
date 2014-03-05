/* <stig/client/program/translate_expr.cc>

   Implements <stig/client/program/translate_expr.h>.

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

#include <stig/client/program/translate_expr.h>

#include <base/thrower.h>
#include <base/zero.h>
#include <stig/sabot/compare_states.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Stig;
using namespace Stig::Client::Program;

void Stig::Client::Program::TranslatePackage(vector<string> &package_name, uint64_t &version_number, const TPackageName *root) {
  assert(&package_name);
  assert(&version_number);
  assert(root);
  TranslatePathName(package_name, root->GetNameList());
  version_number = root->GetIntExpr()->GetLexeme().AsInt();
}

void Stig::Client::Program::TranslatePathName(vector<string> &path_name, const TNameList *root) {
  assert(&path_name);
  assert(root);
  path_name.clear();
  do {
    path_name.push_back(root->GetName()->GetLexeme().GetText());
    auto tail = dynamic_cast<const TNameListTail *>(root->GetOptNameListTail());
    root = tail ? tail->GetNameList() : nullptr;
  } while (root);
}

Sabot::Type::TAny *Stig::Client::Program::NewTypeSabot(const TType *type, void *alloc) {
  class visitor_t final : public TType::TVisitor {
    public:
    visitor_t(void *alloc, Sabot::Type::TAny *&result) : Alloc(alloc), Result(result) {}
    virtual void operator()(const TBoolType *) const override {
      Result = new (Alloc) Sabot::Type::TBool();
    }
    virtual void operator()(const TIntType *) const override {
      Result = new (Alloc) Sabot::Type::TInt64();
    }
    virtual void operator()(const TRealType *) const override {
      Result = new (Alloc) Sabot::Type::TDouble();
    }
    virtual void operator()(const TIdType *) const override {
      Result = new (Alloc) Sabot::Type::TUuid();
    }
    virtual void operator()(const TTimePntType *) const override {
      Result = new (Alloc) Sabot::Type::TTimePoint();
    }
    virtual void operator()(const TTimeDiffType *) const override {
      Result = new (Alloc) Sabot::Type::TDuration();
    }
    virtual void operator()(const TStrType *) const override {
      Result = new (Alloc) Sabot::Type::TStr();
    }
    virtual void operator()(const TOptType *that) const override {
      Result = new (Alloc) Type::TUnaryType<Sabot::Type::TOpt>(that->GetType());
    }
    virtual void operator()(const TSetType *that) const override {
      Result = new (Alloc) Type::TUnaryType<Sabot::Type::TSet>(that->GetType());
    }
    virtual void operator()(const TListType *that) const override {
      Result = new (Alloc) Type::TUnaryType<Sabot::Type::TVector>(that->GetType());
    }
    virtual void operator()(const TDictType *that) const override {
      Result = new (Alloc) Type::TBinaryType<Sabot::Type::TMap>(that->GetKey(), that->GetValue());
    }
    virtual void operator()(const TObjType *that) const override {
      Result = new (Alloc) Type::TRecordType(that);
    }
    virtual void operator()(const TAddrType *that) const override {
      Result = new (Alloc) Type::TTupleType(that);
    }
    virtual void operator()(const TParenType *that) const override {
      that->GetType()->Accept(*this);
    }
    private:
    void *Alloc;
    Sabot::Type::TAny *&Result;
  };
  assert(type);
  assert(alloc);
  Sabot::Type::TAny *result = nullptr;
  type->Accept(visitor_t(alloc, result));
  assert(result);
  return result;
}

Sabot::Type::TAny *Stig::Client::Program::NewTypeSabot(const TExpr *expr, void *alloc) {
  class visitor_t final : public TExpr::TVisitor {
    public:
    visitor_t(void *alloc, Sabot::Type::TAny *&result) : Alloc(alloc), Result(result) {}
    virtual void operator()(const TTrueExpr *) const override {
      Result = new (Alloc) Sabot::Type::TBool();
    }
    virtual void operator()(const TFalseExpr *) const override {
      Result = new (Alloc) Sabot::Type::TBool();
    }
    virtual void operator()(const TIntExpr *) const override {
      Result = new (Alloc) Sabot::Type::TInt64();
    }
    virtual void operator()(const TRealExpr *) const override {
      Result = new (Alloc) Sabot::Type::TDouble();
    }
    virtual void operator()(const TIdExpr *) const override {
      Result = new (Alloc) Sabot::Type::TUuid();
    }
    virtual void operator()(const TTimePntExpr *) const override {
      Result = new (Alloc) Sabot::Type::TTimePoint();
    }
    virtual void operator()(const TTimeDiffExpr *) const override {
      Result = new (Alloc) Sabot::Type::TDuration();
    }
    virtual void operator()(const TSingleQuotedStrExpr *) const override {
      Result = new (Alloc) Sabot::Type::TStr();
    }
    virtual void operator()(const TDoubleQuotedStrExpr *) const override {
      Result = new (Alloc) Sabot::Type::TStr();
    }
    virtual void operator()(const TSingleQuotedRawStrExpr *) const override {
      Result = new (Alloc) Sabot::Type::TStr();
    }
    virtual void operator()(const TDoubleQuotedRawStrExpr *) const override {
      Result = new (Alloc) Sabot::Type::TStr();
    }
    virtual void operator()(const TUnknownExpr *that) const override {
      Result = new (Alloc) Type::TUnaryType<Sabot::Type::TOpt>(that->GetType());
    }
    virtual void operator()(const TOptExpr *that) const override {
      Result = new (Alloc) Type::TUnaryExpr<Sabot::Type::TOpt>(that->GetExpr());
    }
    virtual void operator()(const TEmptyExpr *that) const override {
      Result = NewTypeSabot(that->GetType(), Alloc);
    }
    virtual void operator()(const TSetExpr *that) const override {
      Result = new (Alloc) Type::TUnaryExpr<Sabot::Type::TSet>(that->GetExprList()->GetExpr());
    }
    virtual void operator()(const TListExpr *that) const override {
      Result = new (Alloc) Type::TUnaryExpr<Sabot::Type::TVector>(that->GetExprList()->GetExpr());
    }
    virtual void operator()(const TDictExpr *that) const override {
      auto dict_member = that->GetDictMemberList()->GetDictMember();
      Result = new (Alloc) Type::TBinaryExpr<Sabot::Type::TMap>(dict_member->GetKey(), dict_member->GetValue());
    }
    virtual void operator()(const TObjExpr *that) const override {
      Result = new (Alloc) Type::TRecordExpr(that);
    }
    virtual void operator()(const TAddrExpr *that) const override {
      Result = new (Alloc) Type::TTupleExpr(that);
    }
    virtual void operator()(const TParenExpr *that) const override {
      that->GetExpr()->Accept(*this);
    }
    private:
    void *Alloc;
    Sabot::Type::TAny *&Result;
  };
  assert(expr);
  assert(alloc);
  Sabot::Type::TAny *result = nullptr;
  expr->Accept(visitor_t(alloc, result));
  assert(result);
  return result;
}

Sabot::State::TAny *Stig::Client::Program::NewStateSabot(const TExpr *expr, void *alloc) {
  class visitor_t final : public TExpr::TVisitor {
    public:
    visitor_t(void *alloc, Sabot::State::TAny *&result) : Alloc(alloc), Result(result) {}
    virtual void operator()(const TTrueExpr *that) const override {
      Result = new (Alloc) State::TBool(that);
    }
    virtual void operator()(const TFalseExpr *that) const override {
      Result = new (Alloc) State::TBool(that);
    }
    virtual void operator()(const TIntExpr *that) const override {
      Result = new (Alloc) State::TInt(that);
    }
    virtual void operator()(const TRealExpr *that) const override {
      Result = new (Alloc) State::TReal(that);
    }
    virtual void operator()(const TIdExpr *that) const override {
      Result = new (Alloc) State::TId(that);
    }
    virtual void operator()(const TTimePntExpr *that) const override {
      Result = new (Alloc) State::TTimePnt(that);
    }
    virtual void operator()(const TTimeDiffExpr *that) const override {
      Result = new (Alloc) State::TTimeDiff(that);
    }
    virtual void operator()(const TSingleQuotedStrExpr *that) const override {
      Result = new (Alloc) State::TStr(that);
    }
    virtual void operator()(const TDoubleQuotedStrExpr *that) const override {
      Result = new (Alloc) State::TStr(that);
    }
    virtual void operator()(const TSingleQuotedRawStrExpr *that) const override {
      Result = new (Alloc) State::TStr(that);
    }
    virtual void operator()(const TDoubleQuotedRawStrExpr *that) const override {
      Result = new (Alloc) State::TStr(that);
    }
    virtual void operator()(const TOptExpr *that) const override {
      Result = new (Alloc) State::TOpt(that);
    }
    virtual void operator()(const TUnknownExpr *that) const override {
      Result = new (Alloc) State::TOpt(that);
    }
    virtual void operator()(const TEmptyExpr *that) const override {
      class empty_visitor_t final : public TType::TVisitor {
        public:
        empty_visitor_t(void *alloc, Sabot::State::TAny *&result) : Alloc(alloc), Result(result) {}
        virtual void operator()(const TBoolType *that) const override     { ThrowBadEmpty(that->GetLexeme()); }
        virtual void operator()(const TIntType *that) const override      { ThrowBadEmpty(that->GetLexeme()); }
        virtual void operator()(const TRealType *that) const override     { ThrowBadEmpty(that->GetLexeme()); }
        virtual void operator()(const TIdType *that) const override       { ThrowBadEmpty(that->GetLexeme()); }
        virtual void operator()(const TTimePntType *that) const override  { ThrowBadEmpty(that->GetLexeme()); }
        virtual void operator()(const TTimeDiffType *that) const override { ThrowBadEmpty(that->GetLexeme()); }
        virtual void operator()(const TStrType *that) const override      { ThrowBadEmpty(that->GetLexeme()); }
        virtual void operator()(const TOptType *that) const override      { ThrowBadEmpty(that->GetQuestionMark()->GetLexeme()); }
        virtual void operator()(const TObjType *that) const override      { ThrowBadEmpty(that->GetOpenObj()->GetLexeme()); }
        virtual void operator()(const TAddrType *that) const override     { ThrowBadEmpty(that->GetOpenAddr()->GetLexeme()); }
        virtual void operator()(const TSetType *that) const override {
          Result = new (Alloc) State::TSet(that);
        }
        virtual void operator()(const TListType *that) const override {
          Result = new (Alloc) State::TList(that);
        }
        virtual void operator()(const TDictType *that) const override {
          Result = new (Alloc) State::TDict(that);
        }
        virtual void operator()(const TParenType *that) const override {
          that->GetType()->Accept(*this);
        }
        private:
        void *Alloc;
        Sabot::State::TAny *&Result;
      };
      that->GetType()->Accept(empty_visitor_t(Alloc, Result));
    }
    virtual void operator()(const TSetExpr *that) const override {
      Result = new (Alloc) State::TSet(that);
    }
    virtual void operator()(const TListExpr *that) const override {
      Result = new (Alloc) State::TList(that);
    }
    virtual void operator()(const TDictExpr *that) const override {
      Result = new (Alloc) State::TDict(that);
    }
    virtual void operator()(const TObjExpr *that) const override {
      Result = new (Alloc) State::TObj(that);
    }
    virtual void operator()(const TAddrExpr *that) const override {
      Result = new (Alloc) State::TAddr(that);
    }
    virtual void operator()(const TParenExpr *that) const override {
      that->GetExpr()->Accept(*this);
    }
    private:
    static void ThrowBadEmpty(const Tools::Nycr::TLexeme &lexeme) {
      DEFINE_ERROR(error_t, runtime_error, "type cannot be 'empty'");
      THROW_ERROR(error_t) << lexeme.GetPosRange();
    }
    void *Alloc;
    Sabot::State::TAny *&Result;
  };
  assert(expr);
  assert(alloc);
  Sabot::State::TAny *result = nullptr;
  expr->Accept(visitor_t(alloc, result));
  assert(result);
  return result;
}

Atom::TCore *Stig::Client::Program::TranslateExpr(Atom::TCore::TExtensibleArena *arena, void *core_alloc, const TExpr *expr) {
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  return new (core_alloc) Atom::TCore(arena, Sabot::State::TAny::TWrapper(NewStateSabot(expr, state_alloc)).get());
}

Type::TRecordType::TPin::TPin(const TRecordType *record_type)
    : TPinBase(record_type), RecordType(record_type) {
  assert(record_type);
}

Sabot::Type::TAny *Type::TRecordType::TPin::NewElem(size_t elem_idx, string &name, void *type_alloc) const {
  assert(this);
  assert(&name);
  auto member = RecordType->Members[elem_idx];
  name = member->GetName()->GetLexeme().GetText();
  return NewTypeSabot(member->GetType(), type_alloc);
}

Sabot::Type::TAny *Type::TRecordType::TPin::NewElem(size_t, void *&, void *, void *) const {
  DEFINE_ERROR(error_t, runtime_error, "Type::TRecord::NewElem() with name via string sabot is not implmented for expression trees");
  THROW_ERROR(error_t);
}

Sabot::Type::TAny *Type::TRecordType::TPin::NewElem(size_t, void *) const {
  DEFINE_ERROR(error_t, runtime_error, "Type::TRecord::NewElem() without name is not implmented for expression trees");
  THROW_ERROR(error_t);
}

Type::TRecordType::TRecordType(const TObjType *type) {
  assert(type);
  auto list = dynamic_cast<const TObjTypeMemberList *>(type->GetOptObjTypeMemberList());
  while (list) {
    Members.push_back(list->GetObjTypeMember());
    auto tail = dynamic_cast<const TObjTypeMemberListTail *>(list->GetOptObjTypeMemberListTail());
    list = tail ? tail->GetObjTypeMemberList() : nullptr;
  }
}

size_t Type::TRecordType::GetElemCount() const {
  assert(this);
  return Members.size();
}

Type::TRecordType::TPinBase *Type::TRecordType::Pin(void *alloc) const {
  return new (alloc) TPin(this);
}

Type::TRecordExpr::TPin::TPin(const TRecordExpr *record_expr)
    : TPinBase(record_expr), RecordExpr(record_expr) {
  assert(record_expr);
}

Sabot::Type::TAny *Type::TRecordExpr::TPin::NewElem(size_t elem_idx, string &name, void *type_alloc) const {
  assert(this);
  assert(&name);
  auto member = RecordExpr->Members[elem_idx];
  name = member->GetName()->GetLexeme().GetText();
  return NewTypeSabot(member->GetExpr(), type_alloc);
}

Sabot::Type::TAny *Type::TRecordExpr::TPin::NewElem(size_t, void *&, void *, void *) const {
  DEFINE_ERROR(error_t, runtime_error, "Type::TRecord::NewElem() with name via string sabot is not implmented for expression trees");
  THROW_ERROR(error_t);
}

Sabot::Type::TAny *Type::TRecordExpr::TPin::NewElem(size_t, void *) const {
  DEFINE_ERROR(error_t, runtime_error, "Type::TRecord::NewElem() without name is not implmented for expression trees");
  THROW_ERROR(error_t);
}

Type::TRecordExpr::TRecordExpr(const TObjExpr *expr) {
  assert(expr);
  auto list = dynamic_cast<const TObjMemberList *>(expr->GetOptObjMemberList());
  while (list) {
    Members.push_back(list->GetObjMember());
    auto tail = dynamic_cast<const TObjMemberListTail *>(list->GetOptObjMemberListTail());
    list = tail ? tail->GetObjMemberList() : nullptr;
  }
  sort(
      Members.begin(), Members.end(),
      [](const TObjMember *lhs, const TObjMember *rhs) {
        assert(lhs);
        assert(rhs);
        return lhs->GetName()->GetLexeme().GetText() < rhs->GetName()->GetLexeme().GetText();
      }
  );
}

size_t Type::TRecordExpr::GetElemCount() const {
  assert(this);
  return Members.size();
}

Type::TRecordExpr::TPinBase *Type::TRecordExpr::Pin(void *alloc) const {
  return new (alloc) TPin(this);
}

Type::TTupleType::TPin::TPin(const TTupleType *tuple_type)
    : TPinBase(tuple_type), TupleType(tuple_type) {
  assert(tuple_type);
}

Sabot::Type::TAny *Type::TTupleType::TPin::NewElem(size_t elem_idx, void *type_alloc) const {
  assert(this);
  return NewTypeSabot(TupleType->Members[elem_idx]->GetType(), type_alloc);
}

Type::TTupleType::TTupleType(const TAddrType *type) {
  assert(type);
  auto list = dynamic_cast<const TAddrTypeMemberList *>(type->GetOptAddrTypeMemberList());
  while (list) {
    Members.push_back(list->GetAddrTypeMember());
    auto tail = dynamic_cast<const TAddrTypeMemberListTail *>(list->GetOptAddrTypeMemberListTail());
    list = tail ? tail->GetAddrTypeMemberList() : nullptr;
  }
}

size_t Type::TTupleType::GetElemCount() const {
  assert(this);
  return Members.size();
}

Type::TTupleType::TPinBase *Type::TTupleType::Pin(void *alloc) const {
  return new (alloc) TPin(this);
}

Type::TTupleExpr::TPin::TPin(const TTupleExpr *tuple_expr)
    : TPinBase(tuple_expr), TupleExpr(tuple_expr) {
  assert(tuple_expr);
}

Sabot::Type::TAny *Type::TTupleExpr::TPin::NewElem(size_t elem_idx, void *type_alloc) const {
  assert(this);
  auto member = TupleExpr->Members[elem_idx];
  auto expr = member->GetExpr();
  return dynamic_cast<const TDescOrdering *>(member->GetOptOrdering())
      ? static_cast<Sabot::Type::TAny *>(new (type_alloc) TUnaryExpr<Sabot::Type::TDesc>(expr))
      : NewTypeSabot(expr, type_alloc);
}

Type::TTupleExpr::TTupleExpr(const TAddrExpr *expr) {
  assert(expr);
  auto list = dynamic_cast<const TAddrMemberList *>(expr->GetOptAddrMemberList());
  while (list) {
    Members.push_back(list->GetAddrMember());
    auto tail = dynamic_cast<const TAddrMemberListTail *>(list->GetOptAddrMemberListTail());
    list = tail ? tail->GetAddrMemberList() : nullptr;
  }
}

size_t Type::TTupleExpr::GetElemCount() const {
  assert(this);
  return Members.size();
}

Type::TTupleExpr::TPinBase *Type::TTupleExpr::Pin(void *alloc) const {
  return new (alloc) TPin(this);
}

State::TBool::TBool(const TTrueExpr *)
    : Val(true) {}

State::TBool::TBool(const TFalseExpr *)
    : Val(false) {}

const bool &State::TBool::Get() const {
  assert(this);
  return Val;
}

Sabot::Type::TBool *State::TBool::GetBoolType(void *type_alloc) const {
  return new (type_alloc) Sabot::Type::TBool();
}

State::TInt::TInt(const TIntExpr *expr) {
  assert(expr);
  Val = expr->GetLexeme().AsInt();
}

const int64_t &State::TInt::Get() const {
  assert(this);
  return Val;
}

Sabot::Type::TInt64 *State::TInt::GetInt64Type(void *type_alloc) const {
  return new (type_alloc) Sabot::Type::TInt64();
}

State::TReal::TReal(const TRealExpr *expr) {
  assert(expr);
  Val = expr->GetLexeme().AsDouble();
}

const double &State::TReal::Get() const {
  assert(this);
  return Val;
}

Sabot::Type::TDouble *State::TReal::GetDoubleType(void *type_alloc) const {
  return new (type_alloc) Sabot::Type::TDouble();
}

State::TId::TId(const TIdExpr *expr) {
  assert(expr);
  Val = Base::TUuid(Tools::Nycr::TLexeme::RemoveCurlyBraces(expr->GetLexeme().GetText()).c_str());
}

const TUuid &State::TId::Get() const {
  assert(this);
  return Val;
}

Sabot::Type::TUuid *State::TId::GetUuidType(void *type_alloc) const {
  return new (type_alloc) Sabot::Type::TUuid();
}

State::TTimePnt::TTimePnt(const TTimePntExpr *expr) {
  assert(expr);
  using tm_t = struct tm;
  using field_t = int (tm_t::*);
  constexpr size_t field_count = 6;
  constexpr field_t field_array[field_count] = { &tm_t::tm_year, &tm_t::tm_mon, &tm_t::tm_mday, &tm_t::tm_hour, &tm_t::tm_min, &tm_t::tm_sec };
  constexpr char delim_array[field_count] = { '-', '-', 'T', ':', ':', 'Z' };
  constexpr size_t max_size = 20;
  tm_t tm;
  Zero(tm);
  const string &text = expr->GetLexeme().GetText();
  const char
      *start = text.data() + 1,
      *limit = start + text.size() - 2;  // The adjustments here remove the curly braces.
  char buf[max_size + 1];
  size_t field_idx = 0;
  for (const char *cursor = start; cursor < limit && field_idx < field_count; ++cursor) {
    if (*cursor == delim_array[field_idx]) {
      size_t size = cursor - start;
      if (size <= max_size) {
        memcpy(buf, start, size);
        buf[size] = '\0';
        tm.*(field_array[field_idx]) = atoi(buf);
      }
      ++field_idx;
      ++cursor;
      start = cursor;
    }
  }
  if (field_idx > 0) {
    tm.tm_year -= 1900;
  }
  if (field_idx > 1) {
    tm.tm_mon -= 1;
  }
  Val = system_clock::from_time_t(mktime(&tm) - timezone);
}

const Sabot::TStdTimePoint &State::TTimePnt::Get() const {
  assert(this);
  return Val;
}

Sabot::Type::TTimePoint *State::TTimePnt::GetTimePointType(void *type_alloc) const {
  return new (type_alloc) Sabot::Type::TTimePoint();
}

State::TTimeDiff::TTimeDiff(const TTimeDiffExpr *expr) {
  assert(expr);
  struct diff_t { int days, hrs, mins, secs; };
  using field_t = int (diff_t::*);
  constexpr size_t field_count = 4;
  constexpr field_t field_array[field_count] = { &diff_t::days, &diff_t::hrs, &diff_t::mins, &diff_t::secs };
  constexpr char delim_array[field_count] = { 'T', ':', ':', '}' };
  constexpr size_t max_size = 20;
  diff_t diff;
  Zero(diff);
  const string &text = expr->GetLexeme().GetText();
  const char
      *start = text.data() + 1,
      *limit = start + text.size() - 1;  // The adjustments here remove the opening curly brace but leave the closing one as a delimiter.
  char buf[max_size + 1];
  size_t field_idx = 0;
  for (const char *cursor = start; cursor < limit && field_idx < field_count; ++cursor) {
    if (*cursor == delim_array[field_idx]) {
      size_t size = cursor - start;
      if (size <= max_size) {
        memcpy(buf, start, size);
        buf[size] = '\0';
        diff.*(field_array[field_idx]) = atoi(buf);
      }
      ++field_idx;
      ++cursor;
      start = cursor;
    }
  }
  Val = Sabot::TStdDuration(((diff.days * 86400) + (diff.hrs * 3600) + (diff.mins * 60) + diff.secs) * 1000);
}

const Sabot::TStdDuration &State::TTimeDiff::Get() const {
  assert(this);
  return Val;
}

Sabot::Type::TDuration *State::TTimeDiff::GetDurationType(void *type_alloc) const {
  return new (type_alloc) Sabot::Type::TDuration();
}

State::TStr::TPin::TPin(const TStr *str)
    : TPinBase(str->Val.c_str(), str->Val.c_str() + str->Val.size()) {}

State::TStr::TStr(const TSingleQuotedStrExpr *expr) {
  assert(expr);
  Val = expr->GetLexeme().AsSingleQuotedString();
}

State::TStr::TStr(const TDoubleQuotedStrExpr *expr) {
  assert(expr);
  Val = expr->GetLexeme().AsDoubleQuotedString();
}

State::TStr::TStr(const TSingleQuotedRawStrExpr *expr) {
  assert(expr);
  Val = expr->GetLexeme().AsSingleQuotedRawString();
}

State::TStr::TStr(const TDoubleQuotedRawStrExpr *expr) {
  assert(expr);
  Val = expr->GetLexeme().AsDoubleQuotedRawString();
}

size_t State::TStr::GetSize() const {
  assert(this);
  return Val.size();
}

Sabot::Type::TStr *State::TStr::GetStrType(void *type_alloc) const {
  return new (type_alloc) Sabot::Type::TStr();
}

State::TStr::TPinBase *State::TStr::Pin(void *alloc) const {
  assert(this);
  return new (alloc) TPin(this);
}

State::TOpt::TPin::TPin(const TOpt *opt)
    : TPinBase(opt), Expr(opt->Expr) {}

Sabot::State::TAny *State::TOpt::TPin::NewElemInRange(size_t, void *state_alloc) const {
  assert(this);
  return NewStateSabot(Expr, state_alloc);
}

State::TOpt::TOpt(const TOptExpr *expr) {
  assert(expr);
  Expr = expr->GetExpr();
  Type = nullptr;
}

State::TOpt::TOpt(const TUnknownExpr *expr) {
  assert(expr);
  Type = expr->GetType();
  Expr = nullptr;
}

size_t State::TOpt::GetElemCount() const {
  assert(this);
  return Expr ? 1 : 0;
}

Sabot::Type::TOpt *State::TOpt::GetOptType(void *type_alloc) const {
  assert(this);
  return Expr
      ? static_cast<Sabot::Type::TOpt *>(new (type_alloc) Type::TUnaryExpr<Sabot::Type::TOpt>(Expr))
      : new (type_alloc) Type::TUnaryType<Sabot::Type::TOpt>(Type);
}

State::TOpt::TPinBase *State::TOpt::Pin(void *alloc) const {
  assert(this);
  return new (alloc) TPin(this);
}

State::TDesc::TPin::TPin(const TDesc *desc)
    : TPinBase(desc), Desc(desc) {
  assert(desc);
}

Sabot::State::TAny *State::TDesc::TPin::NewElemInRange(size_t, void *state_alloc) const {
  assert(this);
  return NewStateSabot(Desc->Expr, state_alloc);
}

State::TDesc::TDesc(const TExpr *expr)
    : Expr(expr) {
  assert(expr);
}

size_t State::TDesc::GetElemCount() const {
  assert(this);
  return 1;
}

Sabot::Type::TDesc *State::TDesc::GetDescType(void *type_alloc) const {
  assert(this);
  return new (type_alloc) Type::TUnaryExpr<Sabot::Type::TDesc>(Expr);
}

State::TDesc::TPinBase *State::TDesc::Pin(void *alloc) const {
  assert(this);
  return new (alloc) TPin(this);
}

State::TSet::TPin::TPin(const TSet *set)
    : TPinBase(set), Set(set) {
  assert(set);
}

Sabot::State::TAny *State::TSet::TPin::NewElemInRange(size_t elem_idx, void *state_alloc) const {
  assert(this);
  return NewStateSabot(Set->Members[elem_idx], state_alloc);
}

State::TSet::TSet(const TSetExpr *expr) {
  assert(expr);
  auto list = expr->GetExprList();
  do {
    Members.push_back(list->GetExpr());
    auto tail = dynamic_cast<const TExprListTail *>(list->GetOptExprListTail());
    list = tail ? tail->GetExprList() : nullptr;
  } while (list);
  void
      *lhs_state_alloc = alloca(Sabot::State::GetMaxStateSize() * 2),
      *rhs_state_alloc = reinterpret_cast<uint8_t *>(lhs_state_alloc) + Sabot::State::GetMaxStateSize();
  void *lhs_type_alloc = alloca(Sabot::Type::GetMaxTypeSize() * 2);
  void *rhs_type_alloc = reinterpret_cast<uint8_t *>(lhs_type_alloc) + Sabot::Type::GetMaxTypeSize();
  sort(
    Members.begin(), Members.end(),
    [lhs_state_alloc, rhs_state_alloc, lhs_type_alloc, rhs_type_alloc](const TExpr *lhs, const TExpr *rhs) {
      Sabot::State::TAny::TWrapper
          lhs_state(NewStateSabot(lhs, lhs_state_alloc)),
          rhs_state(NewStateSabot(rhs, rhs_state_alloc));
      return Atom::IsLt(Sabot::CompareStates(*lhs_state, *rhs_state, lhs_type_alloc, rhs_type_alloc));
    }
  );
  Type = nullptr;
}

State::TSet::TSet(const TSetType *type) {
  assert(type);
  Type = type->GetType();
}

size_t State::TSet::GetElemCount() const {
  assert(this);
  return Members.size();
}

Sabot::Type::TSet *State::TSet::GetSetType(void *type_alloc) const {
  assert(this);
  return !Members.empty()
      ? static_cast<Sabot::Type::TSet *>(new (type_alloc) Type::TUnaryExpr<Sabot::Type::TSet>(Members[0]))
      : new (type_alloc) Type::TUnaryType<Sabot::Type::TSet>(Type);
}

State::TSet::TPinBase *State::TSet::Pin(void *alloc) const {
  assert(this);
  return new (alloc) TPin(this);
}

State::TList::TPin::TPin(const TList *list)
    : TPinBase(list), List(list) {
  assert(list);
}

Sabot::State::TAny *State::TList::TPin::NewElemInRange(size_t elem_idx, void *state_alloc) const {
  assert(this);
  return NewStateSabot(List->Members[elem_idx], state_alloc);
}

State::TList::TList(const TListExpr *expr) {
  assert(expr);
  auto list = expr->GetExprList();
  do {
    Members.push_back(list->GetExpr());
    auto tail = dynamic_cast<const TExprListTail *>(list->GetOptExprListTail());
    list = tail ? tail->GetExprList() : nullptr;
  } while (list);
  Type = nullptr;
}

State::TList::TList(const TListType *type) {
  assert(type);
  Type = type->GetType();
}

size_t State::TList::GetElemCount() const {
  assert(this);
  return Members.size();
}

Sabot::Type::TVector *State::TList::GetVectorType(void *type_alloc) const {
  assert(this);
  return !Members.empty()
      ? static_cast<Sabot::Type::TVector *>(new (type_alloc) Type::TUnaryExpr<Sabot::Type::TVector>(Members[0]))
      : new (type_alloc) Type::TUnaryType<Sabot::Type::TVector>(Type);
}

State::TList::TPinBase *State::TList::Pin(void *alloc) const {
  assert(this);
  return new (alloc) TPin(this);
}

State::TDict::TPin::TPin(const TDict *dict)
    : TPinBase(dict), Dict(dict) {
  assert(dict);
}

Sabot::State::TAny *State::TDict::TPin::NewLhsInRange(size_t elem_idx, void *state_alloc) const {
  assert(this);
  return NewStateSabot(Dict->Members[elem_idx]->GetKey(), state_alloc);
}

Sabot::State::TAny *State::TDict::TPin::NewRhsInRange(size_t elem_idx, void *state_alloc) const {
  assert(this);
  return NewStateSabot(Dict->Members[elem_idx]->GetValue(), state_alloc);
}

State::TDict::TDict(const TDictExpr *expr) {
  assert(expr);
  auto list = expr->GetDictMemberList();
  do {
    Members.push_back(list->GetDictMember());
    auto tail = dynamic_cast<const TDictMemberListTail *>(list->GetOptDictMemberListTail());
    list = tail ? tail->GetDictMemberList() : nullptr;
  } while (list);
  void
      *lhs_state_alloc = alloca(Sabot::State::GetMaxStateSize() * 2),
      *rhs_state_alloc = reinterpret_cast<uint8_t *>(lhs_state_alloc) + Sabot::State::GetMaxStateSize();
  void *lhs_type_alloc = alloca(Sabot::Type::GetMaxTypeSize() * 2);
  void *rhs_type_alloc = reinterpret_cast<uint8_t *>(lhs_type_alloc) + Sabot::Type::GetMaxTypeSize();
  sort(
    Members.begin(), Members.end(),
    [lhs_state_alloc, rhs_state_alloc, lhs_type_alloc, rhs_type_alloc](const TDictMember *lhs, const TDictMember *rhs) {
      Sabot::State::TAny::TWrapper
          lhs_state(NewStateSabot(lhs->GetKey(), lhs_state_alloc)),
          rhs_state(NewStateSabot(rhs->GetKey(), rhs_state_alloc));
      return Atom::IsLt(Sabot::CompareStates(*lhs_state, *rhs_state, lhs_type_alloc, rhs_type_alloc));
    }
  );
  LhsType = nullptr;
  RhsType = nullptr;
}

State::TDict::TDict(const TDictType *type) {
  assert(type);
  LhsType = type->GetKey();
  RhsType = type->GetValue();
}

size_t State::TDict::GetElemCount() const {
  assert(this);
  return Members.size();
}

Sabot::Type::TMap *State::TDict::GetMapType(void *type_alloc) const {
  assert(this);
  return !Members.empty()
      ? static_cast<Sabot::Type::TMap *>(new (type_alloc) Type::TBinaryExpr<Sabot::Type::TMap>(Members[0]->GetKey(), Members[0]->GetValue()))
      : new (type_alloc) Type::TBinaryType<Sabot::Type::TMap>(LhsType, RhsType);
}

State::TDict::TPinBase *State::TDict::Pin(void *alloc) const {
  assert(this);
  return new (alloc) TPin(this);
}

State::TObj::TPin::TPin(const TObj *obj)
    : TPinBase(obj), Obj(obj) {
  assert(obj);
}

Sabot::State::TAny *State::TObj::TPin::NewElemInRange(size_t elem_idx, void *state_alloc) const {
  assert(this);
  return NewStateSabot(Obj->Members[elem_idx]->GetExpr(), state_alloc);
}

State::TObj::TObj(const TObjExpr *expr)
    : Expr(expr) {
  assert(expr);
  auto list = dynamic_cast<const TObjMemberList *>(expr->GetOptObjMemberList());
  while (list) {
    Members.push_back(list->GetObjMember());
    auto tail = dynamic_cast<const TObjMemberListTail *>(list->GetOptObjMemberListTail());
    list = tail ? tail->GetObjMemberList() : nullptr;
  }
  sort(
      Members.begin(), Members.end(),
      [](const TObjMember *lhs, const TObjMember *rhs) {
        assert(lhs);
        assert(rhs);
        return lhs->GetName()->GetLexeme().GetText() < rhs->GetName()->GetLexeme().GetText();
      }
  );
}

size_t State::TObj::GetElemCount() const {
  assert(this);
  return Members.size();
}

Sabot::Type::TRecord *State::TObj::GetRecordType(void *type_alloc) const {
  assert(this);
  return new (type_alloc) Type::TRecordExpr(Expr);
}

State::TObj::TPinBase *State::TObj::Pin(void *alloc) const {
  assert(this);
  return new (alloc) TPin(this);
}

State::TAddr::TPin::TPin(const TAddr *addr)
    : TPinBase(addr), Addr(addr) {
  assert(addr);
}

Sabot::State::TAny *State::TAddr::TPin::NewElemInRange(size_t elem_idx, void *state_alloc) const {
  assert(this);
  auto member = Addr->Members[elem_idx];
  auto expr = member->GetExpr();
  return dynamic_cast<const TDescOrdering *>(member->GetOptOrdering())
      ? static_cast<Sabot::State::TAny *>(new (state_alloc) TDesc(expr))
      : NewStateSabot(expr, state_alloc);
}

State::TAddr::TAddr(const TAddrExpr *expr)
    : Expr(expr) {
  assert(expr);
  auto list = dynamic_cast<const TAddrMemberList *>(expr->GetOptAddrMemberList());
  while (list) {
    Members.push_back(list->GetAddrMember());
    auto tail = dynamic_cast<const TAddrMemberListTail *>(list->GetOptAddrMemberListTail());
    list = tail ? tail->GetAddrMemberList() : nullptr;
  }
}

size_t State::TAddr::GetElemCount() const {
  assert(this);
  return Members.size();
}

Sabot::Type::TTuple *State::TAddr::GetTupleType(void *type_alloc) const {
  assert(this);
  return new (type_alloc) Type::TTupleExpr(Expr);
}

State::TAddr::TPinBase *State::TAddr::Pin(void *alloc) const {
  assert(this);
  return new (alloc) TPin(this);
}
