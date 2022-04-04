#ifndef LISTALGORITHMS_H
#define LISTALGORITHMS_H

#include <type_traits>
#include <utility>
#include "TypedValue.hpp"
#include "TypedValueListTrait.hpp"

namespace MetaPrograme {

namespace List {
namespace Algorithms {

typedef int ListIndexType;

template<ListIndexType Index, typename First, typename... Other> class At
{
public:
    using Type = typename std::conditional
    <
        std::is_same<TypedValue<ListIndexType, Index>, TypedValue<ListIndexType, 0>>::value
        , First
        , typename At<Index - 1, Other...>::Type
    >::type;
};

template<int Index, typename T> class At<Index, T>
{
public:
    using Type = T;
};

template<typename SearchElement, int Index, typename... Elements> class Search
{
public:
    using Type = typename std::conditional
    <
        std::is_same<typename At<Index, Elements...>::Type, SearchElement>::value
        , TypedValue<ListIndexType, Index>
        , typename Search<SearchElement, Index + 1, Elements...>::Type
    >::type;
};

#ifndef LIST_SEARCH_ALGORITHM_INSTANTIATE_LIMIT
#define LIST_SEARCH_ALGORITHM_INSTANTIATE_LIMIT 60
#endif

template<typename SearchElement, typename... Elements>
class Search
<
    SearchElement
    /*
     * gcc bug：https://gcc.gnu.org/bugzilla/show_bug.cgi?id=51155#c1
     * 标准修订记录：http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#1315
     * 暂时尝试以一个固定的小范围上限终止编译器的递归实例化
     */
    , LIST_SEARCH_ALGORITHM_INSTANTIATE_LIMIT/*sizeof...(Elements) - 1*/
    , Elements...
>
{
public:
    using Type = typename std::conditional
    <
        std::is_same<typename At<sizeof...(Elements) - 1, Elements...>::Type, SearchElement>::value
        , TypedValue<ListIndexType, sizeof...(Elements) - 1>
        , TypedValue<ListIndexType, -1>
    >::type;
};

}}

template<typename ValueType, typename First, typename... Other> class IsValueExists
{
public:
    bool operator()(ValueType &&value)
    {
        return List::Details::TypeListElementTrait<First>::Value == value ?
            true : IsValueExists<ValueType, Other...>()(std::forward<ValueType>(value));
    }
};

template<typename ValueType, typename T> class IsValueExists<ValueType, T>
{
public:
    bool operator()(ValueType &&value)
    {
        return List::Details::TypeListElementTrait<T>::Value == value;
    }
};

template<typename ValueType, typename... T> inline bool AnyOf(ValueType &&value
    , typename std::enable_if<(sizeof...(T) > 0), int>::type dummy = 0)
{
    return IsValueExists<ValueType, T...>()(std::forward<ValueType>(value));
}

template<typename ValueType, typename... T> inline bool AnyOf(ValueType &&value
    , typename std::enable_if<sizeof...(T) == 0, int>::type dummy = 0)
{
    return false;
}

}

#endif /* LISTALGORITHMS_H */