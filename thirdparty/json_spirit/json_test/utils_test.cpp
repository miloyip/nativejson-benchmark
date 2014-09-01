//          Copyright John W. Wilkinson 2011
// Distributed under the MIT License, see accompanying file LICENSE.txt

// json spirit version 4.08

#include "utils_test.h"

void assert_eq( const double d1, const double d2, const double abs_error )
{
    const double err = fabs( d1 - d2 );

    if( err <= abs_error ) return;

    assert( false );
}
