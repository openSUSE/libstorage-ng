
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE diff

#include <iostream>
#include <iomanip>
#include <sstream>
#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include "Diff.h"

using std::cout;
using std::endl;
using std::ostream;
using std::stringstream;


ostream & operator<<( ostream & stream, const string_vec & lines )
{
    stream << endl;

    for ( size_t i=0; i < lines.size(); ++i )
	stream << std::setfill('0') << std::setw(2) << i+1
	       << ": \"" << lines[i] << "\"" << endl;

    return stream;
}


boost::test_tools::predicate_result
check_diff( const string_vec & input_a,
	    const string_vec & input_b,
	    const string_vec & expected,
	    int		       context = 0 )
{
    boost::test_tools::predicate_result result( false );
    stringstream str;

    string_vec actual = Diff::diff( input_a, input_b, context );

    if ( actual.size() != expected.size() )
    {
	str << "Actual:" << actual << "\nExpected: " << expected;

	result.message() << "Size mismatch: Actual size:"
			 << actual.size()
			 << "; Expected size: " << expected.size()
			 << "\n\n" << str.str();
	return result;
    }


    for ( size_t i = 0;
	  i < expected.size() && i < actual.size();
	  ++i )
    {
	if ( boost::starts_with( expected[i], "@@ ??" ) &&
	     boost::starts_with( actual[i], "@@ " ) )
	    continue;

	if ( actual[i] != expected[i] )
	{
	    str << "Actual:" << actual << "\nExpected: " << expected;

	    result.message() << "\n\nResult line #" << i+1 << " mismatch:"
			     << "\nActual   line: \"" << actual[i]   << "\""
			     << "\nExpected line: \"" << expected[i] << "\""
			     << "\n\n" << str.str();
	    return result;
	}
    }

    return true;
}


BOOST_AUTO_TEST_CASE( diff_simple )
{
    string_vec input01 = {
	"aaa",
	"bbb",
	"ccc",
	"ddd"
    };

    string_vec input02 = {
	"aaa",
	"ccc",
	"ddd"
    };

    string_vec aaa = {
	"aaa"
    };

    string_vec aaa_bbb = {
	"aaa",
	"bbb"
    };

    // The line numbers in the @@ lines are not checked if the line starts with "@@ ???".

    BOOST_CHECK( check_diff( input01, input02, { "@@ ???", "-bbb" } ) );
    BOOST_CHECK( check_diff( input02, input01, { "@@ ???", "+bbb" } ) );
    BOOST_CHECK( check_diff( input01, input01, {}		    ) );
    BOOST_CHECK( check_diff( {},      {},      {}                   ) );
    BOOST_CHECK( check_diff( {},      aaa,     { "@@ -0,0 +1,1 @@", "+aaa" } ) );
    BOOST_CHECK( check_diff( aaa,     {},      { "@@ -1,1 +0,0 @@", "-aaa" } ) );
    BOOST_CHECK( check_diff( {},      aaa_bbb, { "@@ -0,0 +1,2 @@", "+aaa", "+bbb" } ) );
    BOOST_CHECK( check_diff( aaa_bbb, {},      { "@@ -1,2 +0,0 @@", "-aaa", "-bbb" } ) );
}


BOOST_AUTO_TEST_CASE( diff_with_context )
{
    string_vec input01 = {
	"aaa",
	"bbb",
	"ccc",
	"ddd",
	"eee",
	"fff",
	"ggg",
	"hhh",
	"iii",
	"jjj",
	"kkk"
    };

    string_vec input02 = {
	"aaa",
	"bbb",
	"ccc",
	"ddd",
	// "eee",
	"fff",
	"ggg",
	"hhh",
	"iii",
	"jjj",
	"kkk"
    };

    string_vec input03 = {
	"aaa",
	// "bbb",
	"ccc",
	"ddd",
	"eee",
	"fff",
	"ggg",
	"hhh",
	"iii",
	// "jjj",
	"kkk"
    };

    string_vec input04 = {
	// "aaa",
	"bbb",
	"ccc",
	"ddd",
	"eee",
	"fff",
	"ggg",
	"hhh",
	"iii",
	"jjj",
	"kkk"
    };

    string_vec input05 = {
	"aaa",
	"bbb",
	"ccc",
	"ddd",
	"eee",
	"fff",
	"ggg",
	"hhh",
	"iii",
	"jjj"
	// "kkk"
    };

    string_vec input06 = {
	"aaa",
	"bbb",
	"ccc",
	"ddd",
	// "eee",
	"fff",
	"ggg",
	// "hhh",
	"iii",
	"jjj",
	"kkk"
    };

    string_vec input07 = {
	"aaa",
	"bbb",
	"xxx", // changed
	"yyy", // changed
	"eee",
	"fff",
	// "ggg",
	"hhh",
	"iii",
        "zzz", // added
	"jjj",
	"kkk"
    };

    //--------------------------------------------------

    string_vec expected_01_02 = {
	"@@ ???",
	" bbb",
	" ccc",
	" ddd",
	"-eee",
	" fff",
	" ggg",
	" hhh"
    };

    string_vec expected_02_01 = {
	"@@ ???",
	" bbb",
	" ccc",
	" ddd",
	"+eee",
	" fff",
	" ggg",
	" hhh"
    };

    string_vec expected_01_03 = {
	"@@ ???",
	" aaa",
	"-bbb",
	" ccc",
	" ddd",
	" eee",
	"@@ ???",
	" ggg",
	" hhh",
	" iii",
	"-jjj",
	" kkk"
    };

    string_vec expected_03_01 = {
	"@@ ???",
	" aaa",
	"+bbb",
	" ccc",
	" ddd",
	" eee",
	"@@ ???",
	" ggg",
	" hhh",
	" iii",
	"+jjj",
	" kkk"
    };

    string_vec expected_01_04 = {
	"@@ ???",
	"-aaa",
	" bbb",
	" ccc",
	" ddd"
    };

    string_vec expected_01_05 = {
	"@@ ???",
	" hhh",
	" iii",
	" jjj",
	"-kkk"
    };

    string_vec expected_01_06 = {
	"@@ ???",
	" bbb",
	" ccc",
	" ddd",
	"-eee",
	" fff",
	" ggg",
	"-hhh",
	" iii",
	" jjj",
	" kkk"
    };

    string_vec expected_01_07 = {
	"@@ ???",
	" aaa",
	" bbb",
        "-ccc",
        "-ddd",
	"+xxx",
	"+yyy",
	" eee",
	" fff",
	"-ggg",
	" hhh",
	" iii",
        "+zzz",
	" jjj",
	" kkk"
    };

    string_vec expected_00_01 = {
	"@@ -0,0 +1,11 @@",
	"+aaa",
	"+bbb",
	"+ccc",
	"+ddd",
	"+eee",
	"+fff",
	"+ggg",
	"+hhh",
	"+iii",
	"+jjj",
	"+kkk"
    };



    const int context = 3;

    BOOST_CHECK( check_diff( input01, input02, expected_01_02, context ) );
    BOOST_CHECK( check_diff( input01, input03, expected_01_03, context ) );
    BOOST_CHECK( check_diff( input01, input04, expected_01_04, context ) );
    BOOST_CHECK( check_diff( input01, input05, expected_01_05, context ) );
    BOOST_CHECK( check_diff( input01, input06, expected_01_06, context ) );
    BOOST_CHECK( check_diff( input01, input07, expected_01_07, context ) );

    BOOST_CHECK( check_diff( input02, input01, expected_02_01, context ) );
    BOOST_CHECK( check_diff( input03, input01, expected_03_01, context ) );
    BOOST_CHECK( check_diff( {},      input01, expected_00_01, context ) );
}
