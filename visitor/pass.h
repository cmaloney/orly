/* <visitor/pass.h>

   Typical passing conventions for visitors.

   These are set of typical passing conventions for our visitors. They are really type modifiers such as std::add_lvalue_reference
   that exists in <type_traits> header, in fact Ref (see below) is just an alias for it. For visitors, we have a set of types that
   are part of a family, we then have to agree on a passing convention of those types.

   For example, TVisitor (our non-mutating visitor), __could__ pass everything by const-ref, and TMutatingVisitor (our mutating
   visitor) passes them by reference. In fact what happens with TVisitor is that we pass everything by const-ref, __except__ if
   the type happens to be a POD (Plain Old Data) type and the size of it is smaller than or equal to a pointer. (It's more
   efficient or at least as efficient as passing them by const-ref).

   We can say something like:

     class TVisitor : public Visitor::Single::TVisitor<Visitor::Pass, Mpl::TTypeSet<int, double, std::string>> {};

   This constructs a single visitor pure virtuals functions overloaded on the following.
      * int
      * double
      * const std::string &

   Since int and double are passed by value since they are POD types and the size is smaller than or equal to a pointer, and
   std::string is passed by const-ref because it's not a POD type.

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

#include <c14/type_traits.h>

namespace Visitor {

  /* Useful set of decorators for visitors.  Some of them are just simple wrappers around the type modification templates in the
     <type_traits> header in the standard library. */

  /* Add ref to T. Becomes: T &. */
  template <typename T>
  using Ref = std::add_lvalue_reference<T>;

  /* Convenience type alias for Ref<T>::type. */
  template <typename T>
  using TRef = typename Ref<T>::type;

  /* Add const-ref to T. Becomes: const T &. */
  template <typename T>
  using Cref = std::add_lvalue_reference<c14::add_const_t<T>>;

  /* Convenience type alias for Cref<T>::type. */
  template <typename T>
  using TCref = typename Cref<T>::type;

  /* Add pointer to T. Becomes: T *. */
  template <typename T>
  using Ptr = std::add_pointer<T>;

  /* Convenience type alias for Ptr<T>::type. */
  template <typename T>
  using TPtr = typename Ptr<T>::type;

  /* const T * */
  template <typename T>
  using Cptr = std::add_pointer<c14::add_const_t<T>>;

  /* Convenience type alias for Cptr<T>::type. */
  template <typename T>
  using TCptr = typename Cptr<T>::type;

  /* Tests for whether type T is "small". Our definition of "small" may change
     over time, but pass by value vs pass by const-ref depends on this result.
     TVariant also uses this to determine whether to use a TShared<> instance of
     it or to just aggregate it directly. */
  template <typename T>
  using IsSmall =
      std::integral_constant<bool,
                             std::is_pod<c14::decay_t<T>>::value &&
                                 sizeof(c14::decay_t<T>) <= sizeof(int *)>;

  /* Pass by T if T is POD and the size of T is less than or equal to a
     pointer (passing by value is more efficient or
     at least as good as passing by const-ref in this case). Pass by
     const-ref otherwise. */
  template <typename T>
  using Pass = std::conditional<IsSmall<T>::value, T, TCref<T>>;

  /* Convenience type alias for Pass<T>::type. */
  template <typename T>
  using TPass = typename Pass<T>::type;

}  // Visitor
