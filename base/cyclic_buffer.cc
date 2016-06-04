#include <base/cyclic_buffer.h>

#include <unistd.h>

#include <algorithm>
#include <cstring>

#include <base/fd.h>
#include <base/not_implemented.h>
#include <util/io.h>

using namespace Base;
using namespace std;
using namespace Util;

TCyclicBuffer::TCyclicBuffer() {
  Blocks.reserve(MaxBlocks);

  // Start with one block allocated so all code can assume there is always at least
  // one valid block.
  Blocks.push_back(TBlockPtr(new uint8_t[BlockSize]));
}

bool TCyclicBuffer::IsEmpty() const { return Start == Limit && StartOffset == LimitOffset; }

ssize_t TCyclicBuffer::ReadFrom(int fd) {
  size_t write_size = 0;

  // No content left to write to fd.
  if (GetBytesAvailable() == 0) {
    return 0;
  }

  // Contents of last block.
  if (Start == Limit) {
    write_size = LimitOffset - StartOffset;
  } else {  // Rest of block Start is currently inside.
    // NOTE: It is guaranteed (because we advance after write) that we never
    // are at the end of Start block
    write_size = BlockSize - StartOffset;
  }

  // Ensure write_size was set from its default.
  assert(write_size > 0);

  // Write the data.
  ssize_t write_result = write(fd, Blocks[Start].get() + StartOffset, write_size);

  // If there was an error, nothign was written, return the error code.
  if (write_result == -1) {
    return write_result;
  }

  assert(write_result >= 0);

  // Advance, moving into the next block if appropriate.
  StartOffset += size_t(write_result);
  assert(StartOffset <= BlockSize);
  if (StartOffset == BlockSize) {
    ++Start;
    StartOffset = 0;
  }

  return write_result;
}

void TCyclicBuffer::ReadAllFromWarnOverflow(int fd) {
  if (HasOverflowed()) {
    const char *msg = "NOTICE: Output has been truncated.\n";
    WriteExactly(fd, msg, strlen(msg));
  }
  while (GetBytesAvailable() > 0) {
    ReadFrom(fd);
  }
}

ssize_t TCyclicBuffer::WriteTo(TFd &fd) {
  // If at end of block, advance into next block.
  if (BlockSize == LimitOffset) {
    AdvanceLimit();
  }

  // Going to write at least one byte.
  assert(BlockSize > LimitOffset);

  ssize_t read_size = read(fd, Blocks[Limit].get() + LimitOffset, BlockSize - LimitOffset);

  if (read_size == -1) {
    return read_size;
  }

  assert(read_size >= 0);

  LimitOffset += size_t(read_size);

  assert(LimitOffset <= BlockSize);

  return read_size;
}

void TCyclicBuffer::Write(const char *msg, size_t length) {
  size_t offset = 0;

  // Write bytes to blocks until all is written
  while (offset < length) {
    if (BlockSize == LimitOffset) {
      AdvanceLimit();
    }

    size_t write_len = min(BlockSize - LimitOffset, uint64_t(length - offset));

    assert(Limit < Blocks.size());

    assert(Blocks[Limit]);

    memcpy(Blocks[Limit].get() + LimitOffset, msg + offset, write_len);
    offset += write_len;
    LimitOffset += write_len;
  }
  assert(offset == length);
}

size_t TCyclicBuffer::GetBytesAvailable() const {
  size_t available = 0;

  // No extra blocks
  if (Limit == Start) {
    assert(LimitOffset >= StartOffset);
    return LimitOffset - StartOffset;
  }

  assert(Limit > Start);

  // Size left in start block + number of blocks + size left in limit block.
  available += BlockSize - StartOffset;

  // -1 to account since start block is partial and we add it manually.
  available += (Limit - Start - 1) * BlockSize;

  // Number of bytes in the last block.
  available += LimitOffset;

  return available;
}

uint8_t TCyclicBuffer::GetByte(size_t idx) const {
  assert(idx < GetBytesAvailable());
  size_t block_offset = idx / BlockSize;
  size_t cur_block = (Start + block_offset) % MaxBlocks;
  return *(Blocks[cur_block].get() + (idx % BlockSize));
}

bool TCyclicBuffer::HasOverflowed() const { return Overflowed; }

uint64_t TCyclicBuffer::WrappedAdvance(uint64_t offset) {
  ++offset;
  if (offset > MaxBlocks) {
    return 0;
  } else {
    return offset;
  }
}

void TCyclicBuffer::AdvanceLimit() {
  assert(Blocks.size() <= MaxBlocks);

  // Advance the limit to the next block, reset limit offset.
  Limit = WrappedAdvance(Limit);
  LimitOffset = 0;

  // Rotate limit around end of blocks if needed.
  if (Limit > MaxBlocks) {
    Limit = 0;
  }

  // If Limit matches Start and this function is called that means that Limit
  // just moved into the same block as Start (When Start advances into Limit
  // this code isn't touched). Advance Start, reset StartOffset (since we're at
  // the start of the next block). For ease of implementation, Limit always owns
  // the whole block it is inside of rather than allowing the bytes in a block
  // between LimitOffset and StartOffset to be used.
  if (Limit == Start) {
    Start = WrappedAdvance(Start);
    StartOffset = 0;
    Overflowed = true;
  }

  // Add a new buffer
  Blocks.push_back(TBlockPtr(new uint8_t[BlockSize]));

  assert(Blocks.size() > Limit);
}
