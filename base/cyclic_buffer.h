#pragma once

#include <cassert>
#include <memory>
#include <vector>

#include <base/class_traits.h>

namespace Base {

class TFd;

using TBlockPtr = std::unique_ptr<uint8_t[]>;

class TCyclicBuffer {
  public:
  MOVE_ONLY(TCyclicBuffer)

  TCyclicBuffer();

  bool IsEmpty() const;

  // Returns result of write sytem call on the fd.
  ssize_t ReadFrom(int fd);

  // Reads all bytes from the cyclic buffer and puts them into the fd.
  void ReadAllFromWarnOverflow(int fd);

  // Always writes all bytes. May overflow.
  // Returns the result of read system call on teh fd.
  ssize_t WriteTo(TFd &fd);
  void Write(const char *msg, size_t length);

  // Copy the entire cyclic buffer into a string.
  std::string ToString() const;

  size_t GetBytesAvailable() const;

  bool HasOverflowed() const;

  // After max_blocks runs out, starts reusing the start buffer.
  static const uint64_t MaxBlocks = 1024;
  static const uint64_t BlockSize = 4096;

  private:
  // First point written to (or read from). If limit < start, then it has wrapped
  // around.
  uint64_t Start = 0, Limit = 0;
  uint64_t StartOffset = 0, LimitOffset = 0;

  // Returns true iff start has ever been advanced by a write (There was data
  // because the block cap was hit and we had more to write).
  // NOTE: To simplify overflow logic, whole blocks are eaten at once.
  bool Overflowed = false;

  // The storage for the buffer.
  std::vector<TBlockPtr> Blocks;

  static uint64_t WrappedAdvance(uint64_t offset);

  void AdvanceLimit();
};

}  // Base
