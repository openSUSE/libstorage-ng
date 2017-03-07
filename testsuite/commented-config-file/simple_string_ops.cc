
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE commented-config-file

#include <boost/test/unit_test.hpp>

#define protected public
#define private   public
#include "storage/Utils/CommentedConfigFile.h"

using namespace storage;


BOOST_AUTO_TEST_CASE( comment_lines )
{
    CommentedConfigFile subject;

    BOOST_CHECK_EQUAL( subject.is_comment_line( "# Text" ), true );
    BOOST_CHECK_EQUAL( subject.is_comment_line( "#"      ), true );
    BOOST_CHECK_EQUAL( subject.is_comment_line( " #"     ), true );
    BOOST_CHECK_EQUAL( subject.is_comment_line( " \t#"   ), true );
    BOOST_CHECK_EQUAL( subject.is_comment_line( "###"    ), true );

    BOOST_CHECK_EQUAL( subject.is_comment_line( "content # comment" ), false);

    BOOST_CHECK_EQUAL( subject.is_comment_line( ""       ), false );
    BOOST_CHECK_EQUAL( subject.is_comment_line( "   "    ), false );
    BOOST_CHECK_EQUAL( subject.is_comment_line( "\t\t"   ), false );
    BOOST_CHECK_EQUAL( subject.is_comment_line( " \t \t" ), false );
}


BOOST_AUTO_TEST_CASE( empty_lines )
{
    CommentedConfigFile subject;

    BOOST_CHECK_EQUAL( subject.is_empty_line( ""       ), true );
    BOOST_CHECK_EQUAL( subject.is_empty_line( "   "    ), true );
    BOOST_CHECK_EQUAL( subject.is_empty_line( "\t\t"   ), true );
    BOOST_CHECK_EQUAL( subject.is_empty_line( " \t \t" ), true );

    BOOST_CHECK_EQUAL( subject.is_empty_line( " \t \t#" ), false );
    BOOST_CHECK_EQUAL( subject.is_empty_line( " x"      ), false );
    BOOST_CHECK_EQUAL( subject.is_empty_line( "nothing" ), false );
}




string strip( string line )
{
    static CommentedConfigFile subject;

    subject.strip_trailing_whitespace( line );
    return line;
}


BOOST_AUTO_TEST_CASE( strip_trailing_whitespace )
{
    BOOST_CHECK_EQUAL( strip( "foo bar  "    ) , string( "foo bar"   ) );
    BOOST_CHECK_EQUAL( strip( "foo bar"      ) , string( "foo bar"   ) );
    BOOST_CHECK_EQUAL( strip( "  foo bar  "  ) , string( "  foo bar" ) );
    BOOST_CHECK_EQUAL( strip( "foo bar\t"    ) , string( "foo bar"   ) );
    BOOST_CHECK_EQUAL( strip( "foo bar \t "  ) , string( "foo bar"   ) );
    BOOST_CHECK_EQUAL( strip( "   "          ) , string( ""          ) );
    BOOST_CHECK_EQUAL( strip( ""             ) , string( ""          ) );
}




void test_split( const string & line,
                 const string & expected_content,
                 const string & expected_comment )
{
    static CommentedConfigFile subject;

    string content;
    string comment;

    subject.split_off_comment( line, content, comment );

    BOOST_CHECK_EQUAL( content, expected_content );
    BOOST_CHECK_EQUAL( comment, expected_comment );
}


BOOST_AUTO_TEST_CASE( split_line )
{
    test_split( "content # comment"    , "content", "# comment"   );
    test_split( "content   # comment  ", "content", "# comment  " );
    test_split( "content   #"          , "content", "#"           );
    test_split( "content  "            , "content", ""            );
    test_split( "    # comment\t"      , ""       , "# comment\t" );
    test_split( ""                     , ""       , ""            );
}


