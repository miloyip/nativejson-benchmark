#ifndef JSON_SPIRIT_TEST_UTILS
#define JSON_SPIRIT_TEST_UTILS

//          Copyright John W. Wilkinson 2007 - 2014
// Distributed under the MIT License, see accompanying file LICENSE.txt

// json spirit version 4.08

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <cassert>
#include <cmath>

// these functions allow you to inspect the values that caused a test to fail

template< class T1, class T2 >
void assert_eq( const T1& t1, const T2& t2 )
{
    if( t1 == t2 ) return;

    assert( false );
}

template< class T1, class T2 >
void assert_neq( const T1& t1, const T2& t2 )
{
    if( !(t1 == t2) ) return;

    assert( false );
}

void assert_eq( const double d1, const double d2, const double abs_error );

#endif
