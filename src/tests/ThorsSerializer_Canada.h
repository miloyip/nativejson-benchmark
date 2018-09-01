#ifndef THORS_SERIALIZER_CANADA_H
#define THORS_SERIALIZER_CANADA_H

#include "ThorSerialize/Traits.h"
#include "ThorSerialize/SerUtil.h"
#include <vector>
#include <string>

struct Properties
{
    std::string     name;
};
ThorsAnvil_MakeTrait(Properties, name);
inline void getStats(Stat* stat, Properties const& value)
{
    stat->objectCount++;
    stat->memberCount++;
    stat->stringCount += 2;
    stat->stringLength += (4 /*name*/ + value.name.size());
}

struct Cord
{
    std::pair<double, double>       value;
    using value_type = double;
    std::size_t size() const {return 2;}
};
inline void getStats(Stat* stat, Cord const&)
{
    stat->arrayCount++;
    stat->elementCount += 2;
    stat->numberCount += 2;
}

namespace ThorsAnvil
{
    namespace Serialize
    {
template<>
class ContainerMemberExtractorEmplacer<Cord>
{
    public:
        constexpr ContainerMemberExtractorEmplacer() {}
        constexpr std::size_t getHash(std::size_t start) const
        {
            return 0x4567 | start;
        }
        void operator()(PrinterInterface& printer, Cord const& object) const
        {
            PutValueType<double>     valuePutter(printer);
            valuePutter.putValue(object.value.first);
            valuePutter.putValue(object.value.second);
        }
        void operator()(ParserInterface& parser, std::size_t const& index, Cord& object) const
        {
            GetValueType<double>   valueGetter(parser, index == 0 ? object.value.first : object.value.second);
        }
};
template<>
class Traits<Cord>
{
    public:
        static constexpr TraitType type = TraitType::Array;
        typedef ContainerMemberExtractorEmplacer<Cord>    MemberExtractor;
        static MemberExtractor const& getMembers()
        {
            static constexpr MemberExtractor    memberExtractor;
            return memberExtractor;
        }
};

    }
}

using   CordVec = std::vector<Cord>;
using   CordMat = std::vector<CordVec>;

struct Geometry
{
    std::string     type;
    CordMat         coordinates;
};
ThorsAnvil_MakeTrait(Geometry, type, coordinates);
inline void getStats(Stat* stat, Geometry const& value)
{
    stat->objectCount++;
    stat->memberCount += 2;
    stat->stringCount += 3; /* Key + Key + Value*/
    stat->stringLength += (4 /*type*/ + 11 /*coordinates*/ + value.type.size());
    getStats(stat, value.coordinates);
}

struct Feature
{
    std::string     type;
    Properties      properties;
    Geometry        geometry;
};
ThorsAnvil_MakeTrait(Feature, type, properties, geometry);
inline void getStats(Stat* stat, Feature const& value)
{
    stat->objectCount++;
    stat->memberCount   += 3;
    stat->stringCount   += 4; /* Key + Key + Key + Value */
    stat->stringLength  += (4 /*type*/ + 10 /*properties*/ + 8 /*geometry*/ + value.type.size());
    getStats(stat, value.properties);
    getStats(stat, value.geometry);
}
using Features  = std::vector<Feature>;

struct Country
{
    std::string     type;
    Features        features;
};
ThorsAnvil_MakeTrait(Country, type, features);
inline void getStats(Stat* stat, Country const& value)
{
    stat->objectCount++;
    stat->memberCount+=2;
    stat->stringCount += 3; /*Key + Key + Value */
    stat->stringLength += (4 /*types*/ + 8 /*features*/ + value.type.size());
    getStats(stat, value.features);
}

#endif
