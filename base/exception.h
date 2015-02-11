#pragma once

#include <base/as_str.h>
#include <base/code_location.h>

/* Use this macro to define a new error class, like this:  DEFINE(TSomethingBad, std::runtime_error,
 * "something bad happened"); */
#define DEFINE_ERROR(error_t, base_t, desc)                                                     \
  class error_t : public base_t {                                                               \
    public:                                                                                     \
    error_t(const char *msg) : base_t(msg) {}                                                   \
    error_t(const Base::TCodeLocation &here, const char *msg)                                   \
        : base_t(::Base::AsStr(#error_t, here, "; ", msg).c_str()) {}                           \
    error_t(const Base::TCodeLocation &here) : base_t(::Base::AsStr(#error_t, here).c_str()) {} \
    static const char *GetDesc() { return desc; }                                               \
  };
