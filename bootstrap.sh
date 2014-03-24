#!/bin/bash
#
# Copyright 2010-2014 Stig LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set -e

SRC=`pwd`
OUT=$SRC/../out/debug
OUT_BOOTSTRAP=$SRC/../out/bootstrap
CC=g++

if [ -z "$PREFIX" ]; then
  PREFIX="/usr/local"
fi

#Setup install prefix for 'make install'
sed -i 's@PREFIX=.*@PREFIX='$PREFIX'@g' Makefile

common_flags=(-O2 #Stupid gcc 4.7... -flto
  -Wall -Werror -Wextra
  -Wno-type-limits -Wno-delete-non-virtual-dtor -Wno-parentheses -Wno-unused-result
  -DTEST_OUTPUT_DIR="/tmp/"
  -std=c++11 -I$SRC -D"SRC_ROOT=\"$SRC\""
  )

#Build starsha
$CC "${common_flags[@]}" -DDEBUG -Wold-style-cast -I$OUT $SRC/starsha/walk.cc $SRC/starsha/pipe.cc $SRC/starsha/obj_file.cc $SRC/starsha/nycr_file.cc $SRC/starsha/c_family_file.cc $SRC/starsha/cc_file.cc $SRC/starsha/bison_file.cc $SRC/starsha/starsha.cc $SRC/base/cmd.cc $SRC/starsha/c_file.cc $SRC/base/error.cc $SRC/starsha/exe_file.cc $SRC/starsha/note.cc $SRC/starsha/hdr_file.cc $SRC/base/demangle.cc $SRC/base/code_location.cc $SRC/starsha/thread_pool.cc $SRC/starsha/corpus.cc $SRC/starsha/flex_file.cc $SRC/starsha/runner.cc $SRC/base/make_dir.cc $SRC/base/thrower.cc -luuid -lpthread -lrt -ldl -o $SRC/tools/starsha

#Build nycr
mkdir -p $OUT_BOOTSTRAP/tools/nycr/syntax/
bison -rall -o $OUT_BOOTSTRAP/tools/nycr/syntax/nycr.bison.cc $SRC/tools/nycr/syntax/nycr.y
flex -o $OUT_BOOTSTRAP/tools/nycr/syntax/nycr.flex.cc $SRC/tools/nycr/syntax/nycr.l
$CC "${common_flags[@]}" -DBOOTSTRAP -Wno-maybe-uninitialized -Wno-sign-compare -Wno-unused-function -Wno-old-style-cast -I$OUT_BOOTSTRAP $SRC/tools/nycr/error.cc $SRC/tools/nycr/symbol/write_nycr.cc $SRC/tools/nycr/operator.cc $SRC/tools/nycr/final.cc $SRC/base/error.cc $SRC/tools/nycr/kind.cc $SRC/tools/nycr/symbol/named_member.cc $SRC/tools/nycr/symbol/write_cst.cc $SRC/tools/nycr/compound.cc $SRC/tools/nycr/symbol/write_xml.cc $SRC/tools/nycr/symbol/write_bison.cc $SRC/tools/nycr/base.cc $SRC/tools/nycr/syntax/nycr.cst.cc $SRC/tools/nycr/symbol/kind.cc $SRC/base/demangle.cc $OUT_BOOTSTRAP/tools/nycr/syntax/nycr.flex.cc $SRC/tools/nycr/keyword.cc $SRC/tools/nycr/symbol/language.cc $SRC/tools/nycr/symbol/bootstrap.cc $SRC/base/pos.cc $SRC/tools/nycr/symbol/for_each_final.cc $SRC/tools/nycr/symbol/prec_level.cc $SRC/tools/nycr/symbol/member_with_kind.cc $SRC/tools/nycr/symbol/write_flex.cc $SRC/base/cmd.cc $SRC/tools/nycr/rule.cc $SRC/base/piece.cc $SRC/tools/nycr/nycr.cc $SRC/tools/nycr/symbol/compound.cc $SRC/tools/nycr/symbol/rule.cc $SRC/tools/nycr/symbol/for_each_known_kind.cc $SRC/tools/nycr/symbol/keyword.cc $SRC/base/code_location.cc $SRC/tools/nycr/symbol/operator.cc $SRC/tools/nycr/decl.cc $SRC/tools/nycr/lexeme.cc $SRC/base/slice.cc $SRC/tools/nycr/symbol/anonymous_member.cc $SRC/tools/nycr/build.cc $OUT_BOOTSTRAP/tools/nycr/syntax/nycr.bison.cc $SRC/tools/nycr/prec_level.cc $SRC/base/chrono.cc $SRC/tools/nycr/symbol/error_member.cc $SRC/tools/nycr/language.cc $SRC/tools/nycr/symbol/write_dump.cc $SRC/tools/nycr/symbol/name.cc $SRC/tools/nycr/symbol/base.cc $SRC/tools/nycr/indent.cc $SRC/tools/nycr/escape.cc $SRC/server/daemonize.cc $SRC/tools/nycr/symbol/output_file.cc $SRC/tools/nycr/atom.cc $SRC/base/thrower.cc $SRC/starsha/note.cc -lpthread -lrt -g -o $OUT_BOOTSTRAP/tools/nycr/nycr -Wno-overloaded-virtual
mkdir -p $SRC/../.starsha
mkdir -p $OUT