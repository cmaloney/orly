#pragma once

#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include <base/fd.h>

// TODO
template<typename T>
struct generator {};


namespace Subprocess {
  enum class TBlockKind {
    Output,
    Error
  };
  using TByteBlock = std::basic_string_view<uint8_t>;
  using TBlockGenerator = generator<TByteBlock>;
  using TTaggedBlockGenerator = generator<std::tuple<TByteBlock, TBlockKind>>;

  // TODO(cmaloney): Make some easy way to fork off a background / early
  // forked thread so there isn't a big cost to doing the fork/exec.
  TProcessHandle Run(std::vector<std::string>);
  // TProcessHandle Run(std::vector<std::string> cmd, TBlockGenerator Input);

  struct TProcessHandle {
    friend TProcessHandle Run(std::vector<std::string);

    TProcessHandle(const TProcessHandle&) = delete;
    TProcessHandle(TProcessHandle &&) = delete;
    ~TProcessHandle();

    // Gets a coroutine which will read off of stdout/stderr and pump it into the
    // given location.
    TTaggedBlockGenerator GetOutput();

    // Wait for the given process to exit, returning its exit status code
    int Wait() const;
  
  private:
    TProcessHandle(std::vector<std::string> cmd, TBlockGenerator Input);
    int ChildId;
    Base::TFd Output, Error;
  };
  
  // TProcess Start(const std::vector<std::string> &cmd)
  // TResult RunCaptureOutput(std::vector<std::string> &cmd)
}