/* <orly/csv_to_bin/write_cc.cc>

   Implements <orly/csv_to_bin/write_cc.h>.

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

#include <orly/csv_to_bin/write_cc.h>

using namespace std;
using namespace Orly::CsvToBin;

bool KeyRequiresStruct(const Symbol::TTable *table, const Symbol::TKey *key) {
  return table->NumColNotCoveredByKey(key) > 0;
}

void WritePushKey(ostream &strm, const Symbol::TTable *table, const Symbol::TKey *key, const char *name) {
  strm << "cvg_" << name << ".Push(TKey" << name << "(\"" << name << "\", ";
  size_t pos = 0;
  const auto &fields = key->GetFields();
  assert(fields.size());
  for (const auto &field : fields) {
    if (field.Order == Symbol::TKey::Desc) {
      strm << "TDesc<";
      PrintType(strm, field.Col->GetType());
      strm << ">";
    }
    strm << "(v_" << field.Col->GetName() << ")";
    if (pos < fields.size() - 1) {
      strm << ", ";
    }
    ++pos;
  }
  strm << "), ";
  if (KeyRequiresStruct(table, key)) {
    strm << "TVal" << name << "(";
    const size_t num_vals = table->NumColNotCoveredByKey(key);
    pos = 0UL;
    table->ForEachColNotInKey([&](const Symbol::TCol *col) -> bool {
      strm << "v_" << col->GetName();
      if (pos < num_vals - 1) {
        strm << ", ";
      }
      ++pos;
      return true;
    }, key);
    strm << ")";
  } else {
    strm << "true";
  }
  strm << ");" << endl;
}

void WriteForEachRecord(ostream &strm, const Symbol::TTable *table) {
  table->ForEachCol([&](const Symbol::TCol *col) -> bool {
    PrintType(strm, col->GetType());
    strm << " v_" << col->GetName() << ";" << endl;
    return true;
  });
  strm << "while (input.AtRecord()) {" << endl
    << "input >> StartOfRecord;" << endl;
  table->ForEachCol([&](const Symbol::TCol *col) -> bool {
    strm << "CheckAtField(input);" << endl
      << "input >> StartOfField >> v_" << col->GetName() << " >> EndOfField;" << endl;
    return true;
  });
  // push the primary key
  const Symbol::TKey *pk = table->GetPrimaryKey();
  WritePushKey(strm, table, pk, "Primary");
  // push the secondary keys
  table->ForEachSecondaryKey([&](const Symbol::TSecondaryKey *key) -> bool {
    WritePushKey(strm, table, key, key->GetName().c_str());
    return true;
  });
  strm << "input >> EndOfRecord;" << endl
    << "}" << endl;
}

void Orly::CsvToBin::WriteCc(ostream &strm, const Symbol::TTable *table) {
  assert(&strm);
  assert(table);
  strm
      << "#include <orly/csv_to_bin/level3.h>" << endl
      << "#include <orly/csv_to_bin/symbol/kit.h>" << endl
      << "#include <orly/csv_to_bin/translate.h>" << endl
      << "#include <orly/data/core_vector_generator.h>" << endl
      << "#include <orly/desc.h>" << endl << endl
      << "using namespace Orly;" << endl
      << "using namespace Orly::CsvToBin;" << endl
      << "using namespace Orly::Data;" << endl;
  strm << "inline void CheckAtField(TLevel3 &input) {" << endl
    << "  if (unlikely(!input.AtField())) {" << endl
    << "    throw std::runtime_error(\"SQL statement does not match csv input\");" << endl
    << "  }" << endl
    << "}" << endl;
  // write out the primary key's Val struct (if required)
  const Symbol::TKey *pk = table->GetPrimaryKey();
  if (KeyRequiresStruct(table, pk)) {
    WriteValRecordForKey(strm, table, pk, "Primary");
  }
  DefKeyTupleType(strm, pk, "Primary");
  // write out the secondary keys' Val structs
  table->ForEachSecondaryKey([&](const Symbol::TSecondaryKey *key) -> bool {
    if (KeyRequiresStruct(table, key)) {
      WriteValRecordForKey(strm, table, key, key->GetName().c_str());
    }
    DefKeyTupleType(strm, key, key->GetName().c_str());
    return true;
  });
  strm << "void TTranslate::Translate(TLevel3 &input) const {" << endl;
  auto gen_cvg = [&](const Symbol::TKey *key, const char *name) {
    stringstream ss;
    ss << "TVal" << name;
    const std::string val_name = ss.str();
    strm << "std::stringstream ofname_" << name << ";" << endl
      << "ofname_" << name << " << OutPrefix << \"_\" << Index << \"" << name << "\";" << endl
      << "TCoreVectorGenerator<TKey" << name << ", " << (KeyRequiresStruct(table, key) ? val_name.c_str() : "bool") << "> cvg_" << name << "(ofname_" << name << ".str(), MaxKvPerFile);" << endl;
  };
  gen_cvg(pk, "Primary");
  table->ForEachSecondaryKey([&](const Symbol::TSecondaryKey *key) -> bool {
    gen_cvg(key, key->GetName().c_str());
    return true;
  });
  strm << "input >> StartOfFile;" << endl;
  WriteForEachRecord(strm, table);
  strm << "input >> EndOfFile;" << endl
    << "}" << endl;
}

void Orly::CsvToBin::WriteValRecordForKey(std::ostream &strm, const Symbol::TTable *table, const Symbol::TKey *key, const char *name) {
  assert(&strm);
  assert(table);
  assert(key);
  assert(name);
  std::vector<const Symbol::TCol *> col_vec;
  table->ForEachColNotInKey([&](const Symbol::TCol *col) -> bool {
    col_vec.emplace_back(col);
    return true;
  }, key);
  strm << "struct TVal" << name << " final {" << endl;
  strm << "TVal" << name << "(";
  for (size_t i = 0; i < col_vec.size(); ++i) {
    const Symbol::TCol *col = col_vec[i];
    strm << "const ";
    PrintType(strm, col->GetType());
    strm << " &v" << i;
    if (i < col_vec.size() - 1) {
      strm << ", ";
    }
  }
  strm << ") : ";
  for (size_t i = 0; i < col_vec.size(); ++i) {
    const Symbol::TCol *col = col_vec[i];
    strm << col->GetName() << "(v" << i << ")";
    if (i < col_vec.size() - 1) {
      strm << ", ";
    }
  }
  strm << " {}" << endl;
  for (auto col : col_vec) {
    PrintType(strm, col->GetType());
    strm << " " << col->GetName() << ";" << endl;
  }
  strm << "};" << endl;
  for (auto col : col_vec) {
    strm << "RECORD_ELEM(TVal" << name << ", ";
    PrintType(strm, col->GetType());
    strm << ", " << col->GetName() << ");" << endl;
  }
}

void Orly::CsvToBin::WriteKeyTupleType(std::ostream &strm, const Symbol::TKey *key) {
  assert(&strm);
  assert(key);
  const auto &fields = key->GetFields();
  assert(!fields.empty());
  strm << "std::tuple<std::string, ";
  for (size_t i = 0; i < fields.size(); ++i) {
    bool desc = fields[i].Order == Symbol::TKey::Desc;
    if (desc) {
      strm << "TDesc<";
    }
    PrintType(strm, fields[i].Col->GetType());
    if (desc) {
      strm << ">";
    }
    if (i < fields.size() - 1) {
      strm << ", ";
    }
  }
  strm << ">";
}

void Orly::CsvToBin::DefKeyTupleType(std::ostream &strm, const Symbol::TKey *key, const char *name) {
  assert(&strm);
  assert(key);
  assert(name);
  strm << "using TKey" << name << " = ";
  WriteKeyTupleType(strm, key);
  strm << ";" << endl;
}

void Orly::CsvToBin::PrintType(std::ostream &strm, Symbol::TType type) {
  switch (type) {
    case Symbol::TType::Bool: {
      strm << "bool";
      break;
    }
    case Symbol::TType::Id: {
      strm << "Base::TUuid";
      break;
    }
    case Symbol::TType::Int: {
      strm << "int64_t";
      break;
    }
    case Symbol::TType::Real: {
      strm << "double";
      break;
    }
    case Symbol::TType::Str: {
      strm << "std::string";
      break;
    }
    case Symbol::TType::TimePnt: {
      strm << "Base::Chrono::TTimePnt";
      break;
    }
    case Symbol::TType::TimeDiff: {
      throw std::runtime_error("This type needs to be remove");
    }
  }
}
