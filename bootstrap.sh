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
  -std=c++1y
  -Wall -Werror -Wextra
  # Optimize
  -O3 -DNDEBUG
  #-g
  -Wno-unused-parameter -Wno-unused -Wno-unused-variable
  -Wno-unknown-pragmas
  -Wno-old-style-cast
  -Wno-return-type
  -flto

  # Enable threads
  -pthread

  # Better backtraces
  -rdynamic

  # Static resulting binary
  # -static
  # See https://gcc.gnu.org/ml/gcc-help/2010-05/msg00029.html
  # -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
  )

OS=`uname -s`
extra_files=
if [ "$OS" == 'Linux' ]; then
  extra_files=base/pump_epoll.cc
elif [ "$OS" == 'Darwin' ]; then
  extra_files=base/pump_kqueue.cc
else
  echo "Unsupported platfrom '$OS'."
  echo 'No known way to build a asynchronous IO pump'
  echo 'See <base/pump.h>, <base/pump_kqueue.h> for what is needed.'
  exit -1
fi

mkdir -p tools

#Build JHM
$CC -o tools/jhm \
 "${common_flags[@]}" \
  jhm/jobs/util.cc jhm/jobs/dep.cc jhm/jobs/dep_c.cc base/dir_walker.cc util/io.cc jhm/file.cc base/json.cc \
  base/pump.cc base/subprocess.cc cmd/main.cc base/demangle.cc util/error.cc base/split.cc cmd/parse.cc cmd/help.cc \
  base/code_location.cc base/cyclic_buffer.cc jhm/job_runner.cc util/signal.cc util/path.cc jhm/job.cc \
  base/backtrace.cc jhm/jhm.cc jhm/env.cc jhm/jobs/compile_c_family.cc jhm/config.cc util/time.cc jhm/status_line.cc \
  jhm/work_finder.cc jhm/jobs/link.cc base/path.cc jhm/naming.cc base/fd.cc base/unreachable.cc jhm/test.cc \
  "$extra_files" \
  -I./ -DSRC_ROOT=\"`pwd`\" \
  -ldl

mkdir -p ../.jhm
