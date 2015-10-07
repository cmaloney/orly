#include <bit/produce.h>

#include <bit/environment.h>
#include <bit/job_runner.h>

// Figure out the jobs which need to be run to create the given set of targets
// and run them to create the targets.
void Bit::Produce(TEnvironment &environment, std::vector<std::string> Targets) {
  TJobRunner job_runner;

  // Find jobs, process queue results until either there is an error running a
  // job or
}
