/* <stig/spa/stig_args.h>

   Helper functions for the HTTP server.

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

#include <list>
#include <string>
#include <unordered_map>

#include <base/error.h>
#include <base/piece.h>
#include <base/split.h>
#include <server/url_decode.h>
#include <stig/indy/key.h>
#include <stig/uuid.h>
#include <stig/var.h>

/* TODO: The argument parsing here should probably share a decent amount of code with <base/cmd.h>*/
namespace Stig {

  namespace Spa {

      /* Throw an instance of this class when you either cannot find a required argument
         or cannot convert the argument's token into the required type. */
      class TArgError : public Base::TFinalError<TArgError> {
        public:

        /* Construct with the name of the problematic argument. */
        TArgError(const Base::TCodeLocation &loc, size_t offset, const char *msg) : Offset(offset) {
          PostCtor(loc, msg);
        }

        /* The name of the problematic argument.  Never null. */
        size_t GetOffset() const {
          assert(this);
          return Offset;
        }

        private:

        /* See accessor. */
        size_t Offset;
      };  // TArgError

      /* A collection of HTTP arguments. Builds up a map on a first pass through, and as arguments are requested decodes
         them when asked. It has a method to check that all the arguments it was given have been used, and it will abort
         on destruction if AssertAllUsed has not been called. We do this to ensure that STIG Developers call the check
         function. We could have called AssertAllUsed for you in the destructor, but throwing an error there you could
         have already done something bad to the database. */
      class TArgs {

        NO_COPY_SEMANTICS(TArgs);

        public:

        /* A map of strings to Atom::TCore which serves as arguments to a stig program. */
        typedef std::unordered_map<std::string, Indy::TKey> TStigArg;

        /* For Utility */
        typedef Base::TPiece<const char> TStrPiece;

        template<typename TVal>
        struct TTypeDependentFalse {
          static const bool False=false;
        };

        //NOTE: The key is urldecoded already, the value is not!
        typedef std::unordered_map<std::string, TStrPiece> TArgMap;

        /* if query_string is non-null, do a single forward pass over it, stripping off starting '?', splitting at '&',
           and then storing the resulting k-v pairs into the argument map */
        TArgs(const char *query_string=0);

        ~TArgs();

        /* Fakes a VerifyAllUsed. Use ONLY when you aren't handling a real call. */
        void FakeVerify();

        template<typename TVal>
        inline void Get(const char *name, TVal &val) {
          Get(Base::AsPiece(name), val);
        }

        template<typename TVal>
        inline void ConvertSafe(const TStrPiece &name, const std::string &str, TVal &val) {
          try {
            Convert(str, val);
          } catch (const TIntArgError &e) {
            std::ostringstream s;
            s<<"argument '";
            s.write(name.GetStart(), name.GetSize());
            s<<"' not a valid "<<e.GetMsg();

            throw TArgError(HERE, e.GetOffset(), s.str().c_str());
          }
        }


        template<typename TVal>
        void Get(const TStrPiece &name, Base::TOpt<TVal> &val) {
          assert(this);
          assert(&name);
          assert(&val);

          std::string arg;
          if(TryGrabArg(name, arg)) {
            ConvertSafe(name, arg, val);
          }
        }

        template<typename TVal>
        void Get(const TStrPiece &name, TVal &val) {
          assert(this);
          assert(&name);
          assert(&val);

          std::string arg;
          GrabArg(name, arg);
          ConvertSafe(name, arg, val);
        }

        bool GetOpt(const TStrPiece &name, std::string &val) {
          assert(this);
          assert(&name);
          assert(&val);

          return TryGrabArg(name, val);
        }


        template<typename TVal>
        bool GetOpt(const TStrPiece &name, TVal &val) {
          assert(this);
          assert(&name);
          assert(&val);

          std::string arg;
          if(!TryGrabArg(name, arg)) {
            return false;
          }
          ConvertSafe(name, arg, val);
          return true;
        }

        /* This is a little different than the others, in that it finds all items matching the prefix, removes the
           prefix, and builds a map out of them.*/
        template<typename TVal>
        bool GetOpt(const char *prefix, std::unordered_map<std::string, TVal> &args) {
          assert(this);
          assert(prefix);
          assert(&args);

          //TODO: Ugly copies
          std::vector<std::string> to_delete;

          for(auto it: Args) {
            if (it.first.find(prefix) == 0) {
              //Stash away the item for later deletion
              to_delete.push_back(it.first);

              //Insert it into our new argument map.
              //TODO: Assumes value type and copies copiously.
              TVal val;
              std::string val_str;
              ::Server::UrlDecode(it.second, val_str);
              ConvertSafe(Base::AsPiece(it.first), val_str, val);
              args.insert(make_pair(it.first.substr(1), val));
            }
          }

          for(const std::string &it: to_delete) {
            Args.erase(it);
          }

          return to_delete.size() > 0;
        }

        /* Like get, but doesn't touch val if no parameter by the given name is given. Useful for default arguments. */
        template<typename TVal>
        inline bool GetOpt(const char *name, TVal &val) {
          return GetOpt(Base::AsPiece(name), val);
        }


        bool HasVerified() const;

        /* Checks that all the parameters have been used. Throws an error if there are extras. */
        void VerifyAllUsed();

        private:

        //NOTE: This exception should NEVER EVER EVER escape the TArgs class.
        class TIntArgError {
        public:

          TIntArgError(const char *msg, size_t offset=0) : Msg(Base::AssertTrue(msg)), Offset(offset) {}

          const char *GetMsg() const {
            assert(this);

            return Msg;
          }

          size_t GetOffset() const {
            assert(this);

              return Offset;
          }

        private:
          const char *Msg;
          size_t Offset;
        };

        void Convert(const std::string &s, bool &val);
        void Convert(const std::string &s, int &val);
        void Convert(const std::string &s, long &val);
        void Convert(const std::string &str, uint64_t &val);
        void Convert(const std::string &s, TUUID &uuid);
        void Convert(const std::string &s, Var::TVar &var);
        void Convert(const std::string &s, std::chrono::milliseconds &val);

        template<typename TVal>
        void Convert(const std::string &s, Base::TOpt<TVal> &opt_val) {
          assert(this);
          assert(&s);
          assert(&opt_val);

          if(s.size() == 0) {
            opt_val.Reset();
          } else {

            //TODO: the temporary val here should be destroyed.
            TVal val;
            Convert(s, val);
            opt_val = val;
          }
        }

        template<typename TVal>
        void Convert(const std::string &s, Rt::TOpt<TVal> &opt_val) {
          assert(this);
          assert(&s);
          assert(&opt_val);

          if(s.size() == 0) {
            opt_val.Reset();
          } else {

            //TODO: the temporary val here should be destroyed.
            TVal val;
            Convert(s, val);
            opt_val = val;
          }
        }

        template<typename TVal>
        void Convert(const std::string &s, std::list<TVal> &val_list) {
          //Split str by ','
          std::vector<std::string> pieces;
          Base::Split(",", s, pieces);

          for(auto it: pieces) {
            TVal val;
            Convert(it, val);
            val_list.push_back(val);
          }
        }


        template<typename TVal>
        void Convert(const std::string &s, std::unordered_set<TVal> &val_set) {
          //Split str by ','
          std::vector<std::string> pieces;
          Base::Split(",", s, pieces);

          for(auto it: pieces) {
            TVal val;
            Convert(it, val);
            if(val_set.count(val) > 0) {
              //TODO: Make a more specific error message, wait for more to gather.
              throw TIntArgError("duplicate item");
            }
            val_set.insert(val);
          }
        }

        /* Grabs an arg out of the arg map, permanently removing it from the arg map. Also demangles the argument.*/
        bool TryGrabArg(const TStrPiece &name, std::string &out);

        void GrabArg(const TStrPiece &name, std::string &out);

        bool VerifiedAllUsed;

        TArgMap Args;

      };

      /* Make string get a little more efficient. */
      template<>
      void TArgs::Get(const TStrPiece &name, std::string &str);

  } // Spa

} // Stig