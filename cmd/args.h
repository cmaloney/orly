/* Simple command line argument parser / router

Web-style router for command line parameters.

 o Automatically generates --help
 o Accepted named argument forms:
    -f                // Short
    -fvvvn Hello      // Combined short arguments, giving value to last
    -n=hello          // Use '=' to specify value
    --foo --name      // Long argument with no value
    --foo=bar         // Long argument '=' value
    --foo bar         // Long argument with value
 o Arguments can be pulled from a file, just place the arguments in a
    file either as a string or one per line, then specify '@filename'
    on the command line.

Usage:

  //NOTE: must be default constructable.
  struct TOptions {
    std::string Name = "Hello";
    int Verbose = 3;
    std::vector<std::string> Baz;
    std::chrono::seconds Timeout = 0s;
  }

  //NOTE: Router verifies options are all lower case, '-' seperated.
  const static Router<TOption> Options {
    // Named arguments
    Required("name", &TOptions::Name, "Name of thing to do"),
    Optional({"verbose", "v"}, &TOptions::Verbose, Args::Count, "logging verbosity"),

    // Unnamed arguments (May arbitrarily route / delegate to subcommands).
    // NOTE: Subcommands may either claim "new context" or continue adding to the existing
    // argument parsing context (-, -- args from parent still can be specified or not)
    Required(Baz, "baz", "Files to baz");
  }

  int main(int argc, char **argv) {
    TOptions options = Options.Parse(argc, argv);

    std::cout << "Hello, " << options.name << "!" << std::endl;
    return EXIT_SUCCESS;
  }


TODO:
  o enable/disable options
  o docopt style short specification?
  o Boolean parameters should be able to be specified either by
    their flag existing, or by -flag={true,false,yes,no,y,n,t,f}
  o Make missing overloads produce a sane error message (There are a lot of missing ones
currently...)
  o Make missing overloads of ParseArg produce a sane error message
*/
#pragma once

#include <functional>
#include <string>
#include <type_traits>
#include <vector>

#include <base/class_traits.h>
#include <cmd/util.h>

namespace Cmd {

// TODO: Should allow more generic specifiers here, maybe tying to other variables?
//      (Must match count of x?).
enum class TRepetition {
  One,  // Required, no default.
  ZeroOrOne,  // Optional.
  ZeroOrMore,  // Optional.
  OneOrMore  // Collect.
};

// Args have names and descriptions.
struct TArgInfo {
  TArgInfo(bool positional,
           std::vector<std::string> names,
           const char *description,
           TRepetition repitition,
           bool has_value)
      : Positional(positional),
        Names(names),
        Description(description),
        Repitition(repitition),
        HasValue(has_value) {}
  bool Positional = false;
  const std::vector<std::string> Names;
  const char *Description = nullptr;
  TRepetition Repitition;
  const bool HasValue = false;
};

/* Bundle of args that attach their value to a specific object. */
template <typename TOptions>
struct TArgs {
  static_assert(std::is_default_constructible<TOptions>::value,
                "Options object must be default constructible");

  struct TProcessor : public TArgInfo {
    DEFAULT_COPY(TProcessor)
    DEFAULT_MOVE(TProcessor)
    using TConsume = std::function<void(TOptions *options, const std::string &value)>;
    // TODO(cmaloney): Use inheritance/virtual functions instead of std::function/function pointers?
    // What is the tradeoff? Where does one beat the other?

    // TODO(cmaloney): Disallow arguments that start with '-', '--'.
    // Named argument (--foo, -f).
    TProcessor(const std::vector<std::string> &names,
               const char *description,
               TRepetition repitition,
               bool HasValue,
               TConsume &&value_func)
        : TArgInfo{false, names, description, repitition, HasValue},
          TypedConsume(std::move(value_func)) {}

    // TODO(cmaloney): Disallow arguments that start with '-', '--'.
    // Positional argument (a b c d).
    TProcessor(TRepetition repitition,
               const char *identifier,
               const char *description,
               bool HasValue,
               TConsume &&value_func)
        : TArgInfo{true, {identifier}, description, repitition, HasValue},
          TypedConsume(std::move(value_func)) {}

    TConsume TypedConsume;
  };

  TArgs(std::initializer_list<TProcessor> &&args) : Info(std::move(args)) {}

  // TODO(cmaloney): These should live in an arg parser / parser aggregation class, not the arg
  // collectors.
  // Quick argument lookup map.
  const std::vector<TProcessor> Info;
};

/* Helpers for building argument bundles. */

// Optional named argument.
template <typename TRet, typename TOptions>
auto Optional(const std::vector<std::string> &names,
              TRet TOptions::*member,
              const char *description) {
  return typename TArgs<TOptions>::TProcessor(
      names,
      description,
      TRepetition::ZeroOrOne,
      THasValue<TRet>::value,
      [member](TOptions *options,
               const std::string &value) { options->*member = ParseArg<TRet>(value); });
}

// Optional named argument.
template <typename TRet, typename TOptions>
auto Optional(const char name[], TRet TOptions::*member, const char *description) {
  return typename TArgs<TOptions>::TProcessor(
      {std::string(name)},
      description,
      TRepetition::ZeroOrOne,
      THasValue<TRet>::value,
      [member](TOptions *options,
               const std::string &value) { options->*member = ParseArg<TRet>(value); });
}

// Optional positional argument
template <typename TElem, typename TOptions>
auto Optional(std::vector<TElem> TOptions::*member,
              const char *identifier,
              const char *description) {
  return typename TArgs<TOptions>::TProcessor(
      TRepetition::ZeroOrMore,
      identifier,
      description,
      true,
      [member](TOptions *options,
               const std::string &value) { (options->*member).push_back(ParseArg<TElem>(value)); });
}

// Positional argument
template <typename TElem, typename TOptions>
auto Required(TElem TOptions::*member, const char *identifier, const char *description) {
  return typename TArgs<TOptions>::TProcessor(
      TRepetition::One,
      identifier,
      description,
      true,
      [member](TOptions *options,
               const std::string &value) { options->*member = ParseArg<TElem>(value); });
}
// Positional argument
template <typename TElem, typename TOptions>
auto Required(std::vector<TElem> TOptions::*member,
              const char *identifier,
              const char *description) {
  return typename TArgs<TOptions>::TProcessor(
      TRepetition::OneOrMore,
      identifier,
      description,
      true,
      [member](TOptions *options,
               const std::string &value) { (options->*member).push_back(ParseArg<TElem>(value)); });
}

}  // Cmd
