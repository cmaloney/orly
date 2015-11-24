#include <base/cyclic_buffer.h>

#include <unistd.h>

#include <base/fd.h>
#include <base/not_implemented.h>

using namespace Base;

TCyclicBuffer::TCyclicBuffer() { Blocks.reserve(MaxBlocks); }

bool TCyclicBuffer::IsEmpty() const { return Start == Limit && StartOffset == LimitOffset; }

ssize_t TCyclicBuffer::ReadFrom(TFd &fd) {
  size_t write_size = 0;

  // No content left to write to fd.
  if (GetBytesAvailable() == 0) {
    return 0;
  }

  // Contents of last block.
  if (Start == Limit) {
    write_size = Limit - Start;
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
  if (write_result < 0) {
    return write_result;
  }

  // Advance, moving into the next block if appropriate.
  StartOffset += size_t(write_result);
  assert(StartOffset <= BlockSize);
  if (StartOffset == BlockSize) {
    ++Start;
    StartOffset = 0;
  }

  return write_result;
}

ssize_t TCyclicBuffer::WriteTo(TFd &fd);
void TCyclicBuffer::Write(const char *msg, size_t length);

size_t TCyclicBuffer::GetBytesAvailable() {
  size_t available = 0;

  // No extra blocks
  if (Limit == Start) {
    assert(LimitOffset > StartOffset);
    return LimitOffset - StartOffset;
  }

  assert(Limit > Start);

  // Size left in start block + number of blocks + size left in limit block.
  available += BlockSize - StartOffset;
  // -1 to account since start block is partial and we add it manually.
  available += Limit - Start - 1;
  available += LimitOffset;

  return available;
}

uint8_t *TCyclicBuffer::GetNextBlock() {
  assert(Blocks.size() <= MaxBlocks);

  // Reuse buffers at start, advancing write if needed.
  if (Blocks.size() == MaxBlocks) {
    NOT_IMPLEMENTED();
  }

  // Add a new buffer
  Blocks.push_back(TBlockPtr(new uint8_t[BlockSize]));
  return Blocks.back().get();
}
