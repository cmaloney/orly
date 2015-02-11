#pragma once

#include <string>
#include <vector>

namespace Jhm {

class TFile;

std::vector<std::string> MakeDepFile(const TFile *in,
                                     const TFile *out,
                                     std::vector<std::string> &&extra_args);
}