/* Copyright 2015 Theoretical Chaos.

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
#include <memory>
#include <unordered_set>
#include <vector>

#include <bit/naming.h>

namespace Bit {

class TFileInfo;
struct TJobProducer;

// Max-size capped output buffer. Ideally a Twine class but lazy for now.
class TOutputBuffer {
  using TPtr = std::unique_ptr<uint8_t[]>;

  MOVE_ONLY(TOutputBuffer)
  TOutputBuffer() = default;

  private:
  std::vector<TPtr> Blocks;
};

class TJob {
  public:
  template <typename TVal>
  using TSet = std::unordered_set<TVal>;

  struct TOutput {
    enum TResult { NewNeeds, Error, Complete } Result;

    // Write adapters to do subprocess -> OutputBuffer via pump or just
    // using OutputBuffer as a streaming data target directly with no virtual
    // calls.
    // TODO(cmaloney): Allow output of "structured" result objects where there
    // is just a conversion to string available in case of error, as well as
    // introspection available for checking things like "gather the test output"
    TOutputBuffer Output;
  };

  struct TNeeds {
    TSet<TRelPath> Mandatory;
    TSet<TRelPath> IfBuildable;
  };

  struct TId {
    const TJobProducer *Producer;
    TFileInfo *Input;

    bool operator==(const TId &that) const noexcept {
      return Producer == that.Producer && Input == that.Input;
    }

    struct THash {
      size_t operator()(const TId &that) const noexcept {
        // TODO: Write a better hash...
        return HashHelper(that.Producer) + HashHelper(that.Input);
      }
    };

    private:
      template <typename TVal>
      static size_t HashHelper(const TVal &val) {
        return std::hash<TVal>()(val);
      }
  };

  struct TMetadata {
    const TJobProducer *Producer;
    TFileInfo *Input;
    TSet<TFileInfo *> Output;
  };

  virtual ~TJob() = default;

  virtual const TNeeds GetNeeds() = 0;

  /* Allows a job to verify that it's complete. If it returns false here, the command __WILL__ get
     run again when
     all the files returned by GetNeeds() are all done. */
  virtual bool IsComplete() = 0;

  virtual TOutput Run() = 0;

  TFileInfo *GetInput() {
    assert(this);
    return Metadata.Input;
  }

  const TSet<TFileInfo *> &GetOutput() const {
    assert(this);
    return Metadata.Output;
  }

  TFileInfo *GetSoleOutput() const {
    assert(this);
    assert(Metadata.Output.size() == 1);
    for(TFileInfo *f : Metadata.Output) {
      return f;
    }
    __builtin_unreachable();
  }

  protected:
  // NOTE: In theory we can take multiple files in. In  practice we have no instances of that.
  TJob(TMetadata &&metadata);

  private:
  const TMetadata Metadata;
};

std::ostream &operator<<(std::ostream &out, TJob *job);

}  // namespace Bit
