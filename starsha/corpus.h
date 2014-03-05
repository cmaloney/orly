/* <starsha/corpus.h>

   TODO

   Copyright 2010-2014 Stig LLC

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#pragma once

#include <cassert>
#include <functional>
#include <ostream>
#include <queue>
#include <string>
#include <time.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <limits.h>

#include <base/no_copy_semantics.h>
#include <base/opt.h>
#include <base/spin_lock.h>
#include <starsha/note.h>
#include <starsha/thread_pool.h>
#include <base/thrower.h>

namespace Starsha {

  /* TODO */
  class TCorpus {
    NO_COPY_SEMANTICS(TCorpus);
    public:

    /* TODO */
    class TFile {
      NO_COPY_SEMANTICS(TFile);
      public:

      /* TODO */
      class TKind {
        NO_COPY_SEMANTICS(TKind);
        public:

        /* TODO */
        virtual ~TKind();

        /* TODO */
        void ForEachExt(const std::function<bool (const char *)> &cb) const;

        /* TODO */
        const std::vector<const TKind *> &GetAutoProducedKinds() const {
          assert(this);
          return AutoProducedKinds;
        }

        /* TODO */
        const char *GetDesc() const {
          assert(this);
          return Desc;
        }

        /* TODO */
        virtual TFile *NewOutFile(TCorpus *corpus, const std::string &rel_path) const = 0;

        /* TODO */
        virtual TFile *NewSrcFile(TCorpus *corpus, const std::string &rel_path, time_t mod_time) const = 0;

        /* TODO */
        static const TKind *GetKind(const char *ext);

        /* TODO */
        static void Initialize();

        /* TODO */
        static const TKind *TryGetKind(const char *ext);

        /* TODO */
        static const TKind *TryGetKindByRelPath(const std::string &rel_path);

        protected:

        /* TODO */
        TKind(const char *desc, const char *exts, const char *auto_produced_exts);

        private:

        /* TODO */
        const char *Desc, *Exts, *AutoProducedExts;

        /* TODO */
        mutable std::vector<const TKind *> AutoProducedKinds;

        /* TODO */
        const TKind *NextKind;

        /* TODO */
        static const char *ConvExt(const char *ext);

        /* TODO */
        static const TKind *FirstKind;

        /* TODO */
        static std::unordered_map<std::string, const TKind *> KindByExt;

      };  // TKind

      /* TODO */
      bool CanBuild() const {
        assert(this);
        return IsSrc || TryGetProducer();
      }

      /* TODO */
      void CollectBuildLeaves(std::unordered_set<TFile *> &leaves);

      /* TODO */
      virtual void CollectImplFiles(std::unordered_set<TFile *> &impl_files);

      /* TODO */
      const std::string &GetAbsPath() const {
        assert(this);
        return AbsPath;
      }

      /* TODO */
      TCorpus *GetCorpus() const {
        assert(this);
        return Corpus;
      }

      /* TODO */
      bool GetIsSrc() const {
        assert(this);
        return IsSrc;
      }

      /* TODO */
      virtual const TKind *GetKind() const = 0;

      /* TODO */
      const Base::TOpt<time_t> &GetModTime() const {
        assert(this);
        return ModTime;
      }

      /* TODO */
      const std::string &GetRelPath() const {
        assert(this);
        return RelPath;
      }

      /* TODO */
      void SetModTime(time_t mod_time) {
        assert(this);
        ModTime = mod_time;
      }

      /* TODO */
      void SetProducer(TFile *producer);

      /* TODO */
      TFile *TryGetProducer() const;

      /* TODO */
      TNote *TryGetToolConfigNote(const char *tool_key) const;

      /* TODO */
      bool TrySetModTime();

      protected:

      /* TODO */
      template <typename TSomeFile>
      class TSpecificKind
          : public TKind {
        NO_COPY_SEMANTICS(TSpecificKind);
        public:

        /* TODO */
        TSpecificKind(const char *desc, const char *exts, const char *auto_produced_exts = 0)
            : TKind(desc, exts, auto_produced_exts) {
          assert(!Kind);
          Kind = this;
        }

        /* TODO */
        virtual TFile *NewOutFile(TCorpus *corpus, const std::string &rel_path) const {
          return new TSomeFile(corpus, rel_path);
        }

        /* TODO */
        virtual TFile *NewSrcFile(TCorpus *corpus, const std::string &rel_path, time_t mod_time) const {
          return new TSomeFile(corpus, rel_path, mod_time);
        }

        /* TODO */
        static const TSpecificKind *GetKind() {
          assert(Kind);
          return Kind;
        }

        /* TODO */
        static TSomeFile *GetOrNewOutFile(TCorpus *corpus, const std::string &rel_path) {
          assert(corpus);
          assert(&rel_path);
          TSomeFile *result;
          TFile *file = corpus->TryGetFile(rel_path);
          if (file) {
            result = dynamic_cast<TSomeFile *>(file);
            if (!result) {
              THROW << '"' << rel_path << "\" is not " << Kind->GetDesc();
            }
          } else {
            result = new TSomeFile(corpus, rel_path);
          }
          return result;
        }

        private:

        /* TODO */
        static const TSpecificKind *Kind;

      };  // TSpecificKind<TSomeFile>

      /* TODO */
      TFile(TCorpus *corpus, const std::string &rel_path);

      /* TODO */
      TFile(TCorpus *corpus, const std::string &rel_path, time_t mod_time);

      /* TODO */
      virtual ~TFile();

      /* TODO */
      virtual bool ForEachPrereq(const std::function<bool (TFile *)> &cb);

      /* TODO */
      virtual void OnExists();

      /* TODO */
      virtual void OnNew();

      /* TODO */
      virtual void Produce(bool run, const std::unordered_set<TFile *> &targets, std::unordered_set<TFile *> &products);

      private:

      /* TODO */
      void Init(TCorpus *corpus);

      /* TODO */
      bool IsSrc;

      /* TODO */
      std::string RelPath;

      /* TODO */
      Base::TOpt<time_t> ModTime;

      /* TODO */
      TCorpus *Corpus;

      /* TODO */
      std::unordered_set<TFile *> Producers;

      /* TODO */
      std::string AbsPath;

      /* TODO */
      friend class TCorpus;

    };  // TFile

    /* TODO */
    TCorpus(const char *config, const char *config_mixin, size_t worker_count);

    /* TODO */
    ~TCorpus();

    //TODO: Targets shuold really be a std::unordered_set...
    /* TODO */
    void Build(const std::vector<TFile *> &targets, const std::function<bool (TFile *file, const std::vector<std::string> &msgs)> &cb);

    /* TODO */
    void DescribeBuild(TFile *target, const std::function<bool (const std::vector<TFile *> &)> &cb);

    /* TODO */
    void ForEachFile(const std::function<bool (TFile *)> &cb) const;

    //TODO: Targets shuold really be a std::unordered_set...
    /* TODO */
    bool ForEachWave(const std::vector<TFile *> &targets, const std::function<bool (const std::unordered_map<TFile *, std::unordered_set<TFile *>> &)> &cb) const;

    /* TODO */
    const std::string &GetBootstrap() const {
      assert(this);
      return Bootstrap;
    }

    /* TODO */
    time_t GetConfigModTime() const {
      assert(this);
      return ConfigModTime;
    }

    /* TODO */
    const TNote *GetConfigNote() const {
      assert(this);
      return ConfigNote;
    }

    /* TODO */
    const std::string &GetCwd() const {
      assert(this);
      return Cwd;
    }

    /* TODO */
    TFile *GetFile(const std::string &rel_path) const;

    /* TODO */
    TNote *GetNote() const {
      assert(this);
      return Note;
    }

    /* TODO */
    const std::string &GetOut() const {
      assert(this);
      return Out;
    }

    /* TODO */
    const std::string &GetRoot() const {
      assert(this);
      return Root;
    }

    /* TODO */
    const std::string &GetSrc() const {
      assert(this);
      return Src;
    }

    /* TODO */
    TFile *TryGetFile(const std::string &rel_path) const;

    /* TODO */
    const char *TryGetRelPath(const char *path) const;

    /* TODO */
    void WriteNote() const;

    /* TODO */
    static std::string &ChangeExt(std::string &out, const std::string &path, const char *new_ext);

    private:

    /* TODO */
    void Process();

    /* TODO */
    TThreadPool ThreadPool;

    /* TODO */
    TNote *Note, *ConfigNote;

    /* TODO */
    time_t ConfigModTime;

    /* TODO */
    std::string Cwd, Root, Src, Out, Bootstrap, HiddenDir;

    /* TODO */
    std::unordered_map<std::string, TFile *> FileByRelPath;

    /* TODO */
    std::queue<TFile *> NewFiles, FilesToScan;

    Base::TSpinLock FilesToScanLock;

    /* TODO */
    std::unordered_set<std::string> ExcludedDirs, IncludedDirs;

    /* TODO */
    static const char *HiddenDirName, *NotesFileName;

  };  // TCorpus

  /* TODO */
  template <typename TSomeFile>
  const TCorpus::TFile::TSpecificKind<TSomeFile> *TCorpus::TFile::TSpecificKind<TSomeFile>::Kind = 0;

  /* TODO */
  inline std::ostream &operator<<(std::ostream &strm, const std::unordered_set<TCorpus::TFile *> &that) {
    assert(&strm);
    assert(&that);
    for (auto file: that) {
      strm << ' ' << file->GetAbsPath();
    }
    return strm;
  }

}  // Starsha