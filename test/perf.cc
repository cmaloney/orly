/* <test/perf.cc>

   Implements <test/perf.h>

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

#include <test/perf.h>

#include <iostream>

#include <base/cmd.h>
#include <base/split.h>


using namespace Base;
using namespace std;
using namespace Test;

TMetric::~TMetric() {
  assert(PreDtorCalled);
}

TMetric::TMetric(const char *type_name, const char *name, TDirection direction)
    : Direction(direction), Name(name), TypeName(type_name), PreDtorCalled(false) {}

const char *TMetric::GetTypeName() const {
  assert(this);
  return TypeName;
}

const char *TMetric::GetName() const {
  assert(this);
  return Name;
}

TMetric::TDirection TMetric::GetDirection() const {
  assert(this);
  return Direction;
}

TPerf::TPerf(const char *name, TFunc func, chrono::seconds max_runtime)
  : Func(func), MaxRuntime(max_runtime), Name(name), Next(0) {
  if(First) {
    Last->Next = this;
    Last = this;
  } else {
    First = this;
    Last = this;
  }
}

TPerf::TFunc TPerf::GetFunc() const {
  assert(this);
  return Func;
}


std::chrono::seconds TPerf::GetMaxRuntime() const {
  assert(this);
  return MaxRuntime;
}

const char *TPerf::GetName() const {
  assert(this);
  return Name;
}

const TPerf *TPerf::GetNext() const {
  assert(this);
  return Next;
}

const TPerf *TPerf::GetFirst() {
  return First;
}

TPerf *TPerf::First = 0;
TPerf *TPerf::Last = 0;

class TApp : public TCmd {
  NO_COPY_SEMANTICS(TApp);
  public:

  TApp() : Describe(false) {}

  private:

  class TRunner {
    NO_COPY_SEMANTICS(TRunner);
    public:

    TRunner(const TPerf *perf) : Perf(perf) {}

    //TODO: Fork! Run the test in the forked thread, collecting up all it says. Kill the fork if it takes too long.
    /* Spawns a second thread where the perf test is run. Kills the second thread if max_time elapses without it returning. */
    void Run() {
      auto start = chrono::steady_clock::now();
      Perf->GetFunc()();
      chrono::milliseconds elapsed(chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start));
      cout << "  \"elapsed\": " << elapsed.count() << endl;
    }

    private:
    const TPerf *Perf;
  };

  class TMeta : public TCmd::TMeta {
    NO_COPY_SEMANTICS(TMeta);
    public:

    TMeta() : TCmd::TMeta("Stig C++ Perf Test") {
      Param(&TApp::Describe, "describe_tests", Optional, "describe\0", "Describe the tests and their max runtimes, but don't run them.");
    }
  };

  virtual TCmd::TMeta *NewMeta() const {
    assert(this);
    return new TMeta();
  }

  bool ForEachTest(const function<bool (const TPerf *test)> &cb) {
    for (const TPerf *test = TPerf::GetFirst(); test; test = test->GetNext()) {
      if (!cb(test)) {
        return false;
      }
    }
    return true;
  }

  bool JsonForEachTest(const function<bool (const TPerf *test, std::ostream &out)> &cb, ostream &out) {
    bool first = true;
    cout << '{' << endl;
    bool result = ForEachTest([&cb, &first, &out](const TPerf *test) {
      if(!first) {
        cout << ',' << endl;
      } else {
        first = false;
      }
      cout << "  \"" << test->GetName() << "\" : {" << endl;
      bool result = cb(test, out);
      cout << "  }";
      return result;
    });
    cout << endl << '}' << endl;
    return result;
  }

  bool Do() {
    auto func = Describe ? [](const TPerf *test, ostream &out) {
        out << "    \"max_time\": " << test->GetMaxRuntime().count() << endl;
        return true;
      } : [](const TPerf *test, std::ostream &) {
        //TODO: JSONIFY
        TRunner(test).Run();
        return true;
      };
    bool result = JsonForEachTest(func, cout);
    cout << endl;
    return result;
  }

  virtual int Run() {
    assert(this);

    return Do() ? EXIT_SUCCESS : EXIT_FAILURE;
  }

  bool Describe;
};

int main(int argc, char **argv) {
  TApp().Main(argc, argv);
}