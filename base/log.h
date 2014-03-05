/* <base/log.h>

   RAII for opening and closing the system log and command line options for managing the log.

   The options are:

      --log_echo    (le)  Echo the log to stderr.
      --log_all     (la)  Log all messages, or all messages up to the given level.
      --log_warning (lw)  Log warning messages, or up to warning messages (if --log_all).
      --log_notice  (ln)  Log notice messages, or up to notice messages (if --log_all).
      --log_info    (li)  Log info messages, or up to info messages (if --log_all).
      --log_debug   (ld)  Log debug messages, or up to debug messages (if --log_all).

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

#include <cstdlib>
#include <thread>
#include <utility>

#include <base/cmd.h>
#include <base/no_copy_semantics.h>

namespace Base {

  /* RAII for opening and closing the system log. */
  class TLog {
    NO_COPY_SEMANTICS(TLog);
    public:

    /* Command line options for logging. */
    class TCmd
        : public Base::TCmd {
      public:

      /* The flags to pass to openlog(). */
      int GetOpenFlags() const;

      /* The flags to pass to setlogmask(). */
      int GetMask() const;

      /* The options themselves. */
      bool Echo, All, Warning, Notice, Info, Debug;

      protected:

      /* Metadata describing these command line options. */
      class TMeta
          : public Base::TCmd::TMeta {
        public:

        /* Register the options. */
        TMeta(const char *desc);

      };  // TMeta

      /* All options default to false. */
      TCmd()
          : Echo(false), All(false), Warning(false), Notice(false), Info(false), Debug(false) {}

    };  // Cmd

    /* Open the log and set the mask. */
    TLog(const TCmd &cmd);

    /* Close the log. */
    ~TLog();

  };  // TLog

}  // Base
