/* <starsha/corpus.cc>

   Implements <starsha/corpus.h>.

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

#include <starsha/corpus.h>

#include <algorithm>
#include <cstring>
#include <fstream>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <base/make_dir.h>
#include <base/os_error.h>
#include <starsha/all_kinds.h>
#include <starsha/walk.h>

using namespace std;
using namespace Base;
using namespace Starsha;

TCorpus::TFile::TKind::~TKind() {}

void TCorpus::TFile::TKind::ForEachExt(const function<bool (const char *)> &cb) const {
  assert(this);
  assert(&cb);
  for (const char *ext = Exts; *ext; ext += strlen(ext) + 1) {
    if (!cb(ConvExt(ext))) {
      break;
    }
  }
}

const TCorpus::TFile::TKind *TCorpus::TFile::TKind::GetKind(const char *ext) {
  auto kind = TryGetKind(ext);
  if (!kind) {
    THROW << "unknown extension \"" << ext << '"';
  }
  return kind;
}

void TCorpus::TFile::TKind::Initialize() {
  if (KindByExt.empty()) {
    for (auto kind = FirstKind; kind; kind = kind->NextKind) {
      for (const char *ext = kind->Exts; *ext; ext += strlen(ext) + 1) {
        if (*ext != '.') {
          THROW << "file extension \"" << ext << "\" doesn't start with a dot";
        }
        if (!KindByExt.insert(pair<string, const TKind *>(ConvExt(ext), kind)).second) {
          THROW << "multiple file kinds with extension \"" << ext << '"';
        }
      }  // for (exts...)
    }  // for (kinds...)
    for (auto kind = FirstKind; kind; kind = kind->NextKind) {
      if (kind->AutoProducedExts) {
        for (const char *ext = kind->AutoProducedExts; *ext; ext += strlen(ext) + 1) {
          kind->AutoProducedKinds.push_back(GetKind(ConvExt(ext)));
        }  // for (produced exts...)
      }  // if (kind->AutoProducedExts)
    }  // for (kinds...)
  }  // if (empty...)
}

const TCorpus::TFile::TKind *TCorpus::TFile::TKind::TryGetKind(const char *ext) {
  assert(ext);
  auto iter = KindByExt.find(ext);
  return (iter != KindByExt.end()) ? iter->second : 0;
}

const TCorpus::TFile::TKind *TCorpus::TFile::TKind::TryGetKindByRelPath(const string &rel_path) {
  assert(&rel_path);
  auto ext = strrchr(rel_path.c_str(), '.');
  return TryGetKind(ext ? ext : "");
}

TCorpus::TFile::TKind::TKind(const char *desc, const char *exts, const char *auto_produced_exts)
    : Desc(desc), Exts(exts), AutoProducedExts(auto_produced_exts) {
  assert(desc);
  assert(exts);
  assert(KindByExt.empty());
  NextKind = FirstKind;
  FirstKind = this;
}

const char *TCorpus::TFile::TKind::ConvExt(const char *ext) {
  assert(ext);
  return (ext[0] == '.' && ext[1] == '\0') ? "" : ext;
}

const TCorpus::TFile::TKind *TCorpus::TFile::TKind::FirstKind = 0;

unordered_map<string, const TCorpus::TFile::TKind *> TCorpus::TFile::TKind::KindByExt;

void TCorpus::TFile::SetProducer(TFile *producer) {
  assert(this);
  assert(producer);
  Producers.insert(producer);
}

TCorpus::TFile *TCorpus::TFile::TryGetProducer() const {
  assert(this);
  if (Producers.empty()) {
    return 0;
  }
  if (IsSrc || Producers.size() > 1) {
    THROW << '"' << RelPath << "\" " << (IsSrc ? "is in the /src tree and " : "") << "has multiple producers: " << Producers;
  }
  return *(Producers.begin());
}

TNote *TCorpus::TFile::TryGetToolConfigNote(const char *tool_key) const {
  assert(this);
  assert(tool_key);
  assert(Corpus->ConfigNote);
  TNote *tool = 0;
  auto files = Corpus->ConfigNote->TryGetChild("files");
  if (files) {
    auto file = files->TryGetChild(RelPath.c_str());
    if (file) {
      tool = file->TryGetChild(tool_key);
    }
  }
  if (!tool) {
    auto tools = Corpus->ConfigNote->TryGetChild("tools");
    if (tools) {
      tool = tools->TryGetChild(tool_key);
    }
  }
  return tool;
}

bool TCorpus::TFile::TrySetModTime() {
  assert(this);
  struct stat st;
  bool success = (stat(AbsPath.c_str(), &st) == 0);
  if (success) {
    ModTime = st.st_mtime;
    Base::TSpinLock::TLock lock(Corpus->FilesToScanLock);
    Corpus->FilesToScan.push(this);
  }
  return success;
}

TCorpus::TFile::TFile(TCorpus *corpus, const string &rel_path)
    : IsSrc(false), RelPath(rel_path) {
  assert(RelPath[0] != '.');
  Init(corpus);
  TrySetModTime();
}

TCorpus::TFile::TFile(TCorpus *corpus, const string &rel_path, time_t mod_time)
    : IsSrc(true), RelPath(rel_path), ModTime(mod_time) {
  assert(RelPath[0] != '.');
  Init(corpus);
  Base::TSpinLock::TLock lock(Corpus->FilesToScanLock);
  Corpus->FilesToScan.push(this);
}

TCorpus::TFile::~TFile() {}

void TCorpus::TFile::CollectBuildLeaves(unordered_set<TFile *> &leaves) {
  assert(this);
  assert(&leaves);
  if (!IsSrc) {
    auto producer = TryGetProducer();
    if (!producer) {
      THROW << '"' << RelPath << "\" has no producer";
    }
    unordered_set<TFile *> local_leaves;
    bool is_up_to_date = ModTime && *ModTime >= Corpus->ConfigModTime;
    producer->ForEachPrereq([this, &local_leaves, &is_up_to_date](TFile *file){
      file->CollectBuildLeaves(local_leaves);
      if (!(file->ModTime) || !ModTime || *(file->ModTime) > *ModTime) {
        is_up_to_date = false;
      }
      return true;
    });
    if (!local_leaves.empty()) {
      leaves.insert(local_leaves.begin(), local_leaves.end());
    } else if (!is_up_to_date) {
      leaves.insert(this);
    }
  }
}

void TCorpus::TFile::CollectImplFiles(unordered_set<TFile *> &) {}

bool TCorpus::TFile::ForEachPrereq(const function<bool (TFile *)> &cb) {
  assert(this);
  return cb(this);
}

void TCorpus::TFile::OnExists() {}

void TCorpus::TFile::OnNew() {
  assert(this);
  string path;
  for (auto kind: GetKind()->GetAutoProducedKinds()) {
    kind->ForEachExt([this, &path, kind](const char *ext){
      auto file = Corpus->TryGetFile(ChangeExt(path, RelPath, ext));
      if (!file) {
        file = kind->NewOutFile(Corpus, path);
      } else if (file->GetKind() != kind) {
        THROW
            << '"' << path << "\" seems to be produced by \"" << RelPath << "\" but is the wrong kind of file; expected "
            << kind->GetDesc() << " but got " << file->GetKind()->GetDesc();
      }
      file->SetProducer(this);
      return true;
    });
  }
}

void TCorpus::TFile::Produce(bool, const unordered_set<TFile *> &targets, unordered_set<TFile *> &) {
  assert(this);
  assert(&targets);
  THROW << '"' << RelPath << "\" cannot be used to produce: " << targets;
}

void TCorpus::TFile::Init(TCorpus *corpus) {
  assert(this);
  assert(corpus);
  if (!corpus->FileByRelPath.insert(make_pair(RelPath, this)).second) {
    THROW << "duplicate file \"" << RelPath << "\" in corpus";
  }
  assert(RelPath[0] != '.');
  corpus->NewFiles.push(this);
  Corpus = corpus;
  char buf[PATH_MAX];
  if(IsSrc) {
    AbsPath = RelPath;
  } else {
    sprintf(buf, "%s/%s", corpus->GetOut().c_str(), RelPath.c_str());
    AbsPath = buf;
  }
}

TCorpus::TCorpus(const char *config, const char *config_mixin, size_t worker_count)
    : ThreadPool(worker_count) {
  assert(config);
  /* Initialize the file kind knowledge base. */
  TFile::TKind::Initialize();
  /* Find the root by finding the highest starsha-specific hidden directory. */
  struct stat st;
  char buf[PATH_MAX];
  if (!getcwd(buf, PATH_MAX)) {
    throw TOsError(HERE);
  }
  Cwd = buf;

  char *end = buf + strlen(buf);
  do {
    strcpy(end, HiddenDirName);
    if (stat(buf, &st) == 0 && S_ISDIR(st.st_mode)) {
      HiddenDir = buf;
      Root.assign(buf, end);
    }
    for (--end; end >= buf && *end != '/'; --end);
  } while (end >= buf);
  if (Root.empty()) {
    THROW << "could not find hidden dir \"" << HiddenDirName << "\" in or above \"" << Cwd << '"';
  }
  /* Read the notes from the hidden directory. */
  /* extra */ {
    string temp = HiddenDir;
    temp += NotesFileName;
    if (stat(temp.c_str(), &st) == 0) {
      ifstream strm(temp.c_str());
      Note = TNote::TryRead(strm);
    } else {
      Note = 0;
    }
  }
  if (!Note) {
    Note = new TNote("root");
    new TNote(Note, "files");
  }
  /* Find /src under the root. */
  strcpy(buf, Root.c_str());
  end = buf + Root.size();
  strcpy(end, "/src");
  if (stat(buf, &st) < 0) {
    THROW << "could not find /src dir \"" << buf << '"';
  }
  Src = buf;

  //TODO: This should go in starsha.cc, but for now we're aiming at functional, not clean.
  // chdir to the src folder so we can always use relative paths
  chdir(Src.c_str());

  /* Find /out/{config}_{config_mixin} under the root. */
  Out = "../out/";
  Out.append(config);
  if (config_mixin) {
    Out.append("_");
    Out.append(config_mixin);
  }
  /* Find /out/bootstrap under the root. */
  strcpy(end, "/out/bootstrap");
  if (stat(buf, &st) < 0) {
    THROW << "could not find /out/bootstrap dir \"" << buf << '"';
  }
  Bootstrap = buf;
  /* Read the config from the hidden directory. */
  /* extra */ {
    string temp = Src;
    temp += "/";
    temp += config;
    temp += ".starsha";
    if (stat(temp.c_str(), &st) < 0) {
      THROW << "could not find config file \"" << temp << '"';
    }
    ConfigModTime = st.st_mtime;
    ifstream strm(temp.c_str());
    ConfigNote = TNote::TryRead(strm);
    if (!ConfigNote) {
      THROW << "could not read config from \"" << temp << '"';
    }
  }

  // Test if the configuration mixin mod time should be used instead of the base config time.
  if(config_mixin) {
    string temp = Src + '/' + config_mixin + ".starsha_mixin";

    if(stat(temp.c_str(), &st) < 0) {
      THROW << "could not find mixin config file \"" << temp << '"';
    }
    ConfigModTime = max(ConfigModTime, st.st_mtime);
    std::ifstream strm(temp.c_str());
    if(!TNote::TryReadAppend(strm, ConfigNote)) {
      THROW << "could not read mixin config from \"" << temp << '"';
    }

    //NOTE: this is hackish.
    // Basically, we could have multiple blocks, read until we run out of them.
    // Really should live in the TryReadAppend function, but putting this there is
    // harder.
    while(TNote::TryReadAppend(strm, ConfigNote));
  }

  ConfigNote->GetChild("exclude")->ForEachChild([this](TNote *child){
    ExcludedDirs.insert(child->GetKey());
    return true;
  });
  ConfigNote->GetChild("include")->ForEachChild([this](TNote *child){
    IncludedDirs.insert(child->GetKey());
    return true;
  });
  /* Add all of the source files. */
  Walk(
    Src.c_str(),
    [this](const char *rel_path){
      for (const string &excluded_dir: ExcludedDirs) {
        if (strncmp(rel_path, excluded_dir.c_str(), excluded_dir.size()) == 0) {
          for (const string &included_dir: IncludedDirs) {
            if (strncmp(rel_path, included_dir.c_str(), included_dir.size()) == 0) {
              return true;
            }  // if  (included...)
          }  // for (included_dirs...)
          return false;
        }  // if  (excluded...)
      }  // for (excluded_dirs...)
      return true;
    },
    [this](const char *rel_path, time_t mod_time) {
      if (strncmp(rel_path, "tools/nycr/nycr", 15)) {
        auto kind = TFile::TKind::TryGetKindByRelPath(rel_path);
        if (kind) {
          kind->NewSrcFile(this, rel_path, mod_time);
        }
      }
    }
  );
  Process();
}

TCorpus::~TCorpus() {
  assert(this);
  for (auto item: FileByRelPath) {
    delete item.second;
  }
  delete Note;
  delete ConfigNote;
}

void TCorpus::Build(const vector<TCorpus::TFile *> &targets, const function<bool (TFile *file, const vector<string> &msgs)> &cb) {
  assert(this);
  assert(&targets);
  assert(&cb);
  ForEachWave(targets, [this, &cb](const unordered_map<TFile *, unordered_set<TFile *>> &targets_by_producer) {
    for (const pair<TFile *, unordered_set<TFile *>> &item : targets_by_producer) {
      TFile *producer = item.first;
      const unordered_set<TFile *> &targets = item.second;
      ThreadPool.Push(make_pair(item.first->RelPath, [this, producer, targets] {
        for (auto target: targets) {
          MakeDirs(target->AbsPath.c_str());
        }
        unordered_set<TFile *> products;
        producer->Produce(true, targets, products);
        for (auto target: targets) {
          if (products.find(target) == products.end()) {
            THROW << '"' << producer->RelPath << "\" was intended to produce \"" << target->RelPath << "\" but it didn't";
          }
        }
        for (auto product: products) {
          if (!product->TrySetModTime()) {
            THROW << '"' << producer->RelPath << "\" claimed to produce \"" << product->RelPath << "\" but it didn't";
          }
        }
      }));
    }  // for(items...)
    ThreadPool.WaitUntilIdle();
    unordered_map<TFile *, vector<string>> msgs_by_file;
    static const vector<string> no_msgs;
    ThreadPool.ForEachErrorMsg([this, &msgs_by_file](const string &rel_path, const string &msg) {
      msgs_by_file.insert(make_pair(GetFile(rel_path), no_msgs)).first->second.push_back(msg);
      return true;
    });
    for (const pair<TFile *, vector<string>> &item: msgs_by_file) {
      if (!cb(item.first, item.second)) {
        return false;
      }
    }
    Process();
    return true;
  });
}

void TCorpus::DescribeBuild(TFile *target, const function<bool (const vector<TFile *> &)> &cb) {
  assert(this);
  assert(target);
  assert(&cb);
  ForEachWave({target}, [&cb](const unordered_map<TFile *, unordered_set<TFile *>> &targets_by_producer) {
    auto fake_mod_time = time(0);
    vector<TFile *> sorted_files;
    for (const pair<TFile *, unordered_set<TFile *>> &item : targets_by_producer) {
      TFile *producer = item.first;
      const unordered_set<TFile *> &targets = item.second;
      unordered_set<TFile *> products;
      producer->Produce(false, targets, products);
      for (auto product: products) {
        product->SetModTime(fake_mod_time);
        sorted_files.push_back(product);
      }
    }
    sort(sorted_files.begin(), sorted_files.end(), [](const TFile *lhs, const TFile *rhs) {
      return lhs->RelPath < rhs->RelPath;
    });
    return cb(sorted_files);
  });
}

void TCorpus::ForEachFile(const function<bool (TFile *)> &cb) const {
  assert(this);
  assert(&cb);
  for (auto item: FileByRelPath) {
    if (!cb(item.second)) {
      break;
    }
  }
}

bool TCorpus::ForEachWave(const std::vector<TFile *> &targets, const function<bool (const unordered_map<TFile *, unordered_set<TFile *>> &)> &cb) const {
  assert(this);
  assert(&targets);
  assert(&cb);
  for (;;) {
    unordered_set<TFile *> leaves;
    for(auto target: targets) {
        assert(target);
      target->CollectBuildLeaves(leaves);
    }
    if (leaves.empty()) {
      break;
    }
    unordered_map<TFile *, unordered_set<TFile *>> targets_by_producer;
    pair<TFile *, unordered_set<TFile *>> item;
    for (auto product: leaves) {
      assert(item.second.empty());
      item.first = product->TryGetProducer();
      targets_by_producer.insert(item).first->second.insert(product);
    }
    if (!cb(targets_by_producer)) {
      return false;
    }
  }
  return true;
}

TCorpus::TFile *TCorpus::GetFile(const string &rel_path) const {
  assert(this);
  auto file = TryGetFile(rel_path);
  if (!file) {
    THROW << '"' << rel_path << "\" does not exist cannot be produced";
  }
  return file;
}

TCorpus::TFile *TCorpus::TryGetFile(const string &rel_path) const {
  assert(this);
  auto iter = FileByRelPath.find(rel_path);
  return (iter != FileByRelPath.end()) ? iter->second : 0;
}

const char *TCorpus::TryGetRelPath(const char *path) const {
  assert(this);
  assert(path);
  if (*path == '/' || *path == '.') {
    if (strncmp(path, Src.c_str(), Src.size()) == 0) {
      path += Src.size() + 1;
    } else if (strncmp(path, Out.c_str(), Out.size()) == 0) {
      path += Out.size() + 1;
    } else {
      path = 0;
    }
  }
  return path;
}

void TCorpus::WriteNote() const {
  assert(this);
  string temp = HiddenDir;
  temp += NotesFileName;
  ofstream strm(temp.c_str());
  Note->Write(strm);
}

string &TCorpus::ChangeExt(string &out, const string &path, const char *new_ext) {
  assert(&out);
  assert(&path);
  assert(new_ext);
  out = path;
  auto dot = out.find_last_of('.');
  if (dot == string::npos) {
    out += new_ext;
  } else {
    out.replace(dot, out.size(), new_ext);
  }
  return out;
}

void TCorpus::Process() {
  assert(this);
  while (!NewFiles.empty() || !FilesToScan.empty()) {
    while (!NewFiles.empty()) {
      NewFiles.front()->OnNew();
      NewFiles.pop();
    }

    if(!FilesToScan.empty()) {
      queue<TFile *> fts;
      /* lock */ {
        Base::TSpinLock::TLock lock(FilesToScanLock);
        fts = std::move(FilesToScan);
      }

      while(!fts.empty()) {
        fts.front()->OnExists();
        fts.pop();
      }
    }
  }
}

const char
  *TCorpus::HiddenDirName = "/.starsha",
  *TCorpus::NotesFileName = "/.notes";
