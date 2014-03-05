/* <test/perf.h>

   Stig perf testing kit. Contains primitives for perf testing code, and a wrapper which gives us a nice environment
   for executing them.

   Usage

   #include <test/perf.h>

   //NOTE: FooBar will naturally stop timing when the scope is closed.
   //The counters / timing / etc. either must be "decreasing", "steady", or "increasing"
   PERF(Name, 140) { //Maximum run time, in seconds. After the run time runs out, the perf test suite will kill the perf test.
     TIMING(FooBar, Decrease)
     COUNTER(Counter, Steady)
     FooBar.start()

     for(int i=0; i < 10000000; ++i) {
       ++Counter;
     }
     FooBar.stop()
   }

   PERF(Sample2, MaxTime) {
     TIMING(FooBar, Decrease)
     COUNTER(Counter, Steady)
     FooBar.start()

     for(int i=0; i < 10000000; ++i) {
       ++Counter;
     }
     FooBar.stop()
   }

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

#include <atomic>
#include <chrono>
#include <iostream>

#include <base/no_copy_semantics.h>
#include <base/opt.h>
#include <inv_con/ordered_list.h>

#define PERF(name, max_runtime) \
  static void name##_perf(); \
  static Test::TPerf name##_Perf(#name, name##_perf, ::std::chrono::seconds(max_runtime)); \
  static void name##_perf()

#define METRIC(type, name, ...) \
  type name(#name, __VA_ARGS__)

#define TIMING(name, ...) \
  METRIC(::Test::TTimer, name, __VA_ARGS__)

#define COUNTER(name, ...) \
  METRIC(::Test::TCounter, name, __VA_ARGS__)

namespace Test {

  template<typename TFinal>
  class TMetricInfo {
    static const char *Name;
    static const char *Units;
  };

  /* A performance metric. This is the base class for all the different types of performance metrics and performs the
     basic book keeping. */
  class TMetric {
    NO_COPY_SEMANTICS(TMetric);

    public:
    /* The directions in which a metric can be expected to move. */
    enum TDirection {
      Decreasing, //Decreasing in the variable are better
      Steady,     //The metric should stay steady
      Increasing, //Increasing is better
    };

    /* Destructor. Before this is called, PreDtor MUST be called. This means that the destructors of leaf classes of the
       TMetric class hierarchy MUST call PreDtor. */
    virtual ~TMetric();

    protected:

    /* Prints out the value of the metric in the perf test metric format */
    template <typename TMetricVal>
    void PreDtor(const TMetricVal &val) {
      assert(~PreDtorCalled);
      PreDtorCalled = true;

      std::cout << "    \"" << Name << "\": {" << std::endl
        << "      \"type\": \"" << TypeName << "\"," << std::endl
          << "      \"directon\": \"";
      switch (Direction) {
        case Increasing:
          std::cout << '+';
          break;
        case Steady:
          std::cout << '=';
          break;
        case Decreasing:
          std::cout << '-';
          break;
      }
      //TODO: Val needs to be guaranteed to be jsonified.
      std::cout << "\"," << std::endl
          << "      \"val\": " << val << std::endl
          << "    }" << std::endl;
    }

    TMetric(const char *type_name, const char *name, TDirection direction);

    const char *GetTypeName() const;
    const char *GetName() const;
    TDirection GetDirection() const;

    private:
    TDirection Direction;
    const char *Name;
    const char *TypeName;
    bool PreDtorCalled;
  };

  template<typename TVal = int64_t>
  class TGenericCounter : public TMetric {
    NO_COPY_SEMANTICS(TGenericCounter);
    public:
    TGenericCounter(const char *name, TMetric::TDirection direction)
        : TMetric("counter", name, direction), Val(0) {}

    /* See ~TMetric() */
    ~TGenericCounter() {
      PreDtor(Val.load());
    }

    /* Increment the counter */
    TGenericCounter &operator++() {
      assert(this);
      ++Val;
      return *this;
    }

    /* Decrement the counter */
    TGenericCounter &operator--() {
      assert(this);
      --Val;
      return *this;
    }

    /* Reset the counter to zero. */
    void Reset() {
      assert(this);
      Val = 0;
    }

    private:
    //TODO: Add introspection functions
    std::atomic<TVal> Val;
  };

  typedef TGenericCounter<> TCounter;

  template<typename TClock_ = std::chrono::high_resolution_clock>
  class TGenericTimer : public TMetric {
    NO_COPY_SEMANTICS(TGenericTimer);
    public:
    typedef TClock_ TClock;

    TGenericTimer(const char *name, TMetric::TDirection direction) : TMetric("timer", name, direction) {}

    /* Stops the timer if it is still running. Also provides functionality mandated by ~TMetric. */
    ~TGenericTimer() {
      if(StartTime) {
        if(!StopTime) {
          StopTime = TClock::now();
        }
        PreDtor(StopTime - StartTime);
      } else {
        PreDtor("unstarted");
      }
    }

    void Start() {
      assert(this);
      assert(!StartTime);
      StartTime = TClock::now();
    }

    void Stop() {
      assert(this);
      assert(StartTime);
      assert(!StopTime);
      StopTime = TClock::now();
    }

    const Base::TOpt<typename TClock::time_point> &GetStart() const {
      assert(this);
      return StartTime;
    }

    typename TClock::duration GetElapsed() const {
      assert(this);
      assert(StartTime && StopTime);
      return *StopTime - *StartTime;
    }

    Base::TOpt<typename TClock::time_point> GetStop() const {
      assert(this);
      return StopTime;
    }

    bool IsStarted() const {
      return StartTime;
    }

    bool HasFinished() const {
      return StopTime;
    }

    private:
    Base::TOpt<typename TClock::time_point> StartTime, StopTime;
  };

  typedef TGenericTimer<> TTimer;

  class TPerf {
    NO_COPY_SEMANTICS(TPerf);
    public:

    typedef void (*TFunc)();

    TPerf(const char *name, TFunc func, std::chrono::seconds max_runtime);

    //TODO: Should probably do proper cleanup of the TPerf linked list here...
    //~TPerf();

    //TODO: Describe should enumerate all metrics
    //void Describe(std::ostream &out) const;

    TFunc GetFunc() const;

    std::chrono::seconds GetMaxRuntime() const;

    const char *GetName() const;

    const TPerf *GetNext() const;

    static const TPerf *GetFirst();

    private:

    TFunc Func;

    std::chrono::seconds MaxRuntime;

    const char *Name;

    mutable TPerf *Next;

    static TPerf *First;
    static TPerf *Last;
  };
}