/* <stig/indy/disk/util/snappy.h>

   TODO

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

#include <memory>
#include <vector>

#include <string.h>

#include <snappy.h>
#include <snappy-sinksource.h>

#include <base/no_copy_semantics.h>
#include <stig/indy/disk/buf_block.h>
#include <stig/indy/disk/in_file.h>
#include <stig/indy/disk/out_stream.h>
#include <stig/indy/disk/util/volume_manager.h>

namespace Stig {

  namespace Indy {

    namespace Disk {

      namespace Util {

        namespace Snappy {

          /* TODO */
          class TBlockSource
              : public snappy::Source {
            NO_COPY_SEMANTICS(TBlockSource);
            public:

            /* TODO */
            TBlockSource(const std::unique_ptr<TBufBlock> &buf);

            /* TODO */
            virtual ~TBlockSource();

            /* TODO */
            virtual size_t Available() const override;

            /* TODO */
            virtual const char* Peek(size_t *len) override;

            /* TODO */
            virtual void Skip(size_t n) override;

            private:

            /* TODO */
            const std::unique_ptr<TBufBlock> &Buf;

            /* TODO */
            size_t BytesRead;

          };  // TBlockSource

          template <size_t CachePageSize, size_t BlockSize, size_t PhysicalBlockSize, Util::TBufKind BufKind>
          class TStreamSource
              : public snappy::Source {
            NO_COPY_SEMANTICS(TStreamSource);
            public:

            typedef TStream<CachePageSize, BlockSize, PhysicalBlockSize, BufKind, 0UL /*local cache */> TDataInStream;

            /* TODO */
            TStreamSource(TDataInStream &in_stream, size_t avail_bytes)
              : InStream(in_stream), AvailBytes(avail_bytes) {
            }

            /* TODO */
            virtual ~TStreamSource() {}

            /* TODO */
            inline virtual size_t Available() const override {
              assert(this);
              return AvailBytes;
            }

            /* TODO */
            inline virtual const char* Peek(size_t *len) override {
              assert(this);
              size_t offset_in_chunk = InStream.GetOffsetInChunk();
              *len = std::min(TDataInStream::DataChunkSize - offset_in_chunk, AvailBytes);
              return InStream.GetData();
            }

            /* TODO */
            inline virtual void Skip(size_t n) override {
              assert(this);
              assert(AvailBytes >= n);
              AvailBytes -= n;
              InStream.Skip(n);
            }

            private:

            /* TODO */
            TDataInStream &InStream;

            /* TODO */
            size_t AvailBytes;

          };  // TStreamSource

          /* TODO */
          template <size_t PageSize, size_t BlockSize, size_t PhysicalBlockSize, Util::TBufKind BufKind>
          class TBlockSink
              : public snappy::Sink {
            NO_COPY_SEMANTICS(TBlockSink);
            public:

            /* TODO */
            typedef TOutStream<PageSize, BlockSize, PhysicalBlockSize, BufKind> TDataOutStream;

            /* TODO */
            TBlockSink(TDataOutStream &out_stream)
              : OutStream(out_stream) {}

            /* TODO */
            virtual ~TBlockSink() {}

            /* TODO */
            inline virtual void Append(const char *bytes, size_t n) override {
              OutStream.Write(bytes, n);
            }

            private:

            /* TODO */
            TDataOutStream &OutStream;

          };  // TBlockSink

          /* TODO */
          class TIoStreamSource
              : public snappy::Source {
            NO_COPY_SEMANTICS(TIoStreamSource);
            public:

            /* TODO */
            TIoStreamSource(Io::TBinaryInputStream &stream, size_t avail_bytes)
              : InStream(stream), AvailBytes(avail_bytes) {}

            /* TODO */
            virtual ~TIoStreamSource() {}

            /* TODO */
            inline virtual size_t Available() const override {
              assert(this);
              return AvailBytes;
            }

            /* TODO */
            inline virtual const char* Peek(size_t *len) override {
              assert(this);
              *len = std::min(InStream.GetPeekSize(), AvailBytes);
              return InStream.Peek();
            }

            /* TODO */
            inline virtual void Skip(size_t n) override {
              assert(this);
              assert(AvailBytes >= n);
              AvailBytes -= n;
              InStream.SkipExactly(n);
            }

            private:

            /* TODO */
            Io::TBinaryInputStream &InStream;

            /* TODO */
            size_t AvailBytes;

          };  // TIoStreamSource

          /* TODO */
          class TIoStreamSink
              : public snappy::Sink {
            NO_COPY_SEMANTICS(TIoStreamSink);
            public:

            /* TODO */
            TIoStreamSink(Io::TBinaryOutputStream &stream)
              : OutStream(stream) {}

            /* TODO */
            virtual ~TIoStreamSink() {}

            /* TODO */
            inline virtual void Append(const char *bytes, size_t n) override {
              OutStream.WriteExactly(bytes, n);
            }

            private:

            /* TODO */
            Io::TBinaryOutputStream &OutStream;

          };  // TIoStreamSink

          /* TODO */
          class TRawSink
              : public snappy::Sink {
            NO_COPY_SEMANTICS(TRawSink);
            public:

            /* TODO */
            TRawSink(char *buf, size_t max_size)
              : Buf(buf), BytesIn(0UL), MaxBytes(max_size) {}

            /* TODO */
            virtual ~TRawSink() {}

            /* TODO */
            inline virtual void Append(const char *bytes, size_t n) override {
              assert(BytesIn + n <= MaxBytes);
              memcpy(&Buf[BytesIn], bytes, n);
              BytesIn += n;
              assert(BytesIn <= MaxBytes);
            }

            private:

            /* TODO */
            char *Buf;
            size_t BytesIn;
            size_t MaxBytes;

          };  // TRawSink

        }  // Snappy

      }  // Util

    }  // Disk

  }  // Indy

}  // Stig