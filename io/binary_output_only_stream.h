/* <io/binary_output_only_stream.h>

   An output stream in binary format.

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

#pragma once

#include <base/no_copy_semantics.h>
#include <io/binary_output_stream.h>

namespace Io {

  /* An output stream in binary format. */
  class TBinaryOutputOnlyStream
      : public TBinaryOutputStream {
    NO_COPY_SEMANTICS(TBinaryOutputOnlyStream);
    public:

    /* Attach to the given consumer, if any.
       Construct our own pool. */
    explicit TBinaryOutputOnlyStream(
        const std::shared_ptr<TOutputConsumer> &output_consumer = std::shared_ptr<TOutputConsumer>(),
        const TPool::TArgs &args = TPool::TArgs())
        : TBinaryOutputStream(output_consumer, std::make_shared<TPool>(args)) {}

    /* Use the given pool, which must not be null.  Don't push to a consumer. */
    explicit TBinaryOutputOnlyStream(const std::shared_ptr<TPool> &pool)
        : TBinaryOutputStream(std::shared_ptr<TOutputConsumer>(), pool) {}

    /* Attach to the given consumer, which must not be null.
       Use the given pool, which must not be null. */
    TBinaryOutputOnlyStream(const std::shared_ptr<TOutputConsumer> &output_consumer, const std::shared_ptr<TPool> &pool)
        : TBinaryOutputStream(output_consumer, pool) {}

  };  // TBinaryOutputOnlyStream

}  // Io
