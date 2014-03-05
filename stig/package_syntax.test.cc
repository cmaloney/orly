/* <stig/package_syntax.test.cc>

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

#include <iostream>

#include <stig/stig.package.cst.h>

#include <tools/nycr/test.h>
#include <test/kit.h>

using namespace Stig::Package::Syntax;
using namespace Tools::Nycr::Test;

FIXTURE(Take) {
  auto cst = TPackage::ParseStr("x = [1..5] take 2;");
  const char *ts =
    "Package {\n"
    "  OptDefSeq -> DefSeq {\n"
    "    Def -> FuncDef {\n"
    "      Name -> Name;\n"
    "      Eq -> Eq;\n"
    "      Expr -> InfixTake {\n"
    "        Lhs -> RangeCtor {\n"
    "          OpenBracket -> OpenBracket;\n"
    "          Expr -> LiteralExpr {\n"
    "            Literal -> IntLiteral;\n"
    "          }\n"
    "          OptRangeStride -> NoRangeStride;\n"
    "          Dots -> Dots;\n"
    "          RangeEnd -> ClosedRangeEnd {\n"
    "            Expr -> LiteralExpr {\n"
    "              Literal -> IntLiteral;\n"
    "            }\n"
    "            CloseBracket -> CloseBracket;\n"
    "          }\n"
    "        }\n"
    "        TakeKwd -> TakeKwd;"
    "        Rhs -> LiteralExpr {\n"
    "          Literal -> IntLiteral;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "    OptDefSeq -> NoDefSeq;\n"
    "  }\n"
    "}\n";
  auto tt = ParseNode(ts);
  EXPECT_TRUE(cst->Test(tt, 0));
  delete cst;
  delete tt;
}
FIXTURE(While) {
  auto cst = TPackage::ParseStr("x = [1..5] while (that < 3);");
  const char *ts =
    "Package {\n"
    "  OptDefSeq -> DefSeq {\n"
    "    Def -> FuncDef {\n"
    "      Name -> Name;\n"
    "      Eq -> Eq;\n"
    "      Expr -> InfixWhile {\n"
    "        Lhs -> RangeCtor {\n"
    "          OpenBracket -> OpenBracket;\n"
    "          Expr -> LiteralExpr {\n"
    "            Literal -> IntLiteral;\n"
    "          }\n"
    "          OptRangeStride -> NoRangeStride;\n"
    "          Dots -> Dots;\n"
    "          RangeEnd -> ClosedRangeEnd {\n"
    "            Expr -> LiteralExpr {\n"
    "              Literal -> IntLiteral;\n"
    "            }\n"
    "            CloseBracket -> CloseBracket;\n"
    "          }\n"
    "        }\n"
    "        WhileKwd -> WhileKwd;\n"
    "        Rhs -> ParenExpr {\n"
    "          OpenParen -> OpenParen;\n"
    "          Expr -> InfixLt {\n"
    "            Lhs -> ThatExpr {\n"
    "              ThatKwd -> ThatKwd;\n"
    "            }\n"
    "            Lt -> Lt;\n"
    "            Rhs -> LiteralExpr {\n"
    "              Literal -> IntLiteral;\n"
    "            }\n"
    "          }\n"
    "          CloseParen -> CloseParen;\n"
    "        }\n"
    "      }\n"
    "      Semi -> Semi;\n"
    "    }\n"
    "    OptDefSeq -> NoDefSeq;\n"
    "  }\n"
    "}\n";
  auto tt = ParseNode(ts);
  EXPECT_TRUE(cst->Test(tt, 0));
  delete cst;
  delete tt;
}