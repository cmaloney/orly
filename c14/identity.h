/* <c14/identity.h>

   Identity type trait.

   Yes, this is trivial. But we don't wanna have to write this everytime we need of these. When would we ever use this?
   Well, only a few cases I can think of.

     1. Symmetry with std::integral_constant<>

       When we define a type trait that has some bool called 'value' in it, we inherit from std::integral_constant<>, like this:

         template <typename T>
         struct is_bool : public std::integral_constant<bool, std::is_same<T, bool>::value> {};

       If we want to do this with a type instead of a value, using identity<> we can say something symmetric like:

         template <typename T>
         struct add_const_ref : public identity<std::add_lvalue_reference_t<std::add_const_t<T>>> {};

     2. Force template argument deduction to __not__ kick in.

       We want to write a function that will take any type T but only take an rvalue-reference of it.
       We might write something like:

         template <typename TVal>
         void PassThrough(TVal &&val) {}

       Hopefully you already know that this will actually take absolutely everything since this is actually a universal reference.
       (If you didn't, basically if you have T && where T is deduced, then it's a universal reference and binds to __anything__.)

       If at the call site the template parameter was specified we'd be ok.

         int x = 42;
         PassThrough(x);       // This is fine. TVal == int &, TVal && == int & && == int &. got int &.
         PassThrough<int>(x);  // This will fail. TVal == int, TVal && == int &&. got int &.

       So let's force the template parameter to be specified, using identity<>.

         template <typename TVal>
         void PassThrough(identity_t<TVal> &&val) {}

       Now template argument deduction does not kick in and we force the user to have to provide the TVal for us.

       Actually, in this case we could have also done:

         template <typename TVal>
         void PassThrough(std::add_rvalue_reference_t<TVal> val) {}

       Since this will do the same thing as above... but anyway, I hope you get my point.

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

namespace c14 {

  template <typename T>
  struct identity {
    using type = T;
  };

  template <typename T>
  using identity_t = typename identity<T>::type;

}  // c14