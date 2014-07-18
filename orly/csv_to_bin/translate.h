#include <cstddef>
#include <string>
#include <orly/csv_to_bin/level3.h>

class TTranslate {
  public:

  explicit TTranslate(std::string out_prefix, std::size_t max_kv_per_file)
      : Index(0),
        OutPrefix(std::move(out_prefix)),
        MaxKvPerFile(std::move(max_kv_per_file)) {}

  void operator()(Orly::CsvToBin::TLevel3 &level3) const {
    Translate(level3);
    ++Index;
  }

  private:

  void Translate(Orly::CsvToBin::TLevel3 &level3) const;

  mutable std::size_t Index;

  std::string OutPrefix;

  std::size_t MaxKvPerFile;

};  // TTranslate
