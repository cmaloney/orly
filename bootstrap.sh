#!/bin/bash
#
# Copyright 2015 Theoretical Chaos.
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

CC=clang++
common_flags=(
  # Base compile
  -fcolor-diagnostics
  -stdlib=libc++
  -std=c++2a
  -Wall -Werror -Wextra
  -fuse-ld=lld
  -fstrict-vtable-pointers
  -Wthread-safety
  -Weverything
  -Wno-c++98-compat
  -Wno-c++98-compat-pedantic
  -Wno-c99-extensions
  -Wno-padded
  -Wno-weak-vtables
  -Wno-missing-prototypes
  -Wno-disabled-macro-expansion
  # -march=native
  # -mtune=native
  # TODO: -fmodules

  # Optimize
  # -O3
  # -DNDEBUG
  -g
  -O0

  -Wno-unused-parameter -Wno-unused -Wno-unused-variable
  -Wno-unknown-pragmas
  -Wno-old-style-cast
  -Wno-return-type
  # -Wno-nonnull-compare
  # -flto=thin

  # Enable threads
  -pthread

  # Better backtraces
  -rdynamic

  # Static resulting binary
  # -static
  -lc++
  -lc++abi
  # -Wl,--strip-all
  # -Wl,--gc-sections
  -fstrict-vtable-pointers
  # See https://gcc.gnu.org/ml/gcc-help/2010-05/msg00029.html
  # -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
  )

# OS=`uname -s`
# extra_files=
# if [ "$OS" == 'Linux' ]; then
#   extra_files=base/pump_epoll.cc
# elif [ "$OS" == 'Darwin' ]; then
#   extra_files=base/pump_kqueue.cc
# else
#   echo "Unsupported platfrom '$OS'."
#   echo 'No known way to build a asynchronous IO pump'
#   echo 'See <base/pump.h>, <base/pump_kqueue.h> for what is needed.'
#   exit -1
# fi

mkdir -p tools

# Build BIT
$CC -o tools/coroutine                \
  "${common_flags[@]}"          \
  bit/coroutine.cc base/subprocess_coro.cc \
  -I./ -DSRC_ROOT=\"`pwd`\" \
  bit/status_line.cc base/json.cc                                    \
  util/error.cc cmd/help.cc base/json_util.cc cmd/main.cc          \
  bit/config.cc base/split.cc util/path.cc bit/naming.cc util/io.cc   \
  util/signal.cc cmd/parse.cc base/code_location.cc \
  base/unreachable.cc                                                     \
  base/demangle.cc base/fd.cc base/backtrace.cc base/dir_walker.cc                  \
  -D USE_DLADDR \
  -ldl
