#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <base/fd.h>
#include <base/generator.h>

namespace Base::Subprocess {

  using TByteBlock = std::basic_string_view<uint8_t>;
  using TBlockGenerator = cppcoro::generator<TByteBlock>;

  enum class TBlockKind {
    Output,
    Error
  };

  struct TTaggedBlock {
    TBlockKind Kind;
    TByteBlock Bytes;    
  };

  using TTaggedBlockGenerator = cppcoro::generator<TTaggedBlock>;

  //  Execute the given command in a shell.  This will replace the calling process
  //  entirely with the shell process (and hence never return).
  [[noreturn]] void Exec(const std::vector<std::string> &cmd);

  void RunThrowOnError(const std::vector<std::string> &cmd);

  struct TProcessHandle {
    // TODO(cmaloney): Make some easy way to fork off a background / early
    // forked thread so there isn't a big cost to doing the fork/exec.
    TProcessHandle(const std::vector<std::string> &cmd);
    TProcessHandle(const TProcessHandle&) = delete;
    TProcessHandle(TProcessHandle &&) = delete;
    ~TProcessHandle();

    // Gets a coroutine which will read off of stdout/stderr and pump it into the
    // given location.
    // TODO(cmaloney): Make it take a TTaggedBlockGenerator as input
    TTaggedBlockGenerator Communicate() const;

    // Wait for the given process to exit, returning its exit status code
    int Wait() const;
  
  private:
    int ChildId;
    Base::TFd Output, Error;
  };
  

  // TProcess Start(const std::vector<std::string> &cmd)
  // TResult RunCaptureOutput(std::vector<std::string> &cmd)
}
