/* <orly/indy/disk/read_file.cc>

   Implements <orly/indy/disk/read_file.h>.

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

#include <orly/indy/disk/read_file.h>

using namespace Orly::Indy::Disk;
using namespace Orly::Indy::Disk::Util;

template<>
__thread size_t TReadFile<LogicalPageSize, LogicalBlockSize, PhysicalBlockSize, CheckedPage>::HashHitCount;

template<>
__thread size_t TReadFile<LogicalBlockSize, LogicalBlockSize, PhysicalBlockSize, CheckedPage>::HashHitCount;

template<>
__thread TLocalReadFileCache<LogicalPageSize, LogicalBlockSize, PhysicalBlockSize, CheckedPage, true> *TLocalReadFileCache<LogicalPageSize, LogicalBlockSize, PhysicalBlockSize, CheckedPage, true>::Cache = nullptr;