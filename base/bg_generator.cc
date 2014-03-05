/* <base/bg_generator.cc>

   Implements <base/bg_generator.h>.

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

#include <base/bg_generator.h>

#include <cstdlib>
#include <new>

#include <sys/resource.h>

#include <base/error_utils.h>

using namespace std;
using namespace Base;

TAnyBgGenerator::~TAnyBgGenerator() noexcept {
  assert(this);
  /* If this assertion fails, it means you didn't call FinalizeBg() in your
     destructor. */
  assert(State == Unstarted);
}

size_t TAnyBgGenerator::GetDefStackSize() {
  struct rlimit rlimit;
  IfLt0(getrlimit(RLIMIT_STACK, &rlimit));
  return rlimit.rlim_cur;
}

bool TAnyBgGenerator::BgYield() {
  assert(this);
  /* If this assertion fails, you're probably not calling this function from
     the bg. */
  assert(State == Started);
  State = Yielded;
  IfLt0(swapcontext(&BgContext, &FgContext));
  return KeepGoing;
}

void TAnyBgGenerator::FinalizeBg() noexcept {
  assert(this);
  /* Loop while we're not unstarted.  (That is, while we're maybe doing
     something in the bg.) */
  for (;;) {
    switch (State) {
      case Unstarted: {
        /* The bg isn't there.  We're safe to destroy now. */
        return;
      }
      case Started:
      case Yielded: {
        /* Switch to the bg one last time, letting it know (via the return of
           BgYield()) that it's time for it to go. */
        KeepGoing = false;
        if (swapcontext(&FgContext, &BgContext) < 0) {
          abort();
        }
        break;
      }
      case ExitedNormally:
      case ExitedOnException: {
        /* The bg has exited (with or without an exception) so we can free its
           stack and go back to our unstarted state. */
        free(BgContext.uc_stack.ss_sp);
        State = Unstarted;
        ExPtr = exception_ptr();
        break;
      }
    }  // switch
  }  // for
}

bool TAnyBgGenerator::HasBgYielded() const {
  assert(this);
  /* Loop until the bg yields or exits.  If this is our first time through,
     construct the bg context. */
  for (;;) {
    switch (State) {
      case Unstarted: {
        /* If our stack size is zero, it means we were constructed as a do-
           nothing generator, so it's time for us to do nothing.  We don't
           require a bg stack; just change state to indicate we're now done
           doing nothing. */
        if (!StackSize) {
          BgContext.uc_stack.ss_sp = nullptr;
          State = ExitedNormally;
          break;
        }
        /* We have a stack size so we are meant to do some work in the bg.
           Copy our current context as a starting point for the new bg context,
           make it fall-through to the fg context, and allocate a new stack
           for it. */
        IfLt0(getcontext(&BgContext));
        BgContext.uc_link = &FgContext;
        BgContext.uc_stack.ss_size = StackSize;
        BgContext.uc_stack.ss_sp = malloc(StackSize);
        if (!BgContext.uc_stack.ss_sp) {
          throw bad_alloc();
        }
        /* Modify the bg context to start at the springboard function with this
           generator as its single argument. */
        makecontext(
            &BgContext, reinterpret_cast<void (*)()>(BgSpringboard), 1, this);
        /* The bg is now ready to go. */
        State = Started;
        KeepGoing = true;
        break;
      }
      case Started: {
        /* Switch to the bg.  We will return here when the bg called BgYield()
           or falls through. */
        IfLt0(swapcontext(&FgContext, &BgContext));
        break;
      }
      case Yielded: {
        /* The bg yielded. */
        return true;
      }
      case ExitedNormally: {
        /* The bg fell through. */
        return false;
      }
      case ExitedOnException: {
        /* The bg threw an exception, which we will now rethrow. */
        rethrow_exception(ExPtr);
      }
    }  // switch
  }  // for
}

void TAnyBgGenerator::RequestBgYield() {
  assert(this);
  /* Make sure we've yielded something. */
  RequireBgHasYielded();
  /* And now require that we yield again. */
  State = Started;
}

void TAnyBgGenerator::RequireBgHasYielded() const {
  assert(this);
  if (!HasBgYielded()) {
    ThrowSystemError(ENODATA);
  }
}

void TAnyBgGenerator::Swap(TAnyBgGenerator &that) noexcept {
  assert(this);
  assert(&that);
  /* If this assertion fails, it means this or that generator has already
     started running and so cannot be swapped. */
  assert(State == Unstarted);
  assert(that.State == Unstarted);
  /* Swap all the things. */
  swap(StackSize, that.StackSize);
  swap(State,     that.State    );
  swap(KeepGoing, that.KeepGoing);
  swap(ExPtr,     that.ExPtr    );
  swap(BgContext, that.BgContext);
  swap(FgContext, that.FgContext);
  /* Fix-up the fall-through links. */
  BgContext.uc_link = &FgContext;
  that.BgContext.uc_link = &that.FgContext;
}

void TAnyBgGenerator::BgWrapper() noexcept {
  assert(this);
  assert(KeepGoing);
  try {
    /* Do the work. */
    BgMain();
    State = ExitedNormally;
  } catch (...) {
    /* Steal the exception so we can rethrow it in the fg. */
    ExPtr = current_exception();
    State = ExitedOnException;
  }
}

void TAnyBgGenerator::BgSpringboard(TAnyBgGenerator *generator) noexcept {
  generator->BgWrapper();
}
