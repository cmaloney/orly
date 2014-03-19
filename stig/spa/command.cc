/* <stig/spa/command.cc>

   Implements <stig/spa/command.h>.

   NOTE: Most of this code is COPY/PASTED FROM checkpoint.cc

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

#include <stig/spa/command.h>

#include <memory>
#include <string>
#include <thread>

#include <base/impossible_error.h>
#include <base/not_implemented.h>
#include <stig/rt/containers.h>
#include <stig/stig.command.cst.h>
#include <stig/type.h>
#include <tools/nycr/error.h>

using namespace Base;
using namespace Stig;
using namespace Stig::Command;
using namespace Stig::Command::Syntax;
using namespace Stig::Spa;

// TODO: This mutex isn't pretty, but it is necessary to keep bison from going KABOOOOM"
static std::mutex CommandParserLock;

/*
  TODO
  This namespace should not exist but does because of a compile bug that overrides the spa/desktop/checkpoint.cc declarations with these ones for no apparent reason.
checkpoint.test.cc: Assertion 'literal' failed error was caused by this problem.
*/
namespace FooBar {

  /* TODO */
  class TTypeVisitor : public Syntax::TType::TVisitor {
    NO_COPY_SEMANTICS(TTypeVisitor);
    public:

    TTypeVisitor(Type::TType &type) : Type(type) {}

    //virtual void operator()(const Syntax::TTypeOf *) const {throw Base::Flags.KeyError(HERE);}
    virtual void operator()(const Syntax::TParenType *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TRefType *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TTimeDiffType *) const {
      Type = Type::TTimeDiff::Get();
    }
    virtual void operator()(const Syntax::TTimePntType *) const {
      Type = Type::TTimePnt::Get();
    }
    virtual void operator()(const Syntax::TIntType *) const {
      Type = Type::TInt::Get();
    }
    virtual void operator()(const Syntax::TStrType *) const {
      Type = Type::TStr::Get();
    }
    virtual void operator()(const Syntax::TBoolType *) const {
      Type = Type::TBool::Get();
    }
    virtual void operator()(const ::Syntax::TMutableType *that) const {
      Type::TType val_type;
      that->GetType()->Accept(TTypeVisitor(val_type));
      //TODO: Assert here and give a nice warning if someone tries to make a mutable of a mutable.

      class TOptMutableTypeAtVisitor : public TOptMutableTypeAt::TVisitor {
        NO_COPY_SEMANTICS(TOptMutableTypeAtVisitor);
        public:
        TOptMutableTypeAtVisitor(Type::TType &type) : Type(type) {}

        virtual void operator()(const TMutableTypeAt *that) const final {
          that->GetType()->Accept(TTypeVisitor(Type));
        }

        virtual void operator()(const TNoMutableTypeAt *) const final {
          Type = Type::TAddr::Get({});
        }

        private:
        Type::TType &Type;
      };

      Type::TType addr_type;
      that->GetOptMutableTypeAt()->Accept(TOptMutableTypeAtVisitor(addr_type));
      Type = Type::TMutable::Get(addr_type, val_type, val_type);
    }

    virtual void operator()(const Syntax::TRealType *) const {
      Type = Type::TReal::Get();
    }
    virtual void operator()(const Syntax::TIdType *) const {
      Type = Type::TId::Get();
    }
    virtual void operator()(const Syntax::TErrType *that) const {
      Type::TType type;
      TTypeVisitor visitor(type);
      that->GetType()->Accept(visitor);
      Type = Type::TErr::Get(type);
    }
    virtual void operator()(const Syntax::TSeqType *that) const {
      Type::TType type;
      TTypeVisitor visitor(type);
      that->GetType()->Accept(visitor);
      Type = Type::TSeq::Get(type);
    }
    virtual void operator()(const Syntax::TOptType *that) const {
      Type::TType type;
      TTypeVisitor visitor(type);
      that->GetType()->Accept(visitor);
      Type = Type::TOpt::Get(type);
    }
    virtual void operator()(const Syntax::TListType *that) const {
      Type::TType type;
      TTypeVisitor visitor(type);
      that->GetType()->Accept(visitor);
      Type = Type::TList::Get(type);
    }
    virtual void operator()(const Syntax::TDictType *that) const {
      Type::TType key_type;
      Type::TType value_type;
      TTypeVisitor key_visitor(key_type);
      TTypeVisitor value_visitor(value_type);
      that->GetKey()->Accept(key_visitor);
      that->GetValue()->Accept(value_visitor);
      Type = Type::TDict::Get(key_type, value_type);
    }
    virtual void operator()(const Syntax::TSetType *that) const {
      Type::TType type;
      TTypeVisitor visitor(type);
      that->GetType()->Accept(visitor);
      Type = Type::TSet::Get(type);
    }
    //virtual void operator()(const Syntax::TResultOf *) const {NOT_IMPLEMENTED();}
    //virtual void operator()(const Syntax::TParamsOf *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TFuncType *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TAddrType *that) const;
    virtual void operator()(const Syntax::TObjType *) const;

    private:

    Type::TType &Type;

  };  // TTypeVisitor

  /* TODO */
  class TExprVisitor : public Syntax::TExpr::TVisitor {
    NO_COPY_SEMANTICS(TExprVisitor);
    public:

    /* TODO */
    TExprVisitor(Var::TVar &var) : Var(var) {}
    /* TODO */
    virtual void operator()(const Syntax::TBuiltInCeiling *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TBuiltInFloor *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TBuiltInLog *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TBuiltInLog2 *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TBuiltInLog10 *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TBuiltInReplace *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TBuiltInRandom *) const {NOT_IMPLEMENTED(); }
    virtual void operator()(const Syntax::TBuiltInToLower *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TCollatedByExpr *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TCollectedByExpr *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TBuiltInToUpper *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixBitwiseOr *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixDiv *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixLogicalOr *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixExp *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixLt *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixMinus *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixMul *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixGtEq *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixPlus *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixBitwiseAnd *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixLogicalAnd *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixEq *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixBitwiseXor *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixGt *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixLogicalXor *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixIn *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixMatch *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixMod *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixNeq *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixLtEq *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixOrElse *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixAndThen *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixReduce *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixSort *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixFilter *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixSkip *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixSplit *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixTake *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TInfixWhile *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TRefExpr *) const {throw TGeneralCommandError(HERE,"Variable references are not allowed. You probably meant a string literal, in which case, you should add quotes.");}
    virtual void operator()(const Syntax::TParenExpr *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TPostfixIsEmpty *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TPostfixIsKnownExpr*) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TPostfixIsUnknown *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TPostfixCast *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TPostfixAddrMember *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TPostfixSlice *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TPostfixIsKnown *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TPostfixObjMember *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TPostfixCall *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TPrefixExists *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TEffectingExpr *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TAssertExpr *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TWhereExpr *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TPrefixMinus *) const;
    virtual void operator()(const Syntax::TPrefixPlus *) const;
    virtual void operator()(const Syntax::TPrefixLogicalNot *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TPrefixKnown *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TPrefixLengthOf *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TPrefixReverseOf *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TPrefixTimeObj *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TPrefixAddrOf *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TDbKeysExpr *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TPrefixStart *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TPrefixSequence *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TUserIdExpr *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TSessionIdExpr *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TNowExpr *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TLhsExpr *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TRhsExpr *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TThatExpr *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TLiteralExpr *) const;
    virtual void operator()(const Syntax::TIfExpr *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TReadExpr *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TGivenExpr *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TEmptyCtor *) const;
    virtual void operator()(const Syntax::TListCtor *) const;
    virtual void operator()(const Syntax::TRangeCtor *) const {NOT_IMPLEMENTED();}
    virtual void operator()(const Syntax::TObjCtor *) const;
    virtual void operator()(const Syntax::TUnknownCtor *that) const {
      Type::TType type;
      that->GetType()->Accept(TTypeVisitor(type));
      Var = Var::TVar::Opt(Rt::TOpt<Var::TVar>(), type);
    }
    virtual void operator()(const Syntax::TSetCtor *) const;
    virtual void operator()(const Syntax::TDictCtor *) const;
    virtual void operator()(const Syntax::TAddrCtor *) const;
    virtual void operator()(const Syntax::TTimeDiffCtor *) const {throw TGeneralCommandError(HERE, "time diff constructor syntax is not allowed"); }
    virtual void operator()(const Syntax::TTimePntCtor  *) const {throw TGeneralCommandError(HERE, "time diff constructor syntax is not allowed"); }
    virtual void operator()(const Syntax::TPostfixOptCheckpoint *that) const {
      that->GetExpr()->Accept(*this);
      Var = Var::TVar::Opt(Rt::TOpt<Var::TVar>(Var), Var.GetType());
      Var = Var::TVar::Opt(Rt::TOpt<Var::TVar>(), Var.GetType());
    }

    private:

    /* TODO */
    Var::TVar &Var;

  };  // TExprVisitor

  class TOptOrderingVisitor : public TOptOrdering::TVisitor {
    NO_COPY_SEMANTICS(TOptOrderingVisitor);
    public:
    TOptOrderingVisitor(TAddrDir &dir) : Dir(dir) {}

    private:
    virtual void operator()(const TNoOrdering *) const final {
      Dir = TAddrDir::Asc;  //Default
    }
    virtual void operator()(const TDescOrdering *) const final {
      Dir = TAddrDir::Desc;
    }
    virtual void operator()(const TAscOrdering *) const final {
      Dir = TAddrDir::Asc;
    }

    TAddrDir &Dir;
  };

  class TOptAddrMemberListTailVisitor : public TOptAddrMemberListTail::TVisitor {
    NO_COPY_SEMANTICS(TOptAddrMemberListTailVisitor);
    public:

    TOptAddrMemberListTailVisitor(Var::TAddr::TAddrType &addr) : Addr(addr) {}

    virtual void operator()(const TNoAddrMemberListTail *) const {/* DO NOTHING */}
    virtual void operator()(const TAddrMemberListTail *that) const {
      Var::TVar elem;
      auto a_m = that->GetAddrMemberList()->GetAddrMember();
      /* EXTRA */ {
        TExprVisitor expr_visitor(elem);
        a_m->GetExpr()->Accept(expr_visitor);
      }
      TAddrDir dir;
      /* EXTRA */ {
        TOptOrderingVisitor ordering_visitor(dir);
        a_m->GetOptOrdering()->Accept(ordering_visitor);
      }
      Addr.push_back(std::make_pair(dir, elem));
      that->GetAddrMemberList()->GetOptAddrMemberListTail()->Accept(*this);
    }

    private:

    Var::TAddr::TAddrType &Addr;

  };

  class TOptAddrMemberListVisitor : public TOptAddrMemberList::TVisitor {
    NO_COPY_SEMANTICS(TOptAddrMemberListVisitor);
    public:

    TOptAddrMemberListVisitor(Var::TAddr::TAddrType &addr) : Addr(addr) {}

    virtual void operator()(const TAddrMemberList *that) const {
      Var::TVar elem;
      auto a_m = that->GetAddrMember();
      /* EXTRA */ {
        TExprVisitor expr_visitor(elem);
        a_m->GetExpr()->Accept(expr_visitor);
      }
      TAddrDir dir;
      /* EXTRA */ {
        TOptOrderingVisitor ordering_visitor(dir);
        a_m->GetOptOrdering()->Accept(ordering_visitor);
      }
      Addr.push_back(std::make_pair(dir, elem));
      TOptAddrMemberListTailVisitor tail_visitor(Addr);
      that->GetOptAddrMemberListTail()->Accept(tail_visitor);
    }
    virtual void operator()(const TNoAddrMemberList *) const {/* DO NOTHING */}

    private:

    Var::TAddr::TAddrType &Addr;

  };

  class TOptExprListTailVisitor : public Syntax::TOptExprListTail::TVisitor {
    NO_COPY_SEMANTICS(TOptExprListTailVisitor);
    public:

    TOptExprListTailVisitor(std::vector<Var::TVar> &list_) : List(list_) {}

    virtual void operator()(const Syntax::TExprListTail *that) const {
      Var::TVar elem;
      TExprVisitor expr_visitor(elem);
      that->GetExprList()->GetExpr()->Accept(expr_visitor);
      List.push_back(elem);
      that->GetExprList()->GetOptExprListTail()->Accept(*this);
    }
    virtual void operator()(const Syntax::TNoExprListTail *) const {/* DO NOTHING */}

    private:

    std::vector<Var::TVar> &List;

  };

  class TSetOptExprListTailVisitor : public Syntax::TOptExprListTail::TVisitor {
    NO_COPY_SEMANTICS(TSetOptExprListTailVisitor);
    public:

    TSetOptExprListTailVisitor(Rt::TSet<Var::TVar> &set_) : Set(set_) {}

    virtual void operator()(const TExprListTail *that) const {
      Var::TVar elem;
      TExprVisitor expr_visitor(elem);
      that->GetExprList()->GetExpr()->Accept(expr_visitor);
      Set.insert(elem);
      //TSetOptExprListTailVisitor tail_visitor(Set);
      that->GetExprList()->GetOptExprListTail()->Accept(*this);
    }
    virtual void operator()(const TNoExprListTail *) const {/* DO NOTHING */}

    private:

    Rt::TSet<Var::TVar> &Set;

  };

  class TOptDictMemberListTailVisitor : public Syntax::TOptDictMemberListTail::TVisitor {
    NO_COPY_SEMANTICS(TOptDictMemberListTailVisitor);
    public:

    TOptDictMemberListTailVisitor(Rt::TDict<Var::TVar, Var::TVar> &map_) : Map(map_) {}

    virtual void operator()(const TDictMemberListTail *that) const {
      Var::TVar key;
      Var::TVar val;
      TExprVisitor key_visitor(key);
      that->GetDictMemberList()->GetDictMember()->GetKey()->Accept(key_visitor);
      TExprVisitor val_visitor(val);
      that->GetDictMemberList()->GetDictMember()->GetValue()->Accept(val_visitor);
      Map.insert(std::make_pair(key, val));
      that->GetDictMemberList()->GetOptDictMemberListTail()->Accept(*this);
    }
    virtual void operator()(const TNoDictMemberListTail *) const {/* DO NOTHING */}

    private:

    Rt::TDict<Var::TVar, Var::TVar> &Map;

  };

  class TLiteralVisitor : public Syntax::TLiteral::TVisitor {
    NO_COPY_SEMANTICS(TLiteralVisitor);
    public:

    TLiteralVisitor(Var::TVar &var) : Var(var) {}

    virtual void operator()(const TTimeDiffLiteral *that) const {
      Var = Var::TVar(that->GetLexeme().AsTimeDiff());
    }
    virtual void operator()(const TTimePntLiteral *that) const {
      Var = Var::TVar(that->GetLexeme().AsTimePnt());
    }
    virtual void operator()(const TDoubleQuotedStrLiteral *that) const {
      Var = Var::TVar(that->GetLexeme().AsDoubleQuotedString());
    }
    virtual void operator()(const TSingleQuotedRawStrLiteral *that) const {
      Var = Var::TVar(that->GetLexeme().AsSingleQuotedRawString());
    }
    virtual void operator()(const TDoubleQuotedRawStrLiteral *that) const {
      Var = Var::TVar(that->GetLexeme().AsDoubleQuotedRawString());
    }
    virtual void operator()(const TSingleQuotedStrLiteral *that) const {
      Var = Var::TVar(that->GetLexeme().AsSingleQuotedString());
    }
    virtual void operator()(const TRealLiteral *that) const {
      Var = Var::TVar(that->GetLexeme().AsDouble());
    }
    virtual void operator()(const TFalseKwd *) const {
      Var = Var::TVar(false);
    }
    virtual void operator()(const TTrueKwd *) const {
      Var = Var::TVar(true);
    }
    virtual void operator()(const TIntLiteral *that) const {
      Var = Var::TVar(that->GetLexeme().AsInt());
    }
    virtual void operator()(const TIdLiteral *that) const {
      //TODO: The copy is annoying...
      std::string tmp = that->GetLexeme().GetText().substr(1,that->GetLexeme().GetText().size()-2);
      Var = Var::TVar(TUUID(tmp.c_str()));
    }

    private:

    Var::TVar &Var;

  };

  class TOptAddrTypeMemberListTailVisitor : public TOptAddrTypeMemberListTail::TVisitor {
    NO_COPY_SEMANTICS(TOptAddrTypeMemberListTailVisitor);
    public:

    TOptAddrTypeMemberListTailVisitor(Type::TAddr::TElems &type_list) : TypeList(type_list) {}

    virtual void operator()(const TNoAddrTypeMemberListTail *) const {/* DO NOTHING */}
    virtual void operator()(const TAddrTypeMemberListTail *that) const {
      Type::TType type;
      auto a_t_m = that->GetAddrTypeMemberList()->GetAddrTypeMember();
      /* EXTRA */ {
        TTypeVisitor type_visitor(type);
        a_t_m->GetType()->Accept(type_visitor);
      }
      TAddrDir dir;
      /* EXTRA */  {
        TOptOrderingVisitor ordering_visitor(dir);
        a_t_m->GetOptOrdering()->Accept(ordering_visitor);
      }
      TypeList.push_back(std::make_pair(dir, type));
      that->GetAddrTypeMemberList()->GetOptAddrTypeMemberListTail()->Accept(*this);
    }

    private:

    Type::TAddr::TElems &TypeList;

  };  //TOptAddrTypeMemberListTailVisitor

  class TOptAddrTypeMemberListVisitor : public TOptAddrTypeMemberList::TVisitor {
    NO_COPY_SEMANTICS(TOptAddrTypeMemberListVisitor);
    public:

    TOptAddrTypeMemberListVisitor(Type::TAddr::TElems &type_list) : TypeList(type_list) {}

    virtual void operator()(const TNoAddrTypeMemberList *) const {/* DO NOTHING */}
    virtual void operator()(const TAddrTypeMemberList *that) const {
      Type::TType type;
      auto a_t_m = that->GetAddrTypeMember();
      /* EXTRA */ {
        TTypeVisitor type_visitor(type);
        a_t_m->GetType()->Accept(type_visitor);
      }
      TAddrDir dir;
      /* EXTRA */ {
        TOptOrderingVisitor ordering_visitor(dir);
        a_t_m->GetOptOrdering()->Accept(ordering_visitor);
      }
      TypeList.push_back(std::make_pair(dir, type));

      TOptAddrTypeMemberListTailVisitor tail_visitor(TypeList);
      that->GetOptAddrTypeMemberListTail()->Accept(tail_visitor);
    }

    private:

    Type::TAddr::TElems &TypeList;

  };  // TOptAddrTypeMemberListVisitor

  class TOptObjTypeMemberListTailVisitor : public Syntax::TOptObjTypeMemberListTail::TVisitor {
    NO_COPY_SEMANTICS(TOptObjTypeMemberListTailVisitor);
    public:

    TOptObjTypeMemberListTailVisitor(std::map<std::string, Type::TType> &type_map) : TypeMap(type_map) {}

    virtual void operator()(const TObjTypeMemberListTail *that) const {
      Type::TType type;
      TTypeVisitor type_visitor(type);
      that->GetObjTypeMemberList()->GetObjTypeMember()->GetType()->Accept(type_visitor);
      TypeMap[that->GetObjTypeMemberList()->GetObjTypeMember()->GetName()->GetLexeme().GetText()] = type;
      that->GetObjTypeMemberList()->GetOptObjTypeMemberListTail()->Accept(*this);

    }
    virtual void operator()(const TNoObjTypeMemberListTail *) const {/* DO NOTHING */}

    private:

    std::map<std::string, Type::TType> &TypeMap;

  };  // TOptObjTypeMemberListTailVisitor

  class TOptObjTypeMemberListVisitor : public Syntax::TOptObjTypeMemberList::TVisitor {
    NO_COPY_SEMANTICS(TOptObjTypeMemberListVisitor);
    public:

    TOptObjTypeMemberListVisitor(std::map<std::string, Type::TType> &type_map) : TypeMap(type_map) {}

    virtual void operator()(const TObjTypeMemberList *that) const {
      Type::TType type;
      TTypeVisitor type_visitor(type);
      that->GetObjTypeMember()->GetType()->Accept(type_visitor);
      TypeMap[that->GetObjTypeMember()->GetName()->GetLexeme().GetText()] = type;
      TOptObjTypeMemberListTailVisitor tail_visitor(TypeMap);
      that->GetOptObjTypeMemberListTail()->Accept(tail_visitor);
    }
    virtual void operator()(const TNoObjTypeMemberList *) const {/* DO NOTHING */}

    private:

    std::map<std::string, Type::TType> &TypeMap;

  };  // TOptObjTypeMemberListVisitor

  class TOptObjMemberListTailVisitor : public Syntax::TOptObjMemberListTail::TVisitor {
    NO_COPY_SEMANTICS(TOptObjMemberListTailVisitor);
    public:

    TOptObjMemberListTailVisitor(std::unordered_map<std::string, Var::TVar> &map_) : Map(map_) {}

    virtual void operator()(const TNoObjMemberListTail *) const {/* DO NOTHING */}
    virtual void operator()(const TObjMemberListTail *that) const {
      Var::TVar elem;
      TExprVisitor expr_visitor(elem);
      that->GetObjMemberList()->GetObjMember()->GetExpr()->Accept(expr_visitor);
      Map[that->GetObjMemberList()->GetObjMember()->GetName()->GetLexeme().GetText()] = elem;
      that->GetObjMemberList()->GetOptObjMemberListTail()->Accept(*this);
    }

    private:

    std::unordered_map<std::string, Var::TVar> &Map;

  };

  class TOptObjMemberListVisitor : public Syntax::TOptObjMemberList::TVisitor {
    NO_COPY_SEMANTICS(TOptObjMemberListVisitor);
    public:

    TOptObjMemberListVisitor(std::unordered_map<std::string, Var::TVar> &map_) : Map(map_) {}

    virtual void operator()(const TNoObjMemberList *) const {/* DO NOTHING */}
    virtual void operator()(const TObjMemberList *that) const {
      Var::TVar elem;
      TExprVisitor expr_visitor(elem);
      that->GetObjMember()->GetExpr()->Accept(expr_visitor);
      Map[that->GetObjMember()->GetName()->GetLexeme().GetText()] = elem;
      TOptObjMemberListTailVisitor tail_visitor(Map);
      that->GetOptObjMemberListTail()->Accept(tail_visitor);
    }

    private:

    std::unordered_map<std::string, Var::TVar> &Map;

  };  // TOptObjMemberListVisitor

  void TTypeVisitor::operator()(const TAddrType *that) const {
    Type::TAddr::TElems type_list;
    TOptAddrTypeMemberListVisitor visitor(type_list);
    that->GetOptAddrTypeMemberList()->Accept(visitor);
    Type = Type::TAddr::Get(type_list);
  }

  void TTypeVisitor::operator()(const TObjType *that) const {
    std::map<std::string, Type::TType> type_map;
    TOptObjTypeMemberListVisitor visitor(type_map);
    that->GetOptObjTypeMemberList()->Accept(visitor);
    Type = Type::TObj::Get(type_map);
  }

  void TExprVisitor::operator()(const TObjCtor *that) const {
    std::unordered_map<std::string, Var::TVar> map_;
    TOptObjMemberListVisitor visitor(map_);
    that->GetOptObjMemberList()->Accept(visitor);
    Var = Var::TVar::Obj(map_);
  }

  void TExprVisitor::operator()(const TDictCtor *that) const {
    Rt::TDict<Var::TVar, Var::TVar> map_;
    Var::TVar key;
    Var::TVar val;
    TExprVisitor key_visitor(key);
    that->GetDictMemberList()->GetDictMember()->GetKey()->Accept(key_visitor);
    TExprVisitor val_visitor(val);
    that->GetDictMemberList()->GetDictMember()->GetValue()->Accept(val_visitor);
    map_.insert(std::make_pair(key, val));
    TOptDictMemberListTailVisitor tail_visitor(map_);
    that->GetDictMemberList()->GetOptDictMemberListTail()->Accept(tail_visitor);
    Var = Var::TVar::Dict(map_, map_.begin()->first.GetType(), map_.begin()->second.GetType());
  }

  void TExprVisitor::operator()(const TSetCtor *that) const {
    Rt::TSet<Var::TVar> set_;
    Var::TVar elem;
    TExprVisitor expr_visitor(elem);
    that->GetExprList()->GetExpr()->Accept(expr_visitor);
    set_.insert(elem);
    TSetOptExprListTailVisitor tail_visitor(set_);
    that->GetExprList()->GetOptExprListTail()->Accept(tail_visitor);
    Var = Var::TVar::Set(set_, set_.begin()->GetType());
  }

  void TExprVisitor::operator()(const TEmptyCtor *that) const {
    class TConsEmptyTypeVisitor : public TType::TVisitor {
      NO_COPY_SEMANTICS(TConsEmptyTypeVisitor);
      public:

      TConsEmptyTypeVisitor(Var::TVar &var) : Var(var) {}

      virtual void operator()(const TFuncType *) const { throw Base::TImpossibleError(HERE); }
      virtual void operator()(const TDictType *that) const {
        Type::TType key_type;
        Type::TType val_type;
        TTypeVisitor key_visitor(key_type);
        that->GetKey()->Accept(key_visitor);
        TTypeVisitor val_visitor(val_type);
        that->GetValue()->Accept(val_visitor);
        Rt::TDict<Var::TVar, Var::TVar> map_;
        Var = Var::TVar::Dict(map_, key_type, val_type);
      }
      virtual void operator()(const TSetType *that) const {
        Type::TType type;
        TTypeVisitor visitor(type);
        that->GetType()->Accept(visitor);
        Rt::TSet<Var::TVar> set_;
        Var = Var::TVar::Set(set_, type);
      }
      virtual void operator()(const TListType *that) const {
        Type::TType type;
        TTypeVisitor visitor(type);
        that->GetType()->Accept(visitor);
        std::vector<Var::TVar> list_;
        Var = Var::TVar::List(list_, type);
      }
      virtual void operator()(const TOptType *) const { throw Base::TImpossibleError(HERE); }
      virtual void operator()(const TErrType *) const { throw Base::TImpossibleError(HERE); }
      virtual void operator()(const TSeqType *) const { throw Base::TImpossibleError(HERE); }
      virtual void operator()(const TIntType *) const { throw Base::TImpossibleError(HERE); }
      virtual void operator()(const TTimePntType *) const { throw Base::TImpossibleError(HERE); }
      virtual void operator()(const TBoolType *) const { throw Base::TImpossibleError(HERE); }
      virtual void operator()(const TMutableType *) const { throw Base::TImpossibleError(HERE); }
      virtual void operator()(const TStrType *) const { throw Base::TImpossibleError(HERE); }
      virtual void operator()(const TRealType *) const { throw Base::TImpossibleError(HERE); }
      virtual void operator()(const TIdType *) const { throw Base::TImpossibleError(HERE); }
      virtual void operator()(const TTimeDiffType *) const { throw Base::TImpossibleError(HERE); }
      virtual void operator()(const TObjType *) const { throw Base::TImpossibleError(HERE); }
      virtual void operator()(const TAddrType *) const { throw Base::TImpossibleError(HERE); }
      virtual void operator()(const TParenType *) const { throw Base::TImpossibleError(HERE); }
      virtual void operator()(const TRefType *) const {
        NOT_IMPLEMENTED();
      }

      private:
      Var::TVar &Var;
    };  // TConsEmptyTypeVisitor
    TConsEmptyTypeVisitor visitor(Var);
    that->GetType()->Accept(visitor);
  }



  void TExprVisitor::operator()(const TAddrCtor *that) const {
    Var::TAddr::TAddrType addr;
    TOptAddrMemberListVisitor visitor(addr);
    that->GetOptAddrMemberList()->Accept(visitor);
    Var = Var::TVar::Addr(addr);
  }

  void TExprVisitor::operator()(const TListCtor *that) const {
    std::vector<Var::TVar> list_;
    Var::TVar elem;
    TExprVisitor expr_visitor(elem);
    that->GetExprList()->GetExpr()->Accept(expr_visitor);
    list_.push_back(elem);
    TOptExprListTailVisitor tail_visitor(list_);
    that->GetExprList()->GetOptExprListTail()->Accept(tail_visitor);
    Var = Var::TVar::List(list_, list_.begin()->GetType());
  }

  void TExprVisitor::operator()(const TPrefixMinus *that) const {
    Var::TVar var;

    const TLiteralExpr *literal = dynamic_cast<const TLiteralExpr *>(that->GetExpr());
    assert(literal);
    const TIntLiteral *int_literal = dynamic_cast<const TIntLiteral *>(literal->GetLiteral());
    if (int_literal) {
      Var = Var::TVar(-static_cast<int>(int_literal->GetLexeme().AsInt()));
    } else {
      const TRealLiteral *real_literal = dynamic_cast<const TRealLiteral *>(literal->GetLiteral());
      assert(real_literal);
      Var = Var::TVar(-static_cast<double>(real_literal->GetLexeme().AsDouble()));
    }
  }

  void TExprVisitor::operator()(const TPrefixPlus *that) const {
    TExprVisitor visitor(Var);
    that->GetExpr()->Accept(visitor);
  }

  void TExprVisitor::operator()(const TLiteralExpr *that) const {
    TLiteralVisitor visitor(Var);
    that->GetLiteral()->Accept(visitor);
  }

}  // FooBar

bool Stig::Spa::ParseCommand(const char *text, Var::TVar &var) {
  std::lock_guard<std::mutex> Lock(CommandParserLock);
  //Parse the text
  std::unique_ptr<TCommand> cst(TCommand::ParseStr(text));

  //TODO: Promote the parser errors out in a cleaner fashion, so the end user can see why their literal failed.
  if(Tools::Nycr::TError::GetFirstError()) {
    return false;
  }

  //Determine the Var from the expression
  cst->GetExpr()->Accept(FooBar::TExprVisitor(var));

  return true;
}

