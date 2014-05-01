/* <orly/syntax.test.cc>

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

#include <orly/orly.checkpoint.cst.h>

#include <tools/nycr/test.h>
#include <test/kit.h>

using namespace Orly::Checkpoint::Syntax;
using namespace Tools::Nycr::Test;

// installed package

FIXTURE(InstalledPackage) {
  auto cst = TCheckpoint::ParseStr("imports <a/b> #1;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> InstalledPackage {\n"
    "      ImportsKwd -> ImportsKwd;\n"
    "      PackageName -> PackageName {\n"
    "        Lt -> Lt;"
    "        PackageNameMemberList -> PackageNameMemberList {\n"
    "          Name -> Name;\n"
    "          OptPackageNameMemberListTail -> PackageNameMemberListTail {\n"
    "            Slash -> Slash;\n"
    "            PackageNameMemberList -> PackageNameMemberList {\n"
    "              Name -> Name;\n"
    "              OptPackageNameMemberListTail -> NoPackageNameMemberListTail;\n"
    "            }\n"
    "          }\n"
    "        }\n"
    "        Gt -> Gt;"
    "        OptPackageVersion -> PackageVersion {\n"
    "          HashSign -> HashSign;\n"
    "          IntLiteral -> IntLiteral;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

// literal

FIXTURE(IdLiteral) {
  auto cst = TCheckpoint::ParseStr("x <- {00000000-0000-0000-0000-000000000000};");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> LiteralExpr {\n"
    "        Literal -> IdLiteral;\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(IntLiteral) {
  auto cst = TCheckpoint::ParseStr("x <- 3;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> LiteralExpr {\n"
    "        Literal -> IntLiteral;\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(RealLiteral) {
  auto cst = TCheckpoint::ParseStr("x <- 10.5;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> LiteralExpr {\n"
    "        Literal -> RealLiteral;\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(StrLiteral) {
  auto cst = TCheckpoint::ParseStr("x <- '10';");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> LiteralExpr {\n"
    "        Literal -> SingleQuotedStrLiteral;\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(BoolLiteral) {
  auto cst = TCheckpoint::ParseStr("x <- true;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> LiteralExpr {\n"
    "        Literal -> TrueKwd;\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

// type

FIXTURE(ParenType) {
  auto cst = TCheckpoint::ParseStr("x <- unknown (id);");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> UnknownCtor {\n"
    "        UnknownKwd -> UnknownKwd;\n"
    "        Type -> ParenType {\n"
    "          OpenParen -> OpenParen;\n"
    "          Type -> IdType {\n"
    "            IdKwd -> IdKwd;\n"
    "          }\n"
    "          CloseParen -> CloseParen;"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(RefType) {
  auto cst = TCheckpoint::ParseStr("x <- free (new_type);");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> FreeExpr {\n"
    "        FreeKwd -> FreeKwd;\n"
    "        OpenParen -> OpenParen;\n"
    "        Type -> RefType {\n"
    "          Name -> Name;\n"
    "        }\n"
    "        CloseParen -> CloseParen;\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(SeqType) {
  auto cst = TCheckpoint::ParseStr("x <- [1, 2, 3] as int*;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> PostfixCast {\n"
    "        Expr -> ListCtor {\n"
    "          OpenBracket -> OpenBracket;\n"
    "          ExprList -> ExprList {\n"
    "            Expr -> LiteralExpr {\n"
    "              Literal -> IntLiteral;\n"
    "            }\n"
    "            OptExprListTail -> ExprListTail {\n"
    "              Comma -> Comma;\n"
    "              ExprList -> ExprList {\n"
    "                Expr -> LiteralExpr {\n"
    "                  Literal -> IntLiteral;\n"
    "                }\n"
    "                OptExprListTail -> ExprListTail {\n"
    "                  Comma -> Comma;\n"
    "                  ExprList -> ExprList {\n"
    "                    Expr -> LiteralExpr {\n"
    "                      Literal -> IntLiteral;\n"
    "                    }\n"
    "                    OptExprListTail -> NoExprListTail {}\n"
    "                  }\n"
    "                }\n"
    "              }\n"
    "            }\n"
    "          }\n"
    "          CloseBracket -> CloseBracket;\n"
    "        }\n"
    "        AsKwd -> AsKwd;\n"
    "        Type -> SeqType {\n"
    "          Type -> IntType {\n"
    "            IntKwd -> IntKwd;\n"
    "          }\n"
    "          Star -> Star;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

// expr

FIXTURE(ParenExpr) {
  auto cst = TCheckpoint::ParseStr("x <- (1 + 2) * 3;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> InfixMul {\n"
    "        Lhs -> ParenExpr {\n"
    "          OpenParen -> OpenParen;\n"
    "          Expr -> InfixPlus {\n"
    "            Lhs -> LiteralExpr {\n"
    "              Literal -> IntLiteral;\n"
    "            }\n"
    "            Plus -> Plus;"
    "            Rhs -> LiteralExpr {\n"
    "              Literal -> IntLiteral;\n"
    "            }\n"
    "          }\n"
    "          CloseParen -> CloseParen;\n"
    "        }\n"
    "        Star -> Star;\n"
    "        Rhs -> LiteralExpr {\n"
    "          Literal -> IntLiteral;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(RefExpr) {
  auto cst = TCheckpoint::ParseStr("x <- y;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(ThatExpr) {
  auto cst = TCheckpoint::ParseStr("x <- that;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> ThatExpr {\n"
    "        ThatKwd -> ThatKwd;\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

// list constructor

FIXTURE(EmptyList) {
  auto cst = TCheckpoint::ParseStr("x <- empty [time_diff];");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> EmptyCtor {\n"
    "        EmptyKwd -> EmptyKwd;\n"
    "        MixfixType -> ListType {\n"
    "          OpenBracket -> OpenBracket;\n"
    "          Type -> TimeDiffType {\n"
    "            TimeDiffKwd -> TimeDiffKwd;\n"
    "          }\n"
    "          CloseBracket -> CloseBracket;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(NonEmptyList) {
  auto cst = TCheckpoint::ParseStr("x <- [1, 2, 3];");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> ListCtor {\n"
    "        OpenBracket -> OpenBracket;\n"
    "        ExprList -> ExprList {\n"
    "          Expr -> LiteralExpr {\n"
    "            Literal -> IntLiteral;\n"
    "          }\n"
    "          OptExprListTail -> ExprListTail {\n"
    "            Comma -> Comma;\n"
    "            ExprList -> ExprList {\n"
    "              Expr -> LiteralExpr {\n"
    "                Literal -> IntLiteral;\n"
    "              }\n"
    "              OptExprListTail -> ExprListTail {\n"
    "                Comma -> Comma;\n"
    "                ExprList -> ExprList {\n"
    "                  Expr -> LiteralExpr {\n"
    "                    Literal -> IntLiteral;\n"
    "                  }\n"
    "                  OptExprListTail -> NoExprListTail;\n"
    "                }\n"
    "              }\n"
    "            }\n"
    "          }\n"
    "        }\n"
    "        CloseBracket -> CloseBracket;\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

// set constructor

FIXTURE(EmptySet) {
  auto cst = TCheckpoint::ParseStr("x <- empty {str};");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> EmptyCtor {\n"
    "        EmptyKwd -> EmptyKwd;\n"
    "        MixfixType -> SetType {\n"
    "          OpenBrace -> OpenBrace;\n"
    "          Type -> StrType {\n"
    "            StrKwd -> StrKwd;\n"
    "          }\n"
    "          CloseBrace -> CloseBrace;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(NonEmptySet) {
  auto cst = TCheckpoint::ParseStr("x <- {1, 2, 3};");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> SetCtor {\n"
    "        OpenBrace -> OpenBrace;\n"
    "        ExprList -> ExprList {\n"
    "          Expr -> LiteralExpr {\n"
    "            Literal -> IntLiteral;\n"
    "          }\n"
    "          OptExprListTail -> ExprListTail {\n"
    "            Comma -> Comma;\n"
    "            ExprList -> ExprList {\n"
    "              Expr -> LiteralExpr {\n"
    "                Literal -> IntLiteral;\n"
    "              }\n"
    "              OptExprListTail -> ExprListTail {\n"
    "                Comma -> Comma;\n"
    "                ExprList -> ExprList {\n"
    "                  Expr -> LiteralExpr {\n"
    "                    Literal -> IntLiteral;\n"
    "                  }\n"
    "                  OptExprListTail -> NoExprListTail;\n"
    "                }\n"
    "              }\n"
    "            }\n"
    "          }\n"
    "        }\n"
    "        CloseBrace -> CloseBrace;\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

// dictionary constructor

FIXTURE(EmptyDict) {
  auto cst = TCheckpoint::ParseStr("x <- empty {int:bool};");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> EmptyCtor {\n"
    "        EmptyKwd -> EmptyKwd;\n"
    "        MixfixType -> DictType {\n"
    "          OpenBrace -> OpenBrace;\n"
    "          Key -> IntType {\n"
    "            IntKwd -> IntKwd;\n"
    "          }\n"
    "          Colon -> Colon;\n"
    "          Value -> BoolType {\n"
    "            BoolKwd -> BoolKwd;\n"
    "          }\n"
    "          CloseBrace -> CloseBrace;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(NonEmptyDict) {
  auto cst = TCheckpoint::ParseStr("x <- { 1: true, 2: true, 3: false };");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> DictCtor {\n"
    "        OpenBrace -> OpenBrace;\n"
    "        DictMemberList -> DictMemberList {\n"
    "          DictMember -> DictMember {\n"
    "            Key -> LiteralExpr {\n"
    "              Literal -> IntLiteral;\n"
    "            }\n"
    "            Colon -> Colon;\n"
    "            Value -> LiteralExpr {\n"
    "              Literal -> TrueKwd;\n"
    "            }\n"
    "          }\n"
    "          OptDictMemberListTail -> DictMemberListTail {\n"
    "            Comma -> Comma;\n"
    "            DictMemberList -> DictMemberList {\n"
    "              DictMember -> DictMember {\n"
    "                Key -> LiteralExpr {\n"
    "                  Literal -> IntLiteral;\n"
    "                }\n"
    "                Colon -> Colon;\n"
    "                Value -> LiteralExpr {\n"
    "                  Literal -> TrueKwd;\n"
    "                }\n"
    "              }\n"
    "              OptDictMemberListTail -> DictMemberListTail {\n"
    "                Comma -> Comma;\n"
    "                DictMemberList -> DictMemberList {\n"
    "                  DictMember -> DictMember {\n"
    "                    Key -> LiteralExpr {\n"
    "                      Literal -> IntLiteral;\n"
    "                    }\n"
    "                    Colon -> Colon;\n"
    "                    Value -> LiteralExpr {\n"
    "                      Literal -> FalseKwd;\n"
    "                    }\n"
    "                  }\n"
    "                  OptDictMemberListTail -> NoDictMemberListTail;\n"
    "                }\n"
    "              }\n"
    "            }\n"
    "          }\n"
    "        }\n"
    "        CloseBrace -> CloseBrace;\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
    auto tt = ParseNode(ts);
    EXPECT_TRUE(cst->Test(tt.get(), 0));
}

// empty addr

FIXTURE(EmptyAddr) {
  auto cst = TCheckpoint::ParseStr("x <- <[]>;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq;\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> AddrCtor {\n"
    "        OpenAddr -> OpenAddr;\n"
    "        OptAddrMemberList -> NoAddrMemberList {}\n"
    "        CloseAddr -> CloseAddr;\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(NonEmptyAddr) {
  auto cst = TCheckpoint::ParseStr("x <- <[1, asc 1.1]>;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq;\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> AddrCtor {\n"
    "        OpenAddr -> OpenAddr;\n"
    "        OptAddrMemberList -> AddrMemberList {\n"
    "          AddrMember -> AddrMember {\n"
    "            OptOrdering -> NoOrdering;\n"
    "            Expr -> LiteralExpr {\n"
    "              Literal -> IntLiteral;\n"
    "            }\n"
    "          }\n"
    "          OptAddrMemberListTail -> AddrMemberListTail {\n"
    "            Comma -> Comma;\n"
    "            AddrMemberList -> AddrMemberList {\n"
    "              AddrMember -> AddrMember {\n"
    "                OptOrdering -> AscOrdering {\n"
    "                  AscKwd -> AscKwd;\n"
    "                }\n"
    "                Expr -> LiteralExpr {\n"
    "                  Literal -> RealLiteral;\n"
    "                }\n"
    "              }\n"
    "              OptAddrMemberListTail -> NoAddrMemberListTail;\n"
    "            }\n"
    "          }\n"
    "        }\n"
    "        CloseAddr -> CloseAddr;\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

// object constructor

FIXTURE(EmptyObj) {
  auto cst = TCheckpoint::ParseStr("x <- <{}>;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq;\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> ObjCtor {\n"
    "        OpenObj -> OpenObj;\n"
    "        OptObjMemberList -> NoObjMemberList {}\n"
    "        CloseObj -> CloseObj;\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(NonEmptyObj) {
  auto cst = TCheckpoint::ParseStr("x <- <{ .foo: 10, .bar: 2.5 }>;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq;\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> ObjCtor {\n"
    "        OpenObj -> OpenObj;\n"
    "        OptObjMemberList -> ObjMemberList {\n"
    "          ObjMember -> ObjMember {\n"
    "            Dot -> Dot;\n"
    "            Name -> Name;\n"
    "            Colon -> Colon;\n"
    "            Expr -> LiteralExpr {\n"
    "              Literal -> IntLiteral;\n"
    "            }\n"
    "          }\n"
    "          OptObjMemberListTail -> ObjMemberListTail {\n"
    "            Comma -> Comma;\n"
    "            ObjMemberList -> ObjMemberList {\n"
    "              ObjMember -> ObjMember {\n"
    "                Dot -> Dot;\n"
    "                Name -> Name;\n"
    "                Colon -> Colon;\n"
    "                Expr -> LiteralExpr {\n"
    "                  Literal -> RealLiteral;\n"
    "                }\n"
    "              }\n"
    "              OptObjMemberListTail -> NoObjMemberListTail;\n"
    "            }\n"
    "          }\n"
    "        }\n"
    "        CloseObj -> CloseObj;\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

// range constructor

FIXTURE(Range) {
  auto cst = TCheckpoint::ParseStr("x <- [0..10);");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq;\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> RangeCtor {\n"
    "        OpenBracket -> OpenBracket;\n"
    "        Expr -> LiteralExpr {\n"
    "          Literal -> IntLiteral;\n"
    "        }\n"
    "        OptRangeStride -> NoRangeStride;\n"
    "        Dots -> Dots;\n"
    "        RangeEnd -> OpenRangeEnd {\n"
    "          Expr -> LiteralExpr {\n"
    "            Literal -> IntLiteral;\n"
    "          }\n"
    "          CloseParen -> CloseParen;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(RangeWithStrideAndUndefinedEnd) {
  auto cst = TCheckpoint::ParseStr("x <- [2, 4 .. );");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq;\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> RangeCtor {\n"
    "        OpenBracket -> OpenBracket;\n"
    "        Expr -> LiteralExpr {\n"
    "          Literal -> IntLiteral;\n"
    "        }\n"
    "        OptRangeStride -> RangeStride {\n"
    "          Comma -> Comma;\n"
    "          Expr -> LiteralExpr {\n"
    "            Literal -> IntLiteral;\n"
    "          }\n"
    "        }\n"
    "        Dots -> Dots;\n"
    "        RangeEnd -> UndefinedRangeEnd {\n"
    "          CloseParen -> CloseParen;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(PrefixMinus) {
  auto cst = TCheckpoint::ParseStr("x <- -y;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> PrefixMinus {\n"
    "        Minus -> Minus;\n"
    "        Expr -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(PrefixPlus) {
  auto cst = TCheckpoint::ParseStr("x <- +y;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> PrefixPlus {\n"
    "        Plus -> Plus;\n"
    "        Expr -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(PrefixLogicalNot) {
  auto cst = TCheckpoint::ParseStr("x <- not y;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> PrefixLogicalNot {\n"
    "        NotKwd -> NotKwd;\n"
    "        Expr -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(InfixDiv) {
  auto cst = TCheckpoint::ParseStr("x <- y / z;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> InfixDiv {\n"
    "        Lhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "        Slash -> Slash;\n"
    "        Rhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(InfixExp) {
  auto cst = TCheckpoint::ParseStr("x <- y ** z;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> InfixExp {\n"
    "        Lhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "        Stars -> Stars;\n"
    "        Rhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(InfixMinus) {
  auto cst = TCheckpoint::ParseStr("x <- y - z;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> InfixMinus {\n"
    "        Lhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "        Minus -> Minus;\n"
    "        Rhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}
FIXTURE(InfixMod) {
  auto cst = TCheckpoint::ParseStr("x <- y % z;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> InfixMod {\n"
    "        Lhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "        Percent -> Percent;\n"
    "        Rhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(InfixMul) {
  auto cst = TCheckpoint::ParseStr("x <- y * z;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> InfixMul {\n"
    "        Lhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "        Star -> Star;\n"
    "        Rhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(InfixPlus) {
  auto cst = TCheckpoint::ParseStr("x <- y + z;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> InfixPlus {\n"
    "        Lhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "        Plus -> Plus;\n"
    "        Rhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(InfixEq) {
  auto cst = TCheckpoint::ParseStr("x <- y == z;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> InfixEq {\n"
    "        Lhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "        EqEq -> EqEq;\n"
    "        Rhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(InfixNeq) {
  auto cst = TCheckpoint::ParseStr("x <- y != z;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> InfixNeq {\n"
    "        Lhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "        Neq -> Neq;\n"
    "        Rhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(InfixLt) {
  auto cst = TCheckpoint::ParseStr("x <- y < z;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> InfixLt {\n"
    "        Lhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "        Lt -> Lt;\n"
    "        Rhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(InfixLtEq) {
  auto cst = TCheckpoint::ParseStr("x <- y <= z;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> InfixLtEq {\n"
    "        Lhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "        LtEq -> LtEq;\n"
    "        Rhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(InfixGt) {
  auto cst = TCheckpoint::ParseStr("x <- y > z;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> InfixGt {\n"
    "        Lhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "        Gt -> Gt;\n"
    "        Rhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(InfixGtEq) {
  auto cst = TCheckpoint::ParseStr("x <- y >= z;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> InfixGtEq {\n"
    "        Lhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "        GtEq -> GtEq;\n"
    "        Rhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(InfixLogicalAnd) {
  auto cst = TCheckpoint::ParseStr("x <- y and z;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> InfixLogicalAnd {\n"
    "        Lhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "        AndKwd -> AndKwd;\n"
    "        Rhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(InfixLogicalOr) {
  auto cst = TCheckpoint::ParseStr("x <- y or z;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> InfixLogicalOr {\n"
    "        Lhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "        OrKwd -> OrKwd;\n"
    "        Rhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(InfixLogicalXor) {
  auto cst = TCheckpoint::ParseStr("x <- y xor z;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> InfixLogicalXor {\n"
    "        Lhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "        XorKwd -> XorKwd;\n"
    "        Rhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(InfixIn) {
  auto cst = TCheckpoint::ParseStr("x <- y in z;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> InfixIn {\n"
    "        Lhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "        InKwd -> InKwd;\n"
    "        Rhs -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(PostFixObjMember) {
  auto cst = TCheckpoint::ParseStr("x <- y.foo;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> PostfixObjMember {\n"
    "        Expr -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "        Dot -> Dot;\n"
    "        Name -> Name;\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(PostFixIsKnown) {
  auto cst = TCheckpoint::ParseStr("x <- y is known;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> PostfixIsKnown {\n"
    "        Expr -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "        IsKwd -> IsKwd;\n"
    "        KnownKwd -> KnownKwd;\n"
    "        OptKnownBool -> NoKnownBool;\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(PostFixIsKnownBoolLiteral) {
  auto cst = TCheckpoint::ParseStr("x <- y is known true;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> PostfixIsKnown {\n"
    "        Expr -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "        IsKwd -> IsKwd;\n"
    "        KnownKwd -> KnownKwd;\n"
    "        OptKnownBool -> KnownBool {\n"
    "          BoolLiteral -> TrueKwd;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(PostFixIsUnknown) {
  auto cst = TCheckpoint::ParseStr("x <- y is unknown;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> PostfixIsUnknown {\n"
    "        Expr -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "        IsKwd -> IsKwd;\n"
    "        UnknownKwd -> UnknownKwd;\n"
    "        OptKnownBool -> NoKnownBool;\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(PostfixCast) {
  auto cst = TCheckpoint::ParseStr("x <- y as [real?]?;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> PostfixCast {\n"
    "        Expr -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "        AsKwd -> AsKwd;\n"
    "        Type -> OptType {\n"
    "          Type -> ListType {\n"
    "            OpenBracket -> OpenBracket;\n"
    "            Type -> OptType {\n"
    "              Type -> RealType {\n"
    "                RealKwd -> RealKwd;\n"
    "              }\n"
    "              QuestionMark -> QuestionMark;\n"
    "            }\n"
    "            CloseBracket -> CloseBracket;\n"
    "          }\n"
    "          QuestionMark -> QuestionMark;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(PostfixSlice) {
  auto cst = TCheckpoint::ParseStr("x <- y[10];");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> PostfixSlice {\n"
    "        Expr -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "        OpenBracket -> OpenBracket;\n"
    "        Slice -> SingleSlice {\n"
    "          Expr -> LiteralExpr {\n"
    "            Literal -> IntLiteral;\n"
    "          }\n"
    "        }\n"
    "        CloseBracket -> CloseBracket;"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(ReadExpr) {
  auto cst = TCheckpoint::ParseStr("x <- read (time_pnt) from <[\"games\", \"cafe\"]>;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> ReadExpr {\n"
    "        ReadKwd -> ReadKwd;\n"
    "        OpenParen -> OpenParen;\n"
    "        Type -> TimePntType {\n"
    "          TimePntKwd -> TimePntKwd;\n"
    "        }\n"
    "        CloseParen -> CloseParen;\n"
    "        FromKwd -> FromKwd;\n"
    "        Expr -> AddrCtor {\n"
    "          OpenAddr -> OpenAddr;\n"
    "          OptAddrMemberList -> AddrMemberList {\n"
    "            AddrMember -> AddrMember {\n"
    "              OptOrdering -> NoOrdering;\n"
    "              Expr -> LiteralExpr {\n"
    "                Literal -> DoubleQuotedStrLiteral;\n"
    "              }\n"
    "            }\n"
    "            OptAddrMemberListTail -> AddrMemberListTail {\n"
    "              Comma -> Comma;\n"
    "              AddrMemberList -> AddrMemberList {\n"
    "                AddrMember -> AddrMember {\n"
    "                  OptOrdering -> NoOrdering;\n"
    "                  Expr -> LiteralExpr {\n"
    "                    Literal -> DoubleQuotedStrLiteral;\n"
    "                  }\n"
    "                }\n"
    "                OptAddrMemberListTail -> NoAddrMemberListTail;\n"
    "              }\n"
    "            }\n"
    "          }\n"
    "          CloseAddr -> CloseAddr;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(AssertExpr) {
  auto cst = TCheckpoint::ParseStr("x <- (y) assert { that > 10; };");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> AssertExpr {\n"
    "        OpenParen -> OpenParen;\n"
    "        Expr -> RefExpr {\n"
    "          Name -> Name;\n"
    "        }\n"
    "        CloseParen -> CloseParen;\n"
    "        AssertKwd -> AssertKwd;\n"
    "        OpenBrace -> OpenBrace;\n"
    "        OptAssertionSeq -> AssertionSeq {\n"
    "          Assertion -> UnlabeledAssertion {\n"
    "            Expr -> InfixGt {\n"
    "              Lhs -> ThatExpr {\n"
    "                ThatKwd -> ThatKwd;\n"
    "              }\n"
    "              Gt -> Gt;\n"
    "              Rhs -> LiteralExpr {\n"
    "                Literal -> IntLiteral;\n"
    "              }\n"
    "            }\n"
    "            Semi -> Semi;\n"
    "          }\n"
    "          OptAssertionSeq -> NoAssertionSeq {}\n"
    "        }\n"
    "        CloseBrace -> CloseBrace;\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(IfExpr) {
  auto cst = TCheckpoint::ParseStr("x <- ( 10 if y == z else 15 );");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> IfExpr {\n"
    "        OpenParen -> OpenParen;\n"
    "        TrueCase -> LiteralExpr {\n"
    "          Literal -> IntLiteral;\n"
    "        }\n"
    "        IfKwd -> IfKwd;\n"
    "        Predicate -> InfixEq {\n"
    "          Lhs -> RefExpr {\n"
    "            Name -> Name;\n"
    "          }\n"
    "          EqEq -> EqEq;\n"
    "          Rhs -> RefExpr {\n"
    "            Name -> Name;\n"
    "          }\n"
    "        }\n"
    "        ElseKwd -> ElseKwd;\n"
    "        FalseCase -> LiteralExpr {\n"
    "          Literal -> IntLiteral;\n"
    "        }\n"
    "        CloseParen -> CloseParen;\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(GivenExpr) {
  auto cst = TCheckpoint::ParseStr("x <- given ( time_pnt );");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> GivenExpr {\n"
    "        GivenKwd -> GivenKwd;\n"
    "        OpenParen -> OpenParen;\n"
    "        Type -> TimePntType {\n"
    "          TimePntKwd -> TimePntKwd;\n"
    "        }\n"
    "        OptDefaultValue -> NoDefaultValue;\n"
    "        CloseParen -> CloseParen;\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(WhereExpr) {
  auto cst = TCheckpoint::ParseStr("x <- (y + z) where { y = 10; z = 20; };");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> WhereExpr {\n"
    "        OpenParen -> OpenParen;\n"
    "        Expr -> InfixPlus {\n"
    "          Lhs -> RefExpr {\n"
    "            Name -> Name;\n"
    "          }\n"
    "          Plus -> Plus;\n"
    "          Rhs -> RefExpr {\n"
    "            Name -> Name;\n"
    "          }\n"
    "        }\n"
    "        CloseParen -> CloseParen;\n"
    "        WhereKwd -> WhereKwd;\n"
    "        OpenBrace -> OpenBrace;\n"
    "        OptDefSeq -> DefSeq {\n"
    "          Def -> FuncDef {\n"
    "            Name -> Name;\n"
    "            Eq -> Eq;\n"
    "            Expr -> LiteralExpr {\n"
    "              Literal -> IntLiteral;\n"
    "            }\n"
    "            Semi -> Semi;\n"
    "          }\n"
    "          OptDefSeq -> DefSeq {\n"
    "            Def -> FuncDef {\n"
    "              Name -> Name;\n"
    "              Eq -> Eq;\n"
    "              Expr -> LiteralExpr {\n"
    "                Literal -> IntLiteral;\n"
    "              }\n"
    "              Semi -> Semi;\n"
    "            }\n"
    "            OptDefSeq -> NoDefSeq;\n"
    "          }\n"
    "        }\n"
    "        CloseBrace -> CloseBrace;\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(GivenOptDefault) {
  auto cst = TCheckpoint::ParseStr("x <- given ( int or 10 );");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> GivenExpr {\n"
    "        GivenKwd -> GivenKwd;\n"
    "        OpenParen -> OpenParen;\n"
    "        Type -> IntType {\n"
    "          IntKwd -> IntKwd;\n"
    "        }\n"
    "        OptDefaultValue -> DefaultValue {\n"
    "          OrKwd -> OrKwd;\n"
    "          Expr -> LiteralExpr {\n"
    "            Literal -> IntLiteral;\n"
    "          }\n"
    "        }\n"
    "        CloseParen -> CloseParen;\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(EffectingExpr) {
  auto cst = TCheckpoint::ParseStr("x <- (10) effecting { <[ 'games', r'cafe' ]> <- 15; };");

  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> EffectingExpr {\n"
    "        OpenParen -> OpenParen;\n"
    "        Expr -> LiteralExpr {\n"
    "          Literal -> IntLiteral;\n"
    "        }\n"
    "        CloseParen -> CloseParen;\n"
    "        EffectingKwd -> EffectingKwd;\n"
    "        StmtBlock -> StmtBlock {\n"
    "          OpenBrace -> OpenBrace;\n"
    "          OptStmtSeq -> StmtSeq {\n"
    "            Stmt -> MutateStmt {\n"
    "              OptExistenceConstraint -> NoExistenceConstraint;\n"
    "              Lhs -> AddrCtor {\n"
    "                OpenAddr -> OpenAddr;\n"
    "                OptAddrMemberList -> AddrMemberList {\n"
    "                  AddrMember -> AddrMember {\n"
    "                    OptOrdering -> NoOrdering;"
    "                    Expr -> LiteralExpr {\n"
    "                      Literal -> SingleQuotedStrLiteral;\n"
    "                    }\n"
    "                  }\n"
    "                  OptAddrMemberListTail -> AddrMemberListTail {\n"
    "                    Comma -> Comma;\n"
    "                    AddrMemberList -> AddrMemberList {\n"
    "                      AddrMember -> AddrMember {\n"
    "                        OptOrdering -> NoOrdering;\n"
    "                        Expr -> LiteralExpr {\n"
    "                          Literal -> SingleQuotedRawStrLiteral;\n"
    "                        }\n"
    "                      }\n"
    "                      OptAddrMemberListTail -> NoAddrMemberListTail;\n"
    "                    }\n"
    "                  }\n"
    "                }\n"
    "                CloseAddr -> CloseAddr;\n"
    "              }\n"
    "              Mutation -> MutationAssign {\n"
    "                LeftArrow -> LeftArrow;\n"
    "              }\n"
    "              Rhs -> LiteralExpr {\n"
    "                Literal -> IntLiteral;\n"
    "              }\n"
    "              Semi -> Semi;\n"
    "            }\n"
    "            OptStmtSeq -> NoStmtSeq;\n"
    "          }\n"
    "          CloseBrace -> CloseBrace;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

// precedence

FIXTURE(LogicalOrLogicalXor) {
  auto cst = TCheckpoint::ParseStr("x <- true or false xor false;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> InfixLogicalOr {\n"
    "        Lhs -> LiteralExpr {\n"
    "          Literal -> TrueKwd;\n"
    "        }\n"
    "        OrKwd -> OrKwd;\n"
    "        Rhs -> InfixLogicalXor {\n"
    "          Lhs -> LiteralExpr {\n"
    "            Literal -> FalseKwd;\n"
    "          }\n"
    "          XorKwd -> XorKwd;\n"
    "          Rhs -> LiteralExpr {\n"
    "            Literal -> FalseKwd;\n"
    "          }\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(LogicalXorLogicalAnd) {
  auto cst = TCheckpoint::ParseStr("x <- false xor false and true;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> InfixLogicalXor {\n"
    "        Lhs -> LiteralExpr {\n"
    "          Literal -> FalseKwd;\n"
    "        }\n"
    "        XorKwd -> XorKwd;\n"
    "        Rhs -> InfixLogicalAnd {\n"
    "          Lhs -> LiteralExpr {\n"
    "            Literal -> FalseKwd;\n"
    "          }\n"
    "          AndKwd -> AndKwd;"
    "          Rhs -> LiteralExpr {\n"
    "            Literal -> TrueKwd;\n"
    "          }\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(LogicalAndEquality) {
  auto cst = TCheckpoint::ParseStr("x <- false and false == true;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> InfixLogicalAnd {\n"
    "        Lhs -> LiteralExpr {\n"
    "          Literal -> FalseKwd;\n"
    "        }\n"
    "        AndKwd -> AndKwd;\n"
    "        Rhs -> InfixEq {\n"
    "          Lhs -> LiteralExpr {\n"
    "            Literal -> FalseKwd;\n"
    "          }\n"
    "          EqEq -> EqEq;"
    "          Rhs -> LiteralExpr {\n"
    "            Literal -> TrueKwd;\n"
    "          }\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(EqualityInequality) {
  auto cst = TCheckpoint::ParseStr("x <- true != true < false;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> InfixNeq {\n"
    "        Lhs -> LiteralExpr {\n"
    "          Literal -> TrueKwd;\n"
    "        }\n"
    "        Neq -> Neq;\n"
    "        Rhs -> InfixLt {\n"
    "          Lhs -> LiteralExpr {\n"
    "            Literal -> TrueKwd;\n"
    "          }\n"
    "          Lt -> Lt;"
    "          Rhs -> LiteralExpr {\n"
    "            Literal -> FalseKwd;\n"
    "          }\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(InequalityBitwiseOr) {
  auto cst = TCheckpoint::ParseStr("x <- true != false | true;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> InfixNeq {\n"
    "        Lhs -> LiteralExpr {\n"
    "          Literal -> TrueKwd;\n"
    "        }\n"
    "        Neq -> Neq;\n"
    "        Rhs -> InfixBitwiseOr {\n"
    "          Lhs -> LiteralExpr {\n"
    "            Literal -> FalseKwd;\n"
    "          }\n"
    "          Bar -> Bar;"
    "          Rhs -> LiteralExpr {\n"
    "            Literal -> TrueKwd;\n"
    "          }\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(BitwiseOrBitwiseXor) {
  auto cst = TCheckpoint::ParseStr("x <- false | false ^ false;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> InfixBitwiseOr {\n"
    "        Lhs -> LiteralExpr {\n"
    "          Literal -> FalseKwd;\n"
    "        }\n"
    "        Bar -> Bar;\n"
    "        Rhs -> InfixBitwiseXor {\n"
    "          Lhs -> LiteralExpr {\n"
    "            Literal -> FalseKwd;\n"
    "          }\n"
    "          Hat -> Hat;"
    "          Rhs -> LiteralExpr {\n"
    "            Literal -> FalseKwd;\n"
    "          }\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(BitwiseXorBitwiseAnd) {
  auto cst = TCheckpoint::ParseStr("x <- false ^ false & true;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> InfixBitwiseXor {\n"
    "        Lhs -> LiteralExpr {\n"
    "          Literal -> FalseKwd;\n"
    "        }\n"
    "        Hat -> Hat;\n"
    "        Rhs -> InfixBitwiseAnd {\n"
    "          Lhs -> LiteralExpr {\n"
    "            Literal -> FalseKwd;\n"
    "          }\n"
    "          Ampersand -> Ampersand;"
    "          Rhs -> LiteralExpr {\n"
    "            Literal -> TrueKwd;\n"
    "          }\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

// TODO: Currently there are no shift_and_rotate operators
FIXTURE(InequalityShiftAndRotate) {}
FIXTURE(ShiftAndRotateAddAndSub) {}

FIXTURE(BitwiseAndAddAndSub) {
  auto cst = TCheckpoint::ParseStr("x <- 6 & 2 + 3;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> InfixBitwiseAnd {\n"
    "        Lhs -> LiteralExpr {\n"
    "          Literal -> IntLiteral;\n"
    "        }\n"
    "        Ampersand -> Ampersand;\n"
    "        Rhs -> InfixPlus {\n"
    "          Lhs -> LiteralExpr {\n"
    "            Literal -> IntLiteral;\n"
    "          }\n"
    "          Plus -> Plus;"
    "          Rhs -> LiteralExpr {\n"
    "            Literal -> IntLiteral;\n"
    "          }\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(AddAndSubMulAndDiv) {
  auto cst = TCheckpoint::ParseStr("x <- 6 - 2 * 3;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> InfixMinus {\n"
    "        Lhs -> LiteralExpr {\n"
    "          Literal -> IntLiteral;\n"
    "        }\n"
    "        Minus -> Minus;\n"
    "        Rhs -> InfixMul {\n"
    "          Lhs -> LiteralExpr {\n"
    "            Literal -> IntLiteral;\n"
    "          }\n"
    "          Star -> Star;"
    "          Rhs -> LiteralExpr {\n"
    "            Literal -> IntLiteral;\n"
    "          }\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(MulAndDivExp) {
  auto cst = TCheckpoint::ParseStr("x <- 1 / 3 ** 4;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> InfixDiv {\n"
    "        Lhs -> LiteralExpr {\n"
    "          Literal -> IntLiteral;\n"
    "        }\n"
    "        Slash -> Slash;\n"
    "        Rhs -> InfixExp {\n"
    "          Lhs -> LiteralExpr {\n"
    "            Literal -> IntLiteral;\n"
    "          }\n"
    "          Stars -> Stars;\n"
    "          Rhs -> LiteralExpr {\n"
    "            Literal -> IntLiteral;\n"
    "          }\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(ExpUnary) {
  auto cst = TCheckpoint::ParseStr("x <- 3 ** -4 ;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> InfixExp {\n"
    "        Lhs -> LiteralExpr {\n"
    "          Literal -> IntLiteral;\n"
    "        }\n"
    "        Stars -> Stars;\n"
    "        Rhs -> PrefixMinus {\n"
    "          Minus -> Minus;\n"
    "          Expr -> LiteralExpr {\n"
    "            Literal -> IntLiteral;\n"
    "          }\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}

FIXTURE(UnaryFuncAndMember) {
  auto cst = TCheckpoint::ParseStr("x <- -a.dist;");
  const char *ts =
    "Checkpoint {\n"
    "  OptCheckpointStmtSeq -> CheckpointStmtSeq {\n"
    "    OptCheckpointStmtSeq -> NoCheckpointStmtSeq {}\n"
    "    CheckpointStmt -> KvEntry {\n"
    "      Key -> RefExpr {\n"
    "        Name -> Name;\n"
    "      }\n"
    "      LeftArrow -> LeftArrow;\n"
    "      Value -> PrefixMinus {\n"
    "        Minus -> Minus;\n"
    "        Expr -> PostfixObjMember {\n"
    "          Expr -> RefExpr {\n"
    "            Name -> Name;\n"
    "          }\n"
    "          Dot -> Dot;\n"
    "          Name -> Name;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "  }\n"
    "}\n";
  EXPECT_TRUE(cst->Test(ParseNode(ts).get(), 0));
}