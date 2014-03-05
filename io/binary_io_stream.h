/* <io/binary_io_stream.h>

   A bidirectional stream in binary format.

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
#include <io/binary_input_stream.h>
#include <io/binary_output_stream.h>

namespace Io {

  /* A bidirectional stream in binary format. */
  class TBinaryIoStream
      : public TBinaryInputStream,
        public TBinaryOutputStream {
    NO_COPY_SEMANTICS(TBinaryIoStream);
    public:

    /* Attach to the given object, which must be both producer and consumer.  The pointer must not be null.
       Construct our own pool. */
    template <typename TProdAndCons>
    explicit TBinaryIoStream(const std::shared_ptr<TProdAndCons> &prod_and_cons, const TPool::TArgs &args = TPool::TArgs())
        : TBinaryInputStream(prod_and_cons), TBinaryOutputStream(prod_and_cons, std::make_shared<TPool>(args)) {}

    /* Attach to the given object, which must be both producer and consumer.  The pointer must not be null.
       Use the given pool, which also must not be null. */
    template <typename TProdAndCons>
    TBinaryIoStream(const std::shared_ptr<TProdAndCons> &prod_and_cons, const std::shared_ptr<TPool> &pool)
        : TBinaryInputStream(prod_and_cons), TBinaryOutputStream(prod_and_cons, pool) {}

    /* Attach to the given producer, which must not be null.
       Attach to the given consumer, if any.
       Construct our own pool. */
    explicit TBinaryIoStream(
        const std::shared_ptr<TInputProducer> &input_producer,
        const std::shared_ptr<TOutputConsumer> &output_consumer = std::shared_ptr<TOutputConsumer>(),
        const TPool::TArgs &args = TPool::TArgs())
        : TBinaryInputStream(input_producer), TBinaryOutputStream(output_consumer, std::make_shared<TPool>(args)) {}

    /* Attach to the given producer, which must not be null.
       Attach to the given consumer, if any.
       Use the given pool, which also must not be null. */
    TBinaryIoStream(
        const std::shared_ptr<TInputProducer> &input_producer,
        const std::shared_ptr<TOutputConsumer> &output_consumer,
        const std::shared_ptr<TPool> &pool)
        : TBinaryInputStream(input_producer), TBinaryOutputStream(output_consumer, pool) {}

    /* Attach to the given producer, which must not be null.
       Use the given pool, which also must not be null.
       Don't push to a consumer. */
    TBinaryIoStream(
        const std::shared_ptr<TInputProducer> &input_producer,
        const std::shared_ptr<TPool> &pool)
        : TBinaryInputStream(input_producer), TBinaryOutputStream(std::shared_ptr<TOutputConsumer>(), pool) {}

  };  // TBinaryIoStream

}  // Io
