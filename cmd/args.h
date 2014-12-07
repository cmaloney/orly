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
  const bool Optional = true;
  const bool HasValue = false;
};

// TODO: Positional arg info (1, 0-1, 1+)

template <typename TOptions>
std::function<void(TOptions &options, const std::string &arg)> ParseAndStore;

// Adds an optional argument to a collection
template <typename TOptions, typename TVal>
TArg<TOptions> Optional(std::string name, );

//TODO (cmaloney): How to track if a required argument has been used?

template <typename TOptions>
class TArgCollection {
  static_assert(std::is_default_constructible<TOptions>::value,
                "Options object must be default constructible");

  struct TProcessor : public TArgInfo {
    using TConsume = std::function<void(TOptions &options, const std::string &value)>;
    // TODO(cmaloney): Use inheritance/virtual functions instead of std::function/function pointers?
    // What is the tradeoff? Where does one beat the other?

    TConsume ArgConsumer;
  };

  // Quick argument lookup map.
  std::vector<const TProcessor> Info;
  std::unordered_map<const std::string, const TProcessor*> Named;
  std::vector<const TProcessor *> Positional;
};



template <typename TOptions>
class TArgParser {
  void HandleArg(TOptions &target) const {
    const TArgInfo &info = Lookup.at(name);
    if (info.HasValue)
  }


  TOptions result;
};

void Parse(const TArgCollection &arguments, TOptions &options, const int argc, const char *argv[]) {

  // Parse argument through value
  // NOTE: We explicitly don't do anything to values since they could contain who knows what.
  bool has_value;
  TArgInfo info;
  std::tie(has_value, ) = arguments.Find(name)
}



template<typename TOptions>
using TArgs = TArgCollection<TOptions>;

}  // Cmd