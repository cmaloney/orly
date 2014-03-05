/* <starsha/runner.h>

   TODO

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

#pragma once

#include <cassert>
#include <cstring>
#include <exception>
#include <functional>
#include <string>
#include <vector>

#include <base/no_copy_semantics.h>
#include <starsha/pipe.h>

namespace Starsha {

  /* TODO */
  class TRunner {
    NO_COPY_SEMANTICS(TRunner);
    public:

    /* TODO */
    TRunner(const std::string &cmd, size_t buffer_size = 4096);

    /* TODO */
    ~TRunner();

    /* TODO */
    bool ForEachLine(const std::function<bool (bool is_err, const char *line)> &cb);

    /* TODO */
    void Send(const std::string &data) {
      assert(this);
      assert(&data);
      Send(data.data(), data.size());
    }

    /* TODO */
    void Send(const char *data) {
      assert(this);
      assert(data);
      Send(data, strlen(data));
    }

    /* TODO */
    void Send(const char *buf, size_t max_size);

    /* TODO */
    int Wait();

    private:

    /* TODO */
    class TParser {
      NO_COPY_SEMANTICS(TParser);
      public:

      /* TODO */
      TParser(size_t buffer_size);

      /* TODO */
      ~TParser();

      /* TODO */
      const char *Peek() const;

      /* TODO */
      void Pop();

      /* TODO */
      void Push(TPipe *pipe);

      private:

      /* TODO */
      void Freshen() const;

      /* TODO */
      char *Start, *Limit, *PushStart;

      /* TODO */
      bool Done;

      /* TODO */
      mutable char *PeekStart, *PeekLimit;

      /* TODO */
      mutable bool CanPeek;

    };  // TParser;

    /* TODO */
    TPipe::TPoller Poller;

    /* TODO */
    TParser OutParser, ErrParser;

    /* TODO */
    TPipe InPipe, OutPipe, ErrPipe;

    /* TODO */
    int ChildId;

  };  // TRunner

  /* TODO */
  void Run(const std::string &cmd, std::vector<std::string> &lines);

}  // Starsha
