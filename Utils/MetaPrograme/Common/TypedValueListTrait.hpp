#ifndef TYPEDVALUELISTTRAIT_H
#define TYPEDVALUELISTTRAIT_H

#include "TypeListElementTrait.hpp"
#include "TypedValue.hpp"

namespace MetaPrograme {
namespace List {
namespace Details {

template<typename T, T Arg> class TypeListElementTrait<TypedValue<T, Arg>>
{
public:
    static constexpr T Value = Arg;
};

}}}

#endif /* TYPEDVALUELISTTRAIT_H */