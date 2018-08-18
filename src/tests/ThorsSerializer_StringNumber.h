#ifndef THORS_SERIALIZER_EXACT_DOUBLE_H
#define THORS_SERIALIZER_EXACT_DOUBLE_H

#include "ThorSerialize/Traits.h"
#include "ThorSerialize/SerUtil.h"
#include <iostream>
#include <string>
#include <cctype>

class StringNumber
{
    std::string     data;
    friend std::ostream& operator<<(std::ostream& str, StringNumber const& val)
    {
        return str << val.data;
    }
    friend std::istream& operator>>(std::istream& str, StringNumber& val)
    {
        return std::getline(str, val.data);
    }
};
ThorsAnvil_MakeTraitCustom(StringNumber);
inline void getStats(Stat* stat, StringNumber const&)
{
    stat->numberCount++;
}

#endif
