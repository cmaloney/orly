#include <cmd/parse.h>

#include <algorithm>
#include <iomanip>
#include <iostream>

#include <base/not_implemented.h>
#include <base/opt.h>
#include <base/thrower.h>
#include <cmd/standard_args.h>

using namespace Cmd;

bool BeginsWith(const std::string &needle, const std::string &haystack) {
  // TODO(cmaloney): I know there is a more canonical STL way to do this...
  auto it_needle = needle.begin();
  const auto needle_end = needle.end();
  auto it_haystack = haystack.begin();
  const auto haystack_end = haystack.end();
  while(it_haystack != haystack_end) {
    if(it_needle == needle_end) {
      return true;
    }

    if(*it_needle != *it_haystack) {
      return false;
    }
    ++it_needle;
    ++it_haystack;
  }
  return false;
}

void TParser::Parse(const int argc, const char *const argv[]) const {
  uint64_t current_positional = 0, current_positional_count = 0;
  int i = 1;
  auto has_more = [&i, argc]() -> bool { return i < argc; };

  // TODO(cmaloney): Switch to something string_view like.
  auto get_arg = [argc, argv](int idx) -> std::string {
    assert(idx < argc);
    return std::string(argv[idx]);
  };

  // Everything after '--' should be taken as positional arguments.
  bool no_more_interpreted = false;

  while(i < argc) {
    // Look up argument, advance argument position
    auto arg = get_arg(i);
    ++i;

    // TODO(cmaloney): We should only do this calculation iff we have a short or long argument.
    // TODO(cmaloney): The '=' 1 or 2 optional split should happen via Base::Split
    auto split_pos = arg.find_first_of('=');
    const std::string key = split_pos == std::string::npos ? arg : arg.substr(0, split_pos);
    const Base::TOpt<std::string> value_equals =
        split_pos == std::string::npos ? Base::TOpt<std::string>() : arg.substr(split_pos + 1);

    auto get_value_str = [&value_equals, &has_more, &i, &get_arg, &key]() {
      if(value_equals) {
        return *value_equals;
      }
      if(has_more()) {
        return get_arg(i++);
      }
      THROWER(TMissingValue) << "Value required for " << std::quoted(key) << ". Use '--" << key
                                 << "=<value>' or --" << key << " value";
    };

    if(!no_more_interpreted) {
      if(arg == "--") {
        no_more_interpreted = true;
        continue;
      }

      // Long options are '--' followed by a single word option. If the option
      // requires a value that may be specified by writing '=' value
      if(BeginsWith("--", key)) {
        // TODO(cmaloney): Catch the not found index exception
        auto it = Named.find(key.substr(2));
        if(it == Named.end()) {
          THROWER(TArgError) << "No such long option " << std::quoted(key);
        }
        const TConsumer &consumer = *it->second;
        if(consumer.HasValue || value_equals) {
          consumer.Consume(get_value_str());
        } else {
          consumer.Consume("");
        }
        continue;
      }

      if(BeginsWith("@", key)) {
        NOT_IMPLEMENTED_S("Argument file parsing.");
      }

      // Short options are '-' followed by one character options. If one of those
      // options requires a value, then we must find a '=' or value after a space.
      if(BeginsWith("-", key)) {
        auto key_size = key.size();
        for(uint32_t short_pos = 1; short_pos < key_size; ++short_pos) {
          const char c = key.at(short_pos);
          const auto c_str = std::string(1, c);
          auto it = Named.find(c_str);
          if(it == Named.end()) {
            THROWER(TArgError) << "No such short option " << std::quoted(c_str);
          }

          const TConsumer &consumer = *it->second;
          // NOTE: If we have a value as a short option, we must be the last short option
          if(consumer.HasValue || (value_equals && short_pos == key_size - 1)) {
            if(short_pos != key_size - 1) {
              THROWER(TArgError)
                  << "Short argument with value " << std::quoted(std::string(c_str))
                  << "must may only appear at the end of a block of short arguments";
            }
            consumer.Consume(get_value_str());
          } else {
            consumer.Consume("");
          }
        }
        continue;
      }
    }

    // Positional argument
    const TConsumer *consumer = Positional.at(current_positional);
    consumer->Consume(arg);
    if(consumer->Repitition == TRepetition::One) {
      ++current_positional;
    } else {
      ++current_positional_count;
      if(consumer->Repitition == TRepetition::ZeroOrOne && current_positional_count > 1) {
        THROWER(TArgError) << "Too many instances of positional argument "
                               << std::quoted(consumer->Name);
      }
    }
    continue;
  }
}

// Attaches and process the standard arguments to the given parser, processes them.
void Cmd::ParseWithStandard(TParser &parser,
                            std::vector<const TArgInfo *> args,
                            const int argc,
                            const char *const argv[]) {
  // Attach the standard options.
  TStandardOptions standard_options;
  auto standard_args = GetStandardArgs();
  parser.Attach(&standard_args, &standard_options);

  // TODO(cmaloney): Make the arg vec just live in the parser, then help can
  // just grab it from there.
  const auto standard_arg_vec = ExtractArgVector(standard_args);
  args.insert(args.end(), standard_arg_vec.begin(), standard_arg_vec.end());

  // Parse the command line.
  // TODO(cmaloney): pretty print errors one by one instead of bundled together.
  bool has_error = false;
  try {
    parser.Parse(argc, argv);
  } catch(const TArgError &ex) {
    has_error = true;
    throw TErrorExit{-1,
                     Base::AsStr("Errors while parsing arguments:\n",
                                 ex.what(),
                                 '\n',
                                 "Use --help to get a list of options")};
  }

  // Process standard arguments.
  if(standard_options.Help) {
    assert(argc >= 1);
    PrintHelp(argv[0], args);
  }

  if(standard_options.Help || has_error) {
    throw TCleanExit();
  }
}
