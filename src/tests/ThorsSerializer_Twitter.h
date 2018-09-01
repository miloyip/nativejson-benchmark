#ifndef THORS_SERIALIZER_TWITTER_H
#define THORS_SERIALIZER_TWITTER_H

#include "ThorSerialize/Traits.h"
#include "ThorSerialize/SerUtil.h"
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <memory>

using IntVec = std::vector<int>;

template<typename T>
using Opt       = std::unique_ptr<T>;
using Symbol    = std::map<std::string, std::string>;
using Symbols   = std::vector<Symbol>;

struct Metadata
{
    std::string         result_type;
    std::string         iso_language_code;
};
ThorsAnvil_MakeTrait(Metadata, result_type, iso_language_code);
inline void getStats(Stat* stat, Metadata const& value)
{
    stat->objectCount++;
    stat->memberCount += 2;
    stat->stringCount += 4; /* Key, Key, Value, Value */
    stat->stringLength += (11 /*result_type*/ + 17 /*iso_language_code*/ + value.result_type.size() + value.iso_language_code.size());
}

struct URL
{
    std::string         url;
    std::string         expanded_url;
    std::string         display_url;
    IntVec              indices;
};
ThorsAnvil_MakeTrait(URL, url, expanded_url, display_url, indices);
inline void getStats(Stat* stat, URL const& value)
{
    stat->objectCount++;
    stat->memberCount += 4;
    stat->stringCount += 7; /* Key, Key, Key, Key, Value, Value, Value */
    stat->stringLength += (3 /*url*/ + 12 /*expanded_url*/ + 11 /*display_url*/ + 7 /*indices*/
                        + value.url.size() + value.expanded_url.size() + value.display_url.size());
    getStats(stat, value.indices);
}
using URLS = std::vector<URL>;

struct URLObject
{
    URLS                urls;
};
ThorsAnvil_MakeTrait(URLObject, urls);
inline void getStats(Stat* stat, URLObject const& value)
{
    stat->objectCount++;
    stat->memberCount += 1;
    stat->stringCount += 1;
    stat->stringLength += (4 /*urls*/);

    getStats(stat, value.urls);
}

struct Entities
{
    Opt<URLObject>      url;
    URLObject           description;
};
ThorsAnvil_MakeTrait(Entities, url, description);
inline void getStats(Stat* stat, Entities const& value)
{
    stat->objectCount++;
    stat->memberCount += 1;
    stat->stringCount += 1;
    stat->stringLength += 11 /*description*/;
    getStats(stat, value.description);

    getStatsMayNotExist(stat, value.url, 3 /*url*/);
}

struct User
{
    long                id;
    std::string         id_str;
    std::string         name;
    std::string         screen_name;
    std::string         location;
    std::string         description;
    Opt<std::string>    url;
    Entities            entities;
    int                 followers_count;
    int                 friends_count;
    int                 listed_count;
    std::string         created_at;
    int                 favourites_count;
    Opt<int>            utc_offset;
    Opt<std::string>    time_zone;
    bool                geo_enabled;
    bool                verified;
    int                 statuses_count;
    std::string         lang;
    bool                contributors_enabled;
    bool                is_translator;
    bool                is_translation_enabled;
    std::string         profile_background_color;
    std::string         profile_background_image_url;
    std::string         profile_background_image_url_https;
    bool                profile_background_tile;
    std::string         profile_image_url;
    std::string         profile_image_url_https;
    Opt<std::string>    profile_banner_url;
    std::string         profile_link_color;
    std::string         profile_sidebar_border_color;
    std::string         profile_sidebar_fill_color;
    std::string         profile_text_color;
    bool                profile_use_background_image;
    bool                default_profile;
    bool                default_profile_image;
    bool                following;
    bool                follow_request_sent;
    bool                notifications;
};
ThorsAnvil_MakeTrait(User, id, id_str, name, screen_name, location, description, url, entities, followers_count, friends_count, listed_count, created_at, favourites_count, utc_offset, time_zone, geo_enabled, verified, statuses_count, lang, contributors_enabled, is_translator, is_translation_enabled, profile_background_color, profile_background_image_url, profile_background_image_url_https, profile_background_tile, profile_image_url, profile_image_url_https, profile_banner_url, profile_link_color, profile_sidebar_border_color, profile_sidebar_fill_color, profile_text_color, profile_use_background_image, default_profile, default_profile_image, following, follow_request_sent, notifications);
inline void getStats(Stat* stat, User const& value)
{
    stat->objectCount++;
    stat->memberCount += 39;
    stat->stringCount += 39;
    stat->stringLength += (2 /*id*/ + 6 /*id_str*/ + 4 /*name*/ + 11 /*screen_name*/ + 8 /*location*/ + 11 /*description*/
                        + 3 /*url*/ + 8 /*entities*/ + 9 /*protected*/ + 15 /*followers_count*/ + 13 /*friends_count*/ + 12 /*listed_count*/
                        + 10 /*created_at*/ + 16 /*favourites_count*/ + 10 /*utc_offset*/ + 9 /*time_zone*/
                        + 11 /*geo_enabled*/ + 8 /*verified*/ + 14 /*statuses_count*/ + 4 /*lang*/ + 20 /*contributors_enabled*/
                        + 13 /*is_translator*/ + 22 /*is_translation_enabled*/ + 24 /*profile_background_color*/
                        + 28 /*profile_background_image_url*/ + 34 /*profile_background_image_url_https*/
                        + 23 /*profile_background_tile*/ + 17 /*profile_image_url*/ + 23 /*profile_image_url_https*/
                        + 18 /*profile_link_color*/ + 28 /*profile_sidebar_border_color*/
                        + 26 /*profile_sidebar_fill_color*/ + 18 /*profile_text_color*/ + 28 /*profile_use_background_image*/
                        + 15 /*default_profile*/ + 21 /*default_profile_image*/ + 9 /*following*/
                        + 19 /*follow_request_sent*/ + 13 /*notifications*/);

    getStats(stat, value.id);
    getStats(stat, value.id_str);
    getStats(stat, value.name);
    getStats(stat, value.screen_name);
    getStats(stat, value.location);
    getStats(stat, value.description);
    getStats(stat, value.url);
    getStats(stat, value.entities);
    getStats(stat, false); /*protected*/
    getStats(stat, value.followers_count);
    getStats(stat, value.friends_count);
    getStats(stat, value.listed_count);
    getStats(stat, value.created_at);
    getStats(stat, value.favourites_count);
    getStats(stat, value.utc_offset);
    getStats(stat, value.time_zone);
    getStats(stat, value.geo_enabled);
    getStats(stat, value.verified);
    getStats(stat, value.statuses_count);
    getStats(stat, value.lang);
    getStats(stat, value.contributors_enabled);
    getStats(stat, value.is_translator);
    getStats(stat, value.is_translation_enabled);
    getStats(stat, value.profile_background_color);
    getStats(stat, value.profile_background_image_url);
    getStats(stat, value.profile_background_image_url_https);
    getStats(stat, value.profile_background_tile);
    getStats(stat, value.profile_image_url);
    getStats(stat, value.profile_image_url_https);
    getStatsMayNotExist(stat, value.profile_banner_url, 18 /*profile_banner_url*/);
    getStats(stat, value.profile_link_color);
    getStats(stat, value.profile_sidebar_border_color);
    getStats(stat, value.profile_sidebar_fill_color);
    getStats(stat, value.profile_text_color);
    getStats(stat, value.profile_use_background_image);
    getStats(stat, value.default_profile);
    getStats(stat, value.default_profile_image);
    getStats(stat, value.following);
    getStats(stat, value.follow_request_sent);
    getStats(stat, value.notifications);
}
using Users = std::vector<User>;

struct Hashtag
{
    std::string         text;
    IntVec              indices;
};
ThorsAnvil_MakeTrait(Hashtag, text, indices);
inline void getStats(Stat* stat, Hashtag const& value)
{
    stat->objectCount++;
    stat->memberCount += 2;
    stat->stringCount += 3;
    stat->stringLength += (4 /*text*/ + 7 /*indices*/ + value.text.size());
    getStats(stat, value.indices);
}
using Hashtags = std::vector<Hashtag>;

struct UserMention
{
    std::string         screen_name;
    std::string         name;
    long                id;
    std::string         id_str;
    IntVec              indices;
};
ThorsAnvil_MakeTrait(UserMention, screen_name, name, id, id_str, indices);
inline void getStats(Stat* stat, UserMention const& value)
{
    stat->objectCount++;
    stat->memberCount += 5;
    stat->stringCount += 8; /*5 Keys + 3 values*/
    stat->stringLength += (11 /*screen_name*/ + 4 /*name*/ + 2 /*id*/ + 6 /*id_str*/ + 7 /*indices*/
                        + value.screen_name.size() + value.name.size() + value.id_str.size());
    stat->numberCount++;
    getStats(stat, value.indices);
}
using UserMentions = std::vector<UserMention>;

struct Size
{
    int                 w;
    int                 h;
    std::string         resize;
};
ThorsAnvil_MakeTrait(Size, w, h, resize);
inline void getStats(Stat* stat, Size const& value)
{
    stat->objectCount++;
    stat->memberCount += 3;
    stat->stringCount += 4; /* 3 Keys 1 Value */
    stat->stringLength += ( 1 /*w*/ + 1 /*h*/ + 6 /*resize*/
                            +value.resize.size());
    stat->numberCount += 2;
}

struct Sizes
{
    Size                medium;
    Size                small;
    Size                thumb;
    Size                large;
};
ThorsAnvil_MakeTrait(Sizes, medium, small, thumb, large);
inline void getStats(Stat* stat, Sizes const& value)
{
    stat->objectCount++;
    stat->memberCount += 4;
    stat->stringCount += 4;
    stat->stringLength += (6 /*medium*/ + 5 /*small*/ + 5 /*thumb*/ + 5 /*large*/);
    getStats(stat, value.medium);
    getStats(stat, value.small);
    getStats(stat, value.thumb);
    getStats(stat, value.large);
}

struct Media
{
    long                id;
    std::string         id_str;
    IntVec              indices;
    std::string         media_url;
    std::string         media_url_https;
    std::string         url;
    std::string         display_url;
    std::string         expanded_url;
    std::string         type;
    Sizes               sizes;
    Opt<long>           source_status_id;
    Opt<std::string>    source_status_id_str;
};
ThorsAnvil_MakeTrait(Media, id, id_str, indices, media_url, media_url_https, url, display_url, expanded_url, type, sizes, source_status_id, source_status_id_str);
inline void getStats(Stat* stat, Media const& value)
{
    stat->objectCount++;
    stat->memberCount += 10;
    stat->stringCount += 10;
    stat->stringLength += (2 /*id*/ + 6 /*id_str*/ + 7 /*indices*/ + 9 /*media_url*/ + 15 /*media_url_https*/ + 3 /*url*/
                        + 11 /*display_url*/ + 12 /*expanded_url*/ + 4 /*type*/ + 5 /*sizes*/);

    getStats(stat, value.id);
    getStats(stat, value.id_str);
    getStats(stat, value.indices);
    getStats(stat, value.media_url);
    getStats(stat, value.media_url_https);
    getStats(stat, value.url);
    getStats(stat, value.display_url);
    getStats(stat, value.expanded_url);
    getStats(stat, value.type);
    getStats(stat, value.sizes);
    getStatsMayNotExist(stat, value.source_status_id, 16 /*source_status_id*/);
    getStatsMayNotExist(stat, value.source_status_id_str, 20 /*source_status_id_str*/);
}
using Medias = std::vector<Media>;

struct TwitEntities
{
    Hashtags            hashtags;
    Symbols             symbols;
    URLS                urls;
    UserMentions        user_mentions;
    Opt<Medias>         media;
};
ThorsAnvil_MakeTrait(TwitEntities, hashtags, symbols, urls, user_mentions, media);
inline void getStats(Stat* stat, TwitEntities const& value)
{
    stat->objectCount++;
    stat->memberCount += 4;
    stat->stringCount += 4;
    stat->stringLength += (8 /*hashtags*/ + 7 /*symbols*/ + 4 /*urls*/ + 13 /*user_mentions*/);
    getStats(stat, value.hashtags);
    getStats(stat, value.symbols);
    getStats(stat, value.urls);
    getStats(stat, value.user_mentions);
    getStatsMayNotExist(stat, value.media, 5 /*media*/);
}

struct BaseStatus
{
    Metadata            metadata;
    std::string         created_at;
    long                id;
    std::string         id_str;
    std::string         text;
    std::string         source;
    bool                truncated;
    Opt<long>           in_reply_to_status_id;
    Opt<std::string>    in_reply_to_status_id_str;
    Opt<long>           in_reply_to_user_id;
    Opt<std::string>    in_reply_to_user_id_str;
    Opt<std::string>    in_reply_to_screen_name;
    User                user;
    Opt<int>            geo;
    Opt<int>            coordinates;
    Opt<int>            place;
    Opt<int>            contributors;
    int                 retweet_count;
    int                 favorite_count;
    TwitEntities        entities;
    bool                favorited;
    bool                retweeted;
    Opt<bool>           possibly_sensitive;
    std::string         lang;
};
ThorsAnvil_MakeTrait(BaseStatus, metadata, created_at, id, id_str, text, source, truncated, in_reply_to_status_id, in_reply_to_status_id_str, in_reply_to_user_id, in_reply_to_user_id_str, in_reply_to_screen_name, user, geo, coordinates, place, contributors, retweet_count, favorite_count, entities, favorited, retweeted, possibly_sensitive, lang);
inline void getStats(Stat* stat, BaseStatus const& value)
{
    stat->objectCount++;
    stat->memberCount += 23;
    stat->stringCount += 23;
    stat->stringLength += (8 /*metadata*/ + 10 /*created_at*/ + 2 /*id*/ + 6 /*id_str*/ + 4 /*text*/ + 6 /*source*/
                            + 9 /*truncated*/ + 21 /*in_reply_to_status_id*/ + 25 /*in_reply_to_status_id_str*/
                            + 19 /*in_reply_to_user_id*/ + 23 /*in_reply_to_user_id_str*/ + 23 /*in_reply_to_screen_name*/
                            + 4 /*user*/ + 3 /*geo*/ + 11 /*coordinates*/ + 5 /*place*/ + 12 /*contributors*/
                            + 13 /*retweet_count*/ + 14 /*favorite_count*/ + 8 /*entities*/ + 9 /*favorited*/
                            + 9 /*retweeted*/ + 4 /*lang*/);

    getStats(stat, value.metadata);
    getStats(stat, value.created_at);
    getStats(stat, value.id);
    getStats(stat, value.id_str);
    getStats(stat, value.text);
    getStats(stat, value.source);
    getStats(stat, value.truncated);
    getStats(stat, value.in_reply_to_status_id);
    getStats(stat, value.in_reply_to_status_id_str);
    getStats(stat, value.in_reply_to_user_id);
    getStats(stat, value.in_reply_to_user_id_str);
    getStats(stat, value.in_reply_to_screen_name);
    getStats(stat, value.user);
    getStats(stat, value.geo);
    getStats(stat, value.coordinates);
    getStats(stat, value.place);
    getStats(stat, value.contributors);
    getStats(stat, value.retweet_count);
    getStats(stat, value.favorite_count);
    getStats(stat, value.entities);
    getStats(stat, value.favorited);
    getStats(stat, value.retweeted);
    getStatsMayNotExist(stat, value.possibly_sensitive, 18 /*possibly_sensitive*/);
    getStats(stat, value.lang);
}

struct MainStatus: public BaseStatus
{
    Opt<BaseStatus>     retweeted_status;
};
ThorsAnvil_ExpandTrait(BaseStatus, MainStatus, retweeted_status);
inline void getStats(Stat* stat, MainStatus const& value)
{
    getStats(stat, static_cast<BaseStatus const&>(value));
    getStatsMayNotExist(stat, value.retweeted_status, 16 /*retweeted_status*/);
}
using Statuses = std::vector<MainStatus>;

struct SearchMetadata
{
    double              completed_in;
    long                max_id;
    std::string         max_id_str;
    std::string         next_results;
    std::string         query;
    std::string         refresh_url;
    int                 count;
    long                since_id;
    std::string         since_id_str;
};
ThorsAnvil_MakeTrait(SearchMetadata, completed_in, max_id, max_id_str, next_results, query, refresh_url, count, since_id, since_id_str);
inline void getStats(Stat* stat, SearchMetadata const& value)
{
    stat->objectCount++;
    stat->memberCount += 9;
    stat->stringCount += (9 + 5);
    stat->stringLength += (12 /*completed_in*/ + 6 /*max_id*/ + 10 /*max_id_str*/ + 12 /*next_results*/
                            + 5 /*query*/ + 11 /*refresh_url*/ + 5 /*count*/ + 8 /*since_id*/
                            + 12 /*since_id_str*/ + value.max_id_str.size() + value.next_results.size()
                            + value.query.size() + value.refresh_url.size() + value.since_id_str.size());
    stat->numberCount += 4;
}

struct Twitter
{
    Statuses            statuses;
    SearchMetadata      search_metadata;
};
ThorsAnvil_MakeTrait(Twitter, statuses, search_metadata);
inline void getStats(Stat* stat, Twitter const& value)
{
    stat->objectCount++;
    stat->memberCount += 2;
    stat->stringCount += 2;
    stat->stringLength += (8 /*statuses*/ + 15 /*search_metadata*/);
    getStats(stat, value.statuses);
    getStats(stat, value.search_metadata);
}
#endif
