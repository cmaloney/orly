/* <multi_event/multi_event.h>

   Use condition variables to wait for multiple events.

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

#include <cassert>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

#include <base/no_copy_semantics.h>
#include <base/opt.h>

namespace MultiEvent {

  /* Forward declarations. */
  class TEvent;
  class TWaiter;

  /* A one-time event which can be waited for by TWaiter. */
  class TEvent {
    NO_COPY_SEMANTICS(TEvent);
    public:

    typedef std::shared_ptr<TEvent> TPtr;

    /* Wake up any waiters which might be waiting for us.
       Any waiter which waits on us in the future will unblock immediately. */
    void Fire();

    /* A new event. */
    static std::shared_ptr<TEvent> New() {
      std::shared_ptr<TEvent> result(new TEvent(), OnFinalRelease);
      result->Self = result;
      return result;
    }

    private:

    /* Do-little. */
    TEvent()
        : Flag(false) {}

    /* Do-little. */
    virtual ~TEvent();

    /* Delete an event when the last shared pointer releases it. */
    static void OnFinalRelease(TEvent *event) {
      delete event;
    }

    /* True iff. we've fired. */
    bool Flag;

    /* Used to construct shared pointers to this event. */
    std::weak_ptr<TEvent> Self;

    /* Covers Waiters. */
    std::mutex Mutex;

    /* The waiters who are currently waiting for us. */
    std::unordered_set<TWaiter *> Waiters;

    /* TEvent and TWaiter act as a pair. */
    friend class TWaiter;

  };  // TEvent

  /* Waits for one or more TEvents to fire. */
  class TWaiter {
    NO_COPY_SEMANTICS(TWaiter);
    public:

    /* A function which produces events. */
    typedef std::function<void (const std::shared_ptr<TEvent> &)> TConsumer;

    /* A function which consumes events. */
    typedef std::function<void (const TConsumer &)> TProducer;

    /* Do-little. */
    TWaiter() {}

    /* Wait for one or more events.
       Events come from the producer.
       If one or more events fire, wake up and send them the consumer one at a time.
       Return true if we woke up; false if we timed out. */
    bool Wait(const TProducer &producer, const TConsumer &consumer, const Base::TOpt<std::chrono::milliseconds> &timeout);

    /* Wait for one or more events.
       Events are the keys in a map from event to value.
       If one or more events fire, wake up send their associated values to the callback one at a time.
       Return true if we woke up; false if we timed out. */
    template <typename TVal>
    bool Wait(
        const std::unordered_map<std::shared_ptr<TEvent>, TVal> &val_by_event,
        const Base::TOpt<std::chrono::milliseconds> &timeout,
        const std::function<void (const TVal &)> &cb) {
      return Wait(Produce(val_by_event), Consume(val_by_event, cb), timeout);
    }

    /* Make a consumer which collects events into a set. */
    static TConsumer Consume(std::unordered_set<std::shared_ptr<TEvent>> &events) {
      assert(&events);
      return std::bind(
        [](const std::shared_ptr<TEvent> &event, std::unordered_set<std::shared_ptr<TEvent>> &events) {
          events.insert(event);
        },
        std::placeholders::_1,
        std::ref(events)
      );
    }

    /* Make a consumer which trampolines an event to a callback by looking up the event's value. */
    template <typename TVal>
    static TConsumer Consume(const std::unordered_map<std::shared_ptr<TEvent>, TVal> &val_by_event, const std::function<void (const TVal &)> &cb) {
      assert(&val_by_event);
      assert(&cb);
      return std::bind(
        [](const std::shared_ptr<TEvent> &event,
           const std::unordered_map<std::shared_ptr<TEvent>, TVal> &val_by_event,
           const std::function<void (const TVal &)> &cb) {
          cb(val_by_event.find(event)->second);
        },
        std::placeholders::_1,
        std::cref(val_by_event),
        std::cref(cb)
      );
    }

    /* Produce (one at a time) all the events in a set. */
    static TProducer Produce(const std::unordered_set<std::shared_ptr<TEvent>> &events) {
      assert(&events);
      return std::bind(
        [](const TConsumer &consumer, const std::unordered_set<std::shared_ptr<TEvent>> &events) {
          for (auto event: events) {
            consumer(event);
          }
        },
        std::placeholders::_1,
        std::cref(events)
      );
    }

    /* Produce (one at a time) all the events in a map which is keyed by events. */
    template <typename TVal>
    static TProducer Produce(const std::unordered_map<std::shared_ptr<TEvent>, TVal> &val_by_event) {
      assert(&val_by_event);
      return std::bind(
        [](const TConsumer &consumer, const std::unordered_map<std::shared_ptr<TEvent>, TVal> &val_by_event) {
          for (const std::pair<std::shared_ptr<TEvent>, TVal> &event_and_val: val_by_event) {
            consumer(event_and_val.first);
          }
        },
        std::placeholders::_1,
        std::cref(val_by_event)
      );
    }

    private:

    /* Called by TEvent::Fire(). */
    void Notify(const TEvent *event);

    /* Remove this waiter from its events. */
    void Reset();

    /* The events for which we're waiting. */
    std::unordered_set<std::shared_ptr<TEvent>> Events;

    /* Covers Flag. */
    std::mutex Mutex;

    /* Notified when Flag becomes true. */
    std::condition_variable FlagIsSet;

    /* True iff. one of our events has fired. */
    bool Flag;

    /* TEvent and TWaiter act as a pair. */
    friend class TEvent;

  };  // TWaiter

}  // MultiEvent
