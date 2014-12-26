/* Join argument collections together, parse them all at once from argc, argv */
#pragma once

#include <iostream> // TODO(cmaloney)

#include <base/as_str.h>
#include <cmd/args.h>
#include <cmd/standard_args.h>

namespace Cmd {

// Simple option parsing interface.
template <typename TOptions>
TOptions Parse(const TArgs<TOptions> &collection, const int argc, const char * const argv[]);

// Use this if you want to compose option parsers by hand with precise control over behavior
// and whether or not things like --help exist.
class TParser {
  NO_COPY(TParser);
  public:

  TParser() = default;

  // NOTE: This doesn't take ownership of any of the options, just indexes
  // them all.
  // NOTE: Order is important for attaching
  template <typename TOptions>
  void Attach(const TArgs<TOptions> *options, TOptions *out);

  void Parse(int argc, const char *const argv[]) const;

  void PrintHelp(const char *program_name);

  // TODO(cmaloney): Seperate positional from non-positional consumers?
  struct TConsumer {
    template <typename TOptions>
    TConsumer(const typename Cmd::TArgs<TOptions>::TProcessor *option, TOptions *out)
        : HasValue(option->HasValue),
          Name(option->Names.at(0)),
          Repitition(option->Repitition),
          Consume(std::bind(option->TypedConsume, out, std::placeholders::_1)) {}

    bool HasValue;
    const std::string Name;
    TRepetition Repitition;
    std::function<void(const std::string &value)> Consume;
  };


  private:
  // NOTE: We use a unique_ptr so the consumer pointers don't move around on vector reallocation.
  std::vector<std::unique_ptr<TConsumer>> Consumers;

  std::unordered_map<std::string, const TConsumer*> Named;
  std::vector<const TConsumer *> Positional;
}; // TParser

template <typename TOptions>
void TParser::Attach(const TArgs<TOptions> *options, TOptions *out) {
  // Index the positional, named arguments for easy lookup while command line parsing.
  bool last_positional = false;
  for (const auto &arg: options->Info) {
    Consumers.emplace_back(std::make_unique<TConsumer>(&arg, out));
    const TConsumer *consumer = Consumers.back().get();
    if (arg.Positional) {
      if (last_positional) {
        // TODO(cmaloney): Throw a more helpfulu error message with a more specific type.
        throw std::logic_error("Error constructing argument parser: Ambiguous positional argument.");
      }
      Positional.push_back(consumer);

      // If the positional can be specified an unknown number of times (ZeroOrOne, OneOrMore), then more positionals after it would be
      // ambiguous to parse
      if (arg.Repitition == TRepetition::ZeroOrOne || arg.Repitition == TRepetition::OneOrMore) {
        last_positional = true;
      }
    } else {
      for (const std::string &name: arg.Names) {
        Named.emplace(name, consumer);
      }
    }
  }
}

// TODO(cmaloney): Add a parse overload which takes a vector of collections?
// TODO(cmaloney): Need a way to add non-positional only argument bundles to
//                 ensure full usage
// Helper to simplify simple parsing
template <typename TOptions>
TOptions Parse(const TArgs<TOptions> &collection, const int argc, const char * const argv[]) {
  TParser parser;
  TOptions ret;
  TStandardOptions standard_options;
  const auto &standard_args = GetStandardArgs();

  parser.Attach(&standard_args, &standard_options);
  parser.Attach(&collection, &ret);

  // TODO(cmaloney): Unbundle args, pretty print errors one by one
  bool has_error = false;
  try {
    parser.Parse(argc, argv);
  } catch (const TArgError &ex) {
    has_error = true;
    throw TErrorExit{-1, Base::AsStr("Errors while parsing arguments:\n", ex.what(), '\n',"Use --help to get a list of options")};
  }

  if (standard_options.Help) {
    parser.PrintHelp(argv[0]);
  }

  if (standard_options.Help || has_error) {
    throw TCleanExit();
  }

  return ret;
}

} // namespce Cmd;