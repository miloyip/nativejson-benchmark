//          Copyright John W. Wilkinson 2007 - 2014
// Distributed under the MIT License, see accompanying file LICENSE.txt

// json spirit version 4.08

// This demo shows you how to read and write JSON objects and arrays.
// In this demo objects are stored as a vector of name/value pairs.

#include "json_spirit.h"
#include <cassert>
#include <fstream>

#ifndef JSON_SPIRIT_VALUE_ENABLED
#error Please define JSON_SPIRIT_VALUE_ENABLED for the Value type to be enabled 
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

void write_address( Array& a, const Address& addr )
{
    Object addr_obj;

    addr_obj.push_back( Pair( "house_number", addr.house_number_ ) );
    addr_obj.push_back( Pair( "road",         addr.road_ ) );
    addr_obj.push_back( Pair( "town",         addr.town_ ) );
    addr_obj.push_back( Pair( "county",       addr.county_ ) );
    addr_obj.push_back( Pair( "country",      addr.country_ ) );

    a.push_back( addr_obj );
}

Address read_address( const Object& obj )
{
    Address addr;

    for( Object::size_type i = 0; i != obj.size(); ++i )
    {
        const Pair& pair = obj[i];

        const string& name  = pair.name_;
        const Value&  value = pair.value_;

        if( name == "house_number" )
        {
            addr.house_number_ = value.get_int();
        }
        else if( name == "road" )
        {
            addr.road_ = value.get_str();
        }
        else if( name == "town" )
        {
            addr.town_ = value.get_str();
        }
        else if( name == "county" )
        {
            addr.county_ = value.get_str();
        }
        else if( name == "country" )
        {
            addr.country_ = value.get_str();
        }
        else
        {
            assert( false );
        }
    }

    return addr;
}

void write_addrs( const char* file_name, const Address addrs[] )
{
    Array addr_array;

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

    Value value;

    read( is, value );

    const Array& addr_array = value.get_array();

    vector< Address > addrs;

    for( unsigned int i = 0; i < addr_array.size(); ++i )
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
