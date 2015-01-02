/* A expectation inside a unit test fixture.

Expectations state something that is expected to be true. They do not terminate
execution on failure.

TODO(cmaloney): Sample / usage. */

#pragma once

#include <cassert>
#include <sstream>
#include <string>
#include <typeinfo>

#include <base/class_traits.h>
#include <base/code_location.h>
#include <base/demangle.h>
#include <base/no_default_case.h>
#include <test/context.h>

/* Exepects lhs == rhs. */
#define EXPECT_EQ(lhs, rhs) ::Test::Expect::Check(HERE, #lhs, lhs, ::Test::Expect::Eq, #rhs, rhs)

/* Exepects lhs != rhs. */
#define EXPECT_NE(lhs, rhs) ::Test::Expect::Check(HERE, #lhs, lhs, ::Test::Expect::Ne, #rhs, rhs)

/* Exepects lhs < rhs. */
#define EXPECT_LT(lhs, rhs) ::Test::Expect::Check(HERE, #lhs, lhs, ::Test::Expect::Lt, #rhs, rhs)

/* Exepects lhs <= rhs. */
#define EXPECT_LE(lhs, rhs) ::Test::Expect::Check(HERE, #lhs, lhs, ::Test::Expect::Le, #rhs, rhs)

/* Exepects lhs > rhs. */
#define EXPECT_GT(lhs, rhs) ::Test::Expect::Check(HERE, #lhs, lhs, ::Test::Expect::Gt, #rhs, rhs)

/* Exepects lhs >= rhs. */
#define EXPECT_GE(lhs, rhs) ::Test::Expect::Check(HERE, #lhs, lhs, ::Test::Expect::Ge, #rhs, rhs)

/* Exepects arg == true. */
#define EXPECT_TRUE(arg) ::Test::Expect::Check(HERE, ::Test::Expect::IsTrue, #arg, arg)

/* Exepects arg == false. */
#define EXPECT_FALSE(arg) ::Test::Expect::Check(HERE, ::Test::Expect::IsFalse, #arg, arg)

/* Exepects lhs == rhs. */
#define EXPECT_EQ_AT(here, lhs, rhs) \
  ::Test::Expect::Check(here, #lhs, lhs, ::Test::Expect::Eq, #rhs, rhs)

/* Exepects lhs != rhs. */
#define EXPECT_NE_AT(here, lhs, rhs) \
  ::Test::Expect::Check(here, #lhs, lhs, ::Test::Expect::Ne, #rhs, rhs)

/* Exepects lhs < rhs. */
#define EXPECT_LT_AT(here, lhs, rhs) \
  ::Test::Expect::Check(here, #lhs, lhs, ::Test::Expect::Lt, #rhs, rhs)

/* Exepects lhs <= rhs. */
#define EXPECT_LE_AT(here, lhs, rhs) \
  ::Test::Expect::Check(here, #lhs, lhs, ::Test::Expect::Le, #rhs, rhs)

/* Exepects lhs > rhs. */
#define EXPECT_GT_AT(here, lhs, rhs) \
  ::Test::Expect::Check(here, #lhs, lhs, ::Test::Expect::Gt, #rhs, rhs)

/* Exepects lhs >= rhs. */
#define EXPECT_GE_AT(here, lhs, rhs) \
  ::Test::Expect::Check(here, #lhs, lhs, ::Test::Expect::Ge, #rhs, rhs)

/* Exepects arg == true. */
#define EXPECT_TRUE_AT(here, arg) ::Test::Expect::Check(here, ::Test::Expect::IsTrue, #arg, arg)

/* Exepects arg == false. */
#define EXPECT_FALSE_AT(here, arg) ::Test::Expect::Check(here, ::Test::Expect::IsFalse, #arg, arg)

/* Exepects that the given exception class will be thrown during evaluation of the function. */
#define EXPECT_THROW(exception, func)              \
  ::Test::Expect::Check(HERE, #exception, [](bool &pass) { \
    try {                                          \
      func();                                      \
      pass = false;                                \
    } catch(const exception &) {                   \
      pass = true;                                 \
    }                                              \
  })

// TODO: This name is sort of ugh.
/* Like EXPECT_THROW, but captures the callable function. */
#define EXPECT_THROW_FUNC(exception, func)              \
  ::Test::Expect::Check(HERE, #exception, [&func](bool &pass) { \
    try {                                               \
      func();                                           \
      pass = false;                                     \
    } catch(const exception &) {                        \
      pass = true;                                      \
    }                                                   \
  })

/* Exepects that the given exception class will be thrown during evaluation of the function. */
#define EXPECT_THROW_AT(here, exception, func)     \
  ::Test::Expect::Check(here, #exception, [](bool &pass) { \
    try {                                          \
      func();                                      \
      pass = false;                                \
    } catch(const exception &) {                   \
      pass = true;                                 \
    }                                              \
  })

namespace Test {

namespace Expect {

enum TInfix { Lt, Le, Gt, Ge };
enum TEq { Eq, Ne };
enum TPrefix { IsTrue, IsFalse };

auto Log(const Base::TCodeLocation &loc, const char *test, bool pass) {
  return TContext::Log(loc, test, pass);
}

// Prints the C++ type wrapped in parenthesis.
// Usage: std::cout << TType<TArg();
template <typename TArg>
struct TType {};

template <typename TArg>
std::ostream &operator<<(std::ostream &out, const TType<TArg> &) {
  return out << '(' << Base::Demangle<TArg>() << ')';
}

template <typename TLhs, typename TRhs>
void WriteInfixOp(const Base::TCodeLocation &loc,
                  const char *lhs_str,
                  const TLhs &lhs,
                  const char *op_str,
                  const char *rhs_str,
                  const TRhs &rhs,
                  bool result) {
  assert(lhs_str);
  assert(&lhs);
  assert(op_str);
  assert(rhs_str);
  assert(&rhs);

  Log(loc,
      Base::AsStr(TType<TLhs>(), lhs_str, ' ', op_str, ' ', TType<TRhs>(), rhs_str).c_str(),
      result)
      << lhs << ' ' << op_str << ' ' << rhs;
}

template <typename TLhs, typename TRhs>
bool Check(const Base::TCodeLocation &loc,
           const char *lhs_str,
           const TLhs &lhs,
           TEq op,
           const char *rhs_str,
           const TRhs &rhs) {
  assert(lhs_str);
  assert(rhs_str);
  assert(&lhs);
  assert(&rhs);

  bool result;
  const char *op_str = "ERROR";
  switch(op) {
    case Eq: {
      op_str = "==";
      result = (lhs == rhs);
      break;
    }
    case Ne: {
      op_str = "!=";
      result = (lhs != rhs);
      break;
    }
  }
  WriteInfixOp(loc, lhs_str, lhs, op_str, rhs_str, rhs, result);
  return result;
}

template <typename TLhs, typename TRhs>
bool Check(const Base::TCodeLocation &loc,
           const char *lhs_str,
           const TLhs &lhs,
           TInfix op,
           const char *rhs_str,
           const TRhs &rhs) {
  assert(lhs_str);
  assert(rhs_str);
  assert(&lhs);
  assert(&rhs);

  bool result;
  const char *op_str;
  switch(op) {
    case Lt: {
      op_str = "<";
      result = (lhs < rhs);
      break;
    }
    case Le: {
      op_str = "<=";
      result = (lhs <= rhs);
      break;
    }
    case Gt: {
      op_str = ">";
      result = (lhs > rhs);
      break;
    }
    case Ge: {
      op_str = ">=";
      result = (lhs >= rhs);
      break;
    }
  }
  WriteInfixOp(loc, lhs_str, lhs, op_str, rhs_str, rhs, result);
  return result;
}

template <typename TArg>
bool Check(const Base::TCodeLocation &loc, TPrefix op, const char *arg_str, const TArg &arg) {
  assert(arg_str);
  assert(&arg);
  bool result;
  const char *op_str;
  std::ostringstream strm;
  switch(op) {
    case IsTrue: {
      op_str = "";
      result = bool(arg);
      break;
    }
    case IsFalse: {
      op_str = "!";
      result = !bool(arg);
      break;
    }
  }
  Log(loc, Base::AsStr(TType<TArg>(), op_str, arg_str).c_str(), result) << op_str << bool(arg);
  return result;
}

bool Check(const Base::TCodeLocation &loc,
           const char *exc_str,
           const std::function<void(bool &pass)> &func) {
  assert(&loc);
  assert(exc_str);
  assert(&func);
  assert(func);

  // Seperate line to force evaluation order.
  bool result = false;
  func(result);

  Log(loc, Base::AsStr("throw ", exc_str).c_str(), result) << (result ? "throw" : "nothrow");
  return result;
}

}  // Expect

}  // Test