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
  o docopt style short specification?
*/

#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <base/opt.h>

namespace Cmd {
DEFINE_ERROR(TArgError, std::runtime_error, "problem with an argument");
DEFINE_ERROR(TMissingValue, TArgError, "missing argument");

namespace Arg {

  template <typename TVal>
  struct NeedsValue : public std::true_type {};

  template <>
  struct NeedsValue<bool> : public std::false_type {};

} // Arg

template <typename TVal>
TVal ParseArg(const std::string &val);

// Args have names and descriptions
struct TArgInfo {
  const std::vector<std::string> Names;
  const char *Description = nullptr;
  // TODO(cmaloney): Optional should really be a range of valid counts? (0, 0-1, 1, 1+)
  const bool Optional = true;
  const bool HasValue = false;
};

// TODO: Positional arg info (1, 0-1, 1+)

template <typename TOptions>
std::function<void(TOptions &options, const std::string &arg)> ParseAndStore;


enum class TRepetition {
  One, // Required, no default
  ZeroOrOne, // Optional
  OneOrMore // Collect
};

template <typename TOptions>
struct TArgCollection {
  static_assert(std::is_default_constructible<TOptions>::value,
                "Options object must be default constructible");

  struct TProcessor : public TArgInfo {
    DEFAULT_COPY(TProcessor);
    DEFAULT_MOVE(TProcessor);
    using TConsume = std::function<void(TOptions &options, const std::string &value)>;
    // TODO(cmaloney): Use inheritance/virtual functions instead of std::function/function pointers?
    // What is the tradeoff? Where does one beat the other?

    // Named argument (--foo, -f).
    TProcessor(const std::vector<std::string> &names, const char *description, TRepetition rep, TConsume &&value_func);

    // Positional argument (a b c d)
    TProcessor(TRepetition repitition, const char *identifier, const char *description, TConsume &&value_func);

    TConsume Apply;
  };

  TArgCollection(std::initializer_list<TProcessor> &&args);

  const TProcessor &arg_processor();

  TOptions Parse(const int argc, const char *argv[]) const;

  // TODO(cmaloney): These should live in an arg parser / parser aggregation class, not the arg collectors.
  // Quick argument lookup map.
  std::vector<const TProcessor> Info;
  std::unordered_map<std::string, const TProcessor*> Named;
  std::vector<const TProcessor *> Positional;
};

bool BeginsWith(const std::string &needle, const std::string &haystack) {
  // TODO(cmaloney): I know there is a more canonical STL way to do this...
  auto it_needle = needle.begin();
  const auto needle_end = needle.end();
  auto it_haystack = haystack.begin();
  const auto haystack_end = haystack.end();
  while (it_haystack != haystack_end) {
    if (it_needle == needle_end) {
      return true;
    }

    if (*it_needle != *it_haystack) {
      return false;
    }
    ++it_needle;
    ++it_haystack;
  }
  return false;
}

//TODO(cmaloney): This should probably use a parser object to make the code more
// modular and testable.
template <typename TOptions>
TOptions TArgCollection<TOptions>::Parse(const int argc, const char *argv[]) const {

  TOptions result;

  int i = 0;
  auto has_more = [&i, argc] () -> bool {
    return i < argc;
  };

  // TODO(cmaloney): Switch to something string_view like.
  auto get_arg = [argc, argv] (int i) -> std::string {
    return std::string(argv[i]);
  };

  while (i < argc) {
    // Look up argument
    auto arg = get_arg(i);

    // TODO(cmaloney): The '=' 1 or 2 optional split should happen via Base::Split
    auto split_pos = arg.find_first_of('=');
    const std::string key = split_pos == std::string::npos ? arg : arg.substr(0, split_pos);
    const Base::TOpt<std::string> value_equals =
        split_pos == std::string::npos ? Base::TOpt<std::string>() : arg.substr(split_pos+1);


    auto get_value_str = [&value_equals, &has_more, &i, &get_arg, &arg]() {
      if (value_equals) {
        return *value_equals;
      }
      if (has_more()) {
        ++i;
        return get_arg(i);
      }
      THROW_ERROR(TMissingValue) << "Value required for " << std::quoted(arg) << ". Use '--" << arg << "=<value>' or --" << arg << " value";
    };

    // Long options are '--' followed by a single word option. If the option
    // requires a value that may be specified by writing '=' value
    if (BeginsWith("--", arg)) {
      // TODO(cmaloney): Catch the not found index exception
      const TProcessor &processor = *Named.at(arg.substr(2));
      if (processor.HasValue) {
        processor.Apply(result, get_value_str());
      }
      continue;
    }

    // Short options are '-' followed by one character options. If one of those
    // options requires a value, then we must find a '=' or value after a space.
    if (BeginsWith("-", arg)) {
      NOT_IMPLEMENTED();
    }
  }

  return result;

}


template<typename TOptions>
using TArgs = TArgCollection<TOptions>;


// Optional named argument.
template <typename TRet, typename TOptions>
auto Optional(const std::vector<std::string> &names, TRet TOptions::*member, const char *description) {
  return typename TArgCollection<TOptions>::TProcessor(names, description, TRepetition::ZeroOrOne, [member](TOptions &options, const std::string &value) {
      options.*member = ParseArg<TRet>(value);
    });
}

// Optional named argument.
template <typename TRet, typename TOptions>
auto Optional(const char name[], TRet TOptions::*member, const char *description) {
  return typename TArgCollection<TOptions>::TProcessor({std::string(name)}, description, TRepetition::ZeroOrOne, [member](TOptions &options, const std::string &value) {
      options.*member = ParseArg<TRet>(value);
    });
}

// Positional argument
template <typename TRet, typename TOptions>
auto Required(TRet TOptions::*member, TRepetition repitition, const char *identifier, const char *description) {
  return typename TArgCollection<TOptions>::TProcessor(repitition, identifier, description, [member](TOptions &options, const std::string &value) {
      options.*member = ParseArg<TRet>(value);
  });
}

template <typename TOptions>
TOptions Parse(const TArgCollection<TOptions> &collection, const int argc, const char *argv[]) {
  return collection.Parse(argc, argv);
}

}  // Cmd