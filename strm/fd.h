/* <strm/fd.h>

   A Fd which can potentially be both read and written from.

   NOTE: This class makes no guarantees that you can read and write from your particular Fd. The OS system calls will
   generate lots of errors which become exceptions if you try writing to an fd which can't be written to or reading
   from an fd which can't be read from.

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

#include <cassert>

#include <base/fd.h>
#include <base/io_utils.h>
#include <strm/in.h>
#include <strm/out.h>

namespace Strm {

  template <uint64_t MaxInSize_ = 4094, uint64_t MaxOutSize_ = 4096>
  class TFd : public Out::TCons, public In::TProd {
    public:
    static constexpr uint64_t MaxInSize = MaxInSize_;
    static constexpr uint64_t MaxOutSize = MaxOutSize_;

    /* Move in a Base::TFd. We force move to make users choose whether they want a explict copy/dup() fd.
     * control */
    TFd(Base::TFd &&fd) : Fd(std::move(fd)) {}

    /* Get the underlying fd (To perform OS operations on it, for instance) */
    const TFd &GetFd() const {
      assert(this);
      return Fd;
    }

    private:
    /* In::TProd Cycle */
    bool Cycle(size_t release_count, const uint8_t **start, const uint8_t **limit) override final {
      assert(this);

      // Release our one buffer / reset it if requested.
      if (release_count) {
        assert(release_count == 1);
        InLimit = nullptr;
      }

      // Provide more data from performing a read() if requested.
      if (!start) {
        return true;
      }

      // Both limit and start must be set or neither.
      assert(limit);

      // The buffer must be released from the pervious use for us to refill it with data
      assert(!InLimit);

      size_t num_bytes = Base::ReadAtMost(Fd, InBuffer, MaxInSize);

      // If num_bytes is zero, then we're at EOF
      if (num_bytes == 0) {
        return false;
      }

      // Give the consumer the data
      InLimit = InBuffer + num_bytes;
      *start = InBuffer;
      *limit = InLimit;
      return true;
    }

    /* Out::TCons Cycle */
    void Cycle(uint8_t *cursor, uint8_t **start, uint8_t **limit) override final {
      assert(this);

      if (cursor) {
        //Cursor must be at the start of or within the buffer
        assert(cursor >= OutBuffer);
        assert(cursor <= OutBuffer + MaxOutSize);

        // If cursor is not at start of buffer consume the data
        if(cursor != OutBuffer) {
          WriteExactly(Fd, OutBuffer, cursor - OutBuffer);
        }
      }

      if (start) {
        //TODO: We don't check here that the client has given us back the workspace before we give it to the client again
        assert(limit);
        *start = OutBuffer;
        *limit = OutBuffer + MaxOutSize;
      }
    }

    Base::TFd Fd;
    /* Buffer of data read from Fd
       TODO: Switch to a buffer class which manages start/end. */
    uint8_t InBuffer[MaxInSize];

    /* End position inside the buffer */
    uint8_t *InLimit = nullptr;

    /* Workspace to write to Fd
       TODO: Switch to a buffer class which manages start/end. */
    uint8_t OutBuffer[MaxOutSize];

    uint8_t *OutLimit = nullptr;

  };

  using TFdDefault = TFd<>;
}