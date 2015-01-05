#pragma once

#include <base/as_str.h>
#include <base/code_location.h>

/* Use this macro to define a new error class, like this:  DEFINE(TSomethingBad, std::runtime_error,
 * "something bad happened"); */
#define DEFINE_ERROR(error_t, base_t, desc)                              \
  class error_t : public base_t {                                        \
    public:                                                              \
    error_t(const char *msg) : base_t(msg) {}                            \
    error_t(const Base::TCodeLocation &here, const char *msg)            \
        : base_t(::Base::AsStr(Base::GetErrorDescHelper<error_t>()       \
                                   ? Base::GetErrorDescHelper<error_t>() \
                                   : #error_t,                           \
                               here,                                     \
                               "; ",                                     \
                               msg).c_str()) {}                          \
    error_t(const Base::TCodeLocation &here)                             \
        : base_t(::Base::AsStr(Base::GetErrorDescHelper<error_t>()       \
                                   ? Base::GetErrorDescHelper<error_t>() \
                                   : #error_t,                           \
                               here).c_str()) {}                         \
    static const char *GetDesc() { return desc; }                        \
  };

namespace Base {
template <typename TError>
std::is_same<decltype(TError::GetDesc()), const char *> HasGetDescImpl(void *);

template <typename>
std::false_type HasGetDescImpl(...);

template <typename TError>
static constexpr bool HasGetDesc() {
  return decltype(HasGetDescImpl<TError>(nullptr))::value;
}

/* Helper template so that we can prefix methods with a GetDesc() function if the error has one. Not
 * if it doesn't. */
template <typename TError>
std::enable_if_t<HasGetDesc<TError>(), const char *> GetErrorDescHelper() {
  return TError::GetDesc();
}

template <typename TError>
std::enable_if_t<!HasGetDesc<TError>(), const char *> GetErrorDescHelper() {
  return nullptr;
}
}