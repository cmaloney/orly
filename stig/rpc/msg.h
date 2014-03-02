/* <stig/rpc/msg.h> 

   TODO

   Copyright 2010-2014 Tagged
   
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

#include <cassert>
#include <cstdint>
#include <utility>

#include <stig/rpc/blob.h>
#include <stig/rpc/buffer_pool.h>
#include <stig/rpc/safe_transceiver.h>

namespace Stig {

  namespace Rpc {

    /* TODO */
    class TMsg final {
      public:

      /* TODO */
      enum class TKind : uint8_t {

          /* TODO */
          Request = '?',

          /* TODO */
          NormalReply = '.',

          /* TODO */
          ErrorReply = '!'

      };  // TMsg::TKind

      /* TODO */
      using TReqId = uint32_t;

      /* TODO */
      TMsg(TKind kind, TReqId req_id, TBlob &&blob) noexcept
          : Kind(kind), ReqId(req_id), Blob(std::move(blob)) {}

      /* TODO */
      TMsg(TMsg &&that) noexcept
          : Kind(that.Kind), ReqId(that.ReqId), Blob(std::move(that.Blob)) {}

      /* TODO */
      TMsg(const TMsg &) = delete;

      /* TODO */
      TMsg &operator=(TMsg &&that) noexcept {
        assert(this);
        TMsg temp(std::move(that));
        return Swap(temp);
      }

      /* TODO */
      TMsg &operator=(const TMsg &) = delete;

      /* TODO */
      const TBlob &GetBlob() const noexcept {
        assert(this);
        return Blob;
      }

      /* TODO */
      TReqId GetReqId() const noexcept {
        assert(this);
        return ReqId;
      }

      /* TODO */
      TKind GetKind() const noexcept {
        assert(this);
        return Kind;
      }

      /* TODO */
      void Send(TSafeTransceiver *xver, int sock_fd);

      /* TODO */
      TMsg &Swap(TMsg &that) noexcept {
        assert(this);
        assert(&that);
        std::swap(Kind, that.Kind);
        std::swap(ReqId, that.ReqId);
        Blob.Swap(that.Blob);
        return *this;
      }

      /* TODO */
      static TMsg Recv(TBufferPool *buffer_pool, TSafeTransceiver *xver, int sock_fd);

      private:

      /* TODO */
      using TByteCount = uint32_t;

      /* TODO */
      TKind Kind;

      /* TODO */
      TReqId ReqId;

      /* TODO */
      TBlob Blob;

    };  // TMsg

  }  // Rpc

}  // Stig

