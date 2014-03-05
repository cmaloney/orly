/* <strm/past_end.h>

   An error thrown when the end of an input producer or output consumer is
   exceeded.

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

#include <stdexcept>

namespace Strm {

  /* An error thrown when the end of an input producer or output consumer is
     exceeded. */
  class TPastEnd final
      : public std::runtime_error {
    public:

    /* Do-little. */
    TPastEnd();

  };  // TPastEnd

}  // Strm
