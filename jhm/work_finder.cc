/* <jhm/work_finder.h>

   Finds and runs all jobs necessary to create the needed files.

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <jhm/work_finder.h>

#include <fstream>
#include <iostream>
#include <stdexcept>

#include <base/split.h>
#include <base/thrower.h>
#include <jhm/file.h>
#include <jhm/job.h>
#include <jhm/status_line.h>
#include <util/error.h>
#include <util/path.h>
#include <util/stl.h>

using namespace Base;
using namespace Jhm;
using namespace std;
using namespace Util;

string GetCacheFilename(const TFile *file) {
  return AsStr(file->GetPath()) + ".jhm-cache";
}

bool TWorkFinder::AddNeededFile(TFile *file, TJob *job) {
  // If the file both doesn't exist and isn't buildable, error.
  if (file->IsSrc()) {
    return false;
  }

  // Ensure the file has a producer
  TJob *producer = TryGetProducer(file);
  if (!producer) {
    THROW_ERROR(runtime_error) << "No known way to produce file \"" << file << '"';
  }

  // Queue the producer job as necessary to finish.
  bool res = Queue(producer);
  if (res && job) {
    //TODO: Lots of copies (Although they're tiny)
    ToFinish.emplace(producer, job);
  }
  return res;
}

void TWorkFinder::ProcessReady() {
  while (!Ready.empty()) {
    // For each ReadyJob, see if there are any deps we now need.
    uint64_t needed = 0;

    TJob *job = Pop(Ready);

    /* check input */ {
      TFile *file = job->GetInput();
      needed += AddNeededFile(file, job);
    }

    for (TFile *file : job->GetNeeds()) {
      try {
        // Add each file. If any need jobs to complete, the job isn't ready yet.
        needed += AddNeededFile(file, job);
      } catch (const exception &ex) {
        // Add additional information of what needed the file which errored.
        THROW_ERROR(std::runtime_error) << "Needed for job " << job << EndOfPart << ex.what();
      }
    }
    if (needed) {
      // TODO: Lots of copies (Although they're tiny)
      InsertOrFail(Waiting, make_pair(job, needed));
    } else {
      // If we're about to run the job, ensure the output directories for it exist
      // TODO: Move this to a more logical place.
      for (TFile *out : job->GetOutput()) {
        EnsureDirExists(AsStr(out->GetPath()).c_str(), true);
      }
      Runner.Queue(job);
      InsertOrFail(Running, job);
    }
  }
}

TJson::TArray BuildJsonArray(const unordered_set<TFile *> &files) {
  TJson::TArray ret;
  ret.reserve(files.size());

  for(TFile *f: files) {
    ret.emplace_back(AsStr(f->GetPath()));
  }
  return ret;
}

bool TWorkFinder::ProcessResult(TJobRunner::TResult &result) {
  // Job finished / no longer running
  EraseOrFail(Running, result.Job);

  if (result.ExitCode != 0) {
    // TODO: Test if stdout, stderr have text before pritning label and text for each.
    cout << "ERROR: " << result.Job << " returned " << result.ExitCode << '\n' << "STDOUT: \n";
    Base::EchoOutput(move(result.Stdout));
    cout << "STDERR: \n";
    Base::EchoOutput(move(result.Stderr));
    return true;
  }
  // Check if the job is actually complete or not
  if (!result.Job->IsComplete()) {
    Ready.push(result.Job);
    return false;
  }

  TJson::TArray job_output;
  for(TFile *f: result.Job->GetOutput()) {
    job_output.push_back(AsStr(f->GetPath()));
  }

  TJson job_info(TJson::TObject{
      {"build_info", TJson::TObject{
        {"job", TJson::TObject{
          {"name", result.Job->GetName()},
          {"input", AsStr(result.Job->GetInput()->GetPath())},
          {"output", job_output},
          {"needs",  BuildJsonArray(result.Job->GetNeeds()) },
          {"anti_needs", BuildJsonArray(result.Job->GetAntiNeeds()) }
        }},
        {"config_timestamp", ConfigTimestampStr}
      }}});

  for (TFile *out_file : result.Job->GetOutput()) {
    // Sanity check that all output files now exist.
    if (!ExistsPath(AsStr(out_file->GetPath()).c_str())) {
      THROW_ERROR(logic_error) << "Job " << result.Job << " Didn't produce the output file '" << out_file
                               << " which it was supposed to yet returned successfully...";
    }

    // Attach to each output file it's build info
    out_file->PushComputedConfig(TJson(job_info));

    // Write out a job cache file containing the job info
    string cache_filename = GetCacheFilename(out_file);
    ofstream cache_out_name(cache_filename);
    if (!cache_out_name.is_open()) {
      THROW_ERROR(runtime_error) << "Unable to open cache output file \"" << quoted(cache_filename) << '"';
    }
    //NOTE: This locks the config stack, because once we've written the cache, if more things were to add to
    // the config stack, they wouldn't be in the cache file, and that would be :(
    out_file->WriteConfig(cache_out_name);
  }

  // Update every job which was waiting on this job to be waiting on one less thing.
  // NOTE: If the job isn't waiting on anything, it gets pushed to the Ready queue.
  const auto range = ToFinish.equal_range(result.Job);
  if (range.first != range.second) {
    for(auto iter = range.first; iter != range.second; ++iter) {
      // Update each job which was waiting on this job's waiting entry. Queue in ReadyJobs if needed.
      TJob *job = iter->second;
      assert(job);
      auto &count = Waiting.at(job);
      --count;
      if (count == 0) {
        // Move the job to ready, as it has nothing left it's waiting on.
        EraseOrFail(Waiting, job);
        Ready.push(job);
      }
    }
    // TODO: Assert this succeeds (It should be guaranteed to)
    ToFinish.erase(range.first, range.second);
  }

  InsertOrFail(Finished, result.Job);
  return false;
}

bool TWorkFinder::IsBuildable(TFile *file) {
  return file->IsSrc() || TryGetProducer(file);
}

bool TWorkFinder::IsFileDone(TFile *file) {
  if (file->IsSrc()) {
    return true;
  }
  TJob *job = TryGetProducer(file);
  if (job) {
    return IsDone(job);
  } else {
    return false;
  }
}

TJob *TWorkFinder::TryGetProducer(TFile *file) {
  if (file->IsSrc()) {
    return nullptr;
  }

  /* If there's already a producer, don't search. */ {
    auto elem = TryFind(Producers, file);
    if(elem) {
      return *elem;
    }
  }

  TJob *found_producer = nullptr;
  for (TJob *job: GetJobsProducingFile(file)) {
    TFile *input_file = job->GetInput();

    if (IsBuildable(input_file)) {
      bool found_self = false;
      //TODO: Collect / hold errors at this loop, re throw after last iteration.
      for (TFile *f : job->GetOutput()) {
        if (!Producers.emplace(f, job).second) {
          if (Producers.at(f) != job) {
            THROW_ERROR(runtime_error) << "Multiple producers for file "
                                       << f <<  ". Producers: " << Producers.at(f) << ", " << job;
          }
        }
        found_self |= f == file;
      }
      assert(found_self && "Make sure we found ourselves");

      // Note that we found a valid producer so we can return the producer.
      found_producer = job;
    }
  }

  // Check that we actually found a valid producer
  return found_producer;
}

void TWorkFinder::WriteStatusLine() const {
  TStatusLine() << '[' << Finished.size() << '/' << All.size() << "] waiting: " << Running.size();
}

bool TWorkFinder::FinishAll() {
  bool has_failed = false;
  // Continue as long as the runner is returning us more results still, or we have some jobs ready to be processed.
  while (Runner.HasMoreResults() || (!Ready.empty() && Runner.IsReady())) {
    // Either queue the job to run or find what it's depending upon / waiting on, and queue those to run.
    ProcessReady();

    // Everything should have been emptied from the ready queue
    assert(Ready.empty());

    // Something should be in the running queue at this point. Or we're just waiting for more results from the runner.
    assert(!Running.empty() || Runner.HasMoreResults());

    // Sanity check
    // If we haven't failed, then the running, ready, and finished sets should make up the 'all' set.
    // If we've failed 1+ job, then the number failed will be missing.
    if (!has_failed) {
      WriteStatusLine();
      assert(Running.size() + Waiting.size() + Finished.size() == All.size());
    }

    //DEBUG: cout << Running.size() << " + " << Waiting.size() << " + " << Finished.size() << " == " << All.size() << endl;
    //DEBUG: Base::Join(", ", Running, cout); cout << '\n';

    // Wait for 1+ results from the job runner. Process every result returned.
    for(auto &result: Runner.WaitForResults()) {
      has_failed |= ProcessResult(result);
    }
  }

  // Write the last update (Otherwise we sit at one less than complete on successful completion)
  if (!has_failed) {
    WriteStatusLine();
    TStatusLine::Cleanup();
  }

  return !has_failed;
  // Find the jobs with no dependencies,)
}

bool TWorkFinder::IsDone(TJob *job) {
  assert(this);

  if (!Contains(Cache, job)) {
    CacheCheck(job);
  }

  return Contains(Finished, job);
}

//TODO: Lots of generic helper functions (Esp. dealing with nanosecond timestamps), should be factored into library.
template<typename TVal>
auto GrabOne(const std::unordered_set<TVal> &container) {
  assert(container.size() > 0);
  for(const TVal &item : container) {
    return item;
  }
  __builtin_unreachable();
}

TFile *TWorkFinder::TryGetOutputFileFromPath(const std::string &filename) {
  assert(&filename);
  TFile *ret = TryGetFileFromPath(filename);
  // If we aren't buildable, try finding the executable form.
  if (ret && !IsBuildable(ret)) {
    ret = GetFile(TRelPath(AddExtension(TPath(ret->GetRelPath().Path), {""})));
  }
  return ret;
}

TOptTimestamp GetTimestampOutput(TFile *file) {
  assert(file);
  auto ret = file->GetTimestamp();
  if (!file->IsSrc()) {
    ret = Oldest(ret, TryGetTimestamp(GetCacheFilename(file)));
  } else {
    assert(ret);
  }
  return ret;
}

auto GetTimestampInput(TFile *file) {
  return *GetTimestampOutput(file);
}

void TWorkFinder::CacheCheck(TJob *job) {
  assert(job);
  /* For a job to cache-complete, it must meet several requirements
     1. The environmental config must be older than the newest output
     2. The oldest input or need must be newer than the newest output
     3. Every input/need's job must be finished (If they're in out/)
     4. Every output has a cache file which matches the file. Said cache file has an equal or newer timestamp.
     5. Every output's producer job matches this job.

    NOTES:
      - For jobs with unknown outputs, read the set of outputs in from the out file. If we succeed in finding them all,
        set them for the job.
      - A file is considered to have the timstamp of newest('file', 'file.jhm'), if file.jhm exists */

  // We've now checked this job for cache. Mark it so.
  InsertOrFail(Cache, job);

  // TODO: We don't handle removal of a configuration file. Only addition.
  // TODO: There will be a lot of redundant stat() calls on input files... Make TFile hold a timestamp which is set
  // by the cache checking process.

  // Input must be finished.
  // Also: Grab the timestamp while we're at it.
  TFile *in = job->GetInput();
  if (!IsFileDone(in)) {
    return;
  }

  // If config is newer than the source, exit fast.
  auto in_timestamp = GetTimestampInput(in);

  // If the input file is in the source tree, then it's timestamp relative to the config doesn't matter.
  // The newest of the two is considered the file's timestamp for comparison to the output files.
  // NOTE: All job outputs are always not src, so they don't need this check.
  if (in->IsSrc()) {
    in_timestamp = Newest(ConfigTimestamp, in_timestamp);
  } else {
    // If we're in output, we must be newer than the config timestamp, or we need ot be rebuilt
    if (ConfigTimestamp > in_timestamp) {
      return;
    }
  }

  // If the job's command is new / updated, we need to rebuild
  in_timestamp = Newest(job->GetCmdTimestamp(), in_timestamp);

  // For all known outputs (There is always at least one), choose one at random and load it's cache file / treat it as
  // the magic cache entry.
  TFile *base_out = GrabOne(job->GetOutput());

  TConfig ideal_out;
  /* Read the cache as what we want  / need for build_info sections. */ {
    // Read the cache as our ideal cache contents.
    string cache_filename = GetCacheFilename(base_out);
    if (!ExistsPath(cache_filename.c_str())) {
      return;
    }
    ideal_out.LoadComputed(cache_filename);
  }

  // Cache the computed config we've loaded so we only load it once.
  unordered_map<TFile *, vector<Base::TJson>> conf_cache;
  // List of expected outputs
  vector<string> output_filename_list;

  try {
    const string in_name = AsStr(job->GetInput()->GetPath());
    // Check the ideal out matches the job
    if (ideal_out.Read<string>({"build_info","job","name"}) != job->GetName() ||
        ideal_out.Read<string>({"build_info","job","input"}) != in_name ||
        ideal_out.Read<string>({"build_info","config_timestamp"}) != ConfigTimestampStr) {
      return;
    }

    // Every needs must exist, be done. in_timestamp is the newest of all the needs and the input file.
    for (const string &need_filename : ideal_out.Read<vector<string>>({"build_info","job","needs"})) {
      TFile *need = TryGetFileFromPath(need_filename);
      if (!need || !IsFileDone(need)) {
        return;
      }
      in_timestamp = Newest(in_timestamp, GetTimestampInput  (need));
    }

    // Any files which weren't buildable that need to stay not buildable should stay not buildable.
    for (const string &filename : ideal_out.Read<vector<string>>({"build_info","job","anti_needs"})) {
      TFile *anti_need = TryGetFileFromPath(filename);
      if (anti_need && IsBuildable(anti_need)) {
        return;
      }
    }

    output_filename_list = ideal_out.Read<vector<string>>({"build_info","job","output"});

    // The output sets should match. We first check here they're the same size
    // Then in the output loop immediately following this, we ensure that one contains every item in the other.
    if (!job->HasUnknownOutputs()) {
      if (output_filename_list.size() != job->GetOutput().size()) {
        return;
      }
    }


    // Make sure every output is older than the input
    // Also ensure it's basic build info matches.
    for (const string &output_filename : output_filename_list) {
      // TODO: If all the job's outputs are known, iterate over that set rather than trying to infer the filenames from
      // the string representations (Saves us a lot of hassle on execz`utables)
      TFile *output = TryGetOutputFileFromPath(output_filename);
      if (!output) {
        return;
      }
      if (!Util::ExistsPath(AsStr(output->GetPath()).c_str())) {
        return;
      }
      if (IsNewer(in_timestamp, GetTimestampOutput(output))) {
        return;
      }

      // NOTE: Technically all build info should match exactly. But this should be good enough (and faster).
      // Check the ideal out matches the job
      // TODO: Cache the AsStr.
      TConfig output_cache;
      output_cache.LoadComputed(GetCacheFilename(output));
      if (output_cache.Read<string>({"build_info","job","name"}) != job->GetName() ||
          output_cache.Read<string>({"build_info","job","input"}) != in_name) {
        return;
      }

      InsertOrFail(conf_cache, output, output_cache.GetComputed());
    }
  } catch (const TConfig::TNotFound &) {
    // If any of the config files don't contain the requested key(s), then exit / the config must be out of date.
    return;
  }

  // Wohoo! Everything checked out.
  // If the input job doesn't have it's full output set, add it.
  // For every file in the output set, add the cached config
  for (const string &output_filename : output_filename_list) {
    TFile *out_file = TryGetOutputFileFromPath(output_filename);
    if (job->HasUnknownOutputs()) {
      if (!Contains(job->GetOutput(), out_file)) {
        job->AddOutput(out_file);
      }
    }
    out_file->SetComputed(move(conf_cache.at(out_file)));
  }

  if (job->HasUnknownOutputs()) {
    job->MarkAllOutputsKnown();
  }

  // Mark job as finished.
  InsertOrFail(Finished, job);
  // Ensure sure job is part of 'All' (all jobs in Finished must be in All).
  // TODO: Cache finished jobs shouldn't need to be in all (This means that all != finished + ready + waiting);
  All.insert(job);
}

bool TWorkFinder::Queue(TJob *job) {
  // We know about the job. Just look up its status and exit.
  if (Contains(All, job)) {
    return !IsDone(job);
  }

  // New job, add it.
  InsertOrFail(All, job);

  // Queue the job if it isn't done
  if (IsDone(job)) {
    return false;
  } else {
    Ready.push(job);
    return true;
  }
}