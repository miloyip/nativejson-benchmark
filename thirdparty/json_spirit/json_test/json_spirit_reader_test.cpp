//          Copyright John W. Wilkinson 2007 - 2014
// Distributed under the MIT License, see accompanying file LICENSE.txt

// json spirit version 4.08

#include "json_spirit_reader_test.h"
#include "utils_test.h"
#include "json_spirit_reader.h"
#include "json_spirit_value.h" 
#include "json_spirit_writer.h" 

#include <limits.h>
#include <sstream>
#include <boost/assign/list_of.hpp>
#include <boost/timer.hpp>
#include <boost/lexical_cast.hpp>

using namespace json_spirit;
using namespace std;
using namespace boost;
using namespace boost::assign;

namespace
{
    template< class String_type, class Value_type >
    void test_read( const String_type& s, Value_type& value )
    {
        // performs both types of read and checks they produce the same value

        read( s, value );

        Value_type value_2;

        read_or_throw( s, value_2 );

        assert_eq( value, value_2 );
    }

    template< class Config_type >
    struct Test_runner
    {
        typedef typename Config_type::String_type String_type;
        typedef typename Config_type::Object_type Object_type;
        typedef typename Config_type::Array_type Array_type;
        typedef typename Config_type::Value_type Value_type;
        typedef typename Config_type::Pair_type Pair_type;
        typedef typename String_type::value_type  Char_type;
        typedef typename String_type::const_iterator Iter_type;
        typedef std::basic_istringstream< Char_type > Istringstream_type;
        typedef std::basic_istream< Char_type > Istream_type;

        String_type to_str( const char* c_str )
        {
            return ::to_str< String_type >( c_str );
        }

        Test_runner()
        {
        }

        void check_eq( const Object_type& obj_1, const Object_type& obj_2 )
        {
            const typename Object_type::size_type size( obj_1.size() );

            assert_eq( size, obj_2.size() );

            typename Object_type::const_iterator i1 = obj_1.begin();
            typename Object_type::const_iterator i2 = obj_2.begin();

            for( ; i1 != obj_1.end(); ++i1, ++i2 )
            {
                assert_eq( *i1, *i2 ); 
            }
        }

        void add_value( Object_type& obj, const char* c_name, const Value_type& value )
        {
            Config_type::add( obj, to_str( c_name ), value );
        }

        void add_c_str( Object_type& obj, const char* c_name, const char* c_value )
        {
            add_value( obj, c_name, to_str( c_value ) );
        }

        void test_syntax( const char* c_str, bool expected_success = true )
        {
            const String_type str = to_str( c_str );

            Value_type value;

            const bool ok = read( str, value );

            assert_eq( ok, expected_success  );

            try
            {
                read_or_throw( str, value );

                assert( expected_success );
            }
            catch( ... )
            {
                assert( !expected_success );
            }
        }

        template< typename Int >
        void test_syntax( Int min_int, Int max_int )
        {
            ostringstream os;

            os << "[" << min_int << "," << max_int << "]";

            test_syntax( os.str().c_str() );
        }

        void test_syntax()
        {
            test_syntax( "{}" );
            test_syntax( "{ }" );
            test_syntax( "{ } " );
            test_syntax( "{ }  " );
            test_syntax( "{\"\":\"\"}" );
            test_syntax( "{\"test\":\"123\"}" );
            test_syntax( "{\"test\" : \"123\"}" );
            test_syntax( "{\"testing testing testing\":\"123\"}" );
            test_syntax( "{\"\":\"abc\"}" );
            test_syntax( "{\"abc\":\"\"}" );
            test_syntax( "{\"\":\"\"}" );
            test_syntax( "{\"test\":true}" );
            test_syntax( "{\"test\":false}" );
            test_syntax( "{\"test\":null}" );
            test_syntax( "{\"test1\":\"123\",\"test2\":\"456\"}" );
            test_syntax( "{\"test1\":\"123\",\"test2\":\"456\",\"test3\":\"789\"}" );
            test_syntax( "{\"test1\":{\"test2\":\"123\",\"test3\":\"456\"}}" );
            test_syntax( "{\"test1\":{\"test2\":{\"test3\":\"456\"}}}" );
            test_syntax( "{\"test1\":[\"a\",\"bb\",\"cc\"]}" );
            test_syntax( "{\"test1\":[true,false,null]}" );
            test_syntax( "{\"test1\":[true,\"abc\",{\"a\":\"b\"},{\"d\":false},null]}" );
            test_syntax( "{\"test1\":[1,2,-3]}" );
            test_syntax( "{\"test1\":[1.1,2e4,-1.234e-34]}" );
            test_syntax( "{\n"
                          "\t\"test1\":\n"
                          "\t\t{\n"
                          "\t\t\t\"test2\":\"123\",\n"
                          "\t\t\t\"test3\":\"456\"\n"
                          "\t\t}\n"
                          "}\n" );
            test_syntax( "[]" );
            test_syntax( "[ ]" );
            test_syntax( "[1,2,3]" );
            test_syntax( "[ 1, -2, 3]" );
            test_syntax( "[ 1.2, -2e6, -3e-6 ]" );
            test_syntax( "[ 1.2, \"str\", -3e-6, { \"field\" : \"data\" } ]" );

            test_syntax( INT_MIN, INT_MAX );
            test_syntax( LLONG_MIN, LLONG_MAX );
            test_syntax( "[1 2 3]", false );
        }

        Value_type read_cstr( const char* c_str )
        {
            Value_type value;

            test_read( to_str( c_str ), value );

            return value;
        }

        void read_cstr( const char* c_str, Value_type& value )
        {
            test_read( to_str( c_str ), value );
        }

        void check_reading( const char* c_str )
        {
            Value_type value;

            String_type in_s( to_str( c_str ) );

            test_read( in_s, value );

            const String_type result = write_formatted( value ); 

            assert_eq( in_s, result );
        }

        template< typename Int >
        void check_reading( Int min_int, Int max_int )
        {
            ostringstream os;

            os << "[\n"
                   "    " << min_int << ",\n"
                   "    " << max_int << "\n"
                   "]";

            check_reading( os.str().c_str() );
        }

        void test_reading()
        {
            check_reading( "{\n}" );

            Object_type obj;
            Value_type value;

            read_cstr( "{\n"
                       "    \"name 1\" : \"value 1\"\n"
                       "}", value );

            add_c_str( obj, "name 1", "value 1" );

            check_eq( value.get_obj(), obj );

            read_cstr( "{\"name 1\":\"value 1\",\"name 2\":\"value 2\"}", value );

            add_c_str( obj, "name 2", "value 2" );

            check_eq( value.get_obj(), obj );

            read_cstr( "{\n"
                       "    \"name 1\" : \"value 1\",\n"
                       "    \"name 2\" : \"value 2\",\n"
                       "    \"name 3\" : \"value 3\"\n"
                       "}", value );

            add_c_str( obj, "name 3", "value 3" );

            check_eq( value.get_obj(), obj );

            check_reading( "{\n"
                            "    \"\" : \"value\",\n"
                            "    \"name\" : \"\"\n"
                            "}" );

            check_reading( "{\n"
                            "    \"name 1\" : \"value 1\",\n"
                            "    \"name 2\" : {\n"
                            "        \"name 3\" : \"value 3\",\n"
                            "        \"name_4\" : \"value_4\"\n"
                            "    }\n"
                            "}" );

            check_reading( "{\n"
                            "    \"name 1\" : \"value 1\",\n"
                            "    \"name 2\" : {\n"
                            "        \"name 3\" : \"value 3\",\n"
                            "        \"name_4\" : \"value_4\",\n"
                            "        \"name_5\" : {\n"
                            "            \"name_6\" : \"value_6\",\n"
                            "            \"name_7\" : \"value_7\"\n"
                            "        }\n"
                            "    }\n"
                            "}" );

            check_reading( "{\n"
                            "    \"name 1\" : \"value 1\",\n"
                            "    \"name 2\" : {\n"
                            "        \"name 3\" : \"value 3\",\n"
                            "        \"name_4\" : {\n"
                            "            \"name_5\" : \"value_5\",\n"
                            "            \"name_6\" : \"value_6\"\n"
                            "        },\n"
                            "        \"name_7\" : \"value_7\"\n"
                            "    }\n"
                            "}" );

            check_reading( "{\n"
                            "    \"name 1\" : \"value 1\",\n"
                            "    \"name 2\" : {\n"
                            "        \"name 3\" : \"value 3\",\n"
                            "        \"name_4\" : {\n"
                            "            \"name_5\" : \"value_5\",\n"
                            "            \"name_6\" : \"value_6\"\n"
                            "        },\n"
                            "        \"name_7\" : \"value_7\"\n"
                            "    },\n"
                            "    \"name_8\" : \"value_8\",\n"
                            "    \"name_9\" : {\n"
                            "        \"name_10\" : \"value_10\"\n"
                            "    }\n"
                            "}" );

            check_reading( "{\n"
                            "    \"name 1\" : {\n"
                            "        \"name 2\" : {\n"
                            "            \"name 3\" : {\n"
                            "                \"name_4\" : {\n"
                            "                    \"name_5\" : \"value\"\n"
                            "                }\n"
                            "            }\n"
                            "        }\n"
                            "    }\n"
                            "}" );

            check_reading( "{\n"
                            "    \"name 1\" : \"value 1\",\n"
                            "    \"name 2\" : true,\n"
                            "    \"name 3\" : false,\n"
                            "    \"name_4\" : \"value_4\",\n"
                            "    \"name_5\" : true\n"
                            "}" );

            check_reading( "{\n"
                            "    \"name 1\" : \"value 1\",\n"
                            "    \"name 2\" : null,\n"
                            "    \"name 3\" : \"value 3\",\n"
                            "    \"name_4\" : null\n"
                            "}" );

            check_reading( "{\n"
                            "    \"name 1\" : \"value 1\",\n"
                            "    \"name 2\" : 123,\n"
                            "    \"name 3\" : \"value 3\",\n"
                            "    \"name_4\" : -567\n"
                            "}" );

            check_reading( "[\n]" );

            check_reading( "[\n"
                           "    1\n"
                           "]" );

            check_reading( "[\n"
                           "    1,\n"
                           "    1.2,\n"
                           "    \"john]\",\n"
                           "    true,\n"
                           "    false,\n"
                           "    null\n"
                           "]" );

            check_reading( "[\n"
                           "    1,\n"
                           "    [\n"
                           "        2,\n"
                           "        3\n"
                           "    ]\n"
                           "]" );

            check_reading( "[\n"
                           "    1,\n"
                           "    [\n"
                           "        2,\n"
                           "        3\n"
                           "    ],\n"
                           "    [\n"
                           "        4,\n"
                           "        [\n"
                           "            5,\n"
                           "            6,\n"
                           "            7\n"
                           "        ]\n"
                           "    ]\n"
                           "]" );

            check_reading( "[\n"
                           "    {\n"
                           "        \"name\" : \"value\"\n"
                           "    }\n"
                           "]" );

            check_reading( "{\n"
                           "    \"name\" : [\n"
                           "        1\n"
                           "    ]\n"
                           "}" );

            check_reading( "[\n"
                           "    {\n"
                           "        \"name 1\" : \"value\",\n"
                           "        \"name 2\" : [\n"
                           "            1,\n"
                           "            2,\n"
                           "            3\n"
                           "        ]\n"
                           "    }\n"
                           "]" );

            check_reading( "{\n"
                           "    \"name 1\" : [\n"
                           "        1,\n"
                           "        {\n"
                           "            \"name 2\" : \"value 2\"\n"
                           "        }\n"
                           "    ]\n"
                           "}" );

            check_reading( "[\n"
                           "    {\n"
                           "        \"name 1\" : \"value 1\",\n"
                           "        \"name 2\" : [\n"
                           "            1,\n"
                           "            2,\n"
                           "            {\n"
                           "                \"name 3\" : \"value 3\"\n"
                           "            }\n"
                           "        ]\n"
                           "    }\n"
                           "]" );

            check_reading( "{\n"
                           "    \"name 1\" : [\n"
                           "        1,\n"
                           "        {\n"
                           "            \"name 2\" : [\n"
                           "                1,\n"
                           "                2,\n"
                           "                3\n"
                           "            ]\n"
                           "        }\n"
                           "    ]\n"
                           "}" );

            check_reading( INT_MIN, INT_MAX );
            check_reading( LLONG_MIN, LLONG_MAX );
        }

        void test_reading_reals()
        {
            Value_type value;

            const String_type in_s = to_str( "[1.200000000000000,1.234567890123456e+125,-1.234000000000000e-123,"
                                             " 1.000000000000000e-123,1234567890.123456,123]" );

            basic_istringstream< Char_type > is( in_s );

            const bool ok = read( is, value );

            assert_eq( ok, true );
            assert_eq( value.type(), array_type );

            const Array_type arr = value.get_array();

            assert_eq( arr.size(), 6 );
            assert_eq( arr[0].get_real(), 1.2, 1e-16 );
            assert_eq( arr[1].get_real(), 1.234567890123456e+125, 1e+110 );
            assert_eq( arr[2].get_real(), -1.234e-123, 1e+108 );
            assert_eq( arr[3].get_real(), 1e-123, 1e+108 );
            assert_eq( arr[4].get_real(), 1234567890.123456, 1e-7 );
            assert_eq( arr[5].get_real(), 123, 1e-13 );
        }

        void test_from_stream( const char* json_str, bool expected_success,
                               const Error_position& expected_error )
        {
            Value_type value;

            String_type in_s( to_str( json_str ) );

            basic_istringstream< Char_type > is( in_s );

            const bool ok = read( is, value );

            assert_eq( ok, expected_success );

            if( ok )
            {
                assert_eq( in_s, write( value ) );
            }

            try
            {
                basic_istringstream< Char_type > is( in_s );

                read_or_throw( is, value );

                assert_eq( expected_success, true );

                assert_eq( in_s, write( value ) );
            }
            catch( const Error_position error )
            {
                assert_eq( error, expected_error );
            }
        }

        void test_from_stream()
        {
            test_from_stream( "[1,2]", true, Error_position() );
            test_from_stream( "\n\n foo", false, Error_position( 3, 2,"not a value"  ) );
        }

        void test_escape_chars( const char* json_str, const char* c_str )
        {
            Value_type value;

            string s( string( "{\"" ) + json_str + "\" : \"" + json_str + "\"} " );

            read_cstr( s.c_str(), value );

            const Pair_type& pair( *value.get_obj().begin() );

            assert_eq( Config_type::get_name ( pair ), to_str( c_str ) );
            assert_eq( Config_type::get_value( pair ), to_str( c_str ) );
        }

        void test_escape_chars()
        {
            test_escape_chars( "\\t", "\t");
            test_escape_chars( "a\\t", "a\t" );
            test_escape_chars( "\\tb", "\tb" );
            test_escape_chars( "a\\tb", "a\tb" );
            test_escape_chars( "a\\tb", "a\tb" );
            test_escape_chars( "a123\\tb", "a123\tb" );
            test_escape_chars( "\\t\\n\\\\", "\t\n\\" );
            test_escape_chars( "\\/\\r\\b\\f\\\"", "/\r\b\f\"" );
            test_escape_chars( "\\h\\j\\k", "" ); // invalid esc chars
            test_escape_chars( "\\x61\\x62\\x63", "abc" );
            test_escape_chars( "a\\x62c", "abc" );
            test_escape_chars( "\\x01\\x02\\x7F", "\x01\x02\x7F" ); // NB x7F is the greatest char spirit will parse
            test_escape_chars( "\\u0061\\u0062\\u0063", "abc" );
        }

        void check_is_null( const char* c_str  )
        {
            assert_eq( read_cstr( c_str ).type(), null_type ); 
        }

        template< typename T >
        void check_value( const char* c_str, const T& expected_value )
        {
            const Value_type v( read_cstr( c_str ) );
            
            assert_eq( v.template get_value< T >(), expected_value ); 
        }

        void test_values()
        {
            check_value( "1",        1 );
            check_value( "1.5",      1.5 );
            check_value( "\"Test\"", to_str( "Test" ) );
            check_value( "true",     true );
            check_value( "false",    false );
            check_is_null( "null" );
        }

        void check_read_fails( const char* c_str, int line, int column, const string& reason )
        {
            Value_type value;

            try
            {
                read_cstr( c_str, value );

                assert( false );
            }
            catch( const Error_position posn )
            {
                assert_eq( posn, Error_position( line, column, reason ) );
            }
        }

        void test_error_cases()
        {
            check_read_fails( "",                       1, 1,  "not a value" );
            check_read_fails( "foo",                    1, 1,  "not a value" );
            check_read_fails( " foo",                   1, 2,  "not a value" );
            check_read_fails( "  foo",                  1, 3,  "not a value" );
            check_read_fails( "\n\n foo",               3, 2,  "not a value" );
            check_read_fails( "!!!",                    1, 1,  "not a value" );
            check_read_fails( "\"bar",                  1, 1,  "not a value" );
            check_read_fails( "bar\"",                  1, 1,  "not a value" );
            check_read_fails( "[1}",                    1, 3,  "not an array" );
            check_read_fails( "[1,2?",                  1, 5,  "not an array" );
            check_read_fails( "[1,2}",                  1, 5,  "not an array" );
            check_read_fails( "[1;2]",                  1, 3,  "not an array" );
            check_read_fails( "[1,\n2,\n3,]",           3, 2,  "not an array" );
            check_read_fails( "{\"name\":\"value\"]",   1, 16, "not an object" );
            check_read_fails( "{\"name\",\"value\"}",   1, 8,  "no colon in pair" );
            check_read_fails( "{name:\"value\"}",       1, 2,  "not an object" );
            check_read_fails( "{\n1:\"value\"}",        2, 1,  "not an object" );
            check_read_fails( "{\n  name\":\"value\"}", 2, 3,  "not an object" );
            check_read_fails( "{\"name\":foo}",         1, 9,  "not a value" );
            check_read_fails( "{\"name\":value\"}",     1, 9,  "not a value" );
        }

        typedef vector< int > Ints;

        bool test_read_range( Iter_type& first, Iter_type last, Value_type& value )
        {
            Iter_type first_ = first;

            const bool ok = read( first, last, value );

            try
            {
                Value_type value_;

                read_or_throw( first_, last, value_ );

                assert_eq( ok, true );
                assert_eq( value, value_ );
            }
            catch( ... )
            {
                assert_eq( ok, false );
            }

            return ok;
        }

        void check_value_sequence( Iter_type first, Iter_type last, const Ints& expected_values, bool all_input_consumed )
        {
            Value_type value;
            
            for( Ints::size_type i = 0; i < expected_values.size(); ++i )
            {
                const bool ok = test_read_range( first, last, value );

                assert_eq( ok, true );

                const bool is_last( i == expected_values.size() - 1 );

                assert_eq( first == last, is_last ? all_input_consumed : false );
            }
  
            const bool ok = test_read_range( first, last, value );

            assert_eq( ok, false );
        }

        void check_value_sequence( Istream_type& is, const Ints& expected_values, bool all_input_consumed )
        {
            Value_type value;
            
            for( Ints::size_type i = 0; i < expected_values.size(); ++i )
            {
                read_or_throw( is, value );

                assert_eq( value.get_int(), expected_values[i] ); 

                const bool is_last( i == expected_values.size() - 1 );

                assert_eq( is.eof(), is_last ? all_input_consumed : false );
            }
                
            try
            {
                read_or_throw( is, value );

                assert( false );
            }
            catch( ... )
            {
            }
             
            assert_eq( is.eof(), true );
        }

        void check_value_sequence( const char* c_str, const Ints& expected_values, bool all_input_consumed )
        {
            const String_type s( to_str( c_str ) );

            check_value_sequence( s.begin(), s.end(), expected_values, all_input_consumed );

            Istringstream_type is( s );

            check_value_sequence( is, expected_values, all_input_consumed );
        }

        void check_array( const Value_type& value, typename Array_type::size_type expected_size )
        {
            assert_eq( value.type(), array_type );

            const Array_type& arr = value.get_array();

            assert_eq( arr.size(), expected_size );

            for( typename Array_type::size_type i = 0; i < expected_size; ++i )
            {
                const Value_type& val = arr[i];

                assert_eq( val.type(), int_type );
                assert_eq( val.get_int(), int( i + 1 ) );
            }
        }

        void check_reading_array( Iter_type& begin, Iter_type end, typename Array_type::size_type expected_size )
        {
            Value_type value;

            test_read_range( begin, end, value );

            check_array( value, expected_size );
        }

        void check_reading_array( Istream_type& is, typename Array_type::size_type expected_size )
        {
            Value_type value;

            read( is, value );

            check_array( value, expected_size );
        }

        void check_reading_arrays( const char* arrays_str )
        {
            const String_type str( to_str( arrays_str ) );

            Iter_type       begin = str.begin();
            const Iter_type end   = str.end();

            check_reading_array( begin, end, 0 );
            check_reading_array( begin, end, 1 );
            check_reading_array( begin, end, 2 );
            check_reading_array( begin, end, 3 );

            Istringstream_type is( str );

            check_reading_array( is, 0 );
            check_reading_array( is, 1 );
            check_reading_array( is, 2 );
            check_reading_array( is, 3 );
        }

        void test_sequence_of_values()
        {
            check_value_sequence( "",   Ints(), false );
            check_value_sequence( " ",  Ints(), false );
            check_value_sequence( "  ", Ints(), false );
            check_value_sequence( "     10 ",      list_of( 10 ), false );
            check_value_sequence( "     10 11 ",   list_of( 10 )( 11 ), false );
            check_value_sequence( "     10 11 12", list_of( 10 )( 11 )( 12 ), true);
            check_value_sequence( "10 11 12",      list_of( 10 )( 11 )( 12 ), true);

            // 

            check_reading_arrays( "[] [ 1 ] [ 1, 2 ] [ 1, 2, 3 ]" );
 //         check_reading_arrays( "[][1][1,2][1,2,3]" );  // fails due to multi_pass iterator bug,
                                                          // use stream_reader class instead
        }

        void test_uint64( const char* value_str, int expected_int, int64_t expected_int64, uint64_t expected_uint64 )
        {
            const Value_type v( read_cstr( value_str ) );
            
            assert_eq( v.get_int(),    expected_int ); 
            assert_eq( v.get_int64(),  expected_int64 ); 
            assert_eq( v.get_uint64(), expected_uint64 ); 
        } 

        void test_uint64()
        {
            test_uint64( "0", 0, 0, 0 );
            test_uint64( "1", 1, 1, 1 );
            test_uint64( "-1", -1, -1, ULLONG_MAX );
            test_uint64( "18446744073709551615", -1, -1, ULLONG_MAX );
        }

        void test_types()
        {
            Value_type value;

            read( to_str( "[ \"foo\", true, false, 1, 12.3, null ]" ), value );

            assert_eq( value.type(), array_type );

            const Array_type& a = value.get_array();

            assert_eq( a[0].get_str(), to_str( "foo" ) );
            assert_eq( a[1].get_bool(), true );
            assert_eq( a[2].get_bool(), false );
            assert_eq( a[3].get_int(), 1 );
            assert_eq( a[3].get_int64(), 1 );
            assert_eq( a[3].get_uint64(), 1u );
            assert_eq( a[3].get_real(), 1.0 );
            assert_eq( a[4].get_real(), 12.3 );
            assert_eq( a[5].is_null(), true );
        }
        
        void test_comments()
        {
            Value_type value_1;

            read_cstr( "{\n"
                       "    \"name 1\" : \"value 1\",\n"
                       "    \"name 2\" : \"value 2 /* not a comment but data */\",\n"
                       "    \"name 3\" : \"value 3 // not a comment but data\"\n"
                       "}", value_1 );

            Value_type value_2;

            read_cstr( "{// a comment\n "
                       "    \"name 1\" : /* another comment */ \"value 1\",\n"
                       "    \"name 2\" : \"value 2 /* not a comment but data */\",\n"
                       " //   \"name 2\" : \"value 2\",\n"
                       "    \"name 3\" : \"value 3 // not a comment but data\"\n"
                       "/* multi\n"
                       "line\n"
                       "comment */}", value_2 );

            assert_eq( value_1, value_2 );
        }
        
        void run_tests()
        {
            test_syntax();
            test_reading();
            test_reading_reals();
            test_from_stream();
            test_escape_chars();
            test_values();
            test_error_cases();
            test_sequence_of_values();
            test_uint64();
            test_types();
            test_comments();
        }
    };

#if defined( JSON_SPIRIT_WVALUE_ENABLED ) && !defined( BOOST_NO_STD_WSTRING )
    void test_wide_esc_u()
    {
        wValue value;

        test_read( L"[\"\\uABCD\"]", value );

        const wstring s( value.get_array()[0].get_str() );

        assert_eq( s.length(), static_cast< wstring::size_type >( 1u ) );
        assert_eq( s[0], 0xABCD );
    }
#endif

#ifdef JSON_SPIRIT_VALUE_ENABLED
    void test_extended_ascii( const string& s )
    {
        Value value;

        test_read( "[\"" + s + "\"]", value );

        assert_eq( value.get_array()[0].get_str(), "äöüß" );
    }

    void test_extended_ascii()
    {
        test_extended_ascii( "\\u00E4\\u00F6\\u00FC\\u00DF" );
        test_extended_ascii( "äöüß" );
    }
#endif
}

//#include <fstream>

void json_spirit::test_reader()
{
#ifdef JSON_SPIRIT_VALUE_ENABLED
    Test_runner< Config  >().run_tests();
    test_extended_ascii();
#endif
#ifdef JSON_SPIRIT_MVALUE_ENABLED
    Test_runner< mConfig >().run_tests();
#endif
#if defined( JSON_SPIRIT_WVALUE_ENABLED ) && !defined( BOOST_NO_STD_WSTRING )
    Test_runner< wConfig  >().run_tests();
    test_wide_esc_u();
#endif
#if defined( JSON_SPIRIT_WMVALUE_ENABLED ) && !defined( BOOST_NO_STD_WSTRING )
    Test_runner< wmConfig >().run_tests();
#endif

#ifndef _DEBUG
    //ifstream ifs( "test.txt" );

    //string s;

    //getline( ifs, s );

    //timer t;

    //for( int i = 0; i < 2000; ++i )
    //{
    //    Value value;

    //    read( s, value );
    //}

    //cout << t.elapsed() << endl;

//    const string so = write( value );

    //Object obj;

    //for( int i = 0; i < 100000; ++i )
    //{
    //    obj.push_back( Pair( "\x01test\x7F", lexical_cast< string >( i ) ) );
    //}

    //const string s = write( obj );

    //Value value;

    //timer t;

    //read( s, value );

    //cout << t.elapsed() << endl;

    //cout << "obj size " << value.get_obj().size();
#endif
}
