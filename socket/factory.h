/* <socket/factory.h>

   A factory for bound sockets.

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

#include <string>

#include <ifaddrs.h>

#include <base/fd.h>
#include <socket/address.h>

namespace Socket {

  /* A factory for bound sockets. */
  class TFactory final {
    public:

    /* Construct with default options and open a database of the network
       interfaces available on the host on which we're running. */
    TFactory();

    /* Releases our database of network interfaces. */
    ~TFactory();

    /* Construct a new socket and bind it according to the current options.
       Return the socket directly and the address to which it is bound via
       out-param. */
    Base::TFd New(TAddress &address) const;

    /* The name of the network interface to which to bind, or an empty string
       if we don't care which network interface we use.  The default is an
       empty string. */
    std::string InterfaceName;

    /* For each of these flags, if it is true, then we can create a socket
       of the given family.  Both are true by default. */
    bool IPv4, IPv6;

    /* If true, allow other sockets to bind to our port unless there is an
       active listening socket bound to the port already.  This helps avoid
       EADDRINUSE errors.  The default is true. */
    bool ReuseAddr;

    /* If true, set the socket to non-blocking mode.
       The default is false. */
    bool NonBlocking;

    /* If true, set the close-on-exec flag on the socket.
       The default is false. */
    bool CloseOnExec;

    /* The type of socket we will create, as per the socket() call.
       The default is SOCK_STREAM.  We will always use whatever protocol is
       the default for the type. */
    int Type;

    /* The protocol of the socket we will crate, as per the socket() call.
       If 0, we will use whatever protocol is the default for our type.
       The default is 0. */
    int Proto;

    /* The port number to which to bind, or 0 if we want to get a random
       port number.  The default is 0. */
    in_port_t Port;

    private:

    /* Find an interface that matches our critera or throw.
       Never returns null. */
    sockaddr *FindNetworkInterface() const;

    /* First in a linked list of network interfaces. */
    ifaddrs *FirstInterface;

  };  // TFactory

}  // Socket
