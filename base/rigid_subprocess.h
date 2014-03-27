/* <base/rigid_subprocess.h>

   Run a subprocess and communicate with it.

   I/O between the parent and child processes is rigid.  Contrast this with
   <base/subprocess>, in which communication between the parent and child is
   handled with <base/pump>, preventing writes from blocking when the reads
   are delayed.

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

#include <base/fd.h>

namespace Base {

  /* Run a subprocess and communicate with it. */
  class TRigidSubprocess final {
    public:

    /* Launch the subprocess to run the given command. */
    TRigidSubprocess(const char *cmd);

    /* The id of the child process. */
    int GetChildId() const {
      assert(this);
      return ChildId;
    }

    /* An fd from which to read the child's stderr output. */
    const TFd &GetStdErrFromChild() const {
      assert(this);
      return StdErrFromChild;
    }

    /* An fd from which to write the child's stdin input. */
    const TFd &GetStdInToChild() const {
      assert(this);
      return StdInToChild;
    }

    /* An fd from which to read the child's stdout output. */
    const TFd &GetStdOutFromChild() const {
      assert(this);
      return StdOutFromChild;
    }

    /* Wait for the child to complete and return its exit code. */
    int Wait() const;

    private:

    /* See accessors. */
    TFd StdInToChild, StdOutFromChild, StdErrFromChild;

    /* See accessor. */
    int ChildId;

  };  // TRigidSubprocess

}  // Base
