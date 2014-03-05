/* <rpc/unpack.h>

   Unpack a tuple into a function call.

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

#include <cstdint>
#include <tuple>

namespace Rpc {

  /* By default, we pass parameters by constant reference. */
  template <typename TVal> struct Pass { typedef const TVal &type; };

  /* But these types we will pass by value. */
  #define PASS_BY_VAL(val) template <> struct Pass<val> { typedef val type; };
  PASS_BY_VAL(bool);
  PASS_BY_VAL(char);
  PASS_BY_VAL(float);
  PASS_BY_VAL(double);
  PASS_BY_VAL(int8_t);
  PASS_BY_VAL(int16_t);
  PASS_BY_VAL(int32_t);
  PASS_BY_VAL(int64_t);
  PASS_BY_VAL(uint8_t);
  PASS_BY_VAL(uint16_t);
  PASS_BY_VAL(uint32_t);
  PASS_BY_VAL(uint64_t);
  #undef PASS_BY_VAL

  /* Unpack arity-0 with return.  */
  template <typename TRet, typename TContext>
  TRet Unpack(
      TRet (TContext::*member)(),
      TContext *context,
      const std::tuple<> &) {
    return (context->*member)();
  }

  /* Unpack arity-0 without return.  */
  template <typename TContext>
  void Unpack(
      void (TContext::*member)(),
      TContext *context,
      const std::tuple<> &) {
    (context->*member)();
  }

  /* Unpack arity-1 with return.  */
  template <typename TRet, typename TContext, typename TArg0>
  TRet Unpack(
      TRet (TContext::*member)(
        typename Pass<TArg0>::type
      ),
      TContext *context,
      const std::tuple<TArg0> &args) {
    return (context->*member)(
      std::get<0>(args)
    );
  }

  /* Unpack arity-1 without return.  */
  template <typename TContext, typename TArg0>
  void Unpack(
      void (TContext::*member)(
        typename Pass<TArg0>::type
      ),
      TContext *context,
      const std::tuple<TArg0> &args) {
    (context->*member)(
      std::get<0>(args)
    );
  }

  /* Unpack arity-2 with return.  */
  template <typename TRet, typename TContext, typename TArg0, typename TArg1>
  TRet Unpack(
      TRet (TContext::*member)(
        typename Pass<TArg0>::type,
        typename Pass<TArg1>::type
      ),
      TContext *context,
      const std::tuple<TArg0, TArg1> &args) {
    return (context->*member)(
      std::get<0>(args),
      std::get<1>(args)
    );
  }

  /* Unpack arity-2 without return.  */
  template <typename TContext, typename TArg0, typename TArg1>
  void Unpack(
      void (TContext::*member)(
        typename Pass<TArg0>::type,
        typename Pass<TArg1>::type
      ),
      TContext *context,
      const std::tuple<TArg0, TArg1> &args) {
    (context->*member)(
      std::get<0>(args),
      std::get<1>(args)
    );
  }

  /* Unpack arity-3 with return.  */
  template <typename TRet, typename TContext, typename TArg0, typename TArg1, typename TArg2>
  TRet Unpack(
      TRet (TContext::*member)(
        typename Pass<TArg0>::type,
        typename Pass<TArg1>::type,
        typename Pass<TArg2>::type
      ),
      TContext *context,
      const std::tuple<TArg0, TArg1, TArg2> &args) {
    return (context->*member)(
      std::get<0>(args),
      std::get<1>(args),
      std::get<2>(args)
    );
  }

  /* Unpack arity-3 without return.  */
  template <typename TContext, typename TArg0, typename TArg1, typename TArg2>
  void Unpack(
      void (TContext::*member)(
        typename Pass<TArg0>::type,
        typename Pass<TArg1>::type,
        typename Pass<TArg2>::type
      ),
      TContext *context,
      const std::tuple<TArg0, TArg1, TArg2> &args) {
    (context->*member)(
      std::get<0>(args),
      std::get<1>(args),
      std::get<2>(args)
    );
  }

  /* Unpack arity-4 with return.  */
  template <typename TRet, typename TContext, typename TArg0, typename TArg1, typename TArg2, typename TArg3>
  TRet Unpack(
      TRet (TContext::*member)(
        typename Pass<TArg0>::type,
        typename Pass<TArg1>::type,
        typename Pass<TArg2>::type,
        typename Pass<TArg3>::type
      ),
      TContext *context,
      const std::tuple<TArg0, TArg1, TArg2, TArg3> &args) {
    return (context->*member)(
      std::get<0>(args),
      std::get<1>(args),
      std::get<2>(args),
      std::get<3>(args)
    );
  }

  /* Unpack arity-4 without return.  */
  template <typename TContext, typename TArg0, typename TArg1, typename TArg2, typename TArg3>
  void Unpack(
      void (TContext::*member)(
        typename Pass<TArg0>::type,
        typename Pass<TArg1>::type,
        typename Pass<TArg2>::type,
        typename Pass<TArg3>::type
      ),
      TContext *context,
      const std::tuple<TArg0, TArg1, TArg2, TArg3> &args) {
    (context->*member)(
      std::get<0>(args),
      std::get<1>(args),
      std::get<2>(args),
      std::get<3>(args)
    );
  }

  /* Unpack arity-5 with return.  */
  template <typename TRet, typename TContext, typename TArg0, typename TArg1, typename TArg2, typename TArg3, typename TArg4>
  TRet Unpack(
      TRet (TContext::*member)(
        typename Pass<TArg0>::type,
        typename Pass<TArg1>::type,
        typename Pass<TArg2>::type,
        typename Pass<TArg3>::type,
        typename Pass<TArg4>::type
      ),
      TContext *context,
      const std::tuple<TArg0, TArg1, TArg2, TArg3, TArg4> &args) {
    return (context->*member)(
      std::get<0>(args),
      std::get<1>(args),
      std::get<2>(args),
      std::get<3>(args),
      std::get<4>(args)
    );
  }

  /* Unpack arity-5 without return.  */
  template <typename TContext, typename TArg0, typename TArg1, typename TArg2, typename TArg3, typename TArg4>
  void Unpack(
      void (TContext::*member)(
        typename Pass<TArg0>::type,
        typename Pass<TArg1>::type,
        typename Pass<TArg2>::type,
        typename Pass<TArg3>::type,
        typename Pass<TArg4>::type
      ),
      TContext *context,
      const std::tuple<TArg0, TArg1, TArg2, TArg3, TArg4> &args) {
    (context->*member)(
      std::get<0>(args),
      std::get<1>(args),
      std::get<2>(args),
      std::get<3>(args),
      std::get<4>(args)
    );
  }

  /* Unpack arity-6 with return.  */
  template <typename TRet, typename TContext, typename TArg0, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5>
  TRet Unpack(
      TRet (TContext::*member)(
        typename Pass<TArg0>::type,
        typename Pass<TArg1>::type,
        typename Pass<TArg2>::type,
        typename Pass<TArg3>::type,
        typename Pass<TArg4>::type,
        typename Pass<TArg5>::type
      ),
      TContext *context,
      const std::tuple<TArg0, TArg1, TArg2, TArg3, TArg4, TArg5> &args) {
    return (context->*member)(
      std::get<0>(args),
      std::get<1>(args),
      std::get<2>(args),
      std::get<3>(args),
      std::get<4>(args),
      std::get<5>(args)
    );
  }

  /* Unpack arity-6 without return.  */
  template <typename TContext, typename TArg0, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5>
  void Unpack(
      void (TContext::*member)(
        typename Pass<TArg0>::type,
        typename Pass<TArg1>::type,
        typename Pass<TArg2>::type,
        typename Pass<TArg3>::type,
        typename Pass<TArg4>::type,
        typename Pass<TArg5>::type
      ),
      TContext *context,
      const std::tuple<TArg0, TArg1, TArg2, TArg3, TArg4, TArg5> &args) {
    (context->*member)(
      std::get<0>(args),
      std::get<1>(args),
      std::get<2>(args),
      std::get<3>(args),
      std::get<4>(args),
      std::get<5>(args)
    );
  }

  /* Unpack arity-7 with return.  */
  template <typename TRet, typename TContext, typename TArg0, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6>
  TRet Unpack(
      TRet (TContext::*member)(
        typename Pass<TArg0>::type,
        typename Pass<TArg1>::type,
        typename Pass<TArg2>::type,
        typename Pass<TArg3>::type,
        typename Pass<TArg4>::type,
        typename Pass<TArg5>::type,
        typename Pass<TArg6>::type
      ),
      TContext *context,
      const std::tuple<TArg0, TArg1, TArg2, TArg3, TArg4, TArg5, TArg6> &args) {
    return (context->*member)(
      std::get<0>(args),
      std::get<1>(args),
      std::get<2>(args),
      std::get<3>(args),
      std::get<4>(args),
      std::get<5>(args),
      std::get<6>(args)
    );
  }

  /* Unpack arity-7 without return.  */
  template <typename TContext, typename TArg0, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6>
  void Unpack(
      void (TContext::*member)(
        typename Pass<TArg0>::type,
        typename Pass<TArg1>::type,
        typename Pass<TArg2>::type,
        typename Pass<TArg3>::type,
        typename Pass<TArg4>::type,
        typename Pass<TArg5>::type,
        typename Pass<TArg6>::type
      ),
      TContext *context,
      const std::tuple<TArg0, TArg1, TArg2, TArg3, TArg4, TArg5, TArg6> &args) {
    (context->*member)(
      std::get<0>(args),
      std::get<1>(args),
      std::get<2>(args),
      std::get<3>(args),
      std::get<4>(args),
      std::get<5>(args),
      std::get<6>(args)
    );
  }

}  // Rpc
