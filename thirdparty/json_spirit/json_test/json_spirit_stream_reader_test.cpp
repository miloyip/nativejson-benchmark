//          Copyright John W. Wilkinson 2007 - 2014
// Distributed under the MIT License, see accompanying file LICENSE.txt

// json spirit version 4.08

#include "json_spirit_stream_reader_test.h"
#include "utils_test.h"
#include "json_spirit_stream_reader.h"

#include <sstream>
#include <boost/assign/list_of.hpp>

using namespace json_spirit;
using namespace std;
using namespace boost;
using namespace boost::assign;

namespace
{
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

        void check_stream_reader( Stream_reader< Istream_type, Value_type >& reader, const vector< int >& expected_result )
        {
            Value_type v;
            const bool ok = reader.read_next( v );
            assert_eq( ok, true );
            assert_eq( v.type(), array_type );
            assert_eq( v.get_array().size(), expected_result.size() );
            for( vector< int >::size_type i = 0; i < v.get_array().size(); ++i )
            {
                assert_eq( v.get_array()[i], expected_result[i] );
            }
        }

        void check_stream_read_or_throw( Stream_reader_thrower< Istream_type, Value_type >& reader, const vector< int >& expected_result )
        {
            Value_type v;

            try
            {
                reader.read_next( v );
                assert_eq( v.type(), array_type );
                assert_eq( v.get_array().size(), expected_result.size() );
                for( vector< int >::size_type i = 0; i < v.get_array().size(); ++i )
                {
                    assert_eq( v.get_array()[i], expected_result[i] );
                }
            }
            catch( ... )
            {
                assert( false );
            }
        }

        void test_stream_reader( const char* s )
        {
            {
                Istringstream_type is( to_str( s ) );

                Stream_reader< Istream_type, Value_type > reader( is );

                check_stream_reader( reader, vector< int >() );
                check_stream_reader( reader, list_of( 1 ) );
                check_stream_reader( reader, list_of( 1 )( 2 ) );
                check_stream_reader( reader, list_of( 1 )( 2 )( 3 ) );

                Value_type v;
                const bool ok = reader.read_next( v );
                assert_eq( ok, false );
            }
            {
                Istringstream_type is( to_str( s ) );

                Stream_reader_thrower< Istream_type, Value_type > reader( is );

                check_stream_read_or_throw( reader, vector< int >() );
                check_stream_read_or_throw( reader, list_of( 1 ) );
                check_stream_read_or_throw( reader, list_of( 1 )( 2 ) );
                check_stream_read_or_throw( reader, list_of( 1 )( 2 )( 3 ) );

                try
                {
                    Value_type v;
                    reader.read_next( v );
                    assert( false );
                }
                catch( ... )
                {
                }
            }
        }

        void run_tests()
        {
            test_stream_reader( "[][1][1,2][1,2,3]" );
            test_stream_reader( "[] [1] [1,2] [1,2,3]" );
            test_stream_reader( " [] [1] [1,2] [1,2,3] " );
        }
    };
}

void json_spirit::test_stream_reader()
{
#ifdef JSON_SPIRIT_VALUE_ENABLED
    Test_runner< Config  >().run_tests();
#endif
#ifdef JSON_SPIRIT_MVALUE_ENABLED
    Test_runner< mConfig >().run_tests();
#endif

#if defined( JSON_SPIRIT_WVALUE_ENABLED ) && !defined( BOOST_NO_STD_WSTRING )
    Test_runner< wConfig  >().run_tests();
#endif
#if defined( JSON_SPIRIT_WMVALUE_ENABLED ) && !defined( BOOST_NO_STD_WSTRING )
    Test_runner< wmConfig >().run_tests();
#endif
}
