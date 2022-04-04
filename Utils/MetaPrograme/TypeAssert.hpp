#ifndef TYPEASSERT_H
#define TYPEASSERT_H

#include <type_traits>

namespace MetaPrograme {

template<typename... T> class TypeAssert
{
public:
    template<typename CheckedType> class Contain
    {
    public:
        enum
        {
            Value = 0
        };
    };
};

template<typename First, typename... Others> class TypeAssert<First, Others...>
{
public:
    template<typename CheckedType> class Contain
    {
    public:
        enum
        {
            Value = std::is_same<CheckedType, First>::value ? 1 : TypeAssert<Others...>::template Contain<CheckedType>::Value
        };
    };
};

}

#endif /* TYPEASSERT_H */