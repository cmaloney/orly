/* Join argument collections together, parse them all at once from argc, argv */
#pragma once

#include <memory>

#include <base/as_str.h>
#include <cmd/args.h>
#include <cmd/help.h>
#include <cmd/standard_args.h>

namespace Cmd {

DEFINE_ERROR(TMissingValue, TArgError, "missing argument");

// Forward declaration.
class TParser;

// Parse the arguments, deal with --help for a simple argument collection.
template <typename TOptions>
TOptions Parse(const TArgs<TOptions> &collection, const int argc, const char * const argv[]);

// Mid-level interface for parsing arguments.
// Construct a base parser however you want. This will add the standard arguments
// as wellas handle them, and parsre the given argc/argv.
void ParseWithStandard(TParser &parser, std::vector<const TArgInfo*> &&args, const int argc, const char * const argv[]);

// Compose together arbitrary argument collections for precise control.
class TParser {
  NO_COPY(TParser);
  public:

  TParser() = default;

  // TODO(cmaloney): Support nesting like 'git' or 'ip'.
  // Attach arguments building up the overall argument collection this parser
  // accepts.
  //
  // NOTE: This doesn't take ownership of any of the options, just indexes
  // them all.
  // NOTE: Order is important for attaching named arguments.
  template <typename TOptions>
  void Attach(const TArgs<TOptions> *options, TOptions *out);

  void Parse(int argc, const char *const argv[]) const;

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
// Helper to simplify simple parsing.
template <typename TOptions>
TOptions Parse(const TArgs<TOptions> &collection, const int argc, const char * const argv[]) {
  TParser parser;
  TOptions ret;
  parser.Attach(&collection, &ret);
  ParseWithStandard(parser, ExtractArgVector(collection), argc, argv);
  return ret;
}

} // namespce Cmd;