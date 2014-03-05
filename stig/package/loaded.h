/* <stig/package/loaded.h>

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

#include <functional>
#include <unordered_map>
#include <unordered_set>

#include <base/piece.h>
#include <stig/package/api.h>
#include <stig/package/name.h>
#include <stig/uuid.h>
#include <stig/var/impl.h>

namespace Stig {

  namespace Package {

    /* Forward declarations */
    class TFuncHolder;

    class TDlError : public Base::TFinalError<TDlError> {
      public:

      static void *IfNull(const Base::TCodeLocation &code_location, void *handle);

      private:

      TDlError(const Base::TCodeLocation &code_locatoin, const char *msg);
    }; // TDlError

    /* TODO */
    class TLoaderError : public Base::TFinalError<TLoaderError> {
      public:

      /* Constructor. */
      TLoaderError(const Base::TCodeLocation &loc, const char *msg=0) {
        PostCtor(loc, msg);
      }
    };  // TLoaderError

    /* An actual dlopen()'d package. */
    class TLoaded : public std::enable_shared_from_this<TLoaded> {
      NO_COPY_SEMANTICS(TLoaded);
      public:

      typedef std::shared_ptr<const TLoaded> TPtr;

      static TPtr Load(const Jhm::TAbsBase &package_dir, const TVersionedName &name);

      ~TLoaded();

      bool ForEachTest(const std::function<bool (const TTest *)> &cb) const;

      std::shared_ptr<const TFuncHolder> GetFunctionInfo(const Base::TPiece<const char> &func) const;

      const TVersionedName &GetName() const;

      const TTypeByIndexIdMap &GetTypeByIndexMap() const;

      const TIndexIdSet &GetIndexIdSet() const;

      bool ForEachIndexId(const std::function <bool (Base::TUuid *)> &cb) const;

      private:

      TLoaded(const Jhm::TAbsBase &package_dir, const TVersionedName &name);

      TVersionedName Name;

      TLinkInfo *LinkInfo;
      void *Handle;

    }; // TLoaded

    // TODO: A function to make a call to the actual stig function. Note in Spa we build a closure object around this.
    class TFuncHolder {
      NO_COPY_SEMANTICS(TFuncHolder);
      public:

      typedef std::shared_ptr<const TFuncHolder> TPtr;

      const TParamMap &GetParameters() const;
      const Type::TType &GetReturnType() const;

      Atom::TCore Call(TContext &ctx, const TArgMap &args) const;

      private:
      TFuncHolder(const TLoaded::TPtr &package, const TFuncInfo *func);
      TLoaded::TPtr Package;
      const TFuncInfo *Func;

      friend class TLoaded;
    };
  }  // Package

}  // Stig