#include <bit/produce.h>

#include <base/not_implemented.h>
#include <bit/environment.h>
#include <bit/job_runner.h>

using namespace Bit;
using namespace std;

// Figure out the jobs which need to be run to create the given set of targets
// and run them to create the targets.
void Bit::Produce(uint64_t worker_count, TEnvironment &environment, vector<string> Targets) {
  TJobRunner job_runner(worker_count);
  (void)Targets;
  (void)environment;
  return;

  // Find jobs, process queue results until either there is an error running a
  // job or
  NOT_IMPLEMENTED();
}
