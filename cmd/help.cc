#include <cmd/help.h>

#include <sys/ioctl.h>

#include <algorithm>
#include <iostream>

#include <base/as_str.h>
#include <base/not_implemented.h>
#include <base/split.h>

using namespace Base;
using namespace Cmd;

static void Indent(uint64_t pad_size) {
  for (uint64_t i = 0; i < pad_size; ++i) {
    std::cout << ' ';
  }
}

static void PrintIndented(uint64_t line_size, uint64_t pad_size, const char *text, uint64_t pre_pad) {
  assert(pad_size < line_size);

  // Calculate text chunk length per line.
  const uint64_t text_per_line = line_size - pad_size;

  // We can write the help on the same line if the option isn't too long.
  bool pre_padded = false;
  if (pre_pad + 2 > pad_size) {
    std::cout << '\n';
  } else {
    Indent(pad_size - pre_pad);
    pre_padded = true;
  }

  //TODO(cmaloney): strlen is not going to go over well with internationalization eventually.
  const uint64_t text_len = strlen(text);
  for (uint64_t offset=0; offset < text_len; offset += text_per_line) {
    if (pre_padded) {
      pre_padded = false;
    } else {
      Indent(pad_size);
    }
    std::cout.write(text+offset, int64_t(std::min(text_per_line, text_len - offset)));
  }
  std::cout << '\n';
}

static void PrintRepetition(const TRepetition repitition, bool show_zero_or_one=true) {
  switch(repitition) {
    case TRepetition::One:
    std::cout << ' ';
    break;
    case TRepetition::ZeroOrOne:
    std::cout << (show_zero_or_one ? '?' : ' ');
    break;
    case TRepetition::ZeroOrMore:
    std::cout << '*';
    break;
    case TRepetition::OneOrMore:
    std::cout << '+';
    break;
  }
}

void Cmd::PrintHelp(const char *program_name, const std::vector<const TArgInfo*> &args) {
  //TODO(cmaloney): All this code needs to be heavily reworked.
  struct winsize ws;
  ioctl(0, TIOCGWINSZ, &ws);
  auto line_size = std::max(ws.ws_col - 1, 80);

  // TODO(cmaloney): Calculate this based on the max length of a
  // parameter name, bounded by a longest length.
  auto pad_size = 20;

  // TODO(cmaloney): Overview of command text description

  // TODO(cmaloney): Usage line (Needs to adapt to many vs. few parameters)
  std::cout << "USAGE: " << program_name << " [arguments]\n\n";

  // TODO(cmaloney): Alphabetize positional arguments
  bool first_positional = true;
  for (const TArgInfo *arg: args) {
    if (!arg->Positional) {
      continue;
    }

    if (first_positional) {
      std::cout << "Positional arguments:\n";
      first_positional = false;
    }

    // TODO(cmaloney): Specify the parse type which needs to be given
    // Positional args should have exactly one name.
    assert(arg->Names.size() == 1);
    std::cout << arg->Names[0];
    PrintRepetition(arg->Repitition);
    PrintIndented(line_size, pad_size, arg->Description, arg->Names[0].size() + 1);
  }

  if (first_positional == false) {
    std::cout << '\n';
  }

  // TODO(cmaloney): Alphabetize non-positional arguments
  bool first_named = true;

  for (const TArgInfo *arg: args) {
    if (arg->Positional) {
      continue;
    }

    if (first_named) {
      std::cout << "Named arguments:\n";
      first_named = false;
    }

    auto prepad_len = 0;
    bool first_name = true;
    for (const auto &name: arg->Names) {
      if (first_name) {
        first_name = false;
      } else {
        prepad_len += 1;
        std::cout << ',';
        first_name = true;
      }
      if (name.size() == 1) {
        prepad_len += 1;
        std::cout << '-';
      } else {
        prepad_len += 2;
        std::cout << "--";
      }

      prepad_len += name.size();
      std::cout << name;
    }
    PrintRepetition(arg->Repitition, false);
    prepad_len += 1;

    // TODO(cmaloney): Specify the parse type which needs to be given
    // TODO(cmaloney): Print the default value if non-null.
    PrintIndented(line_size, pad_size, arg->Description, prepad_len);
  }
}