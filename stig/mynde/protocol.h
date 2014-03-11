/* <stig/mynde/protocol.h>

   Shared bits for all protocols.

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

#include <cstdint>

#include <base/uuid.h>

namespace Strm {
  namespace Bin {
    class TIn;
  }
}

namespace Stig {
  namespace Mynde {

    // TODO: This is the most logical place for this I have atm... Not a great place for it though...
    static const Base::TUuid MemcachedIndexUuid(uuid_t{'m', 'e', 'm', 'c', 'a', 'c', 'h', 'e',
                                                       'm', 'e', 'm', 'c', 'a', 'c', 'h', 'e'});

    // TODO: This needs a better name.
    class TBuffer {
      public:
      TBuffer();
      ~TBuffer();

      void Fill(uint32_t size, Strm::Bin::TIn &in);

      uint32_t GetSize() const;
      const uint8_t *GetData() const;

      private:
      uint32_t Size;
      uint8_t *Data;
    };

    /* User-friendly request object. Pulls apart the important information for consumption, holds the extra fields. */
    class TRequest {
      public:
      struct TFlags {
        // Either don't respond immediately or buffer the response.
        bool Quiet;

        // Include the key in the response
        bool Key;
      };

      enum class TOpcode {
        Get,
        Set,
        Add,
        Replace,
        Delete,
        Increment,
        Decrement,
        Quit,
        Flush,
        NoOp,
        Version,
        Append,
        Prepend,
        State,
      };

      const TBuffer &GetKey() const {
        assert(this);
        return Key;
      }
      const TBuffer &GetValue() const {
        assert(this);
        return Value;
      }
      const TBuffer &GetExtras() const {
        assert(this);
        return Extras;
      }
      uint64_t GetCas() const {
        assert(this);
        return Cas;
      }
      uint32_t GetOpaque() const {
        assert(this);
        return Opaque;
      }
      TOpcode GetOpcode() const {
        assert(this);
        return Opcode;
      }
      TFlags GetFlags() const {
        assert(this);
        return Flags;
      }

      TRequest(Strm::Bin::TIn &in);

      private:
      // Base fields, human readable
      TOpcode Opcode;
      // Flags / response modifiers
      TFlags Flags;
      uint32_t Opaque;
      uint64_t Cas;

      // Optional request fields. Contents depend on request opcode + parameters.
      TBuffer Extras,  // NOTE: Max 20 bytes
          Key,         // NOTE: Max 250 bytes
          Value;       // NOTE: Max 1000000 bytes
    };

    // TODO: User-friendly response object?
    enum TResponseStatus {
      NoError = 0x0000,
      KeyNotFound = 0x0001,
      KeyExists = 0x0002,
      ValueTooLarge = 0x0003,
      InvalidArguments = 0x0004,
      ItemNotStored = 0x0005,
      IncrDectNonNumeric = 0x0006,  // Increment and decrement aren't allowed on non-numeric values.
      UnknownCommand = 0x0081,
      OutOfMemory = 0x0082
    };

  }  // Mynde
}  // Stig