/* RunTest [.test] (Executable) -> [.test.out]

   Copyright 2017 Theoretical Chaos. */
#pragma once

#include <bit/job.h>

namespace Bit {

namespace Jobs {

class TRunTest final : public TJob {
    public:
    static TJobProducer GetProducer(const TJobConfig &job_config);

    virtual TOutput Run(TFileEnvironment *file_environment) final;
    virtual std::string GetConfigId() const final;
    virtual std::unordered_map<TFileInfo *, TJobConfig> GetOutputExtraData() const final;

    private:
    TRunTest(TMetadata &&metadata, const TJobConfig *job_config);
};

}  // Job

}  // Bit
