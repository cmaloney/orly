/* <socket/factory.cc>

   Implements <socket/factory.h>.

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

#include <socket/factory.h>

#include <cassert>
#include <sstream>
#include <stdexcept>
#include <string>

#include <base/error_utils.h>

using namespace std;
using namespace Base;
using namespace Socket;

TFactory::TFactory()
    : IPv4(true), IPv6(true),
      ReuseAddr(true), NonBlocking(false), CloseOnExec(false),
      Type(SOCK_STREAM), Proto(0), Port(0) {
  IfLt0(getifaddrs(&FirstInterface));
}

TFactory::~TFactory() {
  assert(this);
  freeifaddrs(FirstInterface);
}

TFd TFactory::New(TAddress &address) const {
  assert(this);
  assert(&address);
  /* Find a network interface that matches our criteria. */
  auto *sa = FindNetworkInterface();
  /* Open a socket that matches the network interface and is of the correct
     type and protocol. */
  int type = Type;
  if (NonBlocking) {
    type |= SOCK_NONBLOCK;
  }
  if (CloseOnExec) {
    type |= SOCK_CLOEXEC;
  }
  TFd fd(socket(sa->sa_family, type, Proto));
  if (ReuseAddr) {
    int flag = true;
    IfLt0(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)));
  }
  /* Bind the socket to the address we found. */
  address = *sa;
  address.SetPort(Port);
  Bind(fd, address);
  /* If we asked for a system-assigned port, find out which one we got. */
  if (!Port) {
    address = GetSockName(fd);
  }
  return move(fd);
}

sockaddr *TFactory::FindNetworkInterface() const {
  assert(this);
  sockaddr *result;
  /* This will track the state of our search through the linked list of
     network interfaces. */
  enum {
    not_found, found_loopback, found_wrong_family, found
  } state = not_found;
  for (auto *iface = FirstInterface; state != found && iface;
       iface = iface->ifa_next) {
    /* If we're looking for an interface with a particular name and the
       current interface doesn't match, skip it. */
    if (!InterfaceName.empty() && InterfaceName != iface->ifa_name) {
      continue;
    }
    result = iface->ifa_addr;
    switch (result->sa_family) {
      case AF_INET: {
        /* We've found an IPv4 device or loopback. */
        if (IPv4) {
          const auto *ipv4 = reinterpret_cast<sockaddr_in *>(result);
          state = (ipv4->sin_addr.s_addr != 0x0100007F)
              ? found : found_loopback;
        } else {
          state = found_wrong_family;
        }
        break;
      }
      case AF_INET6: {
        /* We've found an IPv6 device or loopback. */
        if (IPv6) {
          const auto *ipv6 = reinterpret_cast<sockaddr_in6 *>(result);
          state = memcmp(
              &ipv6->sin6_addr, &in6addr_loopback, sizeof(in6addr_loopback))
              ? found : found_loopback;
        } else {
          state = found_wrong_family;
        }
        break;
      }
      default: {
        /* We've found some address family we're not interested in. */
        state = found_wrong_family;
      }
    }  // switch
  }  // for
  /* Throw a good message if we failed to find an interface we could use. */
  switch (state) {
    case not_found: {
      /* We found no interface found at all. */
      if (InterfaceName.empty()) {
        throw runtime_error("no network interfaces found");
      }
      ostringstream strm;
      strm
          << "network interface '" << InterfaceName
          << "' not found";
      throw runtime_error(strm.str());
    }
    case found_loopback: {
      /* We found only lookbacks. */
      if (InterfaceName.empty()) {
        throw runtime_error("no non-loopback network interfaces found");
      }
      ostringstream strm;
      strm
          << "network interface '" << InterfaceName
          << "' is a loopback";
      throw runtime_error(strm.str());
    }
    case found_wrong_family: {
      /* We found only weird address families. */
      if (InterfaceName.empty()) {
        throw runtime_error("no IPv4 or IPv6 network interfaces found");
      }
      ostringstream strm;
      strm
          << "network interface '" << InterfaceName
          << "' doesn't support IPv4 or IPv6";
      throw runtime_error(strm.str());
    }
    default: {
      break;
    }
  }  // switch
  assert(result);
  return result;
}
