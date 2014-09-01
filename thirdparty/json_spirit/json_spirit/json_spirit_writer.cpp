//          Copyright John W. Wilkinson 2007 - 2014
// Distributed under the MIT License, see accompanying file LICENSE.txt

// json spirit version 4.08

#include "json_spirit_writer.h"
#include "json_spirit_writer_template.h"

using namespace json_spirit;

#ifdef JSON_SPIRIT_VALUE_ENABLED
    void json_spirit::write( const Value& value, std::ostream& os, int options, unsigned int precision_of_doubles )
    {
        write_stream( value, os, options, precision_of_doubles );
    }
    std::string json_spirit::write( const Value& value, int options, unsigned int precision_of_doubles )
    {
        return write_string( value, options, precision_of_doubles );
    }

    void json_spirit::write_formatted( const Value& value, std::ostream& os, unsigned int precision_of_doubles )
    {
        write_stream( value, os, pretty_print, precision_of_doubles );
    }

    std::string json_spirit::write_formatted( const Value& value, unsigned int precision_of_doubles )
    {
        return write_string( value, pretty_print, precision_of_doubles );
    }
#endif

#ifdef JSON_SPIRIT_MVALUE_ENABLED
    void json_spirit::write( const mValue& value, std::ostream& os, int options, unsigned int precision_of_doubles )
    {
        write_stream( value, os, options, precision_of_doubles );
    }

    std::string json_spirit::write( const mValue& value, int options, unsigned int precision_of_doubles )
    {
        return write_string( value, options, precision_of_doubles );
    }

    void json_spirit::write_formatted( const mValue& value, std::ostream& os, unsigned int precision_of_doubles )
    {
        write_stream( value, os, pretty_print, precision_of_doubles );
    }

    std::string json_spirit::write_formatted( const mValue& value, unsigned int precision_of_doubles )
    {
        return write_string( value, pretty_print, precision_of_doubles );
    }
#endif

#if defined( JSON_SPIRIT_WVALUE_ENABLED ) && !defined( BOOST_NO_STD_WSTRING )
    void json_spirit::write( const wValue& value, std::wostream& os, int options, unsigned int precision_of_doubles )
    {
        write_stream( value, os, options, precision_of_doubles );
    }

    std::wstring json_spirit::write( const wValue& value, int options, unsigned int precision_of_doubles )
    {
        return write_string( value, options, precision_of_doubles );
    }

    void json_spirit::write_formatted( const wValue& value, std::wostream& os, unsigned int precision_of_doubles )
    {
        write_stream( value, os, pretty_print, precision_of_doubles );
    }

    std::wstring json_spirit::write_formatted( const wValue& value, unsigned int precision_of_doubles )
    {
        return write_string( value, pretty_print, precision_of_doubles );
    }
#endif

#if defined( JSON_SPIRIT_WMVALUE_ENABLED ) && !defined( BOOST_NO_STD_WSTRING )
    void json_spirit::write_formatted( const wmValue& value, std::wostream& os, unsigned int precision_of_doubles )
    {
        write_stream( value, os, pretty_print, precision_of_doubles );
    }

    std::wstring json_spirit::write_formatted( const wmValue& value, unsigned int precision_of_doubles )
    {
        return write_string( value, pretty_print, precision_of_doubles );
    }

    void json_spirit::write( const wmValue& value, std::wostream& os, int options, unsigned int precision_of_doubles )
    {
        write_stream( value, os, options, precision_of_doubles );
    }

    std::wstring json_spirit::write( const wmValue& value, int options, unsigned int precision_of_doubles )
    {
        return write_string( value, options, precision_of_doubles );
    }
#endif
