#ifndef THORS_SERIALIZER_GET_STATS_H
#define THORS_SERIALIZER_GET_STATS_H

#include <map>
#include <vector>
#include <string>

template<typename T>
inline void getStats(Stat* stat, std::unique_ptr<T> const& value)
{
    if (!value) {
        stat->nullCount++;
        return;
    }
    getStats(stat, *value);
}
template<typename T>
inline void getStatsMayNotExist(Stat* stat, std::unique_ptr<T> const& value, std::size_t keySize)
{
    if (value) {
        stat->memberCount++;
        stat->stringCount++;
        stat->stringLength += keySize;
        getStats(stat, *value);
    }
}

inline void getStats(Stat* stat, int const&) { stat->numberCount++; }
inline void getStats(Stat* stat, long const&) { stat->numberCount++; }
inline void getStats(Stat* stat, double const&) { stat->numberCount++; }
inline void getStats(Stat* stat, bool const& value)
{
    (value ? stat->trueCount : stat->falseCount)++;
}
inline void getStats(Stat* stat, std::string  const& value)
{
    stat->stringCount++;
    stat->stringLength += value.size();
}


template<typename V>
inline void getStats(Stat* stat, std::map<std::string, V> const& value)
{
    stat->objectCount++;
    stat->memberCount += value.size();
    for(auto const& da: value) {
        getStats(stat, da.first);
        getStats(stat, da.second);
    }
}

template<typename V>
inline void getStats(Stat* stat, std::vector<V> const& value)
{
    stat->arrayCount++;
    stat->elementCount += value.size();
    for(auto const& da: value) {
        getStats(stat, da);
    }
}


#endif
