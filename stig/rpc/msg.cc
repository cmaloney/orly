/* <stig/rpc/msg.cc> 

   Implements <stig/rpc/msg.h>.

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

#include <stig/rpc/msg.h>

#include <stdexcept>

#include <io/endian.h>

using namespace std;
using namespace Io;
using namespace Stig::Rpc;

void TMsg::Send(TSafeTransceiver *xver, int sock_fd) {
  assert(this);
  assert(xver);
  /* Measure up our blob. */
  size_t block_count, byte_count;
  Blob.GetCounts(block_count, byte_count);
  block_count += 3;
  /* We'll need to send the kind, the request id, and the byte count as well as
     the blocks in the blob itself, so order up some iovecs. */
  auto
      *vecs  = xver->GetIoVecs(block_count),
      *limit = vecs + block_count;
  /* The first iovec holds the kind.
     That's just one byte, so we don't need to worry about network byte order. */
  vecs->iov_base = &Kind;
  vecs->iov_len  = sizeof(Kind);
  ++vecs;
  /* The second iovec holds the request id, in NBO. */
  auto nbo_req_id = SwapEnds(ReqId);
  vecs->iov_base = &nbo_req_id;
  vecs->iov_len  = sizeof(nbo_req_id);
  ++vecs;
  /* The second iovec holds the number of bytes in the blob, in NBO. */
  auto nbo_byte_count = SwapEnds(static_cast<TByteCount>(byte_count));
  vecs->iov_base = &nbo_byte_count;
  vecs->iov_len  = sizeof(nbo_byte_count);
  ++vecs;
  /* The rest of the iovecs hold the blob's data blocks.  Fill them in and fire away. */
  Blob.InitIoVecs(vecs, limit);
  xver->Send(sock_fd);
}

TMsg TMsg::Recv(TBufferPool *buffer_pool, TSafeTransceiver *xver, int sock_fd) {
  assert(buffer_pool);
  assert(xver);
  /* We'll start by receiving a kind, a request id, and the number of bytes in the incoming blob. */
  TKind kind;
  TReqId req_id;
  TByteCount byte_count;
  auto *vecs = xver->GetIoVecs(3);
  vecs->iov_base = &kind;
  vecs->iov_len  = sizeof(kind);
  ++vecs;
  vecs->iov_base = &req_id;
  vecs->iov_len  = sizeof(req_id);
  ++vecs;
  vecs->iov_base = &byte_count;
  vecs->iov_len  = sizeof(byte_count);
  xver->Recv(sock_fd);
  /* The request id and the byte count arrived in network byte order, so it's time to do the endian dance. */
  req_id = SwapEnds(req_id);
  byte_count = SwapEnds(byte_count);
  /* Check the kind we receiver to make sure it's valid. */
  switch (kind) {
    case TKind::Request:
    case TKind::NormalReply:
    case TKind::ErrorReply: {
      break;
    }
    default: {
      throw invalid_argument("Stig RPC message arrived with unknown kind");
    }
  }
  /* Make a buffer space large enough, then receive the blob into it. */
  size_t block_count = (byte_count + BlockSize - 1) / BlockSize;
  TBlob blob(buffer_pool, byte_count);
  vecs = xver->GetIoVecs(block_count);
  auto *limit = vecs + block_count;
  blob.InitIoVecs(vecs, limit);
  xver->Recv(sock_fd);
  /* Construct a new message and return it. */
  return move(TMsg(kind, req_id, move(blob)));
}

