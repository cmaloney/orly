/* <orly/data/core_vector_generator.h>

   A header file used in generated code for building arbitrary core-vectors
   for importing into orly

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

#pragma once

#include <fcntl.h>
#include <unistd.h>

#include <base/class_traits.h>
#include <base/likely.h>
#include <gz/output_consumer.h>
#include <io/binary_output_only_stream.h>
#include <io/device.h>
#include <orly/atom/core_vector_builder.h>
#include <orly/atom/suprena.h>

namespace Orly {

  namespace Data {

    /* TODO */
    template <typename TKey, typename TVal>
    class TCoreVectorGenerator {
      NO_COPY(TCoreVectorGenerator);
      public:

      /* TODO */
      TCoreVectorGenerator(const std::string &file_name = "out", size_t max_kvs_per_file = 50000)
        : Finalized(false),
          IndexId(Base::TUuid::Twister),
          Count(0UL),
          FileName(file_name),
          FileNum(0UL),
          MaxKVPerFile(max_kvs_per_file) {
        InitBuilder();
      }

      /* TODO */
      ~TCoreVectorGenerator() {
        assert(this);
        if (Count > 0) {
          Flush();
        }
      }

      /* This call is not required. It is a nice way of finding out how many
         files were generated. Once it's called you cannot call Push anymore. */
      size_t Finalize() {
        if (unlikely(Finalized)) {
          throw std::runtime_error("Can not Finalize() a TCoreVectorGenerator a second time");
        }
        if (Count > 0) {
          Flush();
        }
        Finalized = true;
        return FileNum;
      }

      /* TODO */
      void Push(const TKey &key, const TVal &val) {
        if (unlikely(Finalized)) {
          throw std::runtime_error("Can not Push() after Finalize() has been called");
        }
        Builder->Push(Base::TUuid(Base::TUuid::Twister));  // The transaction ID
        Builder->Push(1L);  // The meta-data for the transaction (un-used)
        Builder->Push(1L);  // The number of key-value pairs in this transaction
        Builder->Push(IndexId);  // the index id for the following key-value pair
        Builder->Push(key);  // the key, must be a tuple
        Builder->Push(val); // the value (can be any orly convertible c++ type, if it's an object then the meta-data is required. see example generated file)
        ++Count;
        if (Count == MaxKVPerFile) {
          Flush();
        }
      }

      private:

      /* TODO */
      void Flush() {
        assert(Count > 0);
        std::stringstream ss;
        ss << FileName << "_" << ++FileNum << ".bin.gz";
        const std::string fname = ss.str();
        Io::TBinaryOutputOnlyStream strm(std::make_shared<Gz::TOutputConsumer>(fname.c_str(), "w"));
        //Io::TBinaryOutputOnlyStream strm(std::make_shared<Io::TDevice>(open(fname.c_str(), O_WRONLY | O_CREAT, 0777)));
        // Here we adjust the transaction count post-hoc. It's the first entry in the core-vector file
        assert(!Builder->GetCores().empty());
        // it's safe to do a const_cast here because we know the first core is a direct-storage int64_t
        Atom::TCore &tc_core = const_cast<Atom::TCore &>(Builder->GetCores().front());
        Atom::TSuprena suprena; // required for core construction, but unused
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());  // required for core construction, but unused
        tc_core = Atom::TCore(Count, &suprena, state_alloc);
        Builder->Write(strm);
        printf("File [%s] has [%ld] trans\n", fname.c_str(), Count);
        InitBuilder();
      }

      /* TODO */
      void InitBuilder() {
        Count = 0UL;
        Builder = std::make_unique<Atom::TCoreVectorBuilder>();
        const int64_t tmp_num_trans = 0UL;
        Builder->Push(tmp_num_trans);  // number of transactions dummy. We'll replace this when we know the actual amount
        Builder->Push(tmp_num_trans);  // this is a meta-data field. Currently un-used
      }

      bool Finalized;

      /* TODO */
      Base::TUuid IndexId;

      /* TODO */
      int64_t Count;

      /* TODO */
      const std::string FileName;
      size_t FileNum;

      /* TODO */
      const int64_t MaxKVPerFile;

      /* TODO */
      std::unique_ptr<Atom::TCoreVectorBuilder> Builder;

    };  // TCoreVectorGenerator

  }  // Data

}  // Orly
