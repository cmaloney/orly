/* <stig/client/program/translate_expr.h>

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
#include <cstdint>
#include <string>
#include <vector>

#include <stig/atom/kit2.h>
#include <stig/client/program/program.program.cst.h>
#include <stig/sabot/state.h>
#include <stig/sabot/type.h>

namespace Stig {

  namespace Client {

    namespace Program {

      /* Convert a Package Name expression into a vector of strings and a version number */
      void TranslatePackage(std::vector<std::string> &package_name, uint64_t &version_number, const TPackageName *expr);

      /* Convert a Name List expression into a vector of string. */
      void TranslatePathName(std::vector<std::string> &path_name, const TNameList *expr);

      /* TODO */
      Sabot::Type::TAny *NewTypeSabot(const TType *type, void *alloc);

      /* TODO */
      Sabot::Type::TAny *NewTypeSabot(const TExpr *expr, void *alloc);

      /* TODO */
      Sabot::State::TAny *NewStateSabot(const TExpr *expr, void *alloc);

      /* TODO */
      Atom::TCore *TranslateExpr(Atom::TCore::TExtensibleArena *arena, void *core_alloc, const TExpr *expr);

      namespace Type {

        /* TODO */
        template <typename TBase>
        class TUnaryExpr final
            : public TBase {
          public:

          /* TODO */
          using TBasePin = typename TBase::TPin;

          /* TODO */
          class TPin final
              : public TBasePin {
            public:

            /* TODO */
            TPin(const TUnaryExpr *unary)
                : UnaryExpr(unary) {
              assert(unary);
            }

            /* TODO */
            virtual Sabot::Type::TAny *NewElem(void *type_alloc) const override {
              return NewTypeSabot(UnaryExpr->Expr, type_alloc);
            }

            private:

            /* TODO */
            const TUnaryExpr *UnaryExpr;

          };  // TUnaryExpr<TCst, TBase>::TPin

          /* TODO */
          TUnaryExpr(const TExpr *expr)
              : Expr(expr) {
            assert(expr);
          }

          /* Pin the array into memory. */
          virtual TBasePin *Pin(void *pin_alloc) const override {
            return new (pin_alloc) TPin(this);
          }

          private:

          /* TODO */
          const TExpr *Expr;

        };  // Type::TUnaryExpr<TBase>

        /* TODO */
        template <typename TBase>
        class TUnaryType final
            : public TBase {
          public:

          /* TODO */
          using TBasePin = typename TBase::TPin;

          /* TODO */
          class TPin final
              : public TBasePin {
            public:

            /* TODO */
            TPin(const TUnaryType *unary)
                : UnaryType(unary) {
              assert(unary);
            }

            /* TODO */
            virtual Sabot::Type::TAny *NewElem(void *type_alloc) const override {
              return NewTypeSabot(UnaryType->Type, type_alloc);
            }

            private:

            /* TODO */
            const TUnaryType *UnaryType;

          };  // TUnaryType<TCst, TBase>::TPin

          /* TODO */
          TUnaryType(const TType *type)
              : Type(type) {
            assert(type);
          }

          /* Pin the array into memory. */
          virtual TBasePin *Pin(void *pin_alloc) const override {
            return new (pin_alloc) TPin(this);
          }

          private:

          /* TODO */
          const TType *Type;

        };  // Type::TUnaryType<TBase>

        /* TODO */
        template <typename TBase>
        class TBinaryExpr final
            : public TBase {
          public:

          /* TODO */
          using TBasePin = typename TBase::TPin;

          /* TODO */
          class TPin final
              : public TBasePin {
            public:

            /* TODO */
            TPin(const TBinaryExpr *binary)
                : BinaryExpr(binary) {
              assert(binary);
            }

            /* TODO */
            virtual Sabot::Type::TAny *NewLhs(void *type_alloc) const override {
              return NewTypeSabot(BinaryExpr->Lhs, type_alloc);
            }

            /* TODO */
            virtual Sabot::Type::TAny *NewRhs(void *type_alloc) const override {
              return NewTypeSabot(BinaryExpr->Rhs, type_alloc);
            }

            private:

            /* TODO */
            const TBinaryExpr *BinaryExpr;

          };  // TBinaryExpr<TCst, TBase>::TPin

          /* TODO */
          TBinaryExpr(const TExpr *lhs, const TExpr *rhs)
              : Lhs(lhs), Rhs(rhs) {
            assert(lhs);
            assert(rhs);
          }

          /* Pin the array into memory. */
          virtual TBasePin *Pin(void *pin_alloc) const override {
            return new (pin_alloc) TPin(this);
          }

          private:

          /* TODO */
          const TExpr *Lhs, *Rhs;

        };  // Type::TBinaryExpr<TBase>

        /* TODO */
        template <typename TBase>
        class TBinaryType final
            : public TBase {
          public:

          /* TODO */
          using TBasePin = typename TBase::TPin;

          /* TODO */
          class TPin final
              : public TBasePin {
            public:

            /* TODO */
            TPin(const TBinaryType *binary)
                : BinaryType(binary) {
              assert(binary);
            }

            /* TODO */
            virtual Sabot::Type::TAny *NewLhs(void *type_alloc) const override {
              return NewTypeSabot(BinaryType->Lhs, type_alloc);
            }

            /* TODO */
            virtual Sabot::Type::TAny *NewRhs(void *type_alloc) const override {
              return NewTypeSabot(BinaryType->Rhs, type_alloc);
            }

            private:

            /* TODO */
            const TBinaryType *BinaryType;

          };  // TBinaryType<TCst, TBase>::TPin

          /* TODO */
          TBinaryType(const TType *lhs, const TType *rhs)
              : Lhs(lhs), Rhs(rhs) {
            assert(lhs);
            assert(rhs);
          }

          /* Pin the array into memory. */
          virtual TBasePin *Pin(void *pin_alloc) const override {
            return new (pin_alloc) TPin(this);
          }

          private:

          /* TODO */
          const TType *Lhs, *Rhs;

        };  // Type::TBinaryType<TBase>

        /* TODO */
        class TRecordType final
            : public Sabot::Type::TRecord {
          public:

          /* TODO */
          using TPinBase = Sabot::Type::TRecord::TPin;

          /* TODO */
          class TPin final
              : public TPinBase {
            public:

            /* TODO */
            TPin(const TRecordType *record_type);

            /* TODO */
            virtual Sabot::Type::TAny *NewElem(size_t elem_idx, std::string &name, void *type_alloc) const override;

            /* TODO */
            virtual Sabot::Type::TAny *NewElem(
                size_t elem_idx, void *&out_field_name_sabot_state, void *field_name_state_alloc, void *type_alloc) const override;

            /* TODO */
            virtual Sabot::Type::TAny *NewElem(size_t elem_idx, void *type_alloc) const override;

            private:

            /* TODO */
            const TRecordType *RecordType;

          };  // TRecordType::TPin

          /* TODO */
          TRecordType(const TObjType *type);

          /* TODO */
          virtual size_t GetElemCount() const override;

          /* TODO */
          virtual TPinBase *Pin(void *alloc) const override;

          private:

          /* TODO */
          std::vector<const TObjTypeMember *> Members;

        };  // Type::TRecordType

        /* TODO */
        class TRecordExpr final
            : public Sabot::Type::TRecord {
          public:

          /* TODO */
          using TPinBase = Sabot::Type::TRecord::TPin;

          /* TODO */
          class TPin final
              : public TPinBase {
            public:

            /* TODO */
            TPin(const TRecordExpr *record_expr);

            /* TODO */
            virtual Sabot::Type::TAny *NewElem(size_t elem_idx, std::string &name, void *type_alloc) const override;

            /* TODO */
            virtual Sabot::Type::TAny *NewElem(
                size_t elem_idx, void *&out_field_name_sabot_state, void *field_name_state_alloc, void *type_alloc) const override;

            /* TODO */
            virtual Sabot::Type::TAny *NewElem(size_t elem_idx, void *type_alloc) const override;

            private:

            /* TODO */
            const TRecordExpr *RecordExpr;

          };  // TRecordExpr::TPin

          /* TODO */
          TRecordExpr(const TObjExpr *expr);

          /* TODO */
          virtual size_t GetElemCount() const override;

          /* TODO */
          virtual TPinBase *Pin(void *alloc) const override;

          private:

          /* TODO */
          std::vector<const TObjMember *> Members;

        };  // Type::TRecordExpr

        /* TODO */
        class TTupleType final
            : public Sabot::Type::TTuple {
          public:

          /* TODO */
          using TPinBase = Sabot::Type::TTuple::TPin;

          /* TODO */
          class TPin final
              : public TPinBase {
            public:

            /* TODO */
            TPin(const TTupleType *tuple_type);

            /* TODO */
            virtual Sabot::Type::TAny *NewElem(size_t elem_idx, void *type_alloc) const override;

            private:

            /* TODO */
            const TTupleType *TupleType;

          };  // TTupleType::TPin

          /* TODO */
          TTupleType(const TAddrType *type);

          /* TODO */
          virtual size_t GetElemCount() const override;

          /* TODO */
          virtual TPinBase *Pin(void *alloc) const override;

          private:

          /* TODO */
          std::vector<const TAddrTypeMember *> Members;

        };  // Type::TTupleType

        /* TODO */
        class TTupleExpr final
            : public Sabot::Type::TTuple {
          public:

          /* TODO */
          using TPinBase = Sabot::Type::TTuple::TPin;

          /* TODO */
          class TPin final
              : public TPinBase {
            public:

            /* TODO */
            TPin(const TTupleExpr *tuple_expr);

            /* TODO */
            virtual Sabot::Type::TAny *NewElem(size_t elem_idx, void *type_alloc) const override;

            private:

            /* TODO */
            const TTupleExpr *TupleExpr;

          };  // TTupleExpr::TPin

          /* TODO */
          TTupleExpr(const TAddrExpr *expr);

          /* TODO */
          virtual size_t GetElemCount() const override;

          /* TODO */
          virtual TPinBase *Pin(void *alloc) const override;

          private:

          /* TODO */
          std::vector<const TAddrMember *> Members;

        };  // Type::TTupleExpr

      }  // Type

      namespace State {

        /* TODO */
        class TBool final
            : public Sabot::State::TBool {
          public:

          /* TODO */
          TBool(const TTrueExpr *);

          /* TODO */
          TBool(const TFalseExpr *);

          /* TODO */
          virtual const bool &Get() const override;

          /* TODO */
          virtual Sabot::Type::TBool *GetBoolType(void *type_alloc) const override;

          private:

          /* TODO */
          bool Val;

        };  // State::TBool

        /* TODO */
        class TInt final
            : public Sabot::State::TInt64 {
          public:

          /* TODO */
          TInt(const TIntExpr *expr);

          /* TODO */
          virtual const int64_t &Get() const override;

          /* TODO */
          virtual Sabot::Type::TInt64 *GetInt64Type(void *type_alloc) const override;

          private:

          /* TODO */
          int64_t Val;

        };  // State::TInt

        /* TODO */
        class TReal final
            : public Sabot::State::TDouble {
          public:

          /* TODO */
          TReal(const TRealExpr *expr);

          /* TODO */
          virtual const double &Get() const override;

          /* TODO */
          virtual Sabot::Type::TDouble *GetDoubleType(void *type_alloc) const override;

          private:

          /* TODO */
          double Val;

        };  // State::TReal

        /* TODO */
        class TId final
            : public Sabot::State::TUuid {
          public:

          /* TODO */
          TId(const TIdExpr *expr);

          /* TODO */
          virtual const Base::TUuid &Get() const override;

          /* TODO */
          virtual Sabot::Type::TUuid *GetUuidType(void *type_alloc) const override;

          private:

          /* TODO */
          Base::TUuid Val;

        };  // State::TId

        /* TODO */
        class TTimePnt final
            : public Sabot::State::TTimePoint {
          public:

          /* TODO */
          TTimePnt(const TTimePntExpr *expr);

          /* TODO */
          virtual const Sabot::TStdTimePoint &Get() const override;

          /* TODO */
          virtual Sabot::Type::TTimePoint *GetTimePointType(void *type_alloc) const override;

          private:

          /* TODO */
          Sabot::TStdTimePoint Val;

        };  // State::TTimePnt

        /* TODO */
        class TTimeDiff final
            : public Sabot::State::TDuration {
          public:

          /* TODO */
          TTimeDiff(const TTimeDiffExpr *expr);

          /* TODO */
          virtual const Sabot::TStdDuration &Get() const override;

          /* TODO */
          virtual Sabot::Type::TDuration *GetDurationType(void *type_alloc) const override;

          private:

          /* TODO */
          Sabot::TStdDuration Val;

        };  // State::TTimeDiff

        /* TODO */
        class TStr final
            : public Sabot::State::TStr {
          public:

          /* TODO */
          using TPinBase = Sabot::State::TStr::TPin;

          /* TODO */
          class TPin final
              : public TPinBase {
            public:

            /* TODO */
            TPin(const TStr *str);

          };  // TStr::TPin

          /* TODO */
          TStr(const TSingleQuotedStrExpr *expr);

          /* TODO */
          TStr(const TDoubleQuotedStrExpr *expr);

          /* TODO */
          TStr(const TSingleQuotedRawStrExpr *expr);

          /* TODO */
          TStr(const TDoubleQuotedRawStrExpr *expr);

          /* TODO */
          virtual size_t GetSize() const override;

          /* TODO */
          virtual Sabot::Type::TStr *GetStrType(void *type_alloc) const override;

          /* TODO */
          virtual TPinBase *Pin(void *alloc) const override;

          private:

          /* TODO */
          std::string Val;

        };  // State::TStr

        /* TODO */
        class TOpt final
            : public Sabot::State::TOpt {
          public:

          /* TODO */
          using TPinBase = Sabot::State::TOpt::TPin;

          /* TODO */
          class TPin final
              : public TPinBase {
            public:

            /* TODO */
            TPin(const TOpt *opt);

            private:

            /* TODO */
            virtual Sabot::State::TAny *NewElemInRange(size_t elem_idx, void *state_alloc) const override;

            /* TODO */
            const TExpr *Expr;

          };  // TOpt::TPin

          /* TODO */
          TOpt(const TOptExpr *expr);

          /* TODO */
          TOpt(const TUnknownExpr *expr);

          /* TODO */
          virtual size_t GetElemCount() const override;

          /* TODO */
          virtual Sabot::Type::TOpt *GetOptType(void *type_alloc) const override;

          /* TODO */
          virtual TPinBase *Pin(void *alloc) const override;

          private:

          /* TODO */
          const TExpr *Expr;

          /* TODO */
          const TType *Type;

        };  // State::TOpt

        /* TODO */
        class TDesc final
            : public Sabot::State::TDesc {
          public:

          /* TODO */
          using TPinBase = Sabot::State::TDesc::TPin;

          /* TODO */
          class TPin final
              : public TPinBase {
            public:

            /* TODO */
            TPin(const TDesc *set);

            private:

            /* TODO */
            virtual Sabot::State::TAny *NewElemInRange(size_t elem_idx, void *state_alloc) const override;

            /* TODO */
            const TDesc *Desc;

          };  // TDesc::TPin

          /* TODO */
          TDesc(const TExpr *expr);

          /* TODO */
          virtual size_t GetElemCount() const override;

          /* TODO */
          virtual Sabot::Type::TDesc *GetDescType(void *type_alloc) const override;

          /* TODO */
          virtual TPinBase *Pin(void *alloc) const override;

          private:

          /* TODO */
          const TExpr *Expr;

        };  // State::TDesc

        /* TODO */
        class TSet final
            : public Sabot::State::TSet {
          public:

          /* TODO */
          using TPinBase = Sabot::State::TSet::TPin;

          /* TODO */
          class TPin final
              : public TPinBase {
            public:

            /* TODO */
            TPin(const TSet *set);

            private:

            /* TODO */
            virtual Sabot::State::TAny *NewElemInRange(size_t elem_idx, void *state_alloc) const override;

            /* TODO */
            const TSet *Set;

          };  // TSet::TPin

          /* TODO */
          TSet(const TSetExpr *expr);

          /* TODO */
          TSet(const TSetType *type);

          /* TODO */
          virtual size_t GetElemCount() const override;

          /* TODO */
          virtual Sabot::Type::TSet *GetSetType(void *type_alloc) const override;

          /* TODO */
          virtual TPinBase *Pin(void *alloc) const override;

          private:

          /* TODO */
          std::vector<const TExpr *> Members;

          /* TODO */
          const TType *Type;

        };  // State::TSet

        /* TODO */
        class TList final
            : public Sabot::State::TVector {
          public:

          /* TODO */
          using TPinBase = Sabot::State::TVector::TPin;

          /* TODO */
          class TPin final
              : public TPinBase {
            public:

            /* TODO */
            TPin(const TList *list);

            private:

            /* TODO */
            virtual Sabot::State::TAny *NewElemInRange(size_t elem_idx, void *state_alloc) const override;

            /* TODO */
            const TList *List;

          };  // TList::TPin

          /* TODO */
          TList(const TListExpr *expr);

          /* TODO */
          TList(const TListType *type);

          /* TODO */
          virtual size_t GetElemCount() const override;

          /* TODO */
          virtual Sabot::Type::TVector *GetVectorType(void *type_alloc) const override;

          /* TODO */
          virtual TPinBase *Pin(void *alloc) const override;

          private:

          /* TODO */
          std::vector<const TExpr *> Members;

          /* TODO */
          const TType *Type;

        };  // State::TList

        /* TODO */
        class TDict final
            : public Sabot::State::TMap {
          public:

          /* TODO */
          using TPinBase = Sabot::State::TMap::TPin;

          /* TODO */
          class TPin final
              : public TPinBase {
            public:

            /* TODO */
            TPin(const TDict *list);

            private:

            /* TODO */
            virtual Sabot::State::TAny *NewLhsInRange(size_t elem_idx, void *state_alloc) const override;

            /* TODO */
            virtual Sabot::State::TAny *NewRhsInRange(size_t elem_idx, void *state_alloc) const override;

            /* TODO */
            const TDict *Dict;

          };  // TDict::TPin

          /* TODO */
          TDict(const TDictExpr *expr);

          /* TODO */
          TDict(const TDictType *type);

          /* TODO */
          virtual size_t GetElemCount() const override;

          /* TODO */
          virtual Sabot::Type::TMap *GetMapType(void *type_alloc) const override;

          /* TODO */
          virtual TPinBase *Pin(void *alloc) const override;

          private:

          /* TODO */
          std::vector<const TDictMember *> Members;

          /* TODO */
          const TType *LhsType, *RhsType;

        };  // State::TDict

        /* TODO */
        class TObj final
            : public Sabot::State::TRecord {
          public:

          /* TODO */
          using TPinBase = Sabot::State::TRecord::TPin;

          /* TODO */
          class TPin final
              : public TPinBase {
            public:

            /* TODO */
            TPin(const TObj *obj);

            private:

            /* TODO */
            virtual Sabot::State::TAny *NewElemInRange(size_t elem_idx, void *state_alloc) const override;

            /* TODO */
            const TObj *Obj;

          };  // TObj::TPin

          /* TODO */
          TObj(const TObjExpr *expr);

          /* TODO */
          virtual size_t GetElemCount() const override;

          /* TODO */
          virtual Sabot::Type::TRecord *GetRecordType(void *type_alloc) const override;

          /* TODO */
          virtual TPinBase *Pin(void *alloc) const override;

          private:

          /* TODO */
          const TObjExpr *Expr;

          /* TODO */
          std::vector<const TObjMember *> Members;

        };  // State::TObj

        /* TODO */
        class TAddr final
            : public Sabot::State::TTuple {
          public:

          /* TODO */
          using TPinBase = Sabot::State::TTuple::TPin;

          /* TODO */
          class TPin final
              : public TPinBase {
            public:

            /* TODO */
            TPin(const TAddr *addr);

            private:

            /* TODO */
            virtual Sabot::State::TAny *NewElemInRange(size_t elem_idx, void *state_alloc) const override;

            /* TODO */
            const TAddr *Addr;

          };  // TAddr::TPin

          /* TODO */
          TAddr(const TAddrExpr *expr);

          /* TODO */
          virtual size_t GetElemCount() const override;

          /* TODO */
          virtual Sabot::Type::TTuple *GetTupleType(void *type_alloc) const override;

          /* TODO */
          virtual TPinBase *Pin(void *alloc) const override;

          private:

          /* TODO */
          const TAddrExpr *Expr;

          /* TODO */
          std::vector<const TAddrMember *> Members;

        };  // State::TAddr

      }  // State

    }  // Program

  }  // Client

}  // Stig
