/* <tools/nycr/symbol/write_dump.cc>

   Implements <tools/nycr/symbol/write_dump.h>.

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

#include <tools/nycr/symbol/write_dump.h>

#include <cassert>
#include <fstream>

#include <tools/nycr/symbol/output_file.h>

using namespace std;
using namespace Tools::Nycr::Symbol;

void Tools::Nycr::Symbol::WriteDump(const char *root, const char *branch, const char *atom, const TLanguage *language) {
  assert(root);
  assert(language);
  ofstream strm;
  CreateOutputFile(root, branch, atom, language, ".dump.cc", strm);
  strm
      << "#include <cstdio>" << endl
      << "#include <iostream>" << endl
      << "#include <unistd.h>" << endl << endl
      << "#include <base/thrower.h>" << endl
      << "#include <tools/nycr/error.h>" << endl
      << "#include <" << TPath(branch, atom, language) << ".cst.h>" << endl << endl
      << "using namespace std;" << endl << endl
      << "int main(int argc, char *argv[]) {" << endl
      << "  int result;" << endl
      << "  try {" << endl
      << "    for (;;) {" << endl
      << "      int opt = getopt(argc, argv, \"\");" << endl
      << "      if (opt < 0) {" << endl
      << "        break;" << endl
      << "      }" << endl
      << "      THROW << \"unknown option '\" << optopt << '\\\'';" << endl
      << "    }" << endl
      << "    if (optind == argc) {" << endl
      << "      THROW << \"no compiland\";" << endl
      << "    }" << endl
      << "    if (optind < argc - 1) {" << endl
      << "      THROW << \"multiple compilands not allowed\";" << endl
      << "    }" << endl
      << "    auto cst = " << TScope(language) << TType(language->GetName()) << "::ParseFile(argv[optind]);" << endl
      << "    try {" << endl
      << "      if (!Tools::Nycr::TError::GetFirstError()) {" << endl
      << "        cst->Write(cout, 0, 0);" << endl
      << "        result = EXIT_SUCCESS;" << endl
      << "      } else {" << endl
      << "        for (const Tools::Nycr::TError *error = Tools::Nycr::TError::GetFirstError(); error; error = error->GetNextError()) {" << endl
      << "          cerr << error->GetPosRange() << ' ' << error->GetMsg() << endl;" << endl
      << "        }" << endl
      << "        result = EXIT_FAILURE;" << endl
      << "      }" << endl
      << "    } catch (...) {" << endl
      << "      delete cst;" << endl
      << "      throw;" << endl
      << "    }" << endl
      << "    delete cst;" << endl
      << "  } catch (const exception &ex) {" << endl
      << "    cerr << \"exception: \" << ex.what() << endl;" << endl
      << "    result = EXIT_FAILURE;" << endl
      << "  }" << endl
      << "  return result;" << endl
      << "}" << endl;
}
