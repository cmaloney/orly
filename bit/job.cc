#include <bit/job.h>

#include <cassert>

#include <base/split.h>
#include <bit/job_producer.h>

using namespace Base;
using namespace Bit;
using namespace std;

template <typename TVal>
static size_t HashHelper(const TVal &val) {
  return hash<TVal>()(val);
}

bool TJob::TId::operator==(const TId &that) const noexcept {
  return Producer == that.Producer && Input == that.Input;
}

size_t TJob::TId::THash::operator()(const TId &that) const noexcept {
  // TODO: Write a better hash...
  return HashHelper(that.Producer) + HashHelper(that.Input);
}

TFileInfo *TJob::GetInput() const { return Metadata.Input; }

const unordered_set<TFileInfo *> &TJob::GetOutput() const { return Metadata.Output; }

TFileInfo *TJob::GetSoleOutput() const {
  assert(Metadata.Output.size() == 1);
  return *Metadata.Output.begin();
}

const TJobProducer *TJob::GetJobProducer() const { return Metadata.Producer; }

std::ostream &Bit::operator<<(std::ostream &out, TJob *job) {
  out << job->GetJobProducer()->Name << '{' << job->GetInput() << " -> ["
      << Join(job->GetOutput(), ", ") << "]}";
  return out;
}
