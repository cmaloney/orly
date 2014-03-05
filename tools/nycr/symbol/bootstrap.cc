/* <tools/nycr/symbol/bootstrap.cc>

   Implements <tools/nycr/symbol/bootstrap.h>.

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

#include <tools/nycr/symbol/bootstrap.h>

#include <tools/nycr/symbol/anonymous_member.h>
#include <tools/nycr/symbol/base.h>
#include <tools/nycr/symbol/error_member.h>
#include <tools/nycr/symbol/keyword.h>
#include <tools/nycr/symbol/language.h>
#include <tools/nycr/symbol/named_member.h>
#include <tools/nycr/symbol/operator.h>
#include <tools/nycr/symbol/rule.h>

using namespace Tools::Nycr::Symbol;

TLanguage *Tools::Nycr::Symbol::Bootstrap() {
  // Here's the nycr language, written out manually in nycr symbols.
  // (We're so meta, even this acronym.)

  // assoc;
  // left_kwd : assoc = 'left';
  // right_kwd : assoc = 'right';
  // nonassoc_kwd : assoc = 'nonassoc';
  auto assoc = new TBase("assoc");
  new TKeyword("left_kwd", assoc, "left", *Base::TOpt<int>::Unknown);
  new TKeyword("right_kwd", assoc, "right", *Base::TOpt<int>::Unknown);
  new TKeyword("nonassoc_kwd", assoc, "nonassoc", *Base::TOpt<int>::Unknown);

  // empty_kwd = 'empty';
  // error_kwd = 'error';
  // prec_kwd  = 'prec';
  // pri_kwd   = 'pri';
  // rr_kwd    = 'rr';
  // sr_kwd    = 'sr';
  auto empty_kwd = new TKeyword("empty_kwd", 0, "empty", *Base::TOpt<int>::Unknown);
  auto error_kwd = new TKeyword("error_kwd", 0, "error", *Base::TOpt<int>::Unknown);
  auto prec_kwd = new TKeyword("prec_kwd", 0, "prec", *Base::TOpt<int>::Unknown);
  auto pri_kwd = new TKeyword("pri_kwd", 0, "pri", *Base::TOpt<int>::Unknown);
  auto rr_kwd = new TKeyword("rr_kwd", 0, "rr", *Base::TOpt<int>::Unknown);
  auto sr_kwd = new TKeyword("sr_kwd", 0, "sr", *Base::TOpt<int>::Unknown);

  // arrow       = '"->"';
  // colon       = '":"';
  // close_angle = '">"';
  // eq          = '"="';
  // open_angle  = '"<"';
  // semi        = '";"';
  // slash       = '"/"';
  auto arrow = new TKeyword("arrow", 0, "\\-\\>", *Base::TOpt<int>::Unknown);
  auto colon = new TKeyword("colon", 0, "\\:", *Base::TOpt<int>::Unknown);
  auto close_angle = new TKeyword("close_angle", 0, "\\>", *Base::TOpt<int>::Unknown);
  auto eq = new TKeyword("eq", 0, "\\=", *Base::TOpt<int>::Unknown);
  auto open_angle = new TKeyword("open_angle", 0, "\\<", *Base::TOpt<int>::Unknown);
  auto semi = new TKeyword("semi", 0, "\\;", *Base::TOpt<int>::Unknown);
  auto slash = new TKeyword("slash", 0, "\\/", *Base::TOpt<int>::Unknown);

  // int_literal = r'(\+|\-)?[[:digit:]]+';
  auto int_literal = new TKeyword("int_literal", 0, "(\\+|\\-)?[[:digit:]]+", *Base::TOpt<int>::Unknown);

  // str_literal;
  // single_quoted_str_literal     : str_literal = "{SINGLE_QUOTED_STRING}";
  // double_quoted_str_literal     : str_literal = "{DOUBLE_QUOTED_STRING}";
  // single_quoted_raw_str_literal : str_literal = "{SINGLE_QUOTED_RAW_STRING}" pri 1;
  // double_quoted_raw_str_literal : str_literal = "{DOUBLE_QUOTED_RAW_STRING}" pri 1;
  auto str_literal = new TBase("str_literal");
  new TKeyword("single_quoted_str_literal", str_literal, "{SINGLE_QUOTED_STRING}", *Base::TOpt<int>::Unknown);
  new TKeyword("double_quoted_str_literal", str_literal, "{DOUBLE_QUOTED_STRING}", *Base::TOpt<int>::Unknown);
  new TKeyword("single_quoted_raw_str_literal", str_literal, "{SINGLE_QUOTED_RAW_STRING}", 1);
  new TKeyword("double_quoted_raw_str_literal", str_literal, "{DOUBLE_QUOTED_RAW_STRING}", 1);

  // name = "[a-zA-Z]+[a-zA-Z0-9_]*";
  auto name = new TKeyword("name", 0, "[a-zA-Z]+[a-zA-Z0-9_]*", 2);

  // decl;
  // opt_decl_seq;
  // no_decl_seq : opt_decl_seq -> empty;
  // decl_seq : opt_decl_seq -> decl opt_decl_seq;
  auto decl = new TBase("decl");
  auto opt_decl_seq = new TBase("opt_decl_seq");
  new TRule("no_decl_seq", opt_decl_seq);
  auto decl_seq = new TRule("decl_seq", opt_decl_seq);
    new TAnonymousMember(decl_seq, decl);
    new TAnonymousMember(decl_seq, opt_decl_seq);

  // bad_decl : decl -> error semi;
  auto bad_decl = new TRule("bad_decl", decl);
    new TErrorMember(bad_decl);
    new TAnonymousMember(bad_decl, semi);

  // prec_level : decl -> prec_kwd name semi;
  auto prec_level = new TRule("prec_level", decl);
    new TAnonymousMember(prec_level, prec_kwd);
    new TAnonymousMember(prec_level, name);
    new TAnonymousMember(prec_level, semi);

  // opt_super;
  // no_super : opt_super -> empty;
  // super    : opt_super -> colon name;
  auto opt_super = new TBase("opt_super");
  new TRule("no_super", opt_super);
  auto super = new TRule("super", opt_super);
    new TAnonymousMember(super, colon);
    new TAnonymousMember(super, name);

  // member;
  // anonymous_member : member -> name;
  // named_member : member -> name:name colon name:kind;
  // error_member : member -> error_kwd;
  auto member = new TBase("member");
  auto anonymous_member = new TRule("anonymous_member", member);
    new TAnonymousMember(anonymous_member, name);
  auto named_member = new TRule("named_member", member);
    new TNamedMember(named_member, name, "name");
    new TAnonymousMember(named_member, colon);
    new TNamedMember(named_member, name, "kind");
  auto error_member = new TRule("error_member", member);
    new TAnonymousMember(error_member, error_kwd);

  // opt_member_seq;
  // no_member_seq : opt_member_seq -> empty;
  // member_seq : opt_member_seq -> member opt_member_seq
  auto opt_member_seq = new TBase("opt_member_seq");
  new TRule("no_member_seq", opt_member_seq);
  auto member_seq = new TRule("member_seq", opt_member_seq);
    new TAnonymousMember(member_seq, member);
    new TAnonymousMember(member_seq, opt_member_seq);

  // opt_oper_ref;
  // no_oper_ref : opt_oper_ref -> empty;
  // oper_ref    : opt_oper_ref -> prec_kwd name;
  auto opt_oper_ref = new TBase("opt_oper_ref");
  new TRule("no_oper_ref", opt_oper_ref);
  auto oper_ref = new TRule("oper_ref", opt_oper_ref);
    new TAnonymousMember(oper_ref, prec_kwd);
    new TAnonymousMember(oper_ref, name);

  // opt_rhs;
  // no_rhs : opt_rhs -> arrow empty_kwd;
  // rhs : opt_rhs -> arrow member_seq;
  auto opt_rhs = new TBase("opt_rhs");
  auto no_rhs = new TRule("no_rhs", opt_rhs);
    new TAnonymousMember(no_rhs, arrow);
    new TAnonymousMember(no_rhs, empty_kwd);
  auto rhs = new TRule("rhs", opt_rhs);
    new TAnonymousMember(rhs, arrow);
    new TAnonymousMember(rhs, member_seq);
    new TAnonymousMember(rhs, opt_oper_ref);

  // opt_path;
  // opt_path_tail;
  // no_path : opt_path -> empty;
  // no_path_tail : opt_path_tail -> empty;
  // path : opt_path -> name opt_path_tail;
  // path_tail : opt_path_tail -> slash path;
  auto opt_path = new TBase("opt_path");
  auto opt_path_tail = new TBase("opt_path_tail");
  new TRule("no_path", opt_path);
  new TRule("no_path_tail", opt_path_tail);
  auto path = new TRule("path", opt_path);
    new TAnonymousMember(path, name);
    new TAnonymousMember(path, opt_path_tail);
  auto path_tail = new TRule("path_tail", opt_path_tail);
    new TAnonymousMember(path_tail, slash);
    new TAnonymousMember(path_tail, path);

  // opt_expected_sr;
  // no_expected_sr : opt_expected_sr -> empty;
  // expected_sr    : opt_expected_sr -> sr_kwd int_literal;
  auto opt_expected_sr = new TBase("opt_expected_sr");
  new TRule("no_expected_sr", opt_expected_sr);
  auto expected_sr = new TRule("expected_sr", opt_expected_sr);
    new TAnonymousMember(expected_sr, sr_kwd);
    new TAnonymousMember(expected_sr, int_literal);

  // opt_expected_rr;
  // no_expected_rr : opt_expected_rr -> empty;
  // expected_rr    : opt_expected_rr -> sr_kwd int_literal;
  auto opt_expected_rr = new TBase("opt_expected_rr");
  new TRule("no_expected_rr", opt_expected_rr);
  auto expected_rr = new TRule("expected_rr", opt_expected_rr);
    new TAnonymousMember(expected_rr, rr_kwd);
    new TAnonymousMember(expected_rr, int_literal);

  // opt_pri_level;
  // no_pri_level : opt_pri_level -> empty;
  // pri_level    : opt_pri_level -> pri_kwd int_literal;
  auto opt_pri_level = new TBase("opt_pri_level");
  new TRule("no_pri_level", opt_pri_level);
  auto pri_level = new TRule("pri_level", opt_pri_level);
    new TAnonymousMember(pri_level, pri_kwd);
    new TAnonymousMember(pri_level, int_literal);

  // pattern -> eq str_literal opt_pri_level;
  auto pattern = new TRule("pattern");
    new TAnonymousMember(pattern, eq);
    new TAnonymousMember(pattern, str_literal);
    new TAnonymousMember(pattern, opt_pri_level);

  // kind : decl;
  auto kind = new TBase("kind", decl);

  // base : kind -> name opt_super semi;
  auto base = new TRule("base", kind);
    new TAnonymousMember(base, name);
    new TAnonymousMember(base, opt_super);
    new TAnonymousMember(base, semi);

  // language : kind -> name opt_super opt_rhs open_angle opt_path close_angle opt_expected_sr opt_expected_rr semi;
  auto language = new TRule("language", kind);
    new TAnonymousMember(language, name);
    new TAnonymousMember(language, opt_super);
    new TAnonymousMember(language, opt_rhs);
    new TAnonymousMember(language, open_angle);
    new TAnonymousMember(language, opt_path);
    new TAnonymousMember(language, close_angle);
    new TAnonymousMember(language, opt_expected_sr);
    new TAnonymousMember(language, opt_expected_rr);
    new TAnonymousMember(language, semi);

  // rule : kind -> name opt_super opt_rhs semi;
  auto rule = new TRule("rule", kind);
    new TAnonymousMember(rule, name);
    new TAnonymousMember(rule, opt_super);
    new TAnonymousMember(rule, opt_rhs);
    new TAnonymousMember(rule, semi);

  // keyword : kind -> name opt_super pattern semi;
  auto keyword = new TRule("keyword", kind);
    new TAnonymousMember(keyword, name);
    new TAnonymousMember(keyword, opt_super);
    new TAnonymousMember(keyword, pattern);
    new TAnonymousMember(keyword, semi);

  // oper : kind -> name opt_super pattern prec_level_ref:name assoc semi;
  auto oper = new TRule("oper", kind);
    new TAnonymousMember(oper, name);
    new TAnonymousMember(oper, opt_super);
    new TAnonymousMember(oper, pattern);
    new TNamedMember(oper, name, "prec_level_ref");
    new TAnonymousMember(oper, assoc);
    new TAnonymousMember(oper, semi);

  // nycr -> opt_decl_seq <tools/nycr/syntax>;
  auto nycr = new TLanguage("nycr", 0, { "tools", "nycr", "syntax" }, *Base::TOpt<int>::Unknown, *Base::TOpt<int>::Unknown);
    new TAnonymousMember(nycr, opt_decl_seq);

  return nycr;
}
