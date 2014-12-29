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
#include <test/runner.h>

/* Exepects lhs == rhs. */
#define EXPECT_EQ(lhs, rhs) \
    ::Test::Expect(HERE, #lhs, lhs, ::Test::TExpect::Eq, #rhs, rhs)

/* Exepects lhs != rhs. */
#define EXPECT_NE(lhs, rhs) \
    ::Test::Expect(HERE, #lhs, lhs, ::Test::TExpect::Ne, #rhs, rhs)

/* Exepects lhs < rhs. */
#define EXPECT_LT(lhs, rhs) \
    ::Test::Expect(HERE, #lhs, lhs, ::Test::TExpect::Lt, #rhs, rhs)

/* Exepects lhs <= rhs. */
#define EXPECT_LE(lhs, rhs) \
    ::Test::Expect(HERE, #lhs, lhs, ::Test::TExpect::Le, #rhs, rhs)

/* Exepects lhs > rhs. */
#define EXPECT_GT(lhs, rhs) \
    ::Test::Expect(HERE, #lhs, lhs, ::Test::TExpect::Gt, #rhs, rhs)

/* Exepects lhs >= rhs. */
#define EXPECT_GE(lhs, rhs) \
    ::Test::Expect(HERE, #lhs, lhs, ::Test::TExpect::Ge, #rhs, rhs)

/* Exepects arg == true. */
#define EXPECT_TRUE(arg) \
    ::Test::Expect(HERE, ::Test::TExpect::IsTrue, #arg, arg)

/* Exepects arg == false. */
#define EXPECT_FALSE(arg) \
    ::Test::Expect(HERE, ::Test::TExpect::IsFalse, #arg, arg)

/* Exepects lhs == rhs. */
#define EXPECT_EQ_AT(here, lhs, rhs) \
    ::Test::Expect(here, #lhs, lhs, ::Test::TExpect::Eq, #rhs, rhs)

/* Exepects lhs != rhs. */
#define EXPECT_NE_AT(here, lhs, rhs) \
    ::Test::Expect(here, #lhs, lhs, ::Test::TExpect::Ne, #rhs, rhs)

/* Exepects lhs < rhs. */
#define EXPECT_LT_AT(here, lhs, rhs) \
    ::Test::Expect(here, #lhs, lhs, ::Test::TExpect::Lt, #rhs, rhs)

/* Exepects lhs <= rhs. */
#define EXPECT_LE_AT(here, lhs, rhs) \
    ::Test::Expect(here, #lhs, lhs, ::Test::TExpect::Le, #rhs, rhs)

/* Exepects lhs > rhs. */
#define EXPECT_GT_AT(here, lhs, rhs) \
    ::Test::Expect(here, #lhs, lhs, ::Test::TExpect::Gt, #rhs, rhs)

/* Exepects lhs >= rhs. */
#define EXPECT_GE_AT(here, lhs, rhs) \
    ::Test::Expect(here, #lhs, lhs, ::Test::TExpect::Ge, #rhs, rhs)

/* Exepects arg == true. */
#define EXPECT_TRUE_AT(here, arg) \
    ::Test::Expect(here, ::Test::TExpect::IsTrue, #arg, arg)

/* Exepects arg == false. */
#define EXPECT_FALSE_AT(here, arg) \
    ::Test::Expect(here, ::Test::TExpect::IsFalse, #arg, arg)

/* Exepects that the given exception class will be thrown during evaluation of the function. */
#define EXPECT_THROW(exception, func) \
    ::Test::Expect(HERE, #exception, \
                    [](bool &pass) { try { func(); pass = false; } catch (const exception&) { pass = true; } })

//TODO: This name is sort of ugh.
/* Like EXPECT_THROW, but captures the callable function. */
#define EXPECT_THROW_FUNC(exception, func) \
    ::Test::Expect(HERE, #exception, \
                    [&func](bool &pass) { try { func(); pass = false; } catch (const exception&) { pass = true; } })


/* Exepects that the given exception class will be thrown during evaluation of the function. */
#define EXPECT_THROW_AT(here, exception, func) \
    ::Test::Expect(here, #exception, \
                    [](bool &pass) { try { func(); pass = false; } catch (const exception&) { pass = true; }})


namespace Test {


  namespace ExpectOp {
    enum TInfix { Lt, Le, Gt, Ge };
    enum TEq {Eq, Ne};
    enum TPrefix { IsTrue, IsFalse };
  }

  Context::TPrinter LogExpect(bool pass, const Base::TCodeLocation &loc, const char *test) {
    return Context::LogResult(loc, test, pass);
  }

  template <typename TLhs, typename TRhs>
  void Expect(
      const Base::TCodeLocation &code_location,
      const char *lhs_str, const TLhs &lhs,
      ExpectOp::TEq op,
      const char *rhs_str, const TRhs &rhs) {
    assert(lhs_str);
    assert(rhs_str);
    assert(&lhs);
    assert(&rhs);

    const char *op_str = "ERROR";
    switch(op) {
      case ExpectOp::Eq: {
        op_str = "==";
        Pass = (lhs == rhs);
        break;
      }
      case ExpectOp::Ne: {
        op_str = "!=";
        Pass = (lhs != rhs);
        break;
      }
    }
    WriteInfixOp(lhs_str, lhs, op_str, rhs_str, rhs);
  }

  template <typename TLhs, typename TRhs>
  void Expect(
      const Base::TCodeLocation &code_location,
      const char *lhs_str, const TLhs &lhs,
      TInfixOp op,
      const char *rhs_str, const TRhs &rhs)
      : CodeLocation(code_location) {
    assert(lhs_str);
    assert(rhs_str);
    assert(&lhs);
    assert(&rhs);

    const char *op_str;
    switch (op) {
      case Lt: {
        op_str = "<";
        Pass = (lhs < rhs);
        break;
      }
      case Le: {
        op_str = "<=";
        Pass = (lhs <= rhs);
        break;
      }
      case Gt: {
        op_str = ">";
        Pass = (lhs > rhs);
        break;
      }
      case Ge: {
        op_str = ">=";
        Pass = (lhs >= rhs);
        break;
      }
    }
    WriteInfixOp(lhs_str, lhs, op_str, rhs_str, rhs);
  }

  template <typename TArg>
  void Expect(
      const Base::TCodeLocation &code_location,
      TPrefixOp op, const char *arg_str, const TArg &arg)
      : CodeLocation(code_location) {
    assert(arg_str);
    assert(&arg);
    const char *op_str;
    std::ostringstream strm;
    switch (op) {
      case IsTrue: {
        op_str = "";
        Pass = bool(arg);
        break;
      }
      case IsFalse: {
        op_str = "!";
        Pass = !bool(arg);
        break;
      }
      NO_DEFAULT_CASE;
    }
    WriteType<TArg>(strm);
    strm << op_str << arg_str;
    Source = strm.str();
    strm.str("");
    strm << op_str << bool(arg);
    Expression = strm.str();
  }

  void Expect(
    const Base::TCodeLocation &code_location,
    const char *exc_str,
    const std::function<void (bool &pass)> &func) : CodeLocation(code_location) {
    assert(&code_location);
    assert(exc_str);
    assert(&func);
    assert(func);

    std::ostringstream strm;
    strm << "throw " << exc_str;
    Source = strm.str();

    func(Pass);

    Expression = Pass ? "throw" : "nothrow";

  }

    virtual ~TExpect();

    operator bool() const {
      assert(this);
      return Pass;
    }

    template <typename TVal>
    const TExpect &Write(const TVal &val) const {
      assert(this);
      Explanation << val;
      return *this;
    }

    private:
    template <typename TLhs, typename TRhs>
    void WriteInfixOp(const char *lhs_str, const TLhs &lhs, const char *op_str, const char *rhs_str, const TRhs &rhs) {
      assert(this);
      assert(lhs_str);
      assert(&lhs);
      assert(op_str);
      assert(rhs_str);
      assert(&rhs);

      std::ostringstream strm;
      WriteType<TLhs>(strm);
      strm << lhs_str << ' ' << op_str << ' ';
      WriteType<TRhs>(strm);
      strm << rhs_str;
      Source = strm.str();
      strm.str("");
      strm << lhs << ' ' << op_str << ' ' << rhs;
      Expression = strm.str();

      LogExpect
    }

    template <typename TArg>
    static void WriteType(std::ostream &strm) {
      strm << '(' << Base::Demangle<TArg>() << ')';
    }

    Base::TCodeLocation CodeLocation;
    bool Pass;
    std::string Source, Expression;
    mutable std::ostringstream Explanation;
  };

}
