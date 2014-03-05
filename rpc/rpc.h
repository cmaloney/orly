/* <rpc/rpc.h>

   Full-duplex remote procedure calls.

   OVERVIEW OF CLASSES

   Context (base class of shared reference types)
      * represents one end of an RPC connection
      * has a partner context on the other end of the connection
      * writes requests to its partner
      * receives requests and results from its partner
      * serves a single protocol
      * requests are dispatched to member functions
      * creates futures to receive the results of requests
      * keeps its binary I/O stream alive (shared pointer)
      * keeps pending futures alive (shared pointers)
      * is kept alive by pending requests (shared pointers)

   Future (base class with final template; shared reference types)
      * a value to be computed remotely
      * works like a smart pointer to the future type
      * explicit specialization for void
      * synchronizes automatically at dereference time
      * caller can wait for one or many futures
      * belongs to and kept alive by its context (shared pointer)

   Request (base class with final template; shared reference types)
      * a single remote procedure call with parameters
      * represented by a unique number assigned by the caller
      * belongs to and keeps alive its context (shared pointer)

   Entry (base class with final template; constant reference types)
      * an API entry point
      * part of a protocol
      * handled by a member function of a context
      * represented by a unique number assigned by the protocol

   Protocol (base class of constant reference type)
      * an API served by a context
      * maps unique numbers to entries
      * to be constructed as a constant in the data segment

   EXAMPLE

      class TMathService
          : public Rpc::TContext {
        NO_COPY_SEMANTICS(TMathService);
        public:

        static const Rpc::TEntryId AddId = 1001;

        TMathService(const std::shared_ptr<Io::TBinaryIoStream> &strm)
            : Rpc::TContext(TProtocol::Protocol) {
          BinaryIoStream = strm;
        }

        private:

        int Add(int a, int b) {
          return a + b;
        }

        class TProtocol
            : public Rpc::TProtocol {
          NO_COPY_SEMANTICS(TProtocol);
          public:

          static const TProtocol Protocol;

          private:

          TProtocol() {
            Register(AddId, &TMathService::Add);
          }

        };  // TProtocol

      };  // TMathService

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
#include <algorithm>
#include <cstdint>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>

#include <sys/eventfd.h>

#include <base/fd.h>
#include <base/no_copy_semantics.h>
#include <io/binary_io_stream.h>
#include <rpc/unpack.h>

namespace Rpc {

  /* Forward declarations, in the order in which they will be defined. */
  class TContext;
  class TAnyFuture;
  template <typename TVar>
  class TFuture;
  class TAnyRequest;
  template <typename TSomeContext, typename TRet, typename... TArgs>
  class TRequest;
  class TAnyEntry;
  template <typename TSomeContext, typename TRet, typename... TArgs>
  class TEntry;
  class TProtocol;

  /* A number used to identify a specific API entry point. */
  typedef uint16_t TEntryId;

  /* A number used to identify a specific instance of RPC. */
  typedef uint32_t TRequestId;

  /* These values are used to distinguish the kinds of messages. */
  static const char
      ErrorResultIntroducer  = '!',
      NormalResultIntroducer = '.',
      RequestIntroducer      = '?';

  /*
   *  Context
   */

  /* A context from which to send and into which to receive RPC requests and results.
     This is a base class of shared reference types. */
  class TContext
      : public std::enable_shared_from_this<TContext> {
    NO_COPY_SEMANTICS(TContext);
    public:

    /* Thrown when we read a message containing a result that we weren't expecting. */
    class TUnexpectedResult
        : public std::runtime_error {
      public:

      /* Do-little. */
      TUnexpectedResult();

    };  // TUnexpectedResult

    /* Thrown when we read a message that does not conform to our syntax. */
    class TSyntaxError
        : public std::runtime_error {
      public:

      /* Do-little. */
      TSyntaxError();

    };  // TSyntaxError

    /* Do-little. */
    virtual ~TContext();

    /* The binary I/O stream over which we communicate with our partner context. */
    Io::TBinaryIoStream &GetBinaryIoStream() const {
      assert(this);
      assert(BinaryIoStream);
      return *BinaryIoStream;
    }

    /* The protocol we serve. */
    const TProtocol &GetProtocol() const {
      assert(this);
      return Protocol;
    }

    /* True iff. all requests returned by Read() have been handled and we are not waiting for any replies. */
    bool IsIdle() {
      assert(this);
      std::lock_guard<std::mutex> lock(FutureByRequestIdMutex);
      return FutureByRequestId.empty() && UnhandledRequestCount == 0;
    }

    /* Read the next message from our partner.
       If it is a result, forward it to the appropriate future and return null.
       If it is a request, return it.
       It is an error to call this function when we do not have a connection. */
    std::shared_ptr<const TAnyRequest> Read();

    /* Write a request to our partner and return a future which which to access the result.
       It is an error to call this function when we do not have a connection. */
    template <typename TRet, typename... TArgs>
    std::shared_ptr<TFuture<TRet>> Write(TEntryId entry_id, TArgs &&... args);

    protected:

    /* Cache the reference to our protocol and the shared pointer to our connection.
       The connection pointer may be null. */
    TContext(const TProtocol &protocol)
        : Protocol(protocol), NextRequestId(1), UnhandledRequestCount(0) {}

    /* TODO */
    void FailAllFutures(const std::string &error_msg);

    /* See accessor. */
    std::shared_ptr<Io::TBinaryIoStream> BinaryIoStream;

    private:

    /* Find the future associated with the request id, erase it from our map, and return it.
       If there is no such future, throw TUnexpectedResult. */
    std::shared_ptr<TAnyFuture> PopFuture(TRequestId request_id);

    /* See accessor. */
    const TProtocol &Protocol;

    /* Covers NextRequestId and FutureByRequestId. */
    std::mutex FutureByRequestIdMutex;

    /* The id we will use the next time we write a request to our partner. */
    TRequestId NextRequestId;

    /* Each id in this map is a request for which we expect to receive a result.
       The associated future will accept that result when it arrives. */
    std::unordered_map<TRequestId, std::shared_ptr<TAnyFuture>> FutureByRequestId;

    /* The number of requests returned by Read() which have not yet been handled. */
    std::atomic_size_t UnhandledRequestCount;

    /* A lock to prevent multiple writers to the outstream. */
    std::recursive_mutex WriteLock;

    /* For access to WriteLock. */
    friend class TAnyRequest;

  };  // TContext

  /*
   *  Future
   */

  /* The base class of all futures, finalized by TFuture<>. */
  class TAnyFuture {
    NO_COPY_SEMANTICS(TAnyFuture);
    public:

    /* Thrown when an error arises in our partner while servicing a request. */
    class TRemoteError
        : public std::runtime_error {
      public:

      /* Do-little. */
      TRemoteError(const std::string &error_msg);

    };  // TRemoteError

    /* Do-little. */
    virtual ~TAnyFuture();

    /* Returns true iff. we have received our result. */
    operator bool() const;

    /* Wait for this fd to become readable if you want to synchronize against this future manually.
       Do NOT close or alter this fd. */
    const Base::TFd &GetEventFd() const {
      assert(this);
      return EventFd;
    }

    /* Wait for this future to be available.
       If the future contains an error, throw TRemoteError.
       It is safe to call this function from any thread. */
    void Sync() const;

    protected:

    /* Do we have a result yet? */
    enum TResultStatus {

      /* No, still waiting. */
      NoResult,

      /* Yes, got a value. */
      NormalResult,

      /* Yes, got an error. */
      ErrorResult

    };  // TResultStatus

    /* Do-little. */
    TAnyFuture();

    /* Change ResultStatus from NoResult -> ErrorResult and move the given error message into cache. */
    void SetErrorResult(std::string &error_msg);

    /* Change ResultStatus from NoResult -> NormalResult and read the value from the given stream. */
    virtual void SetNormalResult(Io::TBinaryInputStream &strm) = 0;

    /* Change ResultStatus from NoResult -> result_status.
       Called by SetErrorResult() and SetNormalResult().
       If is an error to call this function if ResultStatus != NoResult. */
    void SetResultStatus(TResultStatus result_status);

    private:

    /* See accessor. */
    Base::TFd EventFd;

    /* See TResultStatus.  This is atomic */
    std::atomic<int> ResultStatus;

    /* The error message stored by SetErroResult().
       This is an empty string if ResultStatus != ErrorResult. */
    std::string ErrorMsg;

    /* For SetErrorResult() and SetNormalResult(). */
    friend class TContext;

  };  // TAnyFuture

  /* A value being returned by RPC.
     This is a shared reference type. */
  template <typename TVal>
  class TFuture
      : public TAnyFuture {
    NO_COPY_SEMANTICS(TFuture);
    public:

    /* Do-little. */
    TFuture() {}

    /* The value returned by the server.
       If the server hasn't returned a value yet, wait for it. */
    const TVal operator*() const {
      assert(this);
      Sync();
      return Val;
    }

    /* The value returned by the server.
       If the server hasn't returned a value yet, wait for it. */
    const TVal *operator->() const {
      assert(this);
      Sync();
      return &Val;
    }

    private:

    /* See base class. */
    virtual void SetNormalResult(Io::TBinaryInputStream &strm) {
      assert(this);
      assert(&strm);
      strm >> Val;
      SetResultStatus(NormalResult);
    }

    /* See Get(). */
    TVal Val;

  };  // TFuture<TVal>

  /* Explicit specialization to handle void returns.
     This is the same as the general case, but lacks the Get() function. */
  template <>
  class TFuture<void>
      : public TAnyFuture {
    NO_COPY_SEMANTICS(TFuture);
    public:

    /* Do-little. */
    TFuture() {}

    private:

    /* See base class. */
    virtual void SetNormalResult(Io::TBinaryInputStream &) {
      assert(this);
      SetResultStatus(NormalResult);
    }

  };  // TFuture<void>

  /* See declaration. */
  template <typename TRet, typename... TArgs>
  std::shared_ptr<TFuture<TRet>> TContext::Write(TEntryId entry_id, TArgs &&... args) {
    assert(this);
    Io::TBinaryIoStream &strm = GetBinaryIoStream();
    std::pair<TRequestId, std::shared_ptr<TFuture<TRet>>> item;
    item.second = std::make_shared<TFuture<TRet>>();
    assert(item.second);
    /* extra */ {
      std::lock_guard<std::mutex> lock(FutureByRequestIdMutex);
      item.first = NextRequestId++;
      bool is_unique = FutureByRequestId.insert(item).second;
      assert(item.second);
      assert(is_unique);
    }
    assert(item.second);
    /* extra */ {
      std::lock_guard<std::recursive_mutex> lock(WriteLock);
      strm << RequestIntroducer << item.first << entry_id << std::forward_as_tuple(args...);
      strm.Flush();
    }
    assert(item.second);
    return item.second;
  }

  /*
   *  Request
   */

  /* The base class of all futures, finalized by TRequest<>. */
  class TAnyRequest {
    NO_COPY_SEMANTICS(TAnyRequest);
    public:

    /* Do-little. */
    virtual ~TAnyRequest();

    /* Serve the request. */
    virtual void operator()() const = 0;

    /* The id of this request. */
    TRequestId GetId() const {
      assert(this);
      return Id;
    }

    protected:

    /* Do-little. */
    TAnyRequest(TRequestId id)
        : Id(id) {}

    /* Write an error reply to the given stream such that it can be parsed by TMessageHandler::ReadMessage(). */
    static void WriteError(Io::TBinaryOutputStream &strm, TRequestId request_id, const std::exception &ex);

    /* Write a non-void, non-error reply to the given stream such that it can be parsed by TMessageHandler::ReadMessage(). */
    template <typename TSomeContext, typename TRet, typename... TArgs>
    static void WriteReply(
        Io::TBinaryOutputStream &strm, TRequestId request_id, TSomeContext *context,
        TRet (TSomeContext::*handler)(typename Pass<TArgs>::type...), const std::tuple<TArgs...> &args) {
      assert(&strm);
      assert(context);
      assert(&args);
      TRet ret;
      try {
        ret = Unpack(handler, context, args);
      } catch (const std::exception &ex) {
        std::lock_guard<std::recursive_mutex> lock(context->WriteLock);
        WriteError(strm, request_id, ex);
        --(context->UnhandledRequestCount);
        return;
      }
      std::lock_guard<std::recursive_mutex> lock(context->WriteLock);
      strm << NormalResultIntroducer << request_id << ret;
      strm.Flush();
      --(context->UnhandledRequestCount);
    }

    /* Write a void, non-error reply to the given stream such that it can be parsed by TMessageHandler::ReadMessage(). */
    template <typename TSomeContext, typename... TArgs>
    static void WriteReply(
        Io::TBinaryOutputStream &strm, TRequestId request_id, TSomeContext *context,
        void (TSomeContext::*handler)(typename Pass<TArgs>::type...), const std::tuple<TArgs...> &args) {
      assert(&strm);
      assert(context);
      assert(&args);
      try {
        Unpack(handler, context, args);
      } catch (const std::exception &ex) {
        std::lock_guard<std::recursive_mutex> lock(context->WriteLock);
        WriteError(strm, request_id, ex);
        --(context->UnhandledRequestCount);
        return;
      }
      std::lock_guard<std::recursive_mutex> lock(context->WriteLock);
      strm << NormalResultIntroducer << request_id;
      strm.Flush();
      --(context->UnhandledRequestCount);
    }

    private:

    /* See accessor. */
    TRequestId Id;

  };  // TAnyRequest

  /* An RPC request to be served.
     This is a shared reference type. */
  template <typename TSomeContext, typename TRet, typename... TArgs>
  class TRequest
      : public TAnyRequest {
    NO_COPY_SEMANTICS(TRequest);
    public:

    /* The type of the handler function in TSomeContext which we will call. */
    typedef TRet (TSomeContext::*THandler)(typename Pass<TArgs>::type...);

    /* Caches the given values and reads the request_factory's arguments from the stream. */
    TRequest(TRequestId id, const std::shared_ptr<TSomeContext> &context, THandler handler)
        : TAnyRequest(id), Context(context), Handler(handler) {
      assert(context);
      context->GetBinaryIoStream() >> Args;
    }

    /* See base class. */
    virtual void operator()() const {
      assert(this);
      WriteReply(Context->GetBinaryIoStream(), GetId(), Context.get(), Handler, Args);
    }

    private:

    /* The context which will serve this request. */
    std::shared_ptr<TSomeContext> Context;

    /* The context's handler function. */
    THandler Handler;

    /* The arguments we will pass to the handler. */
    std::tuple<TArgs...> Args;

  };  // TRequest<TSomeContext, TRet, TArgs...>

  /*
   *  Entry
   */

  /* The base class of all entries, finalized by TEntry<>. */
  class TAnyEntry {
    NO_COPY_SEMANTICS(TAnyEntry);
    public:

    /* Do-little. */
    virtual ~TAnyEntry();

    /* Construct a new request for the given context.
       The request will have the given id and whatever arguments we read from the context's stream. */
    virtual std::shared_ptr<TAnyRequest> NewRequest(TRequestId request_id, const std::shared_ptr<TContext> &context) const = 0;

    protected:

    /* Do-little. */
    TAnyEntry() {}

  };  // TAnyEntry

  /* An entry in an API. */
  template <typename TSomeContext, typename TRet, typename... TArgs>
  class TEntry
      : public TAnyEntry {
    NO_COPY_SEMANTICS(TEntry);
    public:

    /* The type of request we manufacture. */
    typedef TRequest<TSomeContext, TRet, TArgs...> TManufacturedRequest;

    /* The type of the handler function called by the type of request we manufacture. */
    typedef typename TManufacturedRequest::THandler THandler;

    /* Do-little. */
    TEntry(THandler handler)
        : Handler(handler) {}

    /* See base class. */
    virtual std::shared_ptr<TAnyRequest> NewRequest(TRequestId request_id, const std::shared_ptr<TContext> &context) const {
      assert(this);
      return std::make_shared<TManufacturedRequest>(request_id, std::dynamic_pointer_cast<TSomeContext>(context), Handler);
    }

    private:

    /* The handler function of TSomeContext which we represent. */
    THandler Handler;

  };  // TEntry<TSomeContext, TRet, TArgs...>

  /*
   *  Protocol
   */

  /* A collection of API entries. */
  class TProtocol {
    NO_COPY_SEMANTICS(TProtocol);
    public:

    /* Thrown when FindEntry() can't find what you're looking for. */
    class TUnknownEntry
        : public std::runtime_error {
      public:

      /* Do-little. */
      TUnknownEntry();

    };  // TUnknownEntry

    /* Do-little. */
    virtual ~TProtocol();

    /* Find the entry for the given id.
       If there is no such entry, throw TUnknownEntry. */
    const TAnyEntry *FindEntry(TEntryId entry_id) const;

    protected:

    /* Do-little. */
    TProtocol() {}

    /* Include the given entry in this protocol. */
    template <typename TSomeContext, typename TRet, typename... TArgs>
    void Register(TEntryId entry_id, typename TEntry<TSomeContext, TRet, TArgs...>::THandler handler) {
      assert(this);
      auto entry = new TEntry<TSomeContext, TRet, TArgs...>(handler);
      try {
        bool is_unique = EntryById.insert(std::make_pair(entry_id, entry)).second;
        assert(is_unique);
      } catch (...) {
        delete entry;
        throw;
      }
    }

    private:

    /* A place to look up request_factories by their ids. */
    std::unordered_map<TEntryId, TAnyEntry *> EntryById;

  };  // TProtocol

}  // Rpc
