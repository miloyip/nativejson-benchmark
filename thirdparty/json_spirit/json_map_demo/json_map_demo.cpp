//          Copyright John W. Wilkinson 2007 - 2014
// Distributed under the MIT License, see accompanying file LICENSE.txt

// json spirit version 4.08

// This demo shows you how to read and write JSON objects and arrays.
// In this demo objects are stored as a map of names to values.

#include "json_spirit.h"
#include <cassert>
#include <fstream>

#ifndef JSON_SPIRIT_MVALUE_ENABLED
#error Please define JSON_SPIRIT_MVALUE_ENABLED for the mValue type to be enabled 
#endif

using namespace std;
using namespace json_spirit;

struct Address
{
    int house_number_;
    string road_;
    string town_;
    string county_;
    string country_;
};

bool operator==( const Address& a1, const Address& a2 )
{
    return ( a1.house_number_ == a2.house_number_ ) &&
           ( a1.road_         == a2.road_ ) &&
           ( a1.town_         == a2.town_ ) &&
           ( a1.county_       == a2.county_ ) &&
           ( a1.country_      == a2.country_ );
}

void write_address( mArray& a, const Address& addr )
{
    mObject addr_obj;

    addr_obj[ "house_number" ] = addr.house_number_;
    addr_obj[ "road"         ] = addr.road_;
    addr_obj[ "town"         ] = addr.town_;
    addr_obj[ "county"       ] = addr.county_;
    addr_obj[ "country"      ] = addr.country_;

    a.push_back( addr_obj );
}

const mValue& find_value( const mObject& obj, const string& name  )
{
    mObject::const_iterator i = obj.find( name );

    assert( i != obj.end() );
    assert( i->first == name );

    return i->second;
}

Address read_address( const mObject& obj )
{
    Address addr;

    addr.house_number_ = find_value( obj, "house_number" ).get_int();
    addr.road_         = find_value( obj, "road"         ).get_str();
    addr.town_         = find_value( obj, "town"         ).get_str();
    addr.county_       = find_value( obj, "county"       ).get_str();
    addr.country_      = find_value( obj, "country"      ).get_str();

    return addr;
}

void write_addrs( const char* file_name, const Address addrs[] )
{
    mArray addr_array;

    for( int i = 0; i < 5; ++i )
    {
        write_address( addr_array, addrs[i] );
    }

    ofstream os( file_name );

    write_formatted( addr_array, os );

    os.close();
}

vector< Address > read_addrs( const char* file_name )
{
    ifstream is( file_name );

    mValue value;

    read( is, value );

    const mArray& addr_array = value.get_array();

    vector< Address > addrs;

    for( vector< Address >::size_type i = 0; i < addr_array.size(); ++i )
    {
        addrs.push_back( read_address( addr_array[i].get_obj() ) );
    }

    return addrs;
}

int main()
{
    const Address addrs[5] = { { 42, "East Street",  "Newtown",     "Essex",         "England" },
                               { 1,  "West Street",  "Hull",        "Yorkshire",     "England" },
                               { 12, "South Road",   "Aberystwyth", "Dyfed",         "Wales"   },
                               { 45, "North Road",   "Paignton",    "Devon",         "England" },
                               { 78, "Upper Street", "Ware",        "Hertfordshire", "England" } };

    const char* file_name( "demo.txt" );

    write_addrs( file_name, addrs );

    vector< Address > new_addrs = read_addrs( file_name );

    assert( new_addrs.size() == 5 );

    for( int i = 0; i < 5; ++i )
    {
        assert( new_addrs[i] == addrs[i] );
    }

	return 0;
}
;