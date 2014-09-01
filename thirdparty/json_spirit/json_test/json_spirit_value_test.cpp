//          Copyright John W. Wilkinson 2007 - 2014
// Distributed under the MIT License, see accompanying file LICENSE.txt

// json spirit version 4.08

#include "json_spirit_value_test.h"
#include "utils_test.h"
#include "json_spirit_value.h"
#include <limits.h>
#include <list>
#include <boost/assign/list_of.hpp>
#include <boost/integer_traits.hpp>

using namespace json_spirit;
using namespace std;
using namespace boost;
using namespace boost::assign;

namespace
{
#ifdef JSON_SPIRIT_VALUE_ENABLED

    const int64_t max_int64   = integer_traits< int64_t  >::max();
    const uint64_t max_uint64 = integer_traits< uint64_t >::max();

    void test_obj_value()
    {
        const Pair p1( "name1", "value1" );
        const Pair p3( "name3", "value3" );

        Object obj_1; obj_1.push_back( p1 );
        Object obj_2; obj_2.push_back( p1 );
        Object obj_3; obj_3.push_back( p3 );

        Value v1( obj_1 );
        Value v2( obj_2 );
        Value v3( obj_3 );

        assert_eq( v1.type(), obj_type );
        assert_eq ( v1, v2 );
        assert_neq( v1, v3 );

        assert_eq( v1.get_obj(), obj_1 );
        assert_eq( v3.get_obj(), obj_3 );
    }

    void test_array_value()
    {
        Array array_1; array_1.push_back( 1 ); array_1.push_back( "2" );
        Array array_2; array_2.push_back( 1 ); array_2.push_back( "2" );
        Array array_3; array_3.push_back( 1 ); array_3.push_back( "X" );

        Value v1( array_1 );
        Value v2( array_2 );
        Value v3( array_3 );

        assert_eq( v1.type(), array_type );
        assert_eq ( v1, v2 );
        assert_neq( v1, v3 );

        assert_eq( v1.get_array(), array_1 );
        assert_eq( v3.get_array(), array_3 );
    }

    void test_bool_value()
    {
        Value v1( true );
        Value v2( true );
        Value v3( false );

        assert_eq( v1.type(), bool_type );
        assert_eq ( v1, v2 );
        assert_neq( v1, v3 );

        assert( v1.get_bool() );
        assert( !v3.get_bool() );
    }

    void test_int_value()
    {
        Value v1( 1 );
        Value v2( 1 );
        Value v3( INT_MAX );

        assert_eq( v1.type(), int_type );
        assert_eq ( v1, v2 );
        assert_neq( v1, v3 );

        unsigned int uint_max = INT_MAX;
        
        assert_eq( v1.get_int(),    1 );
        assert_eq( v1.get_int64(),  1 );
        assert_eq( v1.get_uint64(), 1u );
        assert_eq( v3.get_int(),    INT_MAX );
        assert_eq( v3.get_int64(),  INT_MAX );
        assert_eq( v3.get_uint64(), uint_max );

        Value v4( max_int64 );

        assert_eq( v4.get_int64(), max_int64 );
        assert_eq( v4.get_uint64(), static_cast< uint64_t >( max_int64 ) );

        const uint64_t max_int64_plus_1 = max_int64 + uint64_t( 1 );

        Value v5( max_int64_plus_1 );

        assert_eq( v5.get_uint64(), max_int64_plus_1 );

        Value v6( max_uint64 );

        assert_eq( v6.get_uint64(), max_uint64 );

        Value v7( 0 );

        assert_eq( v7.get_int(),    0 );
        assert_eq( v7.get_int64(),  0 );
        assert_eq( v7.get_uint64(), 0u );

        Value v8( -1 );

        assert_eq( v8.get_int(),   -1 );
        assert_eq( v8.get_int64(), -1 );
        assert_eq( v8.get_uint64(), max_uint64 );
    }

    void test_real_value()
    {
        Value v1( 1.0 );
        Value v2( 1.0 );
        Value v3( 2.0 );

        assert_eq( v1.type(), real_type );
        assert_eq ( v1, v2 );
        assert_neq( v1, v3 );

        assert_eq( v1.get_real(), 1.0 );
        assert_eq( v3.get_real(), 2.0 );
    }

    void test_null_value()
    {
        Value v1;
        Value v2;

        assert_eq( v1.type(), null_type );
        assert_eq( v1.is_null(), true );
        assert_eq( v1, v2 );
        assert_eq( v1.is_null(), true );
        assert_eq( Value( 1 ).is_null(), false );
    }

    template< typename T >
    void test_get_value( const T& t )
    {
        assert_eq( Value( t ).get_value< T >(), t );
    }

    void test_get_value()
    {
        test_get_value( 123 );
        test_get_value( max_int64 );
        test_get_value( 1.23 );
        test_get_value( true );
        test_get_value( false );
        test_get_value( string( "test" ) );

        Array a; a.push_back( 1 ); a.push_back( "2" );

        test_get_value( a );

        Object obj; obj.push_back( Pair( "name1", "value1" ) );

        test_get_value( obj );
    }

    void assert_array_eq( const Value& v, const Array& a )
    {
        assert_eq( v.get_array(), a );
    }

    void assert_obj_eq( const Value& v, const Object& obj )
    {
        assert_eq( v.get_obj(), obj );
    }

    template< typename T >
    void check_copy( const T& t )
    {
        const Value v1( t );
        const Value v2( v1 );
        Value v3;
        v3 = v1;
    
        assert_eq( v1, v2 );
        assert_eq( v1, v3 );
    
        assert_eq( v2.get_value< T >(), t );
        assert_eq( v3.get_value< T >(), t );

        assert_eq( v1.is_uint64(), v2.is_uint64() );
        assert_eq( v1.is_uint64(), v3.is_uint64() );
    }

    void check_copying_null()
    {
        const Value v1;
        const Value v2( v1 );
        Value v3;
        v3 = v1;
    
        assert_eq( v2.type(), null_type );
        assert_eq( v3.type(), null_type );
    }

    void test_copying()
    {
        {
            const Array array_1 = list_of(1)(2);

            Value v1( array_1 );
            const Value v2( v1 );

            assert_array_eq( v1, array_1 );
            assert_array_eq( v2, array_1 );

            v1.get_array()[0] = 3;

            assert_array_eq( v1, list_of(3)(2) );
            assert_array_eq( v2, array_1 );
        }
        {
            const Object obj_1 = list_of( Pair( "a", 1 ) )( Pair( "b", 2 ) );

            Value v1( obj_1 );
            Value v2;
            
            v2 = v1;

            assert_obj_eq( v1, obj_1 );
            assert_obj_eq( v2, obj_1 );

            v1.get_obj()[0] = Pair( "c", 3 );

            assert_obj_eq( v1, list_of( Pair( "c", 3 ) )( Pair( "b", 2 ) ) );
            assert_obj_eq( v2, obj_1 );
        }
        {
            check_copy( 1 );
            check_copy( 2.0 );
            check_copy( max_int64 );
            check_copy( max_uint64 );
            check_copy( string("test") );
            check_copy( true );
            check_copy( false );
            const Array array_1 = list_of(1)(2);
            check_copy( array_1 );
            const Object obj_1 = list_of( Pair( "a", 1 ) )( Pair( "b", 2 ) );
            check_copy( obj_1 );
            check_copying_null();
        }
    }

    template< typename ObjectType > void check_pair_typedefs( ObjectType &object )
    {
        typename ObjectType::value_type::String_type name = object[0].name_;
        typename ObjectType::value_type::Value_type value = object[0].value_;
    }

    void check_pair_typedefs()
    {
        Object o;
        check_pair_typedefs( o );
        
#if defined( JSON_SPIRIT_WVALUE_ENABLED ) && !defined( BOOST_NO_STD_WSTRING )
        wObject wo;
        check_pair_typedefs( wo );
#endif
    }

    void test_obj_map_implemention()
    {
#ifdef JSON_SPIRIT_MVALUE_ENABLED
        mObject obj;

        obj[ "name 1" ] = 1;
        obj[ "name 2" ] = "two";

        assert_eq( obj.size(), 2u );

        assert_eq( obj.find( "name 1" )->second.get_int(), 1 );
        assert_eq( obj.find( "name 2" )->second.get_str(), "two" );
#endif
    }

    template< typename Int >
    void check_an_int_is_a_real( Int i, bool expected_result )
    {
        assert_eq( Value( i ).is_uint64(), expected_result );
    }

    void test_is_uint64()
    {
        check_an_int_is_a_real( 1,                            false );
        check_an_int_is_a_real( static_cast< int64_t  >( 1 ), false );
        check_an_int_is_a_real( static_cast< uint64_t >( 1 ), true );
    }

    template< typename Int >
    void check_an_int_is_a_real( Int i, double expected_result )
    {
        assert_eq( Value( i ).get_real(), expected_result );
    }

    void test_an_int_is_a_real()
    {
        check_an_int_is_a_real( -1, -1.0 );
        check_an_int_is_a_real(  0,  0.0 );
        check_an_int_is_a_real(  1,  1.0 );
        check_an_int_is_a_real( max_int64,  9223372036854775800.0 );
        check_an_int_is_a_real( max_uint64, 18446744073709552000.0 );
    }

    template< typename T >
    void check_wrong_type_exceptions( const Value& v, const string& requested_type_name, const string& actual_type_name )
    {
        try
        {
            v.get_value< T >();

            assert( false );
        }
        catch( const runtime_error& e )
        {
            ostringstream os;

            os << "get_value< " << requested_type_name << " > called on " << actual_type_name << " Value";

            assert_eq( e.what(), os.str() );
        }
    }

    template< typename T >
    void check_wrong_type_exceptions( const string& requested_type_name )
    {
        Value v;

        assert_eq( v.type(), null_type );

        check_wrong_type_exceptions< T >( v, requested_type_name, "null" );
    }

    void test_wrong_type_exceptions()
    {
        check_wrong_type_exceptions< Object >( "Object" );
        check_wrong_type_exceptions< Array >( "Array" );
        check_wrong_type_exceptions< string >( "string" );
        check_wrong_type_exceptions< bool >( "boolean");
        check_wrong_type_exceptions< boost::int64_t >( "integer" );
        check_wrong_type_exceptions< int >( "integer" );
        check_wrong_type_exceptions< double >( "real" );

        Value v( "string" );

        assert_eq( v.type(), str_type );

        check_wrong_type_exceptions< double >( v, "real", "string" );
    }
#endif

    template< class Config_type >
    class Container_constructor_runner
    {
    public:

        Container_constructor_runner()
        {
            vector< double > vd = list_of( 1.2 )( 1.3 );  test_container_constructor( vd );
            vector< int >    vi = list_of( 1 );           test_container_constructor( vi );
                             vi = list_of( 1 )( 2 );      test_container_constructor( vi );
                             vi = list_of( 1 )( 2 )( 3 ); test_container_constructor( vi );
            list< double >   ld = list_of( 1.2 )( 1.3 );  test_container_constructor( ld );
            list< int >      li = list_of( 1 );           test_container_constructor( li );
                             li = list_of( 1 )( 2 );      test_container_constructor( li );
                             li = list_of( 1 )( 2 )( 3 ); test_container_constructor( li );
        }

    private:

        typedef typename Config_type::Array_type Array_type;
        typedef typename Config_type::Value_type Value_type;

        template< class Cont >
        void test_container_constructor( const Cont& cont )
        {
            typedef typename Cont::value_type Cont_value_type;
            const Value_type val( cont.begin(), cont.end() );
            const Array_type& arr = val.get_array();
            Cont result;
            for( unsigned int i = 0; i < arr.size(); ++i )
            {
                result.push_back( arr[i].template get_value< Cont_value_type>() );
            }
            assert_eq( result, cont );
        }
    };

    void test_container_constructor()
    {
#ifdef JSON_SPIRIT_VALUE_ENABLED
       Container_constructor_runner< Config  >();
#endif
#ifdef JSON_SPIRIT_MVALUE_ENABLED
       Container_constructor_runner< mConfig >();
#endif
#if defined( JSON_SPIRIT_WVALUE_ENABLED ) && !defined( BOOST_NO_STD_WSTRING )
        Container_constructor_runner< wConfig  >();
#endif
#if defined( JSON_SPIRIT_WMVALUE_ENABLED ) && !defined( BOOST_NO_STD_WSTRING )
        Container_constructor_runner< wmConfig >();
#endif
    }

    template< class Config_type >
    class Variant_constructor_runner
    {
    public:

        Variant_constructor_runner()
        {
            test_variant_constructor< variant< int, double > >( 1.23 );
            test_variant_constructor< variant< int, double > >( 123 );
            test_variant_constructor< variant< int, double, String_type > >( to_str< String_type >( "foo" ) );
            test_variant_constructor< variant< int, double, String_type, bool > >( true );
            test_variant_constructor< variant< int, double, String_type, bool, boost::int64_t > >( boost::int64_t( 123 ) );
            test_variant_constructor< variant< int, double, String_type, bool, boost::uint64_t > >( boost::uint64_t( 123 ) );

            {
                variant< int, Null > variant = Null();
                const Value_type val( variant );
                assert( val.is_null() );
            }
            
            vector< double > vd = list_of( 1.2 )( 1.3 );   test_variant_array_constructor< double > ( vd );
            vector< int >    vi = list_of( 1 );            test_variant_array_constructor< int >( vi );
                             vi = list_of( 1 )( 2 );       test_variant_array_constructor< int >( vi );
                             vi = list_of( 1 )( 2 )( 3 );  test_variant_array_constructor< int >( vi );
            list< double >   ld = list_of( 1.2 )( 1.3 );   test_variant_array_constructor< double >( ld );
            list< int >      li = list_of( 1 );            test_variant_array_constructor< int >( li );
                             li = list_of( 1 )( 2 );       test_variant_array_constructor< int >( li );
                             li = list_of( 1 )( 2 )( 3 );  test_variant_array_constructor< int >( li );
        }

    private:

        typedef typename Config_type::String_type String_type;
        typedef typename Config_type::Array_type Array_type;
        typedef typename Config_type::Value_type Value_type;

        template< class Variant_t, typename T >
        void test_variant_constructor( const T& t )
        {
            const Variant_t variant( t );
            const Value_type val( variant );
            assert_eq( val.template get_value< T >(), t );
        }

        template< typename T, typename A, template< typename, typename > class Cont >
        void test_variant_array_constructor( const Cont< T, A >& cont )
        {
            const variant< int, Cont< T, A > > variant = cont;
            const Value_type val( variant );
            const Array_type& arr = val.get_array();
            Cont< T, A > result;
            for( unsigned int i = 0; i < arr.size(); ++i )
            {
                result.push_back( arr[i].template get_value< T >() );
            }
            assert_eq( result, cont );
        }
    };

    void test_variant_constructor()
    {
#ifdef JSON_SPIRIT_VALUE_ENABLED
        Variant_constructor_runner< Config  >();
#endif
#ifdef JSON_SPIRIT_MVALUE_ENABLED
        Variant_constructor_runner< mConfig >();

#endif
#if defined( JSON_SPIRIT_WVALUE_ENABLED ) && !defined( BOOST_NO_STD_WSTRING )
        Variant_constructor_runner< wConfig  >();
#endif
#if defined( JSON_SPIRIT_WMVALUE_ENABLED ) && !defined( BOOST_NO_STD_WSTRING )
       Variant_constructor_runner< wmConfig >();
#endif
    }
}

void json_spirit::test_value()
{
#ifdef JSON_SPIRIT_VALUE_ENABLED
    Object obj;
    Value value_str ( "value" );
    Value value_obj ( obj );
    Value value_bool( true );

    Value value_str_2 ( string( "value" ) );
    Value value_obj_2 ( obj );
    Value value_bool_2( false );

    const char* str( "value" );
    Value value_str_2b ( str );

    assert_eq( value_str, value_str );
    assert_eq( value_str, value_str_2 );
    assert_eq( value_str, value_str_2b );
    assert_eq( value_obj, value_obj );
    assert_eq( value_obj, value_obj_2 );
    assert_neq( value_str, value_obj );
    assert_neq( value_str, value_bool );

    Object obj_2;
    obj_2.push_back( Pair( "name", value_str ) );
    Value value_str_3( "xxxxx" );
    Value value_obj_3( obj_2 );

    assert_neq( value_str, value_str_3 );
    assert_neq( value_obj, value_obj_3 );

    test_obj_value();
    test_array_value();
    test_bool_value();
    test_int_value();
    test_real_value();
    test_null_value();
    test_get_value();
    test_copying();
    test_obj_map_implemention();
    test_is_uint64();
    test_an_int_is_a_real();
    test_wrong_type_exceptions();
#endif
    test_container_constructor();
    test_variant_constructor();
}
