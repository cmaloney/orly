#!/bin/bash
#
# Copyright 2010-2014 OrlyAtomics, Inc.
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
  -Wall -Werror -Wextra -Wold-style-cast

  # Optimize
  -O3 -DNDEBUG
  -Wno-unused-parameter -Wno-unused -Wno-unused-variable

  # Enable threads
  -pthread
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

#Build JHM
$CC -o tools/jhm \
 "${common_flags[@]}" \
  jhm/jobs/util.cc jhm/jobs/dep.cc base/dir_walker.cc util/io.cc jhm/file.cc base/json.cc \
  base/pump.cc base/subprocess.cc cmd/main.cc base/demangle.cc util/error.cc base/split.cc cmd/parse.cc cmd/help.cc \
  base/code_location.cc jhm/job_runner.cc util/signal.cc util/path.cc jhm/job.cc \
  base/backtrace.cc jhm/jhm.cc jhm/env.cc jhm/jobs/compile_c_family.cc jhm/config.cc util/time.cc jhm/status_line.cc \
  jhm/work_finder.cc jhm/jobs/link.cc base/path.cc jhm/naming.cc base/fd.cc base/unreachable.cc jhm/test.cc \
  "$extra_files" \
  -I./ -DSRC_ROOT=\"`pwd`\"

#Build make_dep_file
$CC -o tools/make_dep_file \
  "${common_flags[@]}" \
  jhm/make_dep_file.cc util/io.cc base/split.cc base/code_location.cc base/subprocess.cc base/pump.cc \
  base/fd.cc util/error.cc base/demangle.cc cmd/parse.cc cmd/help.cc util/signal.cc base/unreachable.cc cmd/main.cc \
  base/backtrace.cc base/json.cc \
  "$extra_files" \
  -I./ -DSRC_ROOT=\"`pwd`\"

mkdir -p ../.jhm
