#ifndef TYPEDVALUE_H
#define TYPEDVALUE_H

namespace MetaPrograme {

template<typename T, T Arg> class TypedValue
{
    static constexpr T Value = Arg;
};

}

#endif /* TYPEDVALUE_H */