/* <stig/code_gen/cpp_printer.h>

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

#include <ostream>
#include <fstream>

//TODO: Needed for the namspace printer. Should really move the namespace printer to a seperate file
#include <base/split.h>
#include <jhm/naming.h>
#include <stig/code_gen/error.h>
#include <stig/code_gen/util.h>

namespace Stig {

  namespace CodeGen {

    //A simple token class for printing the end of a line
    class TEol {};


    static const TEol Eol;

    class TCppPrinter {
      NO_COPY_SEMANTICS(TCppPrinter);
      public:

      TCppPrinter(const std::string &filename) : Indent(0), Out(filename),  StartOfLine(true) {
        if (!Out.good()) {
          throw TCgError(HERE, ("Unable open file '" + filename + "' for writing IR to.").c_str());
        }
      }

      ~TCppPrinter() {
        Out.close();
      }

      //TODO: It would be nice to kill this function off.
      std::ostream &GetOstream() {
        assert(this);

        return Out;
      }

      template <typename TVal>
      void Write(const TVal &val) {
        assert(&val);
        if(StartOfLine) {
          //TODO: Might be better to cache the string here rather than recaluclate it.
          for(auto i = 0u; i < Indent; ++i) {
            Out << "  ";
          }
          StartOfLine = false;
        }
        Out << val;
      }

      private:
      unsigned int Indent;
      std::ofstream Out;
      bool StartOfLine;

      friend class TIndent;
      friend class TNamespacePrinter;
      friend class TStigNamespacePrinter;
    }; // TCppPrinter

    //TODO: Move all the func defs to cc_printer.cc
    class TIndent {
      NO_COPY_SEMANTICS(TIndent);
      public:
      //TODO: Should have move semantics
      TIndent(TCppPrinter &out) : Out(out) {
        ++Out.Indent;
      }

      ~TIndent() {
        --Out.Indent;
      }

      private:
      TCppPrinter &Out;
    }; // TIndent

    template <>
    inline void TCppPrinter::Write(const TEol &) {
      Out << '\n';
      StartOfLine = true;
    }

    template <typename TVal>
    TCppPrinter &operator<<(TCppPrinter &printer, const TVal &val) {
      assert(&printer);
      printer.Write(val);

      return printer;
    }

    //TODO: Move implementation details to CC
    class TNamespacePrinter {
      public:
      TNamespacePrinter(const Jhm::TNamespace &ns, TCppPrinter &out) : Namespace(ns), Out(out) {
        All(Namespace.Get(), bind(&TNamespacePrinter::Start, this, std::placeholders::_1));
      }

      void Start(const std::string &str) const {
        Out << "namespace " << str << " {" << Eol << Eol;
        ++Out.Indent;
      }

      void End(const std::string &str) const {
        --Out.Indent;
        Out << Eol << "} // " << str << Eol;
      }

      ~TNamespacePrinter() {
        for(auto it = Namespace.Get().begin(); it != Namespace.Get().end(); ++it) {
          End(*it);
        }
      }

      private:
      const Jhm::TNamespace Namespace;
      TCppPrinter &Out;
    }; // TNamespacePrinter

    //TODO: Move implementation details to CC
    class TStigNamespacePrinter {
      public:
      TStigNamespacePrinter(const Jhm::TNamespace &ns, TCppPrinter &out) : Namespace(ns), Out(out) {
        All(Namespace.Get(), bind(&TStigNamespacePrinter::Start, this, std::placeholders::_1));
      }

      void Start(const std::string &str) const {
        Out << "namespace NS" << str << " {" << Eol << Eol;
        ++Out.Indent;
      }

      void End(const std::string &str) const {
        --Out.Indent;
        Out << Eol << "} // NS" << str << Eol;
      }

      ~TStigNamespacePrinter() {
        for(auto it = Namespace.Get().begin(); it != Namespace.Get().end(); ++it) {
          End(*it);
        }
      }

      private:
      const Jhm::TNamespace Namespace;
      TCppPrinter &Out;
    }; // TStigNamespacePrinter

    class TStigNamespace {
      public:
      TStigNamespace(const Jhm::TNamespace &ns) : Namespace(ns) {}

      const Jhm::TNamespace &GetNamespace() const {
        return Namespace;
      }

      private:
      const Jhm::TNamespace Namespace;
    };

    template <>
    inline void TCppPrinter::Write(const TStigNamespace &sns) {
      Base::Join("::", sns.GetNamespace().Get(), [](const std::string &str, TCppPrinter &out) {
        out << "NS" << str;
      }, *this);
    }

  } // Codegen

} // Stig
