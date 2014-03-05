/* <mpl/enable_if.h>

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

#include <c14/type_traits.h>

#include <mpl/not.h>

namespace Mpl {

  class EnableIfImpl {
    private:

    enum class Enable {};

    public:

    template <typename Pred>
    using EnableIf = c14::enable_if_t<Pred::value, Enable>;

  };  // EnableIfImpl

  template <typename Pred>
  using EnableIf = EnableIfImpl::EnableIf<Pred>;

  template <typename Pred>
  using DisableIf = EnableIf<Mpl::Not<Pred>>;

}  // Mpl
