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
  -O3 -DNDEBUG -flto
  -std=c++1y
  -Wall -Werror -Wextra -Wold-style-cast
  -Wno-unused -Wno-unused-parameter -Wno-unused-result
  -Wl,--hash-style=gnu -Wl,--no-copy-dt-needed-entries -Wl,-z,relro -Wl,--no-as-needed
  )

#Build JHM
$CC -o tools/jhm                                                                                                       \
 "${common_flags[@]}"                                                                                                  \
  base/time.cc jhm/jobs/util.cc jhm/job.cc base/slice.cc jhm/test.cc jhm/jobs/flex.cc server/daemonize.cc              \
  io/input_consumer.cc io/output_consumer.cc base/thrower.cc base/error.cc strm/bin/in.cc jhm/naming.cc jhm/jhm.cc     \
  io/output_producer.cc io/chunk_and_pool.cc base/pos.cc base/code_location.cc util/string.cc base/cmd.cc              \
  base/demangle.cc base/piece.cc jhm/job_runner.cc base/subprocess.cc util/path.cc strm/bin/var_int.cc                 \
  jhm/env.cc jhm/jobs/compile_c_family.cc util/error.cc jhm/status_line.cc io/input_producer.cc                        \
  jhm/work_finder.cc base/fd.cc base/pump.cc util/io.cc base/split.cc jhm/config.cc jhm/jobs/link.cc                   \
  jhm/jobs/bison.cc strm/syntax_error.cc jhm/jobs/nycr.cc base/dir_walker.cc jhm/jobs/dep.cc jhm/timestamp.cc          \
  strm/out.cc base/event_semaphore.cc strm/in.cc strm/past_end.cc base/unreachable.cc base/path.cc                     \
  -I./ -DSRC_ROOT=\"`pwd`\"                                                                                            \
  -msse2 -pthread

#Build make_dep_file
$CC -o tools/make_dep_file                                                                                             \
  "${common_flags[@]}"                                                                                                 \
  strm/bin/var_int.cc strm/syntax_error.cc strm/out.cc base/time.cc strm/past_end.cc strm/in.cc strm/bin/in.cc         \
  util/io.cc base/demangle.cc base/code_location.cc base/event_semaphore.cc util/error.cc base/unreachable.cc          \
  jhm/make_dep_file.cc base/thrower.cc base/fd.cc base/split.cc base/subprocess.cc base/error.cc base/pump.cc          \
  server/daemonize.cc                                                                                                  \
  -I./ -DSRC_ROOT=\"`pwd`\"                                                                                            \
  -msse2 -pthread

mkdir -p ../.jhm
