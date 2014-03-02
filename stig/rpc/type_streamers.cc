/* <stig/rpc/type_streamers.cc> 

   Implements <stig/rpc/type_streamers.h>.

   Copyright 2010-2014 Tagged
   
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
   
     http://www.apache.org/licenses/LICENSE-2.0
   
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <stig/rpc/type_streamers.h>

#include <cassert>

#include <stig/rpc/streamers.h>

using namespace std;
using namespace Stig::Rpc;

template <typename TSeq>
static TSeq *NewSeq(TBlob::TReader &reader) {
  TType::TPtr elem;
  reader >> elem;
  return new TSeq(move(elem));
}

TBlob::TReader &Stig::Rpc::operator>>(TBlob::TReader &reader, TType::TPtr &that) {
  assert(&reader);
  assert(&that);
  TType::TKind kind;
  ReadAsIs(reader, kind);
  switch (kind) {
    // Monomorphs
    case TType::TKind::Bool:     { that.reset(new Type::TBool    ); break; }
    case TType::TKind::Id:       { that.reset(new Type::TId      ); break; }
    case TType::TKind::Int:      { that.reset(new Type::TInt     ); break; }
    case TType::TKind::Real:     { that.reset(new Type::TReal    ); break; }
    case TType::TKind::Str:      { that.reset(new Type::TStr     ); break; }
    case TType::TKind::TimeDiff: { that.reset(new Type::TTimeDiff); break; }
    case TType::TKind::TimePnt:  { that.reset(new Type::TTimePnt ); break; }
    // Sequences
    case TType::TKind::List: { that.reset(NewSeq<Type::TList>(reader)); break; }
    case TType::TKind::Opt:  { that.reset(NewSeq<Type::TOpt >(reader)); break; }
    case TType::TKind::Set:  { that.reset(NewSeq<Type::TSet >(reader)); break; }
    // Dict
    case TType::TKind::Dict: {
      TType::TPtr key, val;
      reader >> key >> val;
      that.reset(new Type::TDict(move(key), move(val)));
      break;
    }
    // Addr
    case TType::TKind::Addr: {
      TBlob::TElemCount elem_count;
      ReadNbo(reader, elem_count);
      Type::TAddr::TElems elems;
      elems.reserve(elem_count);
      for (TBlob::TElemCount i = 0; i < elem_count; ++i) {
        Type::TAddr::TDir dir;
        ReadAsIs(reader, dir);
        switch (dir) {
          case Type::TAddr::TDir::Asc:
          case Type::TAddr::TDir::Desc: {
            break;
          }
          default: {
            throw 0;  // TODO
          }
        }
        TType::TPtr elem;
        reader >> elem;
        elems.push_back(make_pair(dir, move(elem)));
      }
      that.reset(new Type::TAddr(move(elems)));
      break;
    }
    // Obj
    case TType::TKind::Obj: {
      TBlob::TElemCount elem_count;
      ReadNbo(reader, elem_count);
      Type::TObj::TElems elems;
      for (TBlob::TElemCount i = 0; i < elem_count; ++i) {
        string name;
        TType::TPtr elem;
        reader >> name >> elem;
        elems.insert(make_pair(move(name), move(elem)));
      }
      that.reset(new Type::TObj(move(elems)));
      break;
    }
    // Uh oh.
    default: {
      throw 0;  // TODO
    }
  }
  return reader;
}

TBlob::TWriter &Stig::Rpc::operator<<(TBlob::TWriter &writer, const TType &that) {
  struct visitor_t final : TType::TVisitor {
    TBlob::TWriter &Writer;
    visitor_t(TBlob::TWriter &writer) : Writer(writer) {}
    // Monomorphs
    virtual void operator()(const Type::TBool     *) const override { OnMono(TType::TKind::Bool    ); }
    virtual void operator()(const Type::TId       *) const override { OnMono(TType::TKind::Id      ); }
    virtual void operator()(const Type::TInt      *) const override { OnMono(TType::TKind::Int     ); }
    virtual void operator()(const Type::TReal     *) const override { OnMono(TType::TKind::Real    ); }
    virtual void operator()(const Type::TStr      *) const override { OnMono(TType::TKind::Str     ); }
    virtual void operator()(const Type::TTimeDiff *) const override { OnMono(TType::TKind::TimeDiff); }
    virtual void operator()(const Type::TTimePnt  *) const override { OnMono(TType::TKind::TimePnt ); }
    void OnMono(TType::TKind kind) const {
      WriteAsIs(Writer, kind);
    }
    // Sequences
    virtual void operator()(const Type::TList *type) const override { OnSeq(TType::TKind::List, type->GetElem()); }
    virtual void operator()(const Type::TOpt  *type) const override { OnSeq(TType::TKind::Opt,  type->GetElem()); }
    virtual void operator()(const Type::TSet  *type) const override { OnSeq(TType::TKind::Set,  type->GetElem()); }
    void OnSeq(TType::TKind kind, const TType *elem) const {
      WriteAsIs(Writer, kind);
      elem->Accept(*this);
    }
    // Dict
    virtual void operator()(const Type::TDict *type) const override {
      WriteAsIs(Writer, TType::TKind::Dict);
      type->GetKey()->Accept(*this);
      type->GetVal()->Accept(*this);
    }
    // Addr
    virtual void operator()(const Type::TAddr *type) const override {
      WriteAsIs(Writer, TType::TKind::Addr);
      WriteNbo<TBlob::TElemCount>(Writer, type->GetElemCount());
      type->ForEachElem(
          [this](Type::TAddr::TDir dir, const TType *elem) {
            WriteAsIs(Writer, dir);
            elem->Accept(*this);
            return true;
          }
      );
    }
    // Obj
    virtual void operator()(const Type::TObj *type) const override {
      WriteAsIs(Writer, TType::TKind::Obj);
      WriteNbo<TBlob::TElemCount>(Writer, type->GetElemCount());
      type->ForEachElem(
          [this](const string &name, const TType *elem) {
            Writer << name;
            elem->Accept(*this);
            return true;
          }
      );
    }
  };
  assert(&writer);
  assert(&that);
  that.Accept(visitor_t(writer));
  return writer;
}

