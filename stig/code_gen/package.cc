/* <stig/code_gen/package.cc>

   Implements <stig/code_gen/package.h>

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

#include <stig/code_gen/package.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

#include <base/make_dir.h>
#include <base/not_implemented_error.h>
#include <base/split.h>
#include <stig/code_gen/obj.h>
#include <stig/code_gen/util.h>
#include <stig/expr/addr_walker.h>
#include <stig/expr/walker.h>
#include <stig/expr/where.h>
#include <stig/package/api_version.h>
#include <stig/type/gen_code.h>
#include <stig/type/impl.h>
#include <stig/type/object_collector.h> //TODO: This header should be renamed
#include <stig/type/stigify.h>
#include <stig/type/util.h>
#include <stig/expr/util.h>

using namespace Base;
using namespace Jhm;
using namespace std;
using namespace std::placeholders;
using namespace Stig;
using namespace Stig::CodeGen;

void ForEachExprInTestBlock(const function<void (const Expr::TExpr::TPtr &expr)> &cb,
      const Symbol::Test::TTestCaseBlock::TPtr &block) {
  assert(&cb);
  assert(cb);
  assert(&block);
  assert(block);

  for(auto &test: block->GetTestCases()) {
    cb(test->GetExpr());
    if(test->GetOptTestCaseBlock()) {
      ForEachExprInTestBlock(cb, test->GetOptTestCaseBlock());
    }
  }
}

void GetAddrsInTestBlock(const Symbol::Test::TTestCaseBlock::TPtr &block,
                         unordered_set<pair<Type::TType, Type::TType>> &addr_set) {
  for(auto &test: block->GetTestCases()) {
    Expr::DatabaseAddrsInExpr(test->GetExpr(), addr_set);
    if(test->GetOptTestCaseBlock()) {
      GetAddrsInTestBlock(test->GetOptTestCaseBlock(), addr_set);
    }
  }
}

TPackage::TPackage(const Symbol::TPackage::TPtr &package) : L0::TPackage(package) {
/* Stages
1) Gather information
   - All objects, needed object comparisons.
   - List of top level functions, their signatures.
   - Effecting blocks, assertion predicates
2) Build function bodies (The tree of inlines)
   - The whole tree of inlines and scopes
      - includes converting sequences into maps when they pass through non-sequence operations
*/
  //Collect all the objects.
  std::function<void (const Expr::TExpr::TPtr &expr)> collect_objects;
  collect_objects = [this, &collect_objects](const Expr::TExpr::TPtr &expr) {
    Expr::ForEachExpr(expr, [this, &collect_objects](const Expr::TExpr::TPtr &expr) {
        Type::CollectObjects(expr->GetType(), Objects);
        const Expr::TWhere *where = expr->TryAs<Expr::TWhere>();
        if(where) {
          for(auto &it: where->GetFunctions()) {
            collect_objects(it->GetExpr());
          }
        }
        return false;
    },true);
  };

  for(auto &func: package->GetFunctions()) {
    collect_objects(func->GetExpr());
  }

  for(auto &test: package->GetTests()) {
    if(test->GetOptWithClause()) {
      for(auto &new_stmt: test->GetOptWithClause()->GetNewStmts()) {
        collect_objects(new_stmt->GetLhs()->GetExpr());
        collect_objects(new_stmt->GetRhs()->GetExpr());
      }
    }
    ForEachExprInTestBlock(collect_objects, test->GetTestCaseBlock());
  }

  /* Get the addrs that are used in the expressions */
  unordered_set<pair<Type::TType, Type::TType>> addr_set;
  for(auto &func: package->GetFunctions()) {
    Expr::DatabaseAddrsInExpr(func->GetExpr(), addr_set);
  }
  for(auto &test: package->GetTests()) {
    /* With clause */
    if(test->GetOptWithClause()) {
      for(auto &new_stmt: test->GetOptWithClause()->GetNewStmts()) {
        Expr::DatabaseAddrsFromNewStmt(new_stmt.get(), addr_set);
      }
    }
    /* Test clause */
    GetAddrsInTestBlock(test->GetTestCaseBlock(), addr_set);
  }
  /* Assign each addr in the addr_set a Uuid and add it to a map */
  for(auto &addr: addr_set) {
    Base::TUuid uuid (TUuid::TimeAndMAC);
    /* Don't include sequence of address */
    if (!addr.first.Is<Type::TSeq>()) {
      AddrMap.insert(std::make_pair(uuid, addr));
      ReverseAddrMap.insert(std::make_pair(addr, uuid));
    }
  }

  //TODO: Collect all the object comparisons.

  assert(package);
  //Build all the function declarations
  for(auto &func: package->GetFunctions()) {
    Exports.insert(TExportFunc::New(this, Symbol->GetNamespace(), func));
  }

  //Build the definitions
  for(auto &func: Exports) {
    //TODO: The shared ptr to the Id Gen here is sort of ugly, but it is needed because Scopes hold pointers to id
    //generators and keep them after we finish building the body (Although they should never be touched again).
    func->Build();
  }

  TId<TIdKind::Test>::TGen TestIdGen;

  for(auto &test: package->GetTests()) {
    assert(&test); // To keep GCC from warning about unused variables.
    std::unique_ptr<TTest> test_ptr(new TTest(this, test, TestIdGen));
    Tests.push_back(test_ptr.get());
    test_ptr.release();
  }
}

TPackage::~TPackage() {
  for(auto &test: Tests) {
    delete test;
  }
}

void TPackage::Emit(const Jhm::TAbsBase &out_dir) const {;
  assert(this);
  assert(&out_dir);

  /* Emit the following files:

     h // Interface which will be used by packages which import this package.
       Include for every object (include all the ones used by our external interface in our external interface)
       Inside the namespace of the package
        Exported function signatures
        The core package API (GetPackageInfo)
     cc // Core implementation code of external interfaces
       Include for all the package interfaces we need
       Include for all the objects we need
       Include for the flux api, stig Rt environment
       Forward declare internal function signatures (I don't think there are any...)
       Define all the EffectBindingSets and AssertionPredicateMaps we need
       Define all the implementation functions
       Build structs for
       Inside the namespace of the package
        Define all the package export functions
        Define GetPackageInfo()

     link.cc // TLinkInfo class, GetApiVesion function.
       Include header for every module in the link
       Define all the API Functions
       Define all the test functions

       Package structures
    stig.sig
     - Function signature of every exported function
  */

  auto dir = Symbol->GetNamespace().Get();
  dir.pop_back();

  /* EXTRA */ {
    std::ostringstream s;
    //TODO: Very fug. MakeDirs should really make all the way out the filename. But that would change the current starsha
    //behavior. For now string mangling is the pain.
    s << '/' << out_dir << '/';
    Join('/',dir,s) << '/';
    MakeDirs(s.str().c_str());
  }

  auto emit_code = [&](const std::function<void (TCppPrinter &, const TRelPath &)> &func, const vector<string> &ext) -> void {
    TRelPath rel_path(dir, TName(Symbol->GetNamespace().Get().back(), ext));
    TCppPrinter out(TAbsPath(out_dir, rel_path).AsStr());
    func(out, rel_path);
  };

  /* EXTRA */ {
    std::ostringstream s;
    auto ns = Symbol->GetNamespace().Get();
    ns.pop_back();
    s << '/' << out_dir << '/' << TNamespace(ns) << '/';
    MakeDirs(s.str().c_str());
  }

  emit_code(bind(&TPackage::WriteHeader, this, _1, _2), {"h"});
  emit_code(bind(&TPackage::WriteCc, this, _1, _2), {"cc"});
  emit_code(bind(&TPackage::WriteLink, this, _1, _2), {"link","cc"});
  emit_code(bind(&TPackage::WriteSignatures, this, _1, _2), {"stig", "sig"});


}

void TPackage::EmitObjectHeaders(const Jhm::TAbsBase &out_dir) const {
  for(auto &it: Objects) {
    GenObjHeader(out_dir, it);
  }
}


void TPackage::WriteStartingComment(TCppPrinter &out, const TRelPath &path) const {
  //TODO: Multi-line comment printer in the TCppPrinter
  out << "/* <" << path << ">" << Eol
      << Eol
      << "   This file was generated by the Stig compiler. */" << Eol
      << Eol;
}

void TPackage::WriteHeader(TCppPrinter &out, const TRelPath &path) const {
  //TODO: Reduce number of needed includes.
  WriteStartingComment(out, path);
  out << "#pragma once" << Eol
      << "#include <stig/package/api.h>" << Eol
      << "#include <stig/package/rt.h>" << Eol
      << "#include <stig/rt.h>" << Eol;

  //TODO: Reduce to only objects needed by the export set.
  All(Objects, bind(GenObjInclude, _1, ref(out)));

  out << Eol;

  TStigNamespacePrinter ns_printer(Symbol->GetNamespace(), out);
  out << "extern const Stig::Package::TInfo PackageInfo;" << Eol
      << Eol;

  for (const auto &addr : AddrMap) {
    char uuid[37];
    addr.first.FormatUnderscore(uuid);
    out << "extern Base::TUuid My" << uuid;
    addr.first.Format(uuid);
    out << ';' << Eol;
  }

  for (auto &it: Exports) {
    it->WriteDecl(out);
    out << ';' << Eol;
  }
}

void TPackage::WriteCc(TCppPrinter &out, const TRelPath &rel_path) const {

  WriteStartingComment(out, rel_path);
  WriteInclude(out);

  //Include for all the package interfaces we need
  WriteImportIncludes(out);

  //TODO: Reduce to no objects used by exported or imported functions.
  //Include for all the objects we need
  All(Objects, bind(GenObjInclude, _1, ref(out)));

  //Include for the flux api, stig Rt environment
  out << Eol
      << "#include <stig/var/mutation.h>" << Eol
      << Eol
      << "using namespace Stig;" << Eol
      << "using namespace Stig::Rt;" << Eol
      << Eol;

  for(auto &it: NeededObjectComparisons) {
    GenObjComparison(it.first, it.second, out);
  }

  //Define all the unit test functions and their meta information
  for(auto &it: Tests) {
    it->Write(out);
    out << Eol;
  }

  //Define function wrappers for exported functions from generic to specific args.
  for(auto &it: Exports) {
    if(it->GetReturnType().Is<Type::TSeq>()) {
      std::cout << "WARNING: Top level function [" << it->GetName() << "] returning sequence cannot currently be called via the Spa server. Cast your sequence to a list in your stigscript if you want to call the function" << std::endl;
      continue;
    }
    out << "Atom::TCore RF_" << it->GetName() << "(Package::TContext &ctx, const Package::TArgMap &args) {" << Eol;

    /* indent */ {
      TIndent indent(out);
      out << "assert(&ctx);" << Eol
          << "assert(&args);" << Eol
      //NOTE: we assume that the arguments have been checked for correctness before here. Here, we simply check that
      //at least some checking has been done, then grab the parameters needed for the function and run.
      //Check that the argument
          << "assert(args.size() == " << it->GetArgs().size() << ");" << Eol
          << Eol
          << "void *state_alloc = alloca(Sabot::State::GetMaxStateSize());" << Eol
          << "return Atom::TCore(ctx.GetArena(), Native::State::New("
          << TStigNamespace(Symbol->GetNamespace()) << "::F" << it->GetName() << "(ctx";
      if(!it->GetArgs().empty()) {
        out << ", ";
      }
      Join(", ", it->GetArgs(), [] (TFunction::TArgs::const_reference arg, TCppPrinter &out) {
        out << "Sabot::AsNative<" << arg.second->GetType() << ">(*Sabot::State::TAny::TWrapper(args.at(\"" << arg.first << "\").GetState(state_alloc)))";
      }, out);
      out << "), state_alloc));" << Eol;
    }
    out << '}' << Eol;
  }
  out << Eol;

  // Define the package info stucts
  for(auto &it: Exports) {
    if(it->GetReturnType().Is<Type::TSeq>()) {
      continue;
    }
    out << "static const Package::TFuncInfo IF_" << it->GetName() << "{" << Eol;
    /* indent*/ {
      TIndent indent(out);
      out << "Package::TParamMap{";
      Join(", ", it->GetArgs(), [](TFunction::TArgs::const_reference arg, TCppPrinter &out) {
        out << "{\"" << arg.first << "\", ";
        Type::GenCode(out.GetOstream(), arg.second->GetType());
        out << '}';
      }, out);
      out << "}," << Eol
          << "/* ret */ "; //NOTE: GetOstream at the start of a line will cause indent to not be printed.
      Type::GenCode(out.GetOstream(), it->GetReturnType());
      out << ',' << Eol
          << "RF_" << it->GetName() << Eol;
    }
    out << "};" << Eol;
  }

  //Inside the namespace of the package
  //TODO: Prefix each of the namespaces with a fixed string 'NS_' so we are guaranteed non-conflicting.
  TStigNamespacePrinter ns_printer(Symbol->GetNamespace(), out);

  for (const auto &addr : AddrMap) {
    char uuid[37];
    addr.first.FormatUnderscore(uuid);
    out << "Base::TUuid My" << uuid;
    addr.first.Format(uuid);
    out << "(\"" << uuid << "\");" << Eol;
  }

  out << "const Stig::Package::TInfo PackageInfo {" << Eol;
  /* indent */ {
    TIndent indent(out);
    out << '"' << Symbol->GetNamespace() << "\"," << Eol
        << Symbol->GetVersion() << ',' << Eol
        << "/* exports */ std::unordered_map<std::string, const Package::TFuncInfo*>{" << Eol;
    /* indent */ {
      TIndent indent(out);
      //TODO: really want a Base::Join on a filtered list of exports...
      bool first = true;
      for(auto &func: Exports) {
        if(func->GetReturnType().Is<Type::TSeq>()) {
          continue;
        }
        if(first) {
          first = false;
        } else {
          out << ',';
        }
        out << "{\"" << func->GetName() << "\", &IF_" << func->GetName() << "}" << Eol;
      }
    }
    out << "}," << Eol
        << "/* tests */ std::vector<const Package::TTest*>{" << Eol;
        Join(',', Tests, [](const TTest *test, TCppPrinter &out) {
          out << "&TI_" << test->GetId() << Eol;
        }, out);
    out << "}," << Eol
        << "/* index ids */ std::unordered_set<Base::TUuid *>{" << Eol;
    /* indent */ {
      TIndent indent(out);
          Join(',', AddrMap, [](const TAddrMap::value_type &addr_pair, TCppPrinter &out) {
            char uuid[37];
            addr_pair.first.FormatUnderscore(uuid);
            out << "&My" << uuid << Eol;
          }, out);
    }
    out << '}' << Eol;

  }
  out << "};" << Eol
      << Eol;

  //  Define all the package export functions
  for(auto &it: Exports) {
    it->WriteDef(out);
    out << Eol
        << Eol;
  }
}

void TPackage::WriteLink(TCppPrinter &out, const TRelPath &path) const {
  WriteStartingComment(out, path);
  out << "#include <unordered_map>" << Eol
      << "#include <utility>" << Eol
      << Eol
      << "#include <base/uuid.h>" << Eol
      << "#include <stig/rt.h>" << Eol
      << "#include <stig/shared_enum.h>" << Eol
      << "#include <stig/type/impl.h>" << Eol
      << Eol;
  // Include header for every module in the link
  WriteInclude(out);
  WriteImportIncludes(out);

  // Define all the link test object, TLinkInfo struct.
  out << Eol
      << "static Stig::Package::TLinkInfo LinkInfo {" << Eol;
  /* instance body*/ {
    TIndent indent(out);
    out << '"' << Symbol->GetNamespace() << "\"," << Eol
        << Symbol->GetVersion() << ',' << Eol
        << '&' << TStigNamespace(Symbol->GetNamespace()) << "::PackageInfo," << Eol
        << "std::unordered_map<std::vector<std::string>, const Stig::Package::TInfo *>{" << Eol;
    /* included packages */ {
      TIndent package_indent(out);
      Join(", ", NeededPackages, [](const Package::TName &name, TCppPrinter &out) {
        out << "{{\"";
        Join("\", \"", name.Get(), out);
        out << "\"}, &" << TStigNamespace(name.Get()) << "::PackageInfo}";
      }, out);
    }
    out << "}," << Eol;
    out << "std::unordered_map<Base::TUuid, std::pair<Stig::Type::TType, Stig::Type::TType>>{" << Eol;
      Join(", \n", AddrMap, [](const TAddrMap::value_type &pair, TCppPrinter &out) {
        class t_addr_printer final
          : public Type::TType::TVisitor {
          public:

          t_addr_printer(TCppPrinter &out)
            : Out(out) {}

          virtual void operator()(const Type::TAddr     *that) const {
            bool is_sequence = false;
            for (auto member : that->GetElems()) {
              is_sequence |= member.second.Is<Type::TSeq>();
            }

            if (is_sequence) {
              Out << "Stig::Type::TSeq::Get(";
            }

            Out << "Stig::Type::TAddr::Get(std::vector<std::pair<Stig::TAddrDir, Stig::Type::TType>> {\n";
            Join(", \n", that->GetElems(), [this](const std::pair<TAddrDir, Type::TType> &elem, TCppPrinter &out) {
              out << "          std::make_pair<Stig::TAddrDir, Stig::Type::TType>(" <<
                  ((elem.first == Stig::TAddrDir::Asc) ?
                    "Stig::TAddrDir::Asc, " :
                    "Stig::TAddrDir::Desc, ");
              elem.second.Accept(*this);
              out << ")";
            }, Out);
            Out << "})";

            if (is_sequence) {
              /* Close off the extra open paren introduced to make a sequence*/
              Out << ")";
            }
          };
          virtual void operator()(const Type::TAny      *) const { Out << "Stig::Type::TAny::Get()"; };
          virtual void operator()(const Type::TBool     *) const { Out << "Stig::Type::TBool::Get()"; };
          virtual void operator()(const Type::TDict     *that) const {
            Out << "Stig::Type::TDict::Get(";
            that->GetKey().Accept(*this);
            Out << ", ";
            that->GetVal().Accept(*this);
            Out << ")";
          };
          virtual void operator()(const Type::TErr      *) const {};
          virtual void operator()(const Type::TFunc     *) const {};
          virtual void operator()(const Type::TId       *) const { Out << "Stig::Type::TId::Get()"; };
          virtual void operator()(const Type::TInt      *) const { Out << "Stig::Type::TInt::Get()"; };
          virtual void operator()(const Type::TList     *that) const {
            Out << "Stig::Type::TList::Get(";
            that->GetElem().Accept(*this);
            Out << ")";
          };
          virtual void operator()(const Type::TMutable  *that) const { that->GetSrcAtAddr().Accept(*this); };
          virtual void operator()(const Type::TObj      *that) const {
            Out << "Stig::Type::TObj::Get(std::map<std::string, Stig::Type::TType> {";
            Join(", ", that->GetElems(), [this](const std::pair<string, Type::TType> &elem, TCppPrinter &out) {
              out << "{"
                  << "\"" << elem.first << "\""
                  << ",";
              elem.second.Accept(*this);
              out << "}";
            }, Out);
            Out << "})";
          };
          virtual void operator()(const Type::TOpt      *that) const {
            Out << "Stig::Type::TOpt::Get(";
            that->GetElem().Accept(*this);
            Out << ")";
          };
          virtual void operator()(const Type::TReal     *) const { Out << "Stig::Type::TReal::Get()"; };
          virtual void operator()(const Type::TSeq      *that) const {
            Out << "Stig::Type::TSeq::Get(";
            that->GetElem().Accept(*this);
            Out << ")";
          };
          virtual void operator()(const Type::TSet      *that) const {
            Out << "Stig::Type::TSet::Get(";
            that->GetElem().Accept(*this);
            Out << ")";
          };
          virtual void operator()(const Type::TStr      *) const { Out << "Stig::Type::TStr::Get()"; };
          virtual void operator()(const Type::TTimeDiff *) const { Out << "Stig::Type::TTimeDiff::Get()"; };
          virtual void operator()(const Type::TTimePnt  *) const { Out << "Stig::Type::TTimePnt::Get()"; };

          private:
          TCppPrinter &Out;
        };
        /* Keys */
        char uuid[37];
        pair.first.Format(uuid);
        out << "    { Base::TUuid(\"" << uuid << "\"), \n";
        /* Value*/
        out << "      std::make_pair<Stig::Type::TType, Stig::Type::TType>(" << Eol;
        out << "        ";
        pair.second.first.Accept(t_addr_printer(out));
        out << "," << Eol;
        out << "        ";
        pair.second.second.Accept(t_addr_printer(out));
        out << ") }";
      }, out);
    out << "}" << Eol;
  }
  out << "};" << Eol
      << Eol
      << "extern \"C\" Stig::Package::TLinkInfo *GetLinkInfo() {" << Eol
      << "  return &LinkInfo;" << Eol
      << '}' << Eol
      << Eol
      << "extern \"C\" int32_t GetApiVersion() {" << Eol
      << "  return " << STIG_API_VERSION << ";" << Eol
      << '}' << Eol;
  out << Eol;
}

void TPackage::WriteSignatures(TCppPrinter &out, const TRelPath &) const {
  for(auto &it: Exports) {
    out << it->GetName() << " is ";
    Type::Stigify(out.GetOstream(), it->GetType());
    out << ";" << Eol;
  }
}

void WritePackageInclude(const Jhm::TNamespace &ns, TCppPrinter &out) {
  out << "#include \"" << ns << ".h\"" << Eol;
}

void TPackage::WriteInclude(TCppPrinter &out) const {
  WritePackageInclude(Symbol->GetNamespace(), out);
}

void TPackage::WriteImportIncludes(TCppPrinter &out) const {
  if(!NeededPackages.empty()) {
    out << Eol;
  }
  for(auto &it: NeededPackages) {
    WritePackageInclude(it, out);
  }
}