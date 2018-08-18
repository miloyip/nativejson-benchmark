#ifndef THORS_SERIALIZER_CITM_CATALOG_H
#define THORS_SERIALIZER_CITM_CATALOG_H

#include "ThorSerialize/Traits.h"
#include "ThorSerialize/SerUtil.h"
#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include <string>

template<typename T>
using Opt = std::unique_ptr<T>;

using IntVec   = std::vector<int>;
struct Event
{
    Opt<int>            description;
    long                id;
    Opt<std::string>    logo;
    std::string         name;
    IntVec              subTopicIds;
    Opt<int>            subjectCode;
    Opt<int>            subtitle;
    IntVec              topicIds;
};
ThorsAnvil_MakeTrait(Event, description, id, logo, name, subTopicIds, subjectCode, subtitle, topicIds);
inline void getStats(Stat* stat, Event const& value)
{
    stat->objectCount++;
    stat->memberCount += 8;
    stat->stringCount += 8 + 1;
    stat->numberCount++;
    (value.description ? stat->numberCount : stat->nullCount)++;
    (value.logo ? stat->stringCount : stat->nullCount)++;
    (value.subjectCode ? stat->numberCount : stat->nullCount)++;
    (value.subtitle ? stat->numberCount : stat->nullCount)++;
    stat->stringLength += (11 /*description*/ + 2 /*id*/ + 4 /*logo*/ + 4 /*name*/ + 11 /*subTopicIds*/
                            + 11 /*subjectCode*/ + 8 /*subtitle*/ + 8 + /*topicIds*/ + value.name.size()
                            + (value.logo ? value.logo->size() : 0));
    getStats(stat, value.subTopicIds);
    getStats(stat, value.topicIds);
}

struct Price
{
    int                 amount;
    int                 audienceSubCategoryId;
    int                 seatCategoryId;
};
ThorsAnvil_MakeTrait(Price, amount, audienceSubCategoryId, seatCategoryId);
inline void getStats(Stat* stat, Price const&)
{
    stat->objectCount++;
    stat->memberCount += 3;
    stat->stringCount += 3;
    stat->numberCount += 3;
    stat->stringLength += (6 /*amount*/ + 21 /*audienceSubCategoryId*/ + 14 /*seatCategoryId*/);
}
using Prices = std::vector<Price>;

struct Area
{
    int                 areaId;
    IntVec              blockIds;
};
ThorsAnvil_MakeTrait(Area, areaId, blockIds);
inline void getStats(Stat* stat, Area const& value)
{
    stat->objectCount++;
    stat->memberCount += 2;
    stat->stringCount += 2;
    stat->numberCount += 1;
    stat->stringLength += (6 /*areaId*/ + 8 /*blockIds*/);
    getStats(stat, value.blockIds);
}
using Areas = std::vector<Area>;

struct SeatCategorie
{
    Areas               areas;
    int                 seatCategoryId;
};
ThorsAnvil_MakeTrait(SeatCategorie, areas, seatCategoryId);
inline void getStats(Stat* stat, SeatCategorie const& value)
{
    stat->objectCount++;
    stat->memberCount += 2;
    stat->stringCount += 2;
    stat->stringLength += (5 /*areas*/ + 14 /*seatCategoryId*/);
    stat->numberCount++;
    getStats(stat, value.areas);
}
using SeatCategories = std::vector<SeatCategorie>;

struct Performance
{
    int                 eventId;
    int                 id;
    Opt<std::string>    logo;
    Opt<std::string>    name;
    Prices              prices;
    SeatCategories      seatCategories;
    Opt<int>            seatMapImage;
    long                start;
    std::string         venueCode;
};
ThorsAnvil_MakeTrait(Performance, eventId, id, logo, name, prices, seatCategories, seatMapImage, start, venueCode);
inline void getStats(Stat* stat, Performance const& value)
{
    stat->objectCount++;
    stat->memberCount += 9;
    stat->stringCount += 10;
    stat->stringLength += (7 /*eventId*/ + 2 /*id*/ + 4 /*logo*/ + 4 /*name*/ + 6 /*prices*/ + 14 /*seatCategories*/
                         + 12 /*seatMapImage*/ + 5 /*start*/ + 9 /*venueCode*/ + value.venueCode.size() 
                         + (value.name ? value.name->size() : 0) + (value.logo ? value.logo->size() : 0));
    stat->numberCount += 3;
    (value.seatMapImage ? stat->numberCount : stat->nullCount)++;
    (value.logo ? stat->stringCount : stat->nullCount)++;
    (value.name ? stat->stringCount : stat->nullCount)++;
    getStats(stat, value.prices);
    getStats(stat, value.seatCategories);
}
using Performances = std::vector<Performance>;

struct VenueNames
{
    std::string     PLEYEL_PLEYEL;
};
ThorsAnvil_MakeTrait(VenueNames, PLEYEL_PLEYEL);
inline void getStats(Stat* stat, VenueNames const& value)
{
    stat->objectCount++;
    stat->memberCount++;
    stat->stringCount += 2;
    stat->stringLength += (13 /*PLEYEL_PLEYEL*/ + value.PLEYEL_PLEYEL.size());
}

struct Perform
{
    std::map<std::string, std::string>  areaNames;
    std::map<std::string, std::string>  audienceSubCategoryNames;
    std::map<std::string, std::string>  blockNames;
    std::map<std::string, Event>        events;
    Performances                        performances;
    std::map<std::string, std::string>  seatCategoryNames;
    std::map<std::string, std::string>  subTopicNames;
    std::map<std::string, std::string>  subjectNames;
    std::map<std::string, std::string>  topicNames;
    std::map<std::string, IntVec>       topicSubTopics;
    VenueNames                          venueNames;
};
ThorsAnvil_MakeTrait(Perform, areaNames, audienceSubCategoryNames, blockNames, events, performances, seatCategoryNames, subTopicNames, subjectNames, topicNames, topicSubTopics, venueNames);
inline void getStats(Stat* stat, Perform const& value)
{
    stat->objectCount++;
    stat->memberCount += 11;
    stat->stringCount += 11; /* All Keys */
    stat->stringLength +=  ( 9 /*areaNames*/ + 24 /*audienceSubCategoryNames*/ + 10 /*blockNames*/
                            + 6 /*events*/ + 12 /*performances*/ + 17 /*seatCategoryNames*/
                            + 13 /*subTopicNames*/ + 12 /*subjectNames*/ + 10 /*topicNames*/
                            + 14 /*topicSubTopics*/ + 10 /*venueNames*/);
    getStats(stat, value.areaNames);
    getStats(stat, value.audienceSubCategoryNames);
    getStats(stat, value.blockNames);
    getStats(stat, value.events);
    getStats(stat, value.performances);
    getStats(stat, value.seatCategoryNames);
    getStats(stat, value.subTopicNames);
    getStats(stat, value.subjectNames);
    getStats(stat, value.topicNames);
    getStats(stat, value.topicSubTopics);
    getStats(stat, value.venueNames);
}

#endif
