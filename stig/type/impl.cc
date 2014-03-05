/* <stig/type/impl.cc>

   Implements <stig/type/impl.h>.

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


#include <stig/type/impl.h>

#include <sstream>

#include <stig/type/addr.h>
#include <stig/type/dict.h>
#include <stig/type/err.h>
#include <stig/type/func.h>
#include <stig/type/list.h>
#include <stig/type/mutable.h>
#include <stig/type/obj.h>
#include <stig/type/opt.h>
#include <stig/type/seq.h>
#include <stig/type/set.h>
#include <stig/type/unknown.h>

namespace Stig {

  namespace Type {

    /* TODO */
    template <typename TLhs>
    class TRhsVisitor
        : public TType::TVisitor {
      public:

      /* TODO */
      TRhsVisitor(const TLhs *lhs, const TType::TDoubleVisitor &double_visitor)
          : Lhs(lhs), DoubleVisitor(double_visitor) {}

      /* TODO */
      virtual void operator()(const TAddr *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const TAny *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const TBool *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const TDict *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const TErr *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const TFunc *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const TId *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const TInt *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const TList *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const TMutable *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const TObj *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const TOpt *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const TReal *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const TSeq *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const TSet *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const TStr *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const TTimeDiff *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const TTimePnt *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }
      virtual void operator()(const TUnknown *rhs) const {
        DoubleVisitor(Lhs, rhs);
      }

      private:

      /* TODO */
      const TLhs *Lhs;

      /* TODO */
      const TType::TDoubleVisitor &DoubleVisitor;

    };  // TRhsVisitor<TLhs>

    /* TODO */
    class TLhsVisitor
        : public TType::TVisitor {
      public:

      /* TODO */
      TLhsVisitor(const TType &rhs, const TType::TDoubleVisitor &double_visitor)
          : Rhs(rhs), DoubleVisitor(double_visitor) {}

      virtual ~TLhsVisitor() {}

      /* TODO */
      virtual void operator()(const TAddr *lhs) const {
        Rhs.Accept(TRhsVisitor<TAddr>(lhs, DoubleVisitor));
      }
      virtual void operator()(const TAny *lhs) const {
        Rhs.Accept(TRhsVisitor<TAny>(lhs, DoubleVisitor));
      }
      virtual void operator()(const TBool *lhs) const {
        Rhs.Accept(TRhsVisitor<TBool>(lhs, DoubleVisitor));
      }
      virtual void operator()(const TDict *lhs) const {
        Rhs.Accept(TRhsVisitor<TDict>(lhs, DoubleVisitor));
      }
      virtual void operator()(const TErr *lhs) const {
        Rhs.Accept(TRhsVisitor<TErr>(lhs, DoubleVisitor));
      }
      virtual void operator()(const TFunc *lhs) const {
        Rhs.Accept(TRhsVisitor<TFunc>(lhs, DoubleVisitor));
      }
      virtual void operator()(const TId *lhs) const {
        Rhs.Accept(TRhsVisitor<TId>(lhs, DoubleVisitor));
      }
      virtual void operator()(const TInt *lhs) const {
        Rhs.Accept(TRhsVisitor<TInt>(lhs, DoubleVisitor));
      }
      virtual void operator()(const TList *lhs) const {
        Rhs.Accept(TRhsVisitor<TList>(lhs, DoubleVisitor));
      }
      virtual void operator()(const TMutable *lhs) const {
        Rhs.Accept(TRhsVisitor<TMutable>(lhs, DoubleVisitor));
      }
      virtual void operator()(const TObj *lhs) const {
        Rhs.Accept(TRhsVisitor<TObj>(lhs, DoubleVisitor));
      }
      virtual void operator()(const TOpt *lhs) const {
        Rhs.Accept(TRhsVisitor<TOpt>(lhs, DoubleVisitor));
      }
      virtual void operator()(const TReal *lhs) const {
        Rhs.Accept(TRhsVisitor<TReal>(lhs, DoubleVisitor));
      }
      virtual void operator()(const TSeq *lhs) const {
        Rhs.Accept(TRhsVisitor<TSeq>(lhs, DoubleVisitor));
      }
      virtual void operator()(const TSet *lhs) const {
        Rhs.Accept(TRhsVisitor<TSet>(lhs, DoubleVisitor));
      }
      virtual void operator()(const TStr *lhs) const {
        Rhs.Accept(TRhsVisitor<TStr>(lhs, DoubleVisitor));
      }
      virtual void operator()(const TTimeDiff *lhs) const {
        Rhs.Accept(TRhsVisitor<TTimeDiff>(lhs, DoubleVisitor));
      }
      virtual void operator()(const TTimePnt *lhs) const {
        Rhs.Accept(TRhsVisitor<TTimePnt>(lhs, DoubleVisitor));
      }
      virtual void operator()(const TUnknown *lhs) const {
        Rhs.Accept(TRhsVisitor<TUnknown>(lhs, DoubleVisitor));
      }

      private:

      /* TODO */
      const TType &Rhs;

      /* TODO */
      const TType::TDoubleVisitor &DoubleVisitor;

    };  // TLhsVisitor

  }  // Type

}  // Stig

using namespace Stig::Type;

TType::TVisitor::TVisitor() {}

TType::TVisitor::~TVisitor() {}

void TType::Accept(const TType &lhs, const TType &rhs, const TDoubleVisitor &double_visitor) {
  lhs.Accept(TLhsVisitor(rhs, double_visitor));
}


/* Helper function for GetMangledName
   num_members (type name_len name)* */
std::string MangleElemMap(const std::map<std::string, TType> &elems) {
  std::ostringstream strm;

  strm << elems.size();

  for (const auto &it: elems) {
    strm << it.second.GetMangledName() << it.first.size() << it.first;
  };

  return strm.str();
};


std::string TType::GetMangledName() const {
  class TTypeVisitor : public TType::TVisitor {
    public:

    /* TODO */
    TTypeVisitor(std::string &name) : Name(name) {}

    virtual void operator()(const TAddr *that) const {
      std::ostringstream strm;

      strm << 'A' << that->GetElems().size();

      for(auto it: that->GetElems()) {
        switch (it.first) {
          case TAddrDir::Asc:
            strm << 'a';
            break;
          case TAddrDir::Desc:
            strm << 'd';
            break;
        }
        strm << it.second.GetMangledName();
      }

      Name = strm.str();
    }
    virtual void operator()(const TAny *) const {
      Name = 'a';
    }
    virtual void operator()(const TBool *) const {
      Name = 'b';
    }
    virtual void operator()(const TDict *that) const {
      Name = 'D';
      Name += that->GetKey().GetMangledName();
      Name += that->GetVal().GetMangledName();
    }
    virtual void operator()(const TErr *that) const {
      Name = 'E';
      Name += that->GetElem().GetMangledName();
    }
    virtual void operator()(const TFunc *that) const {
      Name = 'F';
      Name += that->GetReturnType().GetMangledName();
      Name += that->GetParamObject().GetMangledName();
    }
    virtual void operator()(const Type::TId *) const override {
      Name = 'I';
    }
    virtual void operator()(const TInt *) const {
      Name = 'i';
    }
    virtual void operator()(const TList *that) const {
      Name = 'L';
      Name += that->GetElem().GetMangledName();
    }
    virtual void operator()(const TMutable *that) const {
      Name = "M2";
      Name += that->GetAddr().GetMangledName();
      Name += that->GetVal().GetMangledName();
    }
    virtual void operator()(const TObj *that) const {
      Name = 'O';
      Name += MangleElemMap(that->GetElems());
    }
    virtual void operator()(const TOpt *that) const {
      Name = 'P';
      Name += that->GetElem().GetMangledName();
    }
    virtual void operator()(const TReal *) const {
      Name = 'r';
    }
    virtual void operator()(const TSeq *that) const {
      Name = 'Q';
      Name += that->GetElem().GetMangledName();
    }
    virtual void operator()(const TSet *that) const {
      Name = 'S';
      Name += that->GetElem().GetMangledName();
    }
    virtual void operator()(const TStr *) const {
      Name = 's';
    }
    virtual void operator()(const TTimeDiff *) const {
      Name = 'd';
    }
    virtual void operator()(const TTimePnt *) const {
      Name = 't';
    }

    private:
    std::string &Name;

  };  // TTypeVisitor

  std::string name;
  Accept(TTypeVisitor(name));
  assert(name.length() > 0);
  return name;
}

void TType::Init() {
  Impl = TUnknown::Get().Impl;
}

TType::operator bool() const {
  assert(this);
  return Impl != TUnknown::Get().Impl;
}