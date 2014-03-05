/* <base/bg_generator.h>

   Generate values on a background stack without launching a thread and use
   them on your background stack as though they were being read from a
   container.

   The values must be produced by a generating function.  The generator object
   runs that function on a background (bg) stack, switching control to it when
   values are needed.  To the foreground (fg) thread, the generator looks like
   an iterator.

   For example:

      // This runs in the bg.
      bool Generate(const function<bool (const string &)> &cb) {
        return cb("hello") && cb("doctor") && cb("name");
      }

      // This runs in the fg.
      void PrintStrings() {
        for (TBgGenerator<string>(bind(Generate, _1)) gen; gen; ++gen) {
          cout << *gen << endl;
        }
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

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <exception>
#include <utility>

#include <ucontext.h>

#include <base/no_copy_semantics.h>

namespace Base {

  /* The base class for all bg generators. */
  class TAnyBgGenerator {
    NO_COPY_SEMANTICS(TAnyBgGenerator);
    public:

    /* You MUST call FinalizeBg() in the destructor of your final class.
       Failure to do so will cause the contents of the bg stack to leak.
       There is an assertion against this in the debug build. */
    virtual ~TAnyBgGenerator() noexcept;

    /* The default stack size to use for the bg.  We get this by calling
       rlimit(), so it's the default for the process. */
    static size_t GetDefStackSize();

    protected:

    /* Constructs a do-nothing generator with no stack.  This is very cheap to
       call. */
    TAnyBgGenerator() noexcept
        : StackSize(0), State(Unstarted) {}

    /* Creates a bg stack and prepares to call BgMain() on it, but doesn't
       actually do any bg work until you call HasBgYielded() or
       RequireBgHasYielded().  If you give a stack size of 0, the stack will
       be whatever size GetDefStackSize() says it should be. */
    TAnyBgGenerator(size_t stack_size)
        : StackSize(stack_size ? stack_size : GetDefStackSize()),
          State(Unstarted) {}

    /* Override this to do the work of the bg.  Do one iteration of your task,
       then call BgYield().  This will switch control back to the fg.  If
       BgYield() returns true, proceed to the next iteration; otherwise, fall
       through as fast as you can.  You should also fall through quickly When
       you have no more iterations to do.  It's also ok for this function to
       throw.  In that case, the exception will jump to the fg and be rethrown
       by HasBgYielded(). */
    virtual void BgMain() = 0;

    /* Call this in the bg when you have readied a value for the fg.  If it
       returns true, you should keep going; otherwise, you should fall through
       as quickly as you can. */
    bool BgYield();

    /* Called in the destructor of your final class.  If the bg is has not yet
       exited (normally or otherwise), this function will transfer control to
       it one last time.  If the bg is stopped at BgYield(), that function will
       return false, indicating that the bg should exit.  If hasn't started yet,
       this function does nothing. */
    void FinalizeBg() noexcept;

    /* Runs an iteration of the bg, if necessary, and returns true if the bg
       called BgYield() or false if the bg exited normally.  If the bg throws
       an exception, this function rethrows it.  Calling this function again
       without first calling RequestBgYield() will not run a bg iteration, it
       will just repeat the result of the last call to this function.  You
       must call RequestBgYield() before HasBgYielded() will cause another bg
       iteration. */
    bool HasBgYielded() const;

    /* Request the bg to run again.  If you have not called HasBgYielded() since
       the last time you called RequestBgYield(), the bg will run now;
       otherwise, the bg will run the next time you call HasBgYielded(). */
    void RequestBgYield();

    /* Calls HasBgYielded() and throws ENODATA if it returns false. */
    void RequireBgHasYielded() const;

    /* Swap this generator with that one.  NOTE: It's an error to do this if
       either generator has already started running. */
    void Swap(TAnyBgGenerator &that) noexcept;

    private:

    /* The state of the bg. */
    enum TState {

      /* The bg has not yet entered, which means we were recently constructed
         and the fg has not yet called HasBgYielded(). */
      Unstarted,

      /* The bg has started and we need to transfer control to it again.  We
         will do so the next time the fg calls HasBgYielded(). */
      Started,

      /* The bg has called Yield() and is stopped there.  This means the fg can
         proceed, so HasBgYielded() will return true. */
      Yielded,

      /* The bg has exited by falling through.  This means the fg should stop
         using this generator, so HasBgYielded() will return false. */
      ExitedNormally,

      /* The bg has exited by throwing an exception.  HasBgYielded() will
         rethrow the exception.  */
      ExitedOnException,

    };  // TAnyBgGenerator::TState

    /* The entry point of the bg.  This wraps BgMain(), catching any error it
       throws, and setting the final bg state appropriately. */
    void BgWrapper() noexcept;

    /* Just transfers control the BgWrapper().  This is necessary because the
       makecontext() call can't handle member functions.  (Oh well, one extra
       stack frame never killed anybody, right?) */
    static void BgSpringboard(TAnyBgGenerator *generator) noexcept;

    /* Cached at construction time.  Only zero if we were default-constructed
       as a do-nothing generator.  In any other case, the constructor will
       either cache the requested stack size or the value returned by
       GetDefStackSize(). */
    size_t StackSize;

    /* See TState, above. */
    mutable TState State;

    /* The value returned by BgYield().  This is set to true at construction
       and remains true until FinalizeBg() is called. */
    mutable bool KeepGoing;

    /* A pointer to the exception thrown by BgMain(), if any.  This is rethrown
       by HasBgYielded(). */
    mutable std::exception_ptr ExPtr;

    /* The context objects used to switch between fg and bg.  The bg context
       starts out at BgSpringboard().  HasBgYielded() and FinalizeBg()
       swap from fg to bg, and BgYield() swaps back.  When BgMain() falls
       through, it also swaps back to the fg context. */
    mutable ucontext_t BgContext, FgContext;

  };  // TAnyBgGenerator

  /* Takes a value-generating function and runs it on a bg stack, making the
     generated values available one at a time. */
  template <typename TVal>
  class TBgGenerator final
      : public TAnyBgGenerator {
    NO_COPY_SEMANTICS(TBgGenerator);
    public:

    /* A function which uses a generated value and returns true iff. it wants
       another one. */
    using TUse = std::function<bool (const TVal &)>;

    /* A function which sends values to a user until it either runs out of
       values or the user returns false.  This function should return false
       itself only if the user has done so.  If it runs out of values, it
       should return true. */
    using TGenerate = std::function<bool (const TUse &)>;

    /* Constructs a do-nothing generator with no stack.  This is very cheap to
       call. */
    TBgGenerator() noexcept
        : Cache(nullptr) {}

    /* Takes ownership of the function that generates values and creates a bg
       stack on which to run it.  If you give a stack size of 0, the stack
       will be whatever size GetDefStackSize() says it should be. */
    template <typename TArg>
    TBgGenerator(TArg &&arg, size_t stack_size = 0)
        : TAnyBgGenerator(stack_size),
          Cache(nullptr), Generate(std::forward<TArg>(arg)) {}

    /* Move-construct, leaving the donor as if it were default-constructed. */
    TBgGenerator(TBgGenerator &&that) {
      Swap(that);
    }

    /* Shuts down the bg, if it hasn't already finished. */
    virtual ~TBgGenerator() noexcept {
      assert(this);
      FinalizeBg();
    }

    /* Move-construct, leaving the donor as if it were default-constructed. */
    TBgGenerator &operator=(TBgGenerator &&that) {
      assert(this);
      TBgGenerator temp(std::move(that));
      return Swap(temp);
    }

    /* True if the bg has yielded a value, false if it has exited normally.  If
       has thrown an exception, this function rethrows it. */
    operator bool() const {
      assert(this);
      return HasBgYielded();
    }

    /* A reference to the value the bg has yielded.  If the bg has exited
       normally, this throws ENODATA.  If the bg has thrown an exception, this
       function rethrows it. */
    const TVal &operator*() const {
      assert(this);
      RequireBgHasYielded();
      assert(Cache);
      return *Cache;
    }

    /* A pointer to the value the bg has yielded.  If the bg has exited
       normally, this throws ENODATA.  If the bg has thrown an exception, this
       function rethrows it. */
    const TVal *operator->() const {
      assert(this);
      RequireBgHasYielded();
      assert(Cache);
      return Cache;
    }

    /* Advances to the next value yielded by the bg, or allows the bg to exit
       normally.  If the bg has already already exited normally, this function
       throws ENODATA.  If the bg has thrown an exception, this function
       rethrows it. */
    TBgGenerator &operator++() {
      assert(this);
      RequestBgYield();
      Cache = nullptr;
      return *this;
    }

    /* Swap this generator with that one.  NOTE: It's an error to do this if
       either generator has already started running. */
    TBgGenerator &Swap(TBgGenerator &that) noexcept {
      assert(this);
      assert(&that);
      TAnyBgGenerator::Swap(that);
      std::swap(Cache, that.Cache);
      std::swap(Generate, that.Generate);
      return *this;
    }

    private:

    /* Calls the generating function and yields each value generated. */
    virtual void BgMain() override {
      assert(this);
      /* For each value generated, cache a pointer to it and yield to the fg.
         Keep going until we're out of values or until the fg tells us to stop.
         Between yields, the cache pointer should have been cleared by
         operator++(). */
      Generate(
          [this](const TVal &val) {
            assert(&val);
            assert(!Cache);
            Cache = &val;
            return BgYield();
          }
      );
    }

    /* A pointer to the value last sent back to us by Generate.  When Generate
       isn't running, this will be null.  This will also be null after a call
       to operator++(). */
    const TVal *Cache;

    /* The function which generates values for us in the bg. */
    TGenerate Generate;

  };  // TBgGenerator<TVal>

}  // Base
