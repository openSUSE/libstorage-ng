
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE commented-config-file

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <boost/test/unit_test.hpp>

#include "storage/Utils/Exception.h"

#define protected public
#define private   public
#include "storage/Utils/CommentedConfigFile.h"


using namespace storage;


string_vec test_data()
{
    string_vec lines = {
        "# header 00",
        "# header 01",
        "",
        "# header 02",
        "",
        "# entry 00 comment 00",
        "# entry 00 comment 01",
        "entry 00 content",
        "# entry 01 comment 00",
        "entry 01 content # entry 01 line comment",
        "",
        "# entry 02 comment 00",
        "# entry 02 comment 01  ",
        "# entry 02 comment 02",
        "entry 02 content",
        "entry 03 content",
        "entry 04 content",
        "",
        "# footer 00",
        "# footer 01"
    };

    return lines;
}


BOOST_AUTO_TEST_CASE( formatter_with_entry_comments )
{
    string_vec input = test_data();

    CommentedConfigFile subject;
    subject.parse( input );
    string_vec output = subject.format_lines();

    BOOST_CHECK_EQUAL( input.size(), output.size() );

    for ( size_t i=0; i < input.size(); ++i )
        BOOST_CHECK_EQUAL( input[i], output[i] );
}


string_vec read_lines( const string &filename )
{
    string_vec lines;
    string line;
    std::ifstream file( filename );

    while ( std::getline( file, line ) )
        lines.push_back( line );

    return lines;
}


BOOST_AUTO_TEST_CASE( write_to_file )
{
    string_vec input = test_data();

    CommentedConfigFile subject;
    subject.parse( input );

    string filename = "formatter-test.out";
    BOOST_CHECK_NO_THROW( subject.write( filename ) );

    string_vec lines = read_lines( filename );

    BOOST_CHECK_EQUAL( input.size(), lines.size() );

    for ( size_t i=0; i < input.size(); ++i )
        BOOST_CHECK_EQUAL( input[i], lines[i] );

    remove( filename.c_str() );

    BOOST_CHECK_THROW( subject.write( "/wrglbrmpf/doesntexist/x.out" ), IOException);
}
