/* <orly/compiler.h>

   The controller for the orly compiler. Runs all the phases, exits on first phase failure.

   Copyright 2010-2014 OrlyAtomics, Inc.

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

#include <iostream>
#include <string>

#include <base/path.h>
#include <base/thrower.h>
#include <jhm/naming.h>
#include <orly/package/name.h>

namespace Orly {

  namespace Compiler {

    /* Thrown when we compilation fails. Errors will have been printed to the out-stream. */
    DEFINE_ERROR(TCompileFailure, std::runtime_error, "compile failure");

    Package::TVersionedName Compile(
        Base::TPath core_file,
        const Jhm::TTree &out_tree,
        bool debug_cc,
        bool machine_mode,
        bool semantic_only,
        std::ostream &out_strm = std::cout);

  }  // Compiler

}  // Orly
