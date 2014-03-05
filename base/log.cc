/* <base/log.cc>

   Implements <base/log.h>.

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

#include <base/log.h>

#include <syslog.h>

#include <cassert>

using namespace Base;

int TLog::TCmd::GetOpenFlags() const {
  assert(this);
  return LOG_PID | (Echo ? LOG_PERROR : 0);
}

int TLog::TCmd::GetMask() const {
  assert(this);
  int result = LOG_MASK(LOG_EMERG) | LOG_MASK(LOG_ALERT) | LOG_MASK(LOG_CRIT) | LOG_MASK(LOG_ERR);
  if (All) {
    if (Info) {
      result |= LOG_UPTO(LOG_INFO);
    } else if (Notice) {
      result |= LOG_UPTO(LOG_NOTICE);
    } else if (Warning) {
      result |= LOG_UPTO(LOG_WARNING);
    } else {
      result |= LOG_UPTO(LOG_DEBUG);
    }
  } else {
    if (Warning) {
      result |= LOG_MASK(LOG_WARNING);
    }
    if (Notice) {
      result |= LOG_MASK(LOG_NOTICE);
    }
    if (Info) {
      result |= LOG_MASK(LOG_INFO);
    }
    if (Debug) {
      result |= LOG_MASK(LOG_DEBUG);
    }
  }
  return result;
}

TLog::TCmd::TMeta::TMeta(const char *desc)
    : Base::TCmd::TMeta(desc) {
  Param(&TCmd::Echo,    "log_echo",    Optional, "log_echo\0le\0", "Echo the log to stderr.");
  Param(&TCmd::All,     "log_all",     Optional, "log_all\0la\0", "Log all messages, or all messages up to the given level.");
  Param(&TCmd::Warning, "log_warning", Optional, "log_warning\0lw\0", "Log warning messages, or up to warning messages (if --log_all).");
  Param(&TCmd::Notice,  "log_notice",  Optional, "log_notice\0ln\0", "Log notice messages, or up to notice messages (if --log_all).");
  Param(&TCmd::Info,    "log_info",    Optional, "log_info\0li\0", "Log info messages, or up to info messages (if --log_all).");
  Param(&TCmd::Debug,   "log_debug",   Optional, "log_debug\0ld\0", "Log debug messages, or up to debug messages (if --log_all).");
}

TLog::TLog(const TCmd &cmd) {
  assert(&cmd);
  openlog(cmd.GetProg(), cmd.GetOpenFlags(), LOG_USER);
  setlogmask(cmd.GetMask());
  syslog(LOG_NOTICE, "log started");
}

TLog::~TLog() {
  syslog(LOG_NOTICE, "log stopped");
  closelog();
}
