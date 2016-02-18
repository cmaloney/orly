#include <bit/jobs/link.h>

#include <chrono>
#include <queue>

#include <base/as_str.h>
#include <base/not_implemented.h>
#include <bit/file_info.h>
#include <bit/job_producer.h>
#include <util/stl.h>

using namespace Bit;
using namespace Bit::Jobs;
using namespace std;

static unordered_set<TRelPath> GetOutputName(const TRelPath &input) {
  return {input.SwapExtension(".o", "")};
}

static Base::TOpt<TRelPath> TryGetInputName(const TRelPath &output) {
  return output.AddExtension(".o");
}

TJobProducer TLink::GetProducer(const TJobConfig &job_config) {
  return TJobProducer{"link",
                      TFileType::Object,
                      {TFileType::Executable},
                      TryGetInputName,
                      GetOutputName,
                      // TODO: Should be able to eliminate the lambda wrapper here...
                      [job_config](TJob::TMetadata &&metadata) -> unique_ptr<TJob> {
                        return unique_ptr<TLink>(new TLink(std::move(metadata), &job_config));
                      }};
}

TJob::TOutput TLink::Run(TFileEnvironment *) {
  // Seed the link search with the core input file.
  if (ObjFiles.empty()) {
    TFileInfo *input = GetInput();
    ObjToCheck.insert(input);
    ObjFiles.insert(input);
  }

  // We build a queue so we can fill ObjToCheck as we go, and add more to be processed while we
  // iterate.
  std::queue<TFileInfo *> to_check;
  for (TFileInfo *obj : ObjToCheck) {
    to_check.push(obj);
  }
  ObjToCheck.clear();

  // Get all the link objects of every link object until we have a complete set of link objects.
  vector<string> filtered_includes;  // Hoisted out of loop
  while (!to_check.empty()) {
    TFileInfo *obj = Util::Pop(to_check);
    if (!obj->IsComplete())
      ObjToCheck.insert(obj);
    continue;
  }

  // TODO(cmaloney): This section isn't written out yet by any jobs. Should
  // ideally use the .o "link" argument section rather than encoding C++
  // specific logic here for linking. Make "compile .c" automatically output
  // the list of files to link against.
  NOT_IMPLEMENTED()
  /*
  // Read out the cached link args, add them to our link set.
  if(!obj->GetConfig().TryRead({"c++", "filtered_includes"}, filtered_includes)) {
    continue;
  }

  for(const auto &include : filtered_includes) {
    TFileInfo *include_file = file_env.GetFileInfo(file_env.TryGetRelPath(include));
    Env.TryGetFileFromPath(include);
    assert(include_file);
    if(!include_file) {
      THROWER(std::logic_error) << "Internal Error; We didn't find the C++ source file which "
                                       "should be in the src tre...";
    }
    TFileInfo *obj_file = file_env.GetFileInfo(include_file->RelPath.SwapExtension)
        Env.GetFile(TRelPath(SwapExtension(TPath(include_file->GetRelPath().Path), {"o"})));
    if(Env.IsBuildable(obj_file)) {
      // Add the link. If it's new, queue it to be checked for new links that we need
      if(ObjFiles.insert(obj_file).second) {
        to_check.push(obj_file);
      }
    } else {
      AntiNeeds.insert(obj_file);
    }
  }
  */

  // return ObjFiles;
}

string TLink::GetConfigId() const {
  // TODO(cmaloney): Make stable.
  static const auto now = std::chrono::system_clock::to_time_t(chrono::system_clock::now());
  return Base::AsStr(ctime(&now));
}

std::unordered_map<TFileInfo *, TJobConfig> TLink::GetOutputExtraData() const { return {}; }

TLink::TLink(TMetadata &&metadata, const TJobConfig *) : TJob(move(metadata)) {}
