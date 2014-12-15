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

namespace Cmd {

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

template <typename TOptions>
class TArgCollection {
  static_assert(std::is_default_constructible<TOptions>::value,
                "Options object must be default constructible");

  struct TProcessor : public TArgInfo {
    using TConsume = std::function<void(TOptions &options, const std::string &value)>;
    // TODO(cmaloney): Use inheritance/virtual functions instead of std::function/function pointers?
    // What is the tradeoff? Where does one beat the other?

    TConsume Apply;
  };

  const TProcessor &arg_processor();

  TOptions Parse(const int argc, const char *argv[]) const;

  // Quick argument lookup map.
  std::vector<const TProcessor> Info;
  std::unordered_map<const std::string, const TProcessor*> Named;
  std::vector<const TProcessor *> Positional;
};


// Adds an optional argument to a collection
template <typename TOptions, typename TMember>
typename TArgCollection<TOptions>::TProcessor Optional(std::string name, &TOptions::foo ptr_to_member, const char *description) {
  return TArgCollection<TOptions>::TProcessor
}

//TODO(cmaloney): This should probably use a parser object to make the code more
// modular and testable.
template <typename TOptions>
TOptions TArgCollection<TOptions>::Parse(const int argc, const char *argv[]) const {

  int i = 0;
  auto has_more = [&i, argc] () -> bool {
    return i < argc;
  };

  // TODO(cmaloney): Switch to something string_view like.
  auto get_arg = [argc, argv] (int i) {
    return std::string(argv[i]);
  };

  void try_match = [](const std::string &arg, const char c, const uint64_t index) -> bool {
    if (arg.length() <= index) {
      return false;
    }
    return arg[index] == c;
  }

  while (i < argc) {
    // Look up argument
    auto arg = get_arg(i);

    // TODO(cmaloney): The '=' 1 or 2 optional split should happen via Base::Split
    std::string key, value;
    auto split_pos = arg.find_first_of('=')
    const std::string key = split_pos == string::npos ? arg : arg.substr(0, split_pos);
    const Opt<std::string> value_equals =
        split_pos = string::npos ? TOpt<std::string>() : arg.substr(split_pos+1);

    // Long options are '--' followed by a single word option. If the option
    // requires a value that may be specified by writing '=' value
    if (BeginsWith(arg, "--")) {
      // TODO(cmaloney): Catch the not found index exception
      const TProcessor &processor = *Named.at(arg.substr(2));
      if (processor.HasValue) {

      }
    }

    // Short options are '-' followed by one character options. If one of those
    // options requires a value, then we must find a '=' or value after a space.
    if (try_match(arg, '-')) {
      NOT_IMPLEMENTED();
    }


    // If argument needs a value, eat that as well ('--foo=value' or
    // --foo value)
    if (try_match(arg, '--', 0)) {
      // Short opt has one followed by a collection of short args.

    }

    else if
  }

}


template<typename TOptions>
using TArgs = TArgCollection<TOptions>;

}  // Cmd