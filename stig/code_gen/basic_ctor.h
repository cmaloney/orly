/* <stig/code_gen/basic_ctor.h>

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

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <base/split.h>
#include <stig/shared_enum.h>
#include <stig/code_gen/cpp_printer.h>
#include <stig/code_gen/inline.h>

namespace Stig {

  namespace CodeGen {

    //Addr -> vector<pair<dir, inline>>
    typedef std::vector<std::pair<TAddrDir, TInline::TPtr>> TAddrContainer;
    //Dict -> dict<inline:inline>
    typedef std::unordered_map<TInline::TPtr, TInline::TPtr> TDictContainer;
    //Set -> set<inline>
    typedef std::unordered_set<TInline::TPtr> TSetContainer;
    //List -> vector<inline>
    typedef std::vector<TInline::TPtr> TListContainer;

    //list, set
    inline void WriteCtorElem(const TInline::TPtr &elem, TCppPrinter &out) {
      out << elem;
    }

    //addr
    inline void WriteCtorElem(TAddrContainer::const_reference elem, TCppPrinter &out) {
      out << elem.second;
    }

    //dict
    inline void WriteCtorElem(TDictContainer::const_reference elem, TCppPrinter &out) {
      out << '{' << elem.first << ", " << elem.second << '}';
    }

    /* Forward declaration */
    template <typename TContainer>
    class TBasicCtor;

    template <typename TContainer>
    void WriteCtorStart(const TBasicCtor<TContainer> &, TCppPrinter &) {}

    /* Dependency graph */
    inline void AppendDependsOn(std::unordered_set<TInline::TPtr> &dependency_set, const TAddrContainer &container) {
      for (auto iter : container) {
        dependency_set.insert(iter.second);
        iter.second->AppendDependsOn(dependency_set);
      }
    }

    /* Dependency graph */
    inline void AppendDependsOn(std::unordered_set<TInline::TPtr> &dependency_set, const TDictContainer &container) {
      for (auto iter : container) {
        dependency_set.insert(iter.first);
        iter.first->AppendDependsOn(dependency_set);
        dependency_set.insert(iter.second);
        iter.second->AppendDependsOn(dependency_set);
      }
    }

    /* Dependency graph */
    inline void AppendDependsOn(std::unordered_set<TInline::TPtr> &dependency_set, const TSetContainer &container) {
      for (auto iter : container) {
        dependency_set.insert(iter);
        iter->AppendDependsOn(dependency_set);
      }
    }

    /* Dependency graph */
    inline void AppendDependsOn(std::unordered_set<TInline::TPtr> &dependency_set, const TListContainer &container) {
      for (auto iter : container) {
        dependency_set.insert(iter);
        iter->AppendDependsOn(dependency_set);
      }
    }

    template <typename TContainer>
    class TBasicCtor : public TInline {
      NO_COPY_SEMANTICS(TBasicCtor);
      public:

      typedef std::shared_ptr<const TBasicCtor> TPtr;

      TBasicCtor(const L0::TPackage *package, const Type::TType &ret_type, TContainer &&elements)
          : TInline(package, ret_type), Elements(std::move(elements)) {}

      void WriteExpr(TCppPrinter &out) const {
        WriteCtorStart(*this, out);
        out << '(' << GetReturnType() << "{";
        //Iterate over the list, writing out the elements, C++ making us call the right overload.
        Base::Join(", ", Elements, [](typename TContainer::const_reference elem, TCppPrinter &out) {
          //Stupid c++ templates + overloaded funcs.
          WriteCtorElem(elem, out);
        }, out);
        out << "})";
      }

      /* Dependency graph */
      virtual void AppendDependsOn(std::unordered_set<TInline::TPtr> &dependency_set) const override {
        Stig::CodeGen::AppendDependsOn(dependency_set, Elements);
      }

      size_t GetNumElements() const {
        assert(this);
        return Elements.size();
      }

      private:
      TContainer Elements;

    }; // TBasicCtor

    inline void WriteCtorStart(const TBasicCtor<TDictContainer> &ctor, TCppPrinter &out) {
      assert(&ctor);
      assert(&out);
      out << "DictCtor<" << ctor.GetNumElements() << '>';
    }

  } // CodeGen

} // Stig