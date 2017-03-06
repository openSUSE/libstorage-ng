
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE commented-config-file

#include <boost/test/unit_test.hpp>

#define protected public
#define private   public
#include "storage/Utils/CommentedConfigFile.h"

using namespace storage;


BOOST_AUTO_TEST_CASE( parser_simple )
{
    string_vec input = {
        "aaa bbb ccc",
        "ddd eee fff",
        "ggg hhh iii"
    };

    CommentedConfigFile subject;
    subject.parse( input );

    BOOST_CHECK_EQUAL( subject.get_header_comments().empty(), true );
    BOOST_CHECK_EQUAL( subject.get_footer_comments().empty(), true );
    BOOST_CHECK_EQUAL( subject.get_entry_count(), 3 );
    BOOST_CHECK_EQUAL( subject.get_content(0), input[0] );
    BOOST_CHECK_EQUAL( subject.get_content(1), input[1] );
    BOOST_CHECK_EQUAL( subject.get_content(2), input[2] );
}


BOOST_AUTO_TEST_CASE( parser_with_header )
{
    string_vec input = {
        /** 00 **/  "# header 00",
        /** 01 **/  "# header 01",
        /** 02 **/  "",
        /** 03 **/  "aaa bbb ccc",
        /** 04 **/  "ddd eee fff",
        /** 05 **/  "ggg hhh iii"
    };

    CommentedConfigFile subject;
    subject.parse( input );

    const string_vec & header = subject.get_header_comments();

    BOOST_CHECK_EQUAL( header.empty(), false );
    BOOST_CHECK_EQUAL( header.size(), 3 );
    BOOST_CHECK_EQUAL( header[0], input[0] );
    BOOST_CHECK_EQUAL( header[1], input[1] );
    BOOST_CHECK_EQUAL( header[2], input[2] );

    BOOST_CHECK_EQUAL( subject.get_footer_comments().empty(), true );

    int content_start = 3;

    BOOST_CHECK_EQUAL( subject.get_entry_count(), 3 );
    BOOST_CHECK_EQUAL( subject.get_content(0), input[ content_start     ] );
    BOOST_CHECK_EQUAL( subject.get_content(1), input[ content_start + 1 ] );
    BOOST_CHECK_EQUAL( subject.get_content(2), input[ content_start + 2 ] );
}


BOOST_AUTO_TEST_CASE( parser_with_header_and_footer )
{
    string_vec input = {
        /** 00 **/  "# header 00",
        /** 01 **/  "# header 01",
        /** 02 **/  "",
        /** 03 **/  "aaa bbb ccc",
        /** 04 **/  "ddd eee fff",
        /** 05 **/  "ggg hhh iii",
        /** 06 **/  "",
        /** 07 **/  "# footer 00",
        /** 08 **/  "# footer 01",
        /** 09 **/  ""
    };

    CommentedConfigFile subject;
    subject.parse( input );

    const string_vec & footer = subject.get_footer_comments();
    int footer_start = 6;

    BOOST_CHECK_EQUAL( footer.size(), 4 );
    BOOST_CHECK_EQUAL( footer[0], input[ footer_start   ] );
    BOOST_CHECK_EQUAL( footer[1], input[ footer_start+1 ] );
    BOOST_CHECK_EQUAL( footer[2], input[ footer_start+2 ] );
    BOOST_CHECK_EQUAL( footer[3], input[ footer_start+3 ] );

    const string_vec & header = subject.get_header_comments();

    BOOST_CHECK_EQUAL( header.empty(), false );
    BOOST_CHECK_EQUAL( header.size(), 3 );
    BOOST_CHECK_EQUAL( header[0], input[0] );
    BOOST_CHECK_EQUAL( header[1], input[1] );
    BOOST_CHECK_EQUAL( header[2], input[2] );

    int content_start = 3;

    BOOST_CHECK_EQUAL( subject.get_entry_count(), 3 );
    BOOST_CHECK_EQUAL( subject.get_content(0), input[ content_start     ] );
    BOOST_CHECK_EQUAL( subject.get_content(1), input[ content_start + 1 ] );
    BOOST_CHECK_EQUAL( subject.get_content(2), input[ content_start + 2 ] );
}


BOOST_AUTO_TEST_CASE( parser_with_entry_comments )
{
    string_vec input = {
        /** 00 **/  "# header 00",
        /** 01 **/  "# header 01",
        /** 02 **/  "",
        /** 03 **/  "# header 02",
        /** 04 **/  "",
        /** 05 **/  "# entry 00 comment 00",
        /** 06 **/  "# entry 00 comment 01",
        /** 07 **/  "entry 00 content",
        /** 08 **/  "# entry 01 comment 00",
        /** 09 **/  "entry 01 content    # entry 01 line comment  ",
        /** 10 **/  "",
        /** 11 **/  "# entry 02 comment 00",
        /** 12 **/  "# entry 02 comment 01   ",
        /** 13 **/  "# entry 02 comment 02",
        /** 14 **/  "entry 02 content",
        /** 15 **/  "entry 03 content\t",
        /** 16 **/  "entry 04 content",
        /** 17 **/  "",
        /** 18 **/  "# footer 00",
        /** 19 **/  "# footer 01"
    };

    CommentedConfigFile subject;
    subject.parse( input );

    const string_vec & header = subject.get_header_comments();

    BOOST_CHECK_EQUAL( header.size(), 5 );
    BOOST_CHECK_EQUAL( header[0], input[0] );
    BOOST_CHECK_EQUAL( header[1], input[1] );
    BOOST_CHECK_EQUAL( header[2], input[2] );
    BOOST_CHECK_EQUAL( header[3], input[3] );
    BOOST_CHECK_EQUAL( header[4], input[4] );

    const string_vec & footer = subject.get_footer_comments();
    int footer_start = 17;

    BOOST_CHECK_EQUAL( footer.size(), 3 );
    BOOST_CHECK_EQUAL( footer[0], input[ footer_start   ] );
    BOOST_CHECK_EQUAL( footer[1], input[ footer_start+1 ] );
    BOOST_CHECK_EQUAL( footer[2], input[ footer_start+2 ] );

    BOOST_CHECK_EQUAL( subject.get_entry_count(), 5 );
    BOOST_CHECK_EQUAL( subject.get_content(0), input[ 7 ] );
    BOOST_CHECK_EQUAL( subject.get_content(1), string( "entry 01 content" ) );
    BOOST_CHECK_EQUAL( subject.get_content(2), string( "entry 02 content" ) );
    BOOST_CHECK_EQUAL( subject.get_content(3), string( "entry 03 content" ) );
    BOOST_CHECK_EQUAL( subject.get_content(4), string( "entry 04 content" ) );

    BOOST_CHECK_EQUAL( subject.get_entry(0)->line_comment.empty(), true );
    BOOST_CHECK_EQUAL( subject.get_entry(1)->line_comment, string( "# entry 01 line comment  " ) );
    BOOST_CHECK_EQUAL( subject.get_entry(2)->line_comment.empty(), true );

    string_vec & comment = subject.get_entry(0)->comment_before;
    BOOST_CHECK_EQUAL( comment.size(), 2 );
    BOOST_CHECK_EQUAL( comment[0], input[ 5 ] );
    BOOST_CHECK_EQUAL( comment[1], input[ 6 ] );

    comment = subject.get_entry(1)->comment_before;
    BOOST_CHECK_EQUAL( comment.size(), 1 );
    BOOST_CHECK_EQUAL( comment[0], input[ 8 ] );

    comment = subject.get_entry(2)->comment_before;
    BOOST_CHECK_EQUAL( comment.size(), 4 );
    BOOST_CHECK_EQUAL( comment[0], input[ 10 ] );
    BOOST_CHECK_EQUAL( comment[1], input[ 11 ] );
    BOOST_CHECK_EQUAL( comment[2], input[ 12 ] );
    BOOST_CHECK_EQUAL( comment[3], input[ 13 ] );
}

