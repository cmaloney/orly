/* <base/latch.h>

   Syncrhonously pass a request and reply between threads.

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

#include <base/event_semaphore.h>
#include <base/opt.h>

namespace Base {

  /* The base for all latches. */
  class TAnyLatch {
    NO_COPY_SEMANTICS(TAnyLatch);
    public:

    /* Do-little. */
    virtual ~TAnyLatch() {}

    /* An fd that signals when the reply is ready. */
    const TFd &GetReplyIsReadyFd() const {
      assert(this);
      return ReplyIsReady.GetFd();
    }

    /* An fd that signals when a request is ready. */
    const TFd &GetRequestIsReadyFd() const {
      assert(this);
      return RequestIsReady.GetFd();
    }

    protected:

    /* Do-little. */
    TAnyLatch() {}

    /* Wait for the reply to be ready. */
    void PopReply() {
      assert(this);
      ReplyIsReady.Pop();
    }

    /* Wait for the request to be ready. */
    void PopRequest() {
      assert(this);
      RequestIsReady.Pop();
    }

    /* Signal that the reply is ready. */
    void PushReply() {
      assert(!ReplyIsReady.GetFd().IsReadable());
      ReplyIsReady.Push();
    }

    /* Signal that the request is ready. */
    void PushRequest() {
      assert(!RequestIsReady.GetFd().IsReadable());
      RequestIsReady.Push();
    }

    private:

    /* Signal when the request and reply are ready. */
    TEventSemaphore RequestIsReady, ReplyIsReady;

  };  // TAnyLatch

  /* The request-half of a latch. */
  template <typename TRequest>
  class TRequestLatch
      : public virtual TAnyLatch {
    public:

    /* Wait for the request. */
    void PopRequest(TRequest &request) {
      assert(this);
      TAnyLatch::PopRequest();
      Request.Release(request);
    }

    /* Push the request. */
    template <typename... TArgs>
    void PushRequest(TArgs &&... args) {
      assert(this);
      Request.MakeKnown(std::forward<TArgs>(args)...);
      TAnyLatch::PushRequest();
    }

    protected:

    /* Do-little. */
    TRequestLatch() {}

    private:

    /* The current request.  Only known after PushRequest() and before PopRequest(). */
    TOpt<TRequest> Request;

  };  // TRequestLatch<TRequest>

  /* The request-half of a latch with a void request. */
  template <>
  class TRequestLatch<void>
      : public virtual TAnyLatch {
    public:

    /* Wait for the request. */
    using TAnyLatch::PopRequest;

    /* Push the request. */
    using TAnyLatch::PushRequest;

    protected:

    /* Do-little. */
    TRequestLatch() {}

  };  // TRequestLatch<void>

  /* The reply-half of a latch. */
  template <typename TReply>
  class TReplyLatch
      : public virtual TAnyLatch {
    public:

    /* Wait for the reply. */
    void PopReply(TReply &reply) {
      assert(this);
      TAnyLatch::PopReply();
      Reply.Release(reply);
    }

    /* Push the reply. */
    template <typename... TArgs>
    void PushReply(TArgs &&... args) {
      assert(this);
      Reply.MakeKnown(std::forward<TArgs>(args)...);
      TAnyLatch::PushReply();
    }

    protected:

    /* Do-little. */
    TReplyLatch() {}

    private:

    /* The current reply.  Only known after PushReply() and before PopReply(). */
    TOpt<TReply> Reply;

  };  // TReplyLatch<TReply>

  /* The reply-half of a latch with a void reply. */
  template <>
  class TReplyLatch<void>
      : public virtual TAnyLatch {
    public:

    /* Wait for the reply. */
    using TAnyLatch::PopReply;

    /* Push the reply. */
    using TAnyLatch::PushReply;

    protected:

    /* Do-little. */
    TReplyLatch() {}

  };  // TReplyLatch<void>

  /* Syncrhonously pass a request and reply between threads. */
  template <typename TRequest, typename TReply>
  class TLatch final
      : public TRequestLatch<TRequest>, public TReplyLatch<TReply> {
    NO_COPY_SEMANTICS(TLatch);
    public:

    /* Do-little. */
    TLatch() {}

    /* Wait for a request and push the the reply. */
    template <typename... TArgs>
    void TransactReply(TRequest &request, TArgs &&... args) {
      assert(this);
      TRequestLatch<TRequest>::PopRequest(request);
      TReplyLatch<TReply>::PushReply(std::forward<TArgs>(args)...);
    }

    /* Push a request and wait for the reply. */
    template <typename... TArgs>
    void TransactRequest(TReply &reply, TArgs &&... args) {
      assert(this);
      TRequestLatch<TRequest>::PushRequest(std::forward<TArgs>(args)...);
      TReplyLatch<TReply>::PopReply(reply);
    }

  };  // TLatch<TRequest, TReply>

  /* Syncrhonously pass a request between threads. */
  template <typename TRequest>
  class TLatch<TRequest, void> final
      : public TRequestLatch<TRequest>, public TReplyLatch<void> {
    public:

    /* Do-little. */
    TLatch() {}

    /* Wait for a request and push the the reply. */
    void TransactReply(TRequest &request) {
      assert(this);
      TRequestLatch<TRequest>::PopRequest(request);
      TReplyLatch<void>::PushReply();
    }

    /* Push a request and wait for the reply. */
    template <typename... TArgs>
    void TransactRequest(TArgs &&... args) {
      assert(this);
      TRequestLatch<TRequest>::PushRequest(std::forward<TArgs>(args)...);
      TReplyLatch<void>::PopReply();
    }

  };  // TLatch<TRequest, void>

  /* Syncrhonously pass a reply between threads. */
  template <typename TReply>
  class TLatch<void, TReply> final
      : public TRequestLatch<void>, public TReplyLatch<TReply> {
    NO_COPY_SEMANTICS(TLatch);
    public:

    /* Do-little. */
    TLatch() {}

    /* Wait for a request and push the the reply. */
    template <typename... TArgs>
    void TransactReply(TArgs &&... args) {
      assert(this);
      TRequestLatch<void>::PopRequest();
      TReplyLatch<TReply>::PushReply(std::forward<TArgs>(args)...);
    }

    /* Push a request and wait for the reply. */
    void TransactRequest(TReply &reply) {
      assert(this);
      TRequestLatch<void>::PushRequest();
      TReplyLatch<TReply>::PopReply(reply);
    }

  };  // TLatch<void, TReply>

  /* Synchronize a pair of threads. */
  template <>
  class TLatch<void, void> final
      : public TRequestLatch<void>, public TReplyLatch<void> {
    NO_COPY_SEMANTICS(TLatch);
    public:

    /* Do-little. */
    TLatch() {}

    /* Wait for a request and push the the reply. */
    void TransactReply() {
      assert(this);
      TRequestLatch<void>::PopRequest();
      TReplyLatch<void>::PushReply();
    }

    /* Push a request and wait for the reply. */
    void TransactRequest() {
      assert(this);
      TRequestLatch<void>::PushRequest();
      TReplyLatch<void>::PopReply();
    }

  };  // TLatch<void, TReply>

}  // Base
