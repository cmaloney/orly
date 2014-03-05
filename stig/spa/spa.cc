/* <stig/spa/spa.cc>

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

#include <cassert>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>

#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <base/assert_true.h>
#include <base/cmd.h>
#include <base/dir_iter.h>
#include <base/os_error.h>
#include <base/zero.h>
#include <stig/honcho.h>
#include <stig/spa/flux_capacitor/api.h>
#include <stig/spa/service.h>
#include <stig/spa/stig_args.h>
#include <stig/type.h>
#include <stig/type/stigify.h>
#include <stig/var/jsonify.h>
#include <stig/var/stigify.h>
#include <server/counter.h>
#include <server/daemonize.h>
#include <socket/address.h>
#include <third_party/mongoose/mongoose.h>
#include <tools/nycr/escape.h> // Sort of ug dependency here...

using namespace std;
using namespace Base;
using namespace Stig;
using namespace Stig::Spa;
using namespace Server;
using namespace Socket;
using namespace Tools::Nycr;

/* Counters reported by the health check. */
SERVER_COUNTER(MongooseNewRequests);
SERVER_COUNTER(MongooseEventLogs);
SERVER_COUNTER(GeneralErrors);
SERVER_COUNTER(ArgErrors);
SERVER_COUNTER(UrlDecodeErrors);
SERVER_COUNTER(CallsTo_GetHealth);
SERVER_COUNTER(CallsTo_GetFaveIcon);
SERVER_COUNTER(CallsTo_Try);


typedef Base::TPiece<const char> TStrPiece;

//TODO: Probably move to some header
//TODO: It would be sort of nice to have something like:
//#define LOG TLogger(TDemangle(__func__).GetStr());
//And also to pass in the code location.
class TLogger {
  public:
  TLogger(const char *func, int priority = LOG_NOTICE) : Priority(priority) {
    Strm << '[' << func << ']';
  }

  ~TLogger() {
    syslog(Priority, "%s", Strm.str().c_str());
  }

  template<typename TVal>
  TLogger &operator<<(TVal val) {
    Strm<<'[';
    Strm<<val;
    Strm<<']';
    return *this;
  }


  private:
  int Priority;
  stringstream Strm;
};


/* TODO */
class TCheckpointPathInvalidError : public Base::TFinalError<TCheckpointPathInvalidError> {
  public:

  /* TODO */
  TCheckpointPathInvalidError(const TCodeLocation &loc, const char *msg) {
    PostCtor(loc, msg);
  }

};  // TCheckpointPathInvalidError


//TODO: This is sort of fug to have laying around
static void ParseFilename(string &filename, string &checkpoint_name, unsigned int &CheckpointNum) {
  auto dot = filename.find_last_of('.');
  if (dot != string::npos) {
    string num = filename.substr(dot + 1);
    if (num.length() == 4) {
      // sample.0001
      checkpoint_name = filename.substr(0, dot); // sample
      CheckpointNum = atoi(num.c_str());        // 1
    } else {
      // sample.02
      checkpoint_name = filename; // sample.02
      CheckpointNum = 0;         // 0
    }
  } else {
    stringstream ss(filename);
    int n;
    ss >> n;
    if (ss.fail() || filename.length() != 4) {
      // sample
      checkpoint_name = filename; // sample
      CheckpointNum = 0;         // 0
    } else {
      // 0001
      checkpoint_name = ""; // ""
      CheckpointNum = n;   // 1
    }
  }
}

/* DEANNA COMMENT OUT TO SEE IF THIS IS WRONG ONE?
std::ostream &operator<<(std::ostream &strm, const std::chrono::milliseconds &time) {
  strm << time.count();
  return strm;
}
*/

class TCStringHasher {
  public:

  TCStringHasher() {}

  size_t operator()(const char *that) const {
    return std::_Fnv_hash_bytes(that, strlen(that), 0);
  }

};  // TCStringHasher

/* An HTTP method and URI combo, used for looking up handlers. */
class TKey {
  public:

  /* Construct the given method/URI combo. */
  TKey(const char *method = "", const char *uri = "")
      : Method(method), Uri(uri) {
    assert(method);
    assert(uri);
    Hash = TCStringHasher()(method) ^ TCStringHasher()(uri);
  }

  /* We're equal iff. both method and URI are equal. */
  bool operator==(const TKey &that) const {
    assert(this);
    assert(&that);
    return strcmp(Method, that.Method) == 0 && strcmp(Uri, that.Uri) == 0;
  }

  /* Our hash is sensitive to both our method and our URI. */
  size_t GetHash() const {
    assert(this);
    return Hash;
  }

  /* TODO */
  const char *GetUri() const {
    assert(this);
    return Uri;
  }

  private:

  /* Our method and URI.  Note that we don't copy them but merely point at them. */
  const char *Method, *Uri;

  /* The hash of the above strings. */
  size_t Hash;

};  // TKey


namespace std {

  /* A standard hasher for TKey. */
  template <>
  struct hash<TKey> {

    typedef size_t return_type;
    typedef TKey argument_type;

    size_t operator()(const argument_type &that) const {
      return that.GetHash();
    }

  };  // hash<TKey>

  ostream &operator<<(ostream &out, const chrono::milliseconds &that) {
    assert(&out);
    assert(&that);

    out << that.count();

    return out;

  }

}  // std

/* Our Mongoose-based front-end.

   Constructing an instance of this class will launch background threads which will
   serve the incoming HTTP requests.  The threads will run as long as the instance
   persists.

   All of the On...() functions are event handlers, called by the background threads.
   If they need to act on common data, they need to be synchronized around it with
   a mutex or something similar. */
class TSpa : public Mongoose::TMongoose {
  NO_COPY_SEMANTICS(TSpa);
  public:

  //TODO: Move to <base/service.h> framework. For now going for fast to implement, not pretty.
  class TCmd : public Base::TCmd {

    public:
    TCmd(int argc, char ** argv) : CheckpointArg("checkpoints/"), Daemon(false), PackageDir("packages/"), Port("19380"),
          Threads("10")  {
      Parse(argc, argv, TMeta());
    }

    private:
    class TMeta : public Base::TCmd::TMeta {
      public:
      TMeta() : Base::TCmd::TMeta("STIG Spa Development Server") {
        //TODO: Change to handler func
        Param(&TCmd::CheckpointArg, "checkpoint", Optional, "checkpoint\0c\0",
            "File to use as input checkpoint file, or directory in which checkpoints should be searched for.");
        Param(&TCmd::Daemon, "daemon", Optional, "daemon\0d\0", "Run in daemon mode (Shield from signals, etc.)");
        Param(&TCmd::PackageDir, "package_dir", Optional, "packages\0", "Look for packages in the given directory");
        Param(&TCmd::Port, "port", Optional,
            "port\0p\0", "Port(s) to listen on (See Mongoose documentation on listening_ports)");
        Param(&TCmd::Threads, "thread_count", Optional, "threads\0", "Number of threads to use in mongoose. Must be convertible to an integer.");
      }

      private:
      virtual void WriteAfterDesc(std::ostream &strm) const {
        assert(this);
        assert(&strm);
        strm << "Build: Unknown" << endl // FIXME: Use Version from SCM to tag.
             << endl
             << "Copyright Stig LLC" << endl
             << "Licensed under the Apache License, Version 2.0" << endl;
      }
    };
    public:
    //NOTE: Should be private with accessors. This is quicker to write for now.
    std::string CheckpointArg;
    bool Daemon;
    std::string PackageDir, Port, Threads;
  };

  //TODO: Should really move in the TCmd as, if it goes away before TSpa, TSpa will be left in a bad state.
  TSpa(const TCmd &cmd) : Cmd(cmd) {}

  virtual int Run() {
    SetupCheckpointInfo();

    bool success = false;
    if (Cmd.Daemon) {
      /* We're been asked to launch as a daemon, so it's time to fork. */
      pid_t pid;
      TOsError::IfLt0(HERE, pid = Daemonize());
      if (pid) {
        /* We're the parent, so report the PID of the daemon and we're done. */
        cout << pid << endl;
      } else {
        /* We're the daemonic child, so start serving. */
        success = RunServer();
      }
    } else {
      /* We've not been asked to launch as a deamon, so we will run in console
         mode instead. */
      DefendAgainstSignals();
      success = RunServer(LOG_PERROR);
      /* Throwing this extra EOL makes the console look a little cleaner after
         the user hits ctrl-C. */
      cout << endl;
    }
    return success ? EXIT_SUCCESS : EXIT_FAILURE;
  }

  private:


  /* Called by Mongoose to let us know an event has occurred. */
  virtual void *OnEvent(mg_event event, mg_connection *conn, const mg_request_info *request_info) {
    assert(this);
    bool is_handled = false;
    try {
      switch (event) {
        case MG_NEW_REQUEST: {
          MongooseNewRequests.Increment();
          /* Have OnNewRequest() try to dispatch the call to the proper handler. */
          ostringstream strm;
          is_handled = OnNewRequest(request_info->request_method, request_info->uri, request_info->query_string, strm);
          if (is_handled) {
            /* If we found a handler, send the handler's reply. */
            std::string s = strm.str();
            mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %d\r\n\r\n", s.size());
            mg_printf(conn, s.c_str());
          }
          break;
        }
        case MG_EVENT_LOG: {
          MongooseEventLogs.Increment();
          TLogger("mongoose error") << request_info->log_message << request_info->uri << request_info->query_string;
          break;
        }
        default: {
          is_handled = false;
        }
      }
    } catch (const TArgError &arg_error) {
      ArgErrors.Increment();
      TLogger("arg error") << arg_error.what() <<" at query string offset " << arg_error.GetOffset();
      mg_printf(conn, "HTTP/1.1 400 BAD REQUEST\r\nContent-Type: text/plain\r\n\r\n[%s at query string offset %d]", arg_error.what(), arg_error.GetOffset());
      is_handled = true;
    } catch (const TUrlDecodeError &url_decode_error) {
      UrlDecodeErrors.Increment();
      TLogger("url decode error") << url_decode_error.what() << " at query string offset " << url_decode_error.GetOffset();
      mg_printf(conn, "HTTP/1.1 400 BAD REQUEST\r\nContent-Type: text/plain\r\n\r\n[%s at query string offset %d]", url_decode_error.what(), url_decode_error.GetOffset());
      is_handled = true;
    } catch (const Base::TError &ex) {
      TLogger("general error") << ex.what() << request_info->uri << request_info->query_string;
      mg_printf(conn, "HTTP/1.1 400 BAD REQUEST\r\nContent-Type: text/plain\r\n\r\n[%s][%s]", ex.what(), request_info->query_string);
      is_handled = true;
    }  catch(const std::exception &ex) {
      GeneralErrors.Increment();
      TLogger("std::exception") << ex.what() << request_info->uri << request_info->query_string;
      mg_printf(conn, "HTTP/1.1 400 BAD REQUEST\r\nContent-Type: text/plain\r\n\r\n[std::exception][%s]", ex.what());
      is_handled = true;
    }
    return is_handled ? const_cast<char *>("") : 0;
  }


  /* Handles "GET /favicon.ico."  We handle this just to avoid
     getting a slew of 404 errors in the log.  Browsers are so
     pushy. */
  bool OnGetFavIcon(TArgs &args, ostream &/*strm*/) {
    args.VerifyAllUsed();
    CallsTo_GetFaveIcon.Increment();
    return true;
  }

  /* Handles "GET /sys/counters". */
  bool OnGetCounters(TArgs &args, ostream &strm) {
    assert(&args);
    assert(&strm);

    args.VerifyAllUsed();

    TLogger("GetCounters", LOG_INFO);
    TCounter::Sample();
    strm << "now=" << TCounter::GetSampleTime() << endl;
    strm << "since=" << TCounter::GetResetTime() << endl;
    strm << endl;
    for (const TCounter *counter = TCounter::GetFirstCounter(); counter;counter = counter->GetNextCounter()) {
      strm << counter->GetCodeLocation() << "." << counter->GetName() << "=" << counter->GetCount() << endl;
    }
    return true;
  }

  /* Handles "GET /sys/installed". */
  bool OnGetInstalled(TArgs &args, ostream &strm) {
    assert(&args);
    assert(&strm);

    args.VerifyAllUsed();

    TLogger("GetInstalled", LOG_INFO);

    Base::AssertTrue(Service)->GetPackageManager().YieldInstalled([&strm](const Package::TVersionedName &name) -> bool {
      strm << name.Name << "." << name.Version << endl;
      return true;
    });
    return true;
  }

  /* Handles "GET /sys/health". */
  bool OnGetHealth(TArgs &args, ostream &/*strm*/) {
    assert(this);
    assert(&args);

    args.VerifyAllUsed();

    CallsTo_GetHealth.Increment();
    return true;
  }

  /* Try to dispatch an incoming request to the proper handler.  The 'method', 'uri', and
     'query' strings come to us from Mongoose's parsing of the 1st line of the HTTP
     request.  If we find a handler, dispatch to it, having it write its reply onto the given
     stream.  We'll return success if the handler returns success.  If we don't find a handler,
     leave the reply stream alone and return false. */
  bool OnNewRequest(const char *method, const char *uri, char *query, ostream &strm) {
    assert(this);
    assert(method);
    assert(uri);

    /* Parse the query into key-value pairs.  Note that we require our query items
       to be separated by ampersands.  We don't support the semicolon option. */
    TArgs args(query);

    bool result;

    try {

      /* Lookup the handler and dispatch. */
      const TKey key(method, uri);
      auto iter = HandlerByKey.find(key);
      if(iter != HandlerByKey.end()) {
        result = (this->*(iter->second))(std::ref(args), std::ref(strm));
      } else if (strncmp(uri, "/try/", 5) == 0) {
        result = OnTry(uri+5, args, strm);
      } else if (strncmp(uri, "/do/", 4) == 0) {
        result = OnDo(uri+4, args, strm);
      } else {
        args.FakeVerify();
        result = false;
      }

      //Make a nicer error out of has verified.
      if(!args.HasVerified()) {
        TLogger("args.HasVerified", LOG_CRIT) << method << uri;
        //NOTE: This is a soft fail when not in dev mode. TError::Abort(HERE);
        args.FakeVerify(); //TODO: It would be WAAAYYYY nicer to have an ifdef "DEV" / "PROD"
      }
    } catch (...) {
      //Cleanup args so it doesn't abort on a throw before VerifyAllArgs was called.
      args.FakeVerify();
      throw;
    }

    return result;

  }

  bool OnPoll(TArgs &args, ostream &strm) {
    assert(this);

    std::unordered_set<TUUID> notifiers;
    TOpt<chrono::milliseconds> timeout;
    TUUID session;
    args.Get("notifiers", notifiers);
    args.Get("timeout", timeout);
    args.Get("session", session);
    args.VerifyAllUsed();

    TLogger("Poll") << timeout;

    std::unordered_map<TUUID, FluxCapacitor::TNotifierState> notifier_states;
    Base::AssertTrue(Service)->Poll(session, notifiers, timeout, notifier_states);
    bool first;
    for(auto notifier_state: notifier_states) {
      if(!first) {
        strm << ",";
      }
      strm<<"["<<notifier_state.first<<","<< TService::GetNotifierStateStr(notifier_state.second)<<"]";
      first = false;
    }
    return true;
  }

  /* Handles "POST /sys/create_session". */
  bool OnPostCreateSession(TArgs &args, ostream &strm) {
    assert(this);
    TOpt<TUUID> acct;
    int ttl;

    args.Get("acct", acct);
    args.Get("ttl", ttl);
    args.VerifyAllUsed();

    TLogger("CreateSession") << acct << ttl;

    TUUID session;
    Base::AssertTrue(Service)->CreateSession(acct, ttl, session);
    strm<<session;
    return true;
  }

  /* Handles "POST /sys/create_private_pov". */
  bool OnPostCreatePrivatePov(TArgs &args, ostream &strm) {
    assert(this);

    TUUID session;
    TOpt<TUUID> parent;
    int ttl;
    bool paused = false;
    args.Get("session", session);
    args.Get("parent", parent);
    args.Get("ttl", ttl);
    args.GetOpt("paused", paused);
    args.VerifyAllUsed();

    TLogger("CreatePrivatePov") << session << parent << ttl;

    TUUID ppov;
    Base::AssertTrue(Service)->CreatePrivatePov(session, parent, ttl, paused, ppov);
    strm<<ppov;
    return true;
  }

  /* Handles "POST /sys/create_shared_pov". */
  bool OnPostCreateSharedPov(TArgs &args, ostream &strm) {
    assert(this);

    TOpt<TUUID> parent;
    int ttl;
    bool paused = false;
    args.Get("parent", parent);
    args.Get("ttl", ttl);
    args.GetOpt("paused", paused);
    args.VerifyAllUsed();

    TLogger("CreateSharedPov") << parent << ttl;

    TUUID spov;
    Base::AssertTrue(Service)->CreateSharedPov(parent, ttl, paused, spov);
    strm<<spov;
    return true;
  }

  /* Handles "POST /sys/reset_counters". */
  bool OnPostResetCounters(TArgs &args, ostream &strm) {
    assert(this);

    args.VerifyAllUsed();

    TLogger("ResetCounters");

    strm << TCounter::Reset() << endl;
    return true;
  }

  /* Handles "POST /sys/install_package". */
  bool OnPostInstallPackage(TArgs &args, ostream &/*strm*/) {
    assert(this);

    std::string file;
    args.Get("file", file);
    args.VerifyAllUsed();

    TLogger("InstallPackage") << file;

    //TODO: Make it so end user can give a set.
    Base::AssertTrue(Service)->GetPackageManager().Install({Package::TVersionedName::Parse(AsPiece(file))});

    return true;
  }

  /* Handles "POST /sys/uninstall_package". */
  bool OnPostUninstallPackage(TArgs &args, ostream &/*strm*/) {
    assert(this);

    std::string file;
    args.Get("file", file);
    args.VerifyAllUsed();

    TLogger("UninstallPackage") << file;

    //TODO: Make it so end user can give a set.
    Base::AssertTrue(Service)->GetPackageManager().Uninstall({Package::TVersionedName::Parse(AsPiece(file))});
    return true;
  }

  bool OnDo(const char *, TArgs &, ostream &) {
    assert(this);

    throw Base::TNotImplementedError(HERE);
  }

  bool OnTry(const char *uri, TArgs &args, ostream &strm) {
    assert(this);

    CallsTo_Try.Increment();

    //TODO: Pull checking if the package exists to here.
    //TODO: Better enum arg grabbing (json, stig).
    //TODO: Refactor so that argument list checking functionality is here
    TUUID ppov;
    std::unordered_set<TUUID> notify_povs;
    std::string output("json");
    args.Get("private_pov", ppov);
    args.GetOpt("notify_pov", notify_povs);
    args.GetOpt("output", output);

    const char *dot_pos = strrchr(uri, '.');

    //Allow '/' as seperator if we don't find a '.'
    if(dot_pos == 0) {
      dot_pos = strrchr(uri, '/');
      if(dot_pos == 0) {
        throw TArgError(HERE, 0, "Could not find the function name. A '.' or '/' should seperate the function and package name.");
      }
    }

    if(output != "json" && output != "stig") {
      throw TArgError(HERE, 0,"argument 'output' has an invalid value. Must be 'json' or 'stig'");
    }

    //Get the function
    auto func = Base::AssertTrue(Service)->GetPackageManager().Get(Jhm::TNamespace(TPiece<const char>(uri, dot_pos)))
        ->GetFunctionInfo(AsPiece(dot_pos + 1));

    //Get the arguments
    TArgs::TStigArg prog_args;
    Atom::TCore::TExtensibleArena *arena = Service->GetArena();
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    for(auto &it: func->GetParameters()) {
      Var::TVar arg_var;
      //TODO: The string mashing and then .c_str() here is fugly.
      args.Get(('.' + it.first).c_str(), arg_var);
      Indy::TKey arg_key(Atom::TCore(arena, Sabot::State::TAny::TWrapper(Var::NewSabot(state_alloc, arg_var)).get()), arena);
      prog_args.insert(make_pair(it.first, arg_key));
    }

    args.VerifyAllUsed();

    //Call the function
    std::unordered_map<TUUID, TUUID> notifiers;

    TLogger("Try") << uri;

    Var::TVar result;
    try {
      //TODO: It would be nice if try wrapped the function in a Stig TErr if the call failed for us. It probably should...
      void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
      Atom::TSuprena suprena;
      Atom::TCore result_core;
      Base::AssertTrue(Service)->Try(func, ppov, notify_povs, prog_args, result_core, suprena, notifiers);
      result = Var::ToVar(*Sabot::State::TAny::TWrapper(result_core.NewState(&suprena, state_alloc)));
    } catch (const Rt::TRuntimeError &ex) {
      if (output == "json") {
        strm << "{ \"error\": " << Tools::Nycr::TEscape(ex.what()) << "}" << std::endl;
        return true;
      } else {
        //TODO: Stigify exception result
        throw;
      }
    }
    if(output == "json") {
      strm << "{ \"type\": \"";
      Stig::Type::Stigify(strm, result.GetType());
      strm << "\", \"result\": ";
      Stig::Var::Jsonify(strm, result);

      if(notify_povs.size()) {
        strm << ", \"notifiers\": ";
        if(notifiers.size()) {
          strm << '{';
          bool first = true;
          for(auto it: notifiers) {
            if(!first) {
              strm << ",";
            } else {
              first = false;
            }
            strm << '"' << it.first << "\":\"" << it.second << '"';
          }
          strm << '}';
        } else {
          strm << "\"no update\"";
        }
      }
      strm << '}';

    } else if(output == "stig") {
      strm << "<{ .result: ";
      Stig::Var::Stigify(strm, result);

      if(notify_povs.size()) {
        strm << ", .notifiers: ";
        if(notifiers.size()) {
          strm << '{';
          bool first = true;
          for(const auto &it: notifiers) {
            if(!first) {
              strm << ',';
            }
            Stig::Var::Stigify(strm, it.first);
            strm << ':';
            Stig::Var::Stigify(strm, it.second);
            first = false;
          }
          strm << '}';
        } else {
          strm << "unknown {id:id}";
        }
      }

      strm << "}>";
    } else {
      assert(false); //TODO //I should be Jason's soft assert.
    }
    strm << std::endl;
    return true;
  }

  /* Opens the log, launches the HTTP service on the given port,
     and waits for the signal to exit.  Returns success/failure. */
  bool RunServer(int extra_log_flags = 0) {
    assert(this);
    bool success;
    /* Lifespan of TService */ {
      TService service;
      Service = &service; // Set the pointer to service
      service.SetPackageDir(Cmd.PackageDir);
      if (CheckpointSource.size() > 0) {
        try {
          service.LoadCheckpoint(CheckpointSource);
        } catch (const std::exception &ex) {
          cerr << ex.what();
          TLogger("error loading checkpoint", LOG_ERR) << ex.what();
          return false;
        }
      }
      /* Start logging. */
      openlog("spa", LOG_PID | extra_log_flags, LOG_USER);
      TLogger("start");
      try {
        /* Launch the HTTP listener/dispatcher on a gaggle of background
           threads.  These threads will run until the 'service' object
           we construct here goes out of scope. */
        const char *opts[] = {"enable_directory_listing", "no",
                              "listening_ports"         , Cmd.Port.c_str(),
                              "num_threads"             , Cmd.Threads.c_str(),
                              0};
        Start(opts);
        /* Wait for the signal to stop serving, then we're done. */
        pause();
        success = true;
      } catch (const exception &ex) {
        /* An exception that gets this far is reported as an error in the log. */
        TLogger("exception", LOG_ERR)<<ex.what();
        success = false;
      }
      /* Shutdown */
      TLogger("shutting down");
      Stop();
      service.Finalize();
      service.SaveCheckpoint(CheckpointOut);
      Service = nullptr;
    }
    TLogger("stop");
    return success;
  }

  /* Parse CheckpointArg into CheckpointOut and CheckpointSource */
  void SetupCheckpointInfo() {
    assert(this);

    std::string checkpoint_dir, checkpoint_name;
    const char *checkpoint_ext = ".checkpoint";
    unsigned int CheckpointNum;

    //TODO: This logic is sort of mangled because we moved it out from other places to here. Going with the simple
    //assign fix for now.
    CheckpointSource = Cmd.CheckpointArg;

    struct stat sb;
    if (stat(CheckpointSource.c_str(), &sb)) {
      cout << "stat failed: " << CheckpointSource << endl;
      throw TCheckpointPathInvalidError(HERE, "stat failed on the specified checkpoint path");
    }
    if (S_ISREG(sb.st_mode)) {
      // File
      auto dot = CheckpointSource.find_last_of('.');
      // Does not end in '.checkpoint'
      if (dot == string::npos || CheckpointSource.substr(dot) != checkpoint_ext) {
        throw TCheckpointPathInvalidError(HERE, "Checkpoint files must end in '.checkpoint'");
      }
      auto slash = CheckpointSource.find_last_of('/');
      string filename;
      // Set checkpoint directory and filename
      if (slash == string::npos) {
        checkpoint_dir = "./";
        filename = CheckpointSource;
      } else {
        checkpoint_dir = CheckpointSource.substr(0, slash + 1);
        filename.assign(CheckpointSource, slash + 1, dot - slash - 1);
      }
      // Split filename into name and number
      ParseFilename(filename, checkpoint_name, CheckpointNum);
    } else if (S_ISDIR(sb.st_mode)) {
      // Directory
      // Append a slash at the end if it's missing
      if (*(CheckpointSource.end()-1) != '/') {
        CheckpointSource += '/';
      }
      // Set checkpoint directory
      checkpoint_dir = CheckpointSource;
      Base::TDirIter dir_iter(CheckpointSource.c_str());
      string filename;
      string max, cur;
      string max_name, cur_name;
      unsigned int max_num = 0, cur_num;
      for (; dir_iter; ++dir_iter) {
        // 'max' and 'cur' are names including the extension
        cur = dir_iter.GetName();
        auto dot = cur.find_last_of('.');
        // Doesn't have a '.checkpoint' extension, so skip
        if (dot == string::npos || cur.substr(dot) != checkpoint_ext) continue;
        filename = cur.substr(0, dot);
        ParseFilename(filename, cur_name, cur_num);
        // Compare name first, if name is equal, compare numbers
        if (max_name < cur_name || (max_name == cur_name && max_num < cur_num)) {
          max = cur;
          max_name = cur_name;
          max_num = cur_num;
        }
      }
      if (max.length()) {
        CheckpointSource += max;
        checkpoint_name = max_name;
        CheckpointNum = max_num;
      } else {
        CheckpointSource.erase();
        checkpoint_name.erase();
        CheckpointNum = 0;
      }
    } else {
      throw TCheckpointPathInvalidError(HERE, "The specified checkpoint path is neither a file or a directory");
    }

    ostringstream oss;
    oss << checkpoint_dir;
    if(checkpoint_name.size()) {
      oss << checkpoint_name << '.';
    }
    oss << setw(4) << setfill('0') << CheckpointNum + 1 << ".checkpoint";
    CheckpointOut = oss.str();
  }

  /* Writes out a time-based report to the given stream. */
  static void WriteTime(ostream &strm, const char *verb, time_t timeval) {
    strm << verb << " at second " << timeval << ", which is " << ctime(&timeval);
  }

  /* A mapping from method-uri pairs to handlers.  This is where OnNewRequest()
     looks up the handler to invoke for a given request. */
  static const unordered_map<TKey, bool (TSpa::*)(TArgs &, ostream &)> HandlerByKey;

  //NOTE: These are strings because otherwise we would have to stringify them to pass them into Mongoose's option array.
  std::string CheckpointOut;
  std::string CheckpointSource;
  const TCmd &Cmd;
  THoncho Honcho;
  TService *Service;
};  // TSpa

/* See the declaration. */
const unordered_map<TKey, bool (TSpa::*)(TArgs &, ostream &)> TSpa::HandlerByKey = {
  { TKey("GET", "/sys/health"), &TSpa::OnGetHealth },
  { TKey("GET", "/sys/counters"), &TSpa::OnGetCounters },
  { TKey("GET", "/sys/installed"), &TSpa::OnGetInstalled },
  { TKey("POST", "/sys/reset_counters"), &TSpa::OnPostResetCounters },
  { TKey("GET", "/sys/reset_counters"), &TSpa::OnPostResetCounters },
  { TKey("GET", "/favicon.ico"), &TSpa::OnGetFavIcon },
  { TKey("POST", "/sys/install_package"), &TSpa::OnPostInstallPackage },
  { TKey("GET", "/sys/install_package"), &TSpa::OnPostInstallPackage },
  { TKey("POST", "/sys/uninstall_package"), &TSpa::OnPostUninstallPackage },
  { TKey("GET", "/sys/uninstall_package"), &TSpa::OnPostUninstallPackage },
  { TKey("POST", "/sys/create_session"), &TSpa::OnPostCreateSession },
  { TKey("GET", "/sys/create_session"), &TSpa::OnPostCreateSession },
  { TKey("POST", "/sys/create_private_pov"), &TSpa::OnPostCreatePrivatePov },
  { TKey("GET", "/sys/create_private_pov"), &TSpa::OnPostCreatePrivatePov },
  { TKey("POST", "/sys/create_shared_pov"), &TSpa::OnPostCreateSharedPov },
  { TKey("GET", "/sys/create_shared_pov"), &TSpa::OnPostCreateSharedPov },
  { TKey("POST", "/sys/poll"), &TSpa::OnPoll },
  { TKey("GET", "/sys/poll"), &TSpa::OnPoll }
};

//TODO: Factor out
int main(int argc, char **argv) {
  TSpa::TCmd cmd(argc, argv);
  TSpa spa(cmd);
  return spa.Run();
}