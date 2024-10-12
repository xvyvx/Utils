#ifndef TYPELIST_H
#define TYPELIST_H

#include "./Common/MPListAlgorithms.hpp"

namespace MetaPrograme {

template<typename... Results> class TypeList
{
public:
    template<typename... Elements> class From
    {
    public:
        template<typename... SubstractElements> class Substract
        {
        public:
            using Result = TypeList<Results...>;
        };
    };

    template<typename First, typename... Others> class From<First, Others...>
    {
    public:
        template<typename... SubstractElements> class Substract
        {
        public:
            using Result = TypeList<First, Others..., Results...>;
        };

        template<typename FirstSubstractElement, typename... OtherSubstractElements>
            class Substract<FirstSubstractElement, OtherSubstractElements...>
        {
        public:
            using Result = typename std::conditional
            <
                std::is_same
                <
                    typename List::Algorithms::Search<First, 0, FirstSubstractElement, OtherSubstractElements...>::Type
                    , TypedValue<MetaPrograme::List::Algorithms::ListIndexType, -1>
                >::value
                , typename TypeList<First, Results...>
                    ::template From<Others...>
                    ::template Substract<FirstSubstractElement, OtherSubstractElements...>::Result
                , typename TypeList<Results...>
                    ::template From<Others...>
                    ::template Substract<FirstSubstractElement, OtherSubstractElements...>::Result
            >::type;
        };
    };

    template<typename SearchType> class Have
    {
    public:
        enum
        {
            Value = !std::is_same
                <
                    typename List::Algorithms::Search<SearchType, 0, Results...>::Type
                    , TypedValue<MetaPrograme::List::Algorithms::ListIndexType, -1>
                >::value
        };
    };

    enum
    {
        Empty = sizeof...(Results) == 0 ? 1 : 0
    };

    template<typename T> static bool Contain(T &&value)
    {
        return AnyOf<T, Results...>(std::forward<T>(value));
    }
};

}

#endif /* TYPELIST_H */