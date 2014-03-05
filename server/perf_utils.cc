/* <server/perf_utils.cc>

   Implements <server/perf_utils.h>.

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

#include <server/perf_utils.h>

#include <algorithm>
#include <cmath>
#include <getopt.h>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;
using namespace Base;
using namespace Server;

mutex TAnyActor::Mutex;

condition_variable TAnyActor::Cond;

bool TAnyActor::GoFlag = false;

TAnyFactory *TAnyFactory::First = 0, *TAnyFactory::Last = 0;

class TStats {
  public:

  TStats(const vector<TAnyActor *> &actors, int spin_count, double (TAnyActor::*getter)() const) {
    assert(!actors.empty());
    IsSingle = (actors.size() == 1);
    if (IsSingle) {
      Mean = (actors[0]->*getter)();
    } else {
      double total = 0;
      for (size_t i = 0; i < actors.size(); ++i) {
        int sample = GetSample(actors, spin_count, getter, i);
        if (i) {
          Min = min(Min, sample);
          Max = max(Max, sample);
        } else {
          Min = sample;
          Max = sample;
        }
        total += sample;
      }
      double denom = static_cast<double>(actors.size());
      double mean = total / denom;
      for (size_t i = 0; i < actors.size(); ++i) {
        double sample = GetSample(actors, spin_count, getter, i);
        double diff = sample - mean;
        Dev += diff * diff;
      }
      Dev = sqrt(Dev / denom);
      Mean = static_cast<int>(mean + 0.5);
    }
  }

  void Write(ostream &strm) const {
    assert(this);
    if (IsSingle) {
      strm << setw(4) << Mean;
    } else {
      strm << "min = " << setw(5) << Min
           << ", max = " << setw(5) << Max
           << ", mean = " << setw(5) << Mean
           << ", dev = " << Dev;
    }
  }

  int GetSample(const vector<TAnyActor *> &actors, int spin_count, double (TAnyActor::*getter)() const, size_t i) {
    return static_cast<int>(((actors[i]->*getter)() * 1000000000.0 / static_cast<double>(spin_count)) + 0.5);
  }

  private:

  bool IsSingle;

  int Min, Max, Mean;

  double Dev;

};  // TStats

ostream &operator<<(ostream &strm, const TStats &that) {
  assert(&that);
  that.Write(strm);
  return strm;
}

static void Run(const TAnyFactory *factory, int actor_count, int spin_count) {
  assert(factory);
  cout
      << "kind = " << factory->GetKind()
      << ", is_reentrant = " << (factory->GetIsReentrant() ? "true" : "false")
      << ", actor_count = " << actor_count
      << ", spin_count = " << spin_count
      << endl;
  vector<TAnyActor *> actors(actor_count);
  for (int i = 0; i < actor_count; ++i) {
    actors[i] = factory->NewActor(spin_count);
  }
  TAnyActor::Go();
  for (int i = 0; i < actor_count; ++i) {
    actors[i]->Join();
  }
  cout
    << "acquire: " << TStats(actors, spin_count, &TAnyActor::GetAcquireTime) << endl
    << "release: " << TStats(actors, spin_count, &TAnyActor::GetReleaseTime) << endl
    << "  total: " << TStats(actors, spin_count, &TAnyActor::GetTotalTime) << endl;
  for (int i = 0; i < actor_count; ++i) {
    delete actors[i];
  }
}

static void ParseCmdLine(
    int argc, char *argv[],
    int &is_comparison, const TAnyFactory *&factory, int &actor_count, int &spin_count) {
  is_comparison = false;
  const char *kind = 0;
  int is_reentrant = false;
  actor_count = 1;
  spin_count = 100000;
  option opts[] = {
    {"comparison", false, &is_comparison, true},
    {"kind", true, 0, 'k'},
    {"reentrant", false, &is_reentrant, true},
    {"actor_count", true, 0, 'a'},
    {"spin_count", true, 0, 's'},
    {0, 0, 0, 0}
  };
  for (;;) {
    int idx;
    int result = getopt_long_only(argc, argv, "", opts, &idx);
    if (result < 0) {
      break;
    }
    switch (result) {
      case 0: {
        break;
      }
      case 'k': {
        if (!optarg) {
          cerr << argv[0] << ": missing argument for '--kind'" << endl;
          exit(EXIT_FAILURE);
        }
        kind = optarg;
        break;
      }
      case 'a': {
        if (!optarg) {
          cerr << argv[0] << ": missing argument for '--actor_count'" << endl;
          exit(EXIT_FAILURE);
        }
        actor_count = atoi(optarg);
        break;
      }
      case 's': {
        if (!optarg) {
          cerr << argv[0] << ": missing argument for '--spin_count'" << endl;
          exit(EXIT_FAILURE);
        }
        spin_count = atoi(optarg);
        break;
      }
      default: {
        exit(EXIT_FAILURE);
      }
    }
  }
  if (optind < argc) {
    cerr << argv[0] << ": extra argument(s)" << endl;
    exit(EXIT_FAILURE);
  }
  if (actor_count < 1 || actor_count > 100 || spin_count < 0 || spin_count > 10000000) {
    cerr << "error: value(s) out of range" << endl;
    exit(EXIT_FAILURE);
  }
  if (kind) {
    factory = TAnyFactory::TryFind(kind, is_reentrant);
    if (!factory) {
      cerr << "could not find factory" << endl;
      exit(EXIT_FAILURE);
    }
  } else {
    factory = TAnyFactory::GetFirst();
  }
  assert(factory);
}

int main(int argc, char *argv[]) {
  int is_comparison, actor_count, spin_count;
  const TAnyFactory *factory;
  ParseCmdLine(argc, argv, is_comparison, factory, actor_count, spin_count);
  if (is_comparison) {
    for (const TAnyFactory *factory = TAnyFactory::GetFirst(); factory; factory = factory->GetNext()) {
      Run(factory, actor_count, spin_count);
    }
  } else {
    Run(factory, actor_count, spin_count);
  }
  return EXIT_SUCCESS;
}
