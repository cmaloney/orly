#include <mpl/contains.h>
#include <mpl/extend.h>
#include <mpl/get_at.h>
#include <mpl/get_difference.h>
#include <mpl/get_idx.h>
#include <mpl/get_intersection.h>
#include <mpl/get_size.h>
#include <mpl/get_slice.h>
#include <mpl/get_union.h>
#include <mpl/get_unique.h>
#include <mpl/is_same.h>
#include <mpl/is_subset_of.h>
#include <mpl/merge.h>
#include <mpl/not.h>
#include <mpl/reverse.h>
#include <mpl/sort.h>
#include <mpl/type_list.h>
#include <mpl/type_set.h>

#include <test/kit.h>

using namespace Mpl;

/**
 *   TTypeList.
 **/

// TGetAt<TTypeList, Idx>
static_assert(std::is_same<TGetAt<TTypeList<int>, 0>, int>::value, "");
static_assert(std::is_same<TGetAt<TTypeList<std::string, double>, 1>, double>::value, "");
static_assert(std::is_same<TGetAt<TTypeList<int, double, double>, 2>, double>::value, "");
static_assert(std::is_same<TGetAt<TTypeList<int, double, std::string>, 2>, std::string>::value, "");

// TGetIdx<TTypeList, TElem>
static_assert(GetIdx<TTypeList<int>, int>::value == 0, "");
static_assert(GetIdx<TTypeList<std::string, int>, int>::value == 1, "");
static_assert(GetIdx<TTypeList<int, double>, double>::value == 1, "");
static_assert(GetIdx<TTypeList<int, double, std::string>, std::string>::value == 2, "");

// TExtend<TTypeList, TTypeList>
static_assert(std::is_same<TExtend<TTypeList<>, TTypeList<int>>, TTypeList<int>>::value, "");
static_assert(std::is_same<TExtend<TTypeList<int>, TTypeList<int, std::string>>, TTypeList<int, int, std::string>>::value, "");
static_assert(std::is_same<TExtend<TTypeList<int, double>, TTypeList<>>, TTypeList<int, double>>::value, "");
static_assert(std::is_same<TExtend<TTypeList<double>, TTypeList<std::string>>, TTypeList<double, std::string>>::value, "");

// TGetSlice<TList, IndexSequence>
static_assert(std::is_same<TGetSlice<TTypeList<int, int>, std::make_index_sequence<1>>, TTypeList<int>>::value, "");
static_assert(std::is_same<TGetSlice<TTypeList<std::string, int>, c14::make_index_range<0, 2>>, TTypeList<std::string, int>>::value, "");
static_assert(std::is_same<TGetSlice<TTypeList<int, double, double>, c14::make_index_range<1, 2>>, TTypeList<double>>::value, "");
static_assert(std::is_same<TGetSlice<TTypeList<std::string, double, std::string>, std::index_sequence<0, 2> >, TTypeList<std::string, std::string> >::value, "");

// TGetUnique<TTypeList>
static_assert(std::is_same<TGetUnique<TTypeList<int, int>>, TTypeList<int>>::value, "");
static_assert(std::is_same<TGetUnique<TTypeList<std::string, int>>, TTypeList<std::string, int>>::value, "");
static_assert(std::is_same<TGetUnique<TTypeList<int, double, double>>, TTypeList<int, double>>::value, "");
static_assert(std::is_same<TGetUnique<TTypeList<std::string, double, std::string>>, TTypeList<std::string, double>>::value, "");

using TIntegral = TTypeList<int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t>;

template <typename TLhs, typename TRhs>
struct TLess
    : public std::integral_constant<
          bool,
          (GetIdx<TIntegral, TLhs>::value < GetIdx<TIntegral, TRhs>::value)> {};

template <typename TLhs, typename TRhs>
using TGreater = Mpl::Not<TLess<TLhs, TRhs>>;

// Merge<TCompare, TTypeList, TTypeList>
static_assert(
    std::is_same<TMerge<TLess, TTypeList<int8_t, int32_t, uint16_t>, TTypeList<int16_t, uint32_t, uint64_t>>,
                 TTypeList<int8_t, int16_t, int32_t, uint16_t, uint32_t, uint64_t>>::value,
    "");

// TSort<TCompare, TTypeList>
static_assert(std::is_same<TSort<TLess, TTypeList<>>, TTypeList<>>::value, "");
static_assert(std::is_same<TSort<TLess, TTypeList<int>>, TTypeList<int>>::value, "");
static_assert(
    std::is_same<TSort<TLess, TTypeList<int8_t, int32_t, uint16_t, int16_t, uint32_t, int64_t, uint64_t>>,
                 TTypeList<int8_t, int16_t, int32_t, int64_t, uint16_t, uint32_t, uint64_t>>::value,
    "");
static_assert(
    std::is_same<TSort<TGreater, TTypeList<int8_t, int32_t, uint16_t, int16_t, uint32_t, int64_t, uint64_t>>,
                 TReverse<TTypeList<int8_t, int16_t, int32_t, int64_t, uint16_t, uint32_t, uint64_t>>>::value,
    "");

/**
 *   TTypeSet.
 **/

// Contains<TTypeSet>::value
static_assert(!Contains<TTypeSet<>, int>::value, "");
static_assert(Contains<TTypeSet<int>, int>::value, "");
static_assert(Contains<TTypeSet<int, double>, double>::value, "");
static_assert(!Contains<TTypeSet<int, double>, std::string>::value, "");

// GetSize<TTypeSet>::value
static_assert(GetSize<TTypeSet<>>::value == 0, "");
static_assert(GetSize<TTypeSet<int>>::value == 1, "");
static_assert(GetSize<TTypeSet<int, double, std::string>>::value == 3, "");
static_assert(GetSize<TTypeSet<int, double, int, double>>::value == 2, "");

// IsSubsetOf<TLhs, TRhs>::value
static_assert(IsSubsetOf<TTypeSet<int, double>, TTypeSet<int, double, std::string>>::value, "");
static_assert(!IsSubsetOf<TTypeSet<int, double>, TTypeSet<int, std::string>>::value, "");
static_assert(IsSubsetOf<TTypeSet<int, double, int>, TTypeSet<double, int>>::value, "");

// IsSame<TLhs, TRhs>::value
static_assert(IsSame<TTypeSet<int, double>, TTypeSet<double, int>>::value, "");
static_assert(IsSame<TTypeSet<int, double, int>, TTypeSet<double, int>>::value, "");
static_assert(IsSame<TTypeSet<int, double, int>, TTypeSet<double, double, int>>::value, "");
static_assert(!IsSame<TTypeSet<int, int>, TTypeSet<double, int>>::value, "");

// TGetUnion<TLhs, TRhs>
static_assert(
    IsSame<TGetUnion<TTypeSet<int, int, double>, TTypeSet<int, std::string>>, TTypeSet<int, double, std::string>>::value, "");
static_assert(
    IsSame<TGetUnion<TTypeSet<int, double>, TTypeSet<int>>, TTypeSet<double, int>>::value, "");

// TGetDifference<TLhs, TRhs>
static_assert(
    IsSame<TGetDifference<TTypeSet<int, std::string, double>, TTypeSet<int, float, double>>, TTypeSet<std::string>>::value, "");
static_assert(
    IsSame<TGetDifference<TTypeSet<int, std::string, double>, TTypeSet<std::string, int, float, double>>, TTypeSet<>>::value, "");

// TGetIntersection<TLhs, TRhs>
static_assert(
    IsSame<TGetIntersection<TTypeSet<int, std::string, double>, TTypeSet<int, float, double>>, TTypeSet<int, double>>::value, "");
static_assert(IsSame<TGetIntersection<TTypeSet<int, std::string, double>, TTypeSet<std::string, int, float, double>>,
                     TTypeSet<int, double, std::string>>::value,
              "");

