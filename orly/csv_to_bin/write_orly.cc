/* <orly/csv_to_bin/write_orly.cc>

   Implements <orly/csv_to_bin/write_orly.h>.

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

#include <orly/csv_to_bin/write_orly.h>

using namespace std;
using namespace Orly::CsvToBin;
using namespace Orly::CodeGen;

inline bool KeyRequiresStruct(const Symbol::TTable *table, const Symbol::TKey *key) {
  return table->NumColNotCoveredByKey(key) > 0;
}

void WriteFuncParam(TCppPrinter &strm, const Symbol::TCol *col) {
  assert(&strm);
  assert(col);
  strm << col->GetName() << " = given::(";
  PrintOrlyType(strm, col->GetType(), col->IsNull());
  strm << ");" << Eol;
}

void WriteKeyInsert(TCppPrinter &strm, const Symbol::TTable *table, const Symbol::TKey *key, const char *name) {
  assert(&strm);
  assert(table);
  assert(key);
  assert(name);
  const auto &fields = key->GetFields();
  assert(!fields.empty());
  strm << "new <[\"" << name << "\", ";
  size_t pos = 0UL;
  for (const auto &field : fields) {
    if (field.Order == Symbol::TKey::TOrder::Desc) {
      strm << "desc ";
    }
    strm << field.Col->GetName();
    if (pos < fields.size() - 1) {
      strm << ", ";
    }
    ++pos;
  }
  strm << "]> <- ";
  if (KeyRequiresStruct(table, key)) {
    WriteValObjectForKey(strm, table, key);
  } else {
    strm << "true";
  }
  strm << ";" << Eol;
}

void WriteInsertFunc(TCppPrinter &strm, const Symbol::TTable *table) {
  assert(&strm);
  assert(table);
  strm << "NewRow = ((true) effecting {" << Eol;
  /* insert function effecting indent. */ {
    TIndent ind(strm);
    // primary index insert
    const Symbol::TKey *pk = table->GetPrimaryKey();
    WriteKeyInsert(strm, table, pk, "Primary");
    // secondary index inserts
    table->ForEachSecondaryKey([&](const Symbol::TSecondaryKey *key) -> bool {
      WriteKeyInsert(strm, table, key, key->GetName().c_str());
      return true;
    });
  }
  strm << "}) where {" << Eol;
  /* insert function where indent. */ {
    TIndent ind(strm);
    table->ForEachCol([&](const Symbol::TCol *col) -> bool { WriteFuncParam(strm, col); return true; });
  }
  strm << "};" << Eol;
}

void WriteGetterFuncsForKey(TCppPrinter &strm, const Symbol::TTable *table, const Symbol::TKey *key, const char *name) {
  assert(&strm);
  assert(table);
  assert(key);
  const auto &fields = key->GetFields();
  strm << "GetRowBy__" << Base::Join(fields,
                        "__",
                        [](TCppPrinter &out, const Symbol::TKey::TField &field) {
                          out << field.Col->GetName();
                        }) << " = (<{";
  /* the values from the key */
  strm << Base::Join(fields,
                     ",\n      ",
                     [](TCppPrinter &out, const Symbol::TKey::TField &field) {
                       out << "." << field.Col->GetName() << ": " << field.Col->GetName();
                     }
                     );
  assert(fields.size());
  /* the values from the value */
  table->ForEachColNotInKey([&](const Symbol::TCol *col) -> bool {
    strm << ",\n      ." << col->GetName() << ": val_obj." << col->GetName();
    return true;
  }, key);
  strm << "}>) where {" << Eol;
  /* sub-func indent */ {
    TIndent ind(strm);
    for (const auto &field : fields) {
      WriteFuncParam(strm, field.Col);
    }
    if (KeyRequiresStruct(table, key)) {
      strm << "val_obj = *<[\"" << name << "\", "
        << Base::Join(fields,
                      ", ",
                      [](TCppPrinter &out, const Symbol::TKey::TField &field) {
                        if (field.Order == Symbol::TKey::TOrder::Desc) {
                          out << "desc ";
                        }
                        out << field.Col->GetName();
                      })
       << "]>::(" << name << "_obj);" << Eol;
    }
  }
  strm << "};" << Eol << Eol;
  /* provide a way to scan */
  for (size_t ncol = 0; ncol < fields.size(); ++ncol) {
    strm << "Get" << name << "Rows" << (ncol > 0 ? "By" : "");
    for (size_t i = 0; i < ncol; ++i) {
      strm << "__" << fields[i].Col->GetName();
    }
    strm << " = (<{";
    // fields from the key
    for (size_t i = 0; i < fields.size(); ++i) {
      if (i > 0) {
        strm << ", " << Eol << "      ";
      }
      strm << "." << fields[i].Col->GetName() << ": k." << (i + 1);
    }
    // fields from the value
    table->ForEachColNotInKey([&](const Symbol::TCol *col) -> bool {
      strm << "," << Eol << "      ." << col->GetName() << ": *k::(" << name << "_obj)." << col->GetName();
      return true;
    }, key);
    strm << "}> as [row_obj]) where {" << Eol;
    /* sub-func indent */ {
      TIndent ind(strm);
      for (size_t i = 0; i < ncol; ++i) {
        WriteFuncParam(strm, fields[i].Col);
      }
      strm << "k = keys (";
      if (KeyRequiresStruct(table, key)) {
        strm << name << "_obj";
      } else {
        strm << "bool";
      }
      strm << ") @ <[\"" << name << "\"";
      size_t p = 0;
      /* defined */
      for (;p < ncol; ++p) {
        const auto &field = fields[p];
        strm << ", ";
        if (field.Order == Symbol::TKey::TOrder::Desc) {
          strm << "desc ";
        }
        strm << fields[p].Col->GetName();
      }
      /* free */
      for (;p < fields.size(); ++p) {
        const auto &field = fields[p];
        strm << ", ";
        if (field.Order == Symbol::TKey::TOrder::Desc) {
          strm << "desc ";
        }
        strm << "free::(";
        PrintOrlyType(strm, field.Col->GetType(), field.Col->IsNull());
        strm << ")";
      }
      strm << "]>;" << Eol;
    }
    strm << "};" << Eol << Eol;
  }
}

void Orly::CsvToBin::WriteOrly(TCppPrinter &strm, const Symbol::TTable *table) {
  assert(&strm);
  assert(table);
  strm << "package #1;" << Eol << Eol;
  // write out row object type
  WriteValObjectTypeForRow(strm, table);
  // write out primary index val type
  const Symbol::TKey *pk = table->GetPrimaryKey();
  if (KeyRequiresStruct(table, pk)) {
    WriteValObjectTypeForKey(strm, table, pk, "Primary");
  }
  // write out secondary index val types
  table->ForEachSecondaryKey([&](const Symbol::TSecondaryKey *key) -> bool {
    if (KeyRequiresStruct(table, key)) {
      WriteValObjectTypeForKey(strm, table, key, key->GetName().c_str());
    }
    return true;
  });
  /* write out the insert functions */
  WriteInsertFunc(strm, table);
  /* write out the get functions */
  WriteGetterFuncsForKey(strm, table, pk, "Primary");
  table->ForEachSecondaryKey([&](const Symbol::TSecondaryKey *key) -> bool {
    WriteGetterFuncsForKey(strm, table, key, key->GetName().c_str());
    return true;
  });
}

void Orly::CsvToBin::WriteValObjectTypeForRow(CodeGen::TCppPrinter &strm, const Symbol::TTable *table) {
  assert(&strm);
  assert(table);
  strm << "row_obj is <{" << Eol;
  TIndent ind(strm);
  const size_t num_vals = table->GetColCount();
  size_t pos = 0;
  table->ForEachCol([&](const Symbol::TCol *col) -> bool {
    strm << "." << col->GetName() << ": ";
    PrintOrlyType(strm, col->GetType(), col->IsNull());
    if (pos < num_vals - 1) {
      strm << "," << Eol;
    }
    ++pos;
    return true;
  });
  strm << "}>;" << Eol << Eol;
}

void Orly::CsvToBin::WriteValObjectTypeForKey(TCppPrinter &strm, const Symbol::TTable *table, const Symbol::TKey *key, const char *name) {
  assert(&strm);
  assert(table);
  assert(key);
  assert(name);
  strm << name << "_obj is <{" << Eol;
  TIndent ind(strm);
  const size_t num_vals = table->NumColNotCoveredByKey(key);
  size_t pos = 0;
  table->ForEachColNotInKey([&](const Symbol::TCol *col) -> bool {
    strm << "." << col->GetName() << ": ";
    PrintOrlyType(strm, col->GetType(), col->IsNull());
    if (pos < num_vals - 1) {
      strm << "," << Eol;
    }
    ++pos;
    return true;
  }, key);
  strm << "}>;" << Eol << Eol;
}

void Orly::CsvToBin::WriteValObjectForKey(TCppPrinter &strm, const Symbol::TTable *table, const Symbol::TKey *key) {
  assert(&strm);
  assert(table);
  assert(key);
  strm << "<{";
  const size_t num_vals = table->NumColNotCoveredByKey(key);
  size_t pos = 0;
  table->ForEachColNotInKey([&](const Symbol::TCol *col) -> bool {
    if (pos > 0) {
      strm << "      ";
    }
    strm << "." << col->GetName() << ": " << col->GetName();
    if (pos < num_vals - 1) {
      strm << ", " << Eol;
    }
    ++pos;
    return true;
  }, key);
  strm << "}>";
}

void Orly::CsvToBin::PrintOrlyType(CodeGen::TCppPrinter &strm, Symbol::TType type, bool is_null) {
  assert(&strm);
  assert(&type);
  switch (type) {
    case Symbol::TType::Bool: {
      strm << "bool";
      break;
    }
    case Symbol::TType::Id: {
      strm << "id";
      break;
    }
    case Symbol::TType::Int: {
      strm << "int";
      break;
    }
    case Symbol::TType::Real: {
      strm << "real";
      break;
    }
    case Symbol::TType::Str: {
      strm << "str";
      break;
    }
    case Symbol::TType::TimePnt: {
      strm << "time_pnt";
      break;
    }
  }
  if (is_null) {
    strm << "?";
  }
}
