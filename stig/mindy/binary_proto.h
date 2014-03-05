/* <stig/mindy/binary_proto.h>

   Read and write the memcahced binary protocol.

   Utilities to read, write, and validate packets from the memcached protocol.

   From the generic core pieces (Request, response), to validating values passed to each. Does everything except
   run the command.

   General usage as a server:
   TRequestHeader req;
   while(has_data_to_read) {
    in >> req;
    if(!req->Validate()) {
      //ERROR!
    }

    //Variable length fields attached to the request
    TRequestFields fields;
    req->GetAdditionalFields(fields);

    switch (req.command) {
      case OP_GET:
      ...
    }
   }

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

#include <base/layout.h>
#include <strm/bin/in.h>
#include <strm/bin/out.h>

namespace Stig {
namespace Mindy {

// Protocol raw opcodes
enum class TRawOpcode {
    Get = 0x00,
    Set = 0x01,
    Add = 0x02,
    Replace = 0x03,
    Delete = 0x04,
    Increment = 0x05,
    Decrement = 0x06,
    Quit = 0x07,
    Flush = 0x08,
    GetQ = 0x09,
    NoOp = 0x0A,
    Version = 0x0B,
    GetK = 0x0C,
    GetKQ = 0x0D,
    Append = 0x0E,
    Prepend = 0x0F,
    State = 0x10,
    SetQ = 0x11,
    AddQ = 0x12,
    ReplaceQ = 0x13,
    DeleteQ = 0x14,
    IncrementQ = 0x15,
    DecrementQ = 0x16,
    QuitQ = 0x17,
    FlushQ = 0x18,
    AppendQ = 0x19,
    PrependQ = 0x1A
};

/* Memcached binary request header, as specified by the protocol

   Can be read from and written to a binary stream. */
struct PACKED TRequestHeader {
  //Translates the uint8_t opcode to a command, along with flags (Condensing quiet/not, as well as return key/not)

  uint8_t Magic;   //0x80
  uint8_t Opcode;
  uint16_t KeyLength;
  uint8_t ExtrasLength;
  uint8_t DataType; // 0x00
  uint16_t Reserved; // 0x00
  uint32_t TotalBodyLength;
  uint32_t Opaque;
  uint64_t Cas;

  // Returns true iff the request header contains all valid values
  bool IsValid();

};

//NOTE: These don't do any validation
Strm::Bin::TIn &operator>>(Strm::Bin::TIn &in, TRequestHeader &that);
Strm::Bin::TOut &operator<<(Strm::Bin::TOut &out, const TRequestHeader &that);


static_assert(sizeof(TRequestHeader) == 24, "According to the binary protocol specification");

/* Memcached binary response header, as specified by the protocol

   Can be read from and written to a binary stream. */
struct PACKED TResponseHeader {
  uint8_t Magic; // 0x81
  uint8_t Opcode;
  uint16_t KeyLength;
  uint8_t ExtrasLength;
  uint8_t DataType; // 0x00
  uint16_t Status;
  uint32_t TotalBodyLength;
  uint32_t Opaque;
  uint64_t Cas;

};

//NOTE: These don't do any validation
Strm::Bin::TIn &operator>>(Strm::Bin::TIn &in, TResponseHeader &that);
Strm::Bin::TOut &operator<<(Strm::Bin::TOut &out, const TResponseHeader &that);

static_assert(sizeof(TResponseHeader) == 24, "According to the binary protocol specification");

} // Mindy
} // Stig