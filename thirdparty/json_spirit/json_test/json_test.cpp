//          Copyright John W. Wilkinson 2007 - 2014
// Distributed under the MIT License, see accompanying file LICENSE.txt

// json spirit version 4.08

#include "json_spirit_value_test.h"
#include "json_spirit_writer_test.h"
#include "json_spirit_reader_test.h"
#include "json_spirit_stream_reader_test.h"
#include "json_spirit_utils_test.h"

#include <string>
#include <iostream>

using namespace std;
using namespace json_spirit;

int main()
{
    test_value();
    test_writer();
    test_reader();
    test_stream_reader();
    test_utils();

    cout << "all tests passed" << endl << endl;
    cout << "press any key to continue";

    string s;
    cin >> s;

	return 0;
}

