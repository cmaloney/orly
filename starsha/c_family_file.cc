/* <starsha/c_family_file.cc>

   Implements <starsha/c_family_file.h>.

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

#include <starsha/c_family_file.h>

#include <cstring>
#include <iostream>
#include <sstream>

#include <starsha/hdr_file.h>
#include <starsha/obj_file.h>
#include <starsha/runner.h>
#include <starsha/string_builder.h>

using namespace std;
using namespace Base;
using namespace Starsha;

void TCFamilyFile::CollectImplFiles(unordered_set<TFile *> &impl_files) {
  assert(this);
  assert(&impl_files);
  for (auto hdr_file: HdrFiles) {
    auto impl_file = hdr_file->TryGetBuildableImplFile();
    if (impl_file) {
      impl_file->CollectImplFiles(impl_files);
    }
  }
}

TCFamilyFile::~TCFamilyFile() {}

bool TCFamilyFile::ForEachPrereq(const function<bool (TFile *)> &cb) {
  assert(this);
  assert(&cb);
  if (!cb(this)) {
    return false;
  }
  for (auto hdr_file: HdrFiles) {
    if (!cb(hdr_file)) {
      return false;
    }
  }
  return true;
}

void TCFamilyFile::OnExists() {
  assert(this);
  if (GetModTime()) {
    if (!ReadNote()) {
      /* There is no note for us, so we'll have to build one.  This involves getting gcc to scan this file and report back all the headers this file
         includes.  This takes time, which is why we keep the note if we can. */
      TNote *note = GetCorpus()->GetNote()->GetChild("files")->ReplaceOrNewChild(GetRelPath().c_str());
      note->SetChildValueAs("mod_time", *GetModTime());
      note = note->GetOrNewChild("includes");
      vector<string> lines;
      /* extra: build and run the command, capturing its output lines */ {
        string cmd_line;
        TStringBuilder(cmd_line)
            << Cmd << Flags << " -M -MG -I" << GetCorpus()->GetSrc() << " -I" << GetCorpus()->GetOut()
            << " -D'SRC_ROOT=\"" << GetCorpus()->GetSrc() << "/\"' " << GetAbsPath();
        Run(cmd_line, lines);
      }
      /* Join all the lines together into a single text.  Why?  Because gcc is actually sending us a single, long line, but it is breaking it into
         continuations in order to keep it under an 80-col limit.  This niceness on its part is making our lives more complicated. */
      string text;
      for (const string &line: lines) {
        /* If the line ends with a backslash, omit the backslash. */
        const char
            *start = line.data(),
            *end   = start + line.size(),
            *sniff = end - 1;
        if (*sniff == '\\') {
          end = sniff;
        }
        text.append(start, end - start);
      }
      /* What's this?  A tokenizer?  Why yes!  This project has compiler parts all over the place. */
      char
          *csr = const_cast<char *>(text.c_str()),
          *end = csr + text.size(),
          *token = nullptr;
      /* Loop through the rest of the string, picking off tokens. */
      bool skipping = true;
      size_t token_count = 0;
      for (;;) {
        int c = (csr < end) ? *csr : -1;
        if (skipping) {
          /* We're skipping whitespace. */
          if (c == -1) {
            /* ... and we're spent. */
            break;
          }
          if (isgraph(c)) {
            /* Found the start of a token.  Switch modes. */
            token = csr;
            skipping = false;
          } else {
            /* Found more whitespace. */
            ++csr;
          }
        } else {
          /* We're collecting a token. */
          if (isgraph(c)) {
            /* Found more of the token. */
            ++csr;
          } else {
            /* Found the end of the token. */
            *csr = '\0';
            /* The first two tokens are cruft.  Just ignore them. */
            if (++token_count >= 3) {
              /* If the included file exists, we get the abs path to it.  It might be under /src or /out, or it might not be part of the corpus.
                 If the included file doesn't exist, we get the rel path.  This we can consider part of the corpus.
                 Whatever it is, try to convert the token into a rel path. */
              const char *rel_path = GetCorpus()->TryGetRelPath(token);
              if (rel_path) {
                /* We found a rel path to an included header. */
                HdrFiles.push_back(TSpecificKind<THdrFile>::GetOrNewOutFile(GetCorpus(), rel_path));
                new TNote(note, rel_path);
              }
            }
            /* Done with this token.  Switch modes. */
            skipping = true;
          }
        }  // if (skipping) else
      }  // for (;;)
    }  // if (!ReadNote())
  }  // if (GetModTime())
}

void TCFamilyFile::Init() {
  assert(this);
  ObjFile = 0;
  auto tool = TryGetToolConfigNote(Cmd);
  if (tool) {
    auto flags = tool->TryGetChild("flags");
    if (flags) {
      ostringstream strm;
      flags->ForEachChild([&strm](const TNote *child) {
        strm << ' ' << child->GetKey();
        return true;
      });
      Flags = strm.str();
    }
  }
}

void TCFamilyFile::Produce(bool run, const unordered_set<TFile *> &targets, unordered_set<TFile *> &products) {
  assert(this);
  assert(&targets);
  assert(&products);
  if (targets.size() != 1) {
    THROW << '"' << GetRelPath() << "\" must produce one target at a time";
  }
  if (run) {
    TFile *target = *targets.begin();
    assert(target);
    string cmd_line;
    TStringBuilder(cmd_line)
        << Cmd << Flags << " -c -I" << GetCorpus()->GetSrc() << " -I" << GetCorpus()->GetOut() << " -o" << target->GetAbsPath()
        << " -D'SRC_ROOT=\"" << GetCorpus()->GetSrc() << "/\"' " << GetAbsPath();
    vector<string> lines;
    Run(cmd_line, lines);
  }
  products = targets;
}

bool TCFamilyFile::ReadNote() {
  assert(this);
  TNote *note = GetCorpus()->GetNote()->GetChild("files")->TryGetChild(GetRelPath().c_str());
  if (!note) {
    return false;
  }
  time_t mod_time;
  note->GetChildValueAs("mod_time", mod_time);
  if (mod_time < *GetModTime()) {
    return false;
  }
  note->GetChild("includes")->ForEachChild([this](TNote *child){
    HdrFiles.push_back(TSpecificKind<THdrFile>::GetKind()->GetOrNewOutFile(GetCorpus(), child->GetKey()));
    return true;
  });
  return true;
}
