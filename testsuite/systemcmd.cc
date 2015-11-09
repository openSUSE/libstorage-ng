
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage
#define BOOST_TEST_IGNORE_NON_ZERO_CHILD_CODE

#include <boost/version.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <string>
#include <vector>

#include "storage/Exception.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/SystemCmd.h"


using namespace std;
using namespace storage;


string
join(const vector<string>& input)
{
    return boost::join(input, "\n") + "\n";
}

BOOST_AUTO_TEST_CASE(hello_stdout)
{
    vector<string> stdout = {
	"stdout #1: hello",
	"stdout #2: stdout"
    };

    SystemCmd cmd( "helpers/echoargs hello stdout" );

    BOOST_CHECK_EQUAL(join(cmd.stdout()), join(stdout));
    BOOST_CHECK(cmd.stderr().empty());
    BOOST_CHECK(cmd.retcode() == 0);
}

BOOST_AUTO_TEST_CASE(hello_stderr)
{
    vector<string> stderr = {
	"stderr #1: hello",
	"stderr #2: stderr"
    };
    
    SystemCmd cmd("helpers/echoargs_stderr hello stderr");
    
    BOOST_CHECK_EQUAL(join(cmd.stderr()),join(stderr));
    BOOST_CHECK(cmd.stdout().empty());
}

BOOST_AUTO_TEST_CASE(hello_mixed)
{
    vector<string> stdout = {
	"line #1: stdout #1: mixed",
	"line #3: stdout #2: stdout",
	"line #5: stdout #3: stderr"
    };

    vector<string> stderr = {
	"line #2: stderr #1: to",
	"line #4: stderr #2: and"
    };

    SystemCmd cmd( "helpers/echoargs_mixed mixed to stdout and stderr" );

    BOOST_CHECK_EQUAL(join(cmd.stdout()), join(stdout));
    BOOST_CHECK_EQUAL(join(cmd.stderr()), join(stderr));
}

BOOST_AUTO_TEST_CASE(retcode_42)
{
#if BOOST_VERSION >= 1058000
    setenv("BOOST_TEST_CATCH_SYSTEM_ERRORS", "no", 1);

    SystemCmd cmd("helpers/retcode 42");

    BOOST_CHECK(cmd.retcode() == 42);
#endif
}

BOOST_AUTO_TEST_CASE(non_existent_no_throw)
{
    BOOST_CHECK_NO_THROW({SystemCmd cmd("/bin/wrglbrmpf", SystemCmd::ThrowBehaviour::NoThrow);});
}

BOOST_AUTO_TEST_CASE(non_existent_throw)
{
    BOOST_CHECK_THROW({SystemCmd cmd("/bin/wrglbrmpf", SystemCmd::ThrowBehaviour::DoThrow);},
	               CommandNotFoundException);
}

BOOST_AUTO_TEST_CASE(segfault_no_throw)
{
    BOOST_CHECK_NO_THROW({SystemCmd cmd( "helpers/segfaulter", SystemCmd::ThrowBehaviour::NoThrow);});
}

BOOST_AUTO_TEST_CASE(segfault_throw)
{
    BOOST_CHECK_THROW({SystemCmd cmd( "helpers/segfaulter", SystemCmd::ThrowBehaviour::DoThrow);},
		      SystemCmdException);
}

BOOST_AUTO_TEST_CASE(non_exec_no_throw)
{
    BOOST_CHECK_NO_THROW({SystemCmd cmd( "/etc/fstab", SystemCmd::ThrowBehaviour::NoThrow);});
}

BOOST_AUTO_TEST_CASE(non_exec_throw)
{
    BOOST_CHECK_THROW({SystemCmd cmd( "/etc/fstab", SystemCmd::ThrowBehaviour::DoThrow);},
		      SystemCmdException);
}

