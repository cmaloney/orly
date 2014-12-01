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

CC=g++
common_flags=(
  # Base compile
  -std=c++1y
  -Wall -Werror -Wextra -Wold-style-cast

  # Optimize
  -O3 -DNDEBUG -flto
  -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-parameter

  #Linking
  -Wl,--hash-style=gnu -Wl,--no-copy-dt-needed-entries -Wl,-z,relro -Wl,--no-as-needed -pthread
  )


#Build JHM
$CC -o tools/jhm                                                                                                       \
 "${common_flags[@]}"                                                                                                  \
  jhm/jobs/util.cc jhm/jobs/link.cc jhm/jobs/compile_c_family.cc jhm/jobs/bison.cc     \
  base/dir_walker.cc jhm/env.cc jhm/jhm.cc jhm/config.cc util/path.cc base/fd.cc base/split.cc util/time.cc \
  base/thrower.cc base/demangle.cc util/io.cc base/cmd.cc jhm/job.cc base/backtrace.cc jhm/jobs/flex.cc \
  jhm/naming.cc base/unreachable.cc base/path.cc jhm/status_line.cc              \
  jhm/jobs/nycr.cc base/code_location.cc jhm/test.cc jhm/work_finder.cc util/error.cc base/pump.cc jhm/jobs/dep.cc     \
  jhm/job_runner.cc base/subprocess.cc base/event_semaphore.cc                                                         \
  -I./ -DSRC_ROOT=\"`pwd`\"

#Build make_dep_file
$CC -o tools/make_dep_file                                                                                             \
  "${common_flags[@]}"                                                                                                 \
  base/backtrace.cc base/unreachable.cc     \
  jhm/make_dep_file.cc base/fd.cc base/code_location.cc base/thrower.cc base/subprocess.cc        \
  base/split.cc util/io.cc base/demangle.cc base/pump.cc util/error.cc base/event_semaphore.cc                         \
  -I./ -DSRC_ROOT=\"`pwd`\"

mkdir -p ../.jhm