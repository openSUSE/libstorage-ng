
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/version.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <string>
#include <vector>

#include "storage/Utils/Exception.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/SystemCmd.h"


using namespace std;
using namespace storage;


string
join(const vector<string>& input)
{
    return boost::join(input, "\n") + "\n";
}


class Fixture
{
public:

    Fixture()
    {
	set_logger(get_stdout_logger());
    }

};


BOOST_TEST_GLOBAL_FIXTURE(Fixture);


BOOST_AUTO_TEST_CASE(hello_stdout)
{
    vector<string> stdout = {
	"stdout #1: hello",
	"stdout #2: stdout"
    };

    SystemCmd cmd("../helpers/echoargs hello stdout");

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

    SystemCmd cmd("../helpers/echoargs_stderr hello stderr");

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

    SystemCmd cmd("../helpers/echoargs_mixed mixed to stdout and stderr");

    BOOST_CHECK_EQUAL(join(cmd.stdout()), join(stdout));
    BOOST_CHECK_EQUAL(join(cmd.stderr()), join(stderr));
}


BOOST_AUTO_TEST_CASE(pipe_stdin)
{
    vector<string> stdout = {
        "Hello, cruel world",
        "I'm leaving you today"
    };

    SystemCmd::Options cmd_options("cat");
    cmd_options.stdin_text = "Hello, cruel world\nI'm leaving you today";

    SystemCmd cmd(cmd_options);

    BOOST_CHECK_EQUAL(join(cmd.stdout()), join(stdout));
}


BOOST_AUTO_TEST_CASE(retcode_42)
{
    SystemCmd cmd("../helpers/retcode 42");

    BOOST_CHECK(cmd.retcode() == 42);
}


BOOST_AUTO_TEST_CASE(non_existent_no_throw)
{
    BOOST_CHECK_NO_THROW({
	SystemCmd cmd("/bin/wrglbrmpf", SystemCmd::ThrowBehaviour::NoThrow);
	BOOST_CHECK_EQUAL(cmd.retcode(), 127);
    });
}


BOOST_AUTO_TEST_CASE(non_existent_throw)
{
    BOOST_CHECK_THROW({SystemCmd cmd("/bin/wrglbrmpf", SystemCmd::ThrowBehaviour::DoThrow);},
		      CommandNotFoundException);
}


BOOST_AUTO_TEST_CASE(segfault_no_throw)
{
    BOOST_CHECK_NO_THROW({
	SystemCmd cmd("../helpers/segfaulter", SystemCmd::ThrowBehaviour::NoThrow);
	BOOST_CHECK_EQUAL(cmd.retcode(), -127);
    });
}


BOOST_AUTO_TEST_CASE(segfault_throw)
{
    BOOST_CHECK_THROW({SystemCmd cmd("../helpers/segfaulter", SystemCmd::ThrowBehaviour::DoThrow);},
		      SystemCmdException);
}


BOOST_AUTO_TEST_CASE(non_exec_no_throw)
{
    BOOST_CHECK_NO_THROW({
	SystemCmd cmd("../helpers/segfaulter.cc", SystemCmd::ThrowBehaviour::NoThrow);
	BOOST_CHECK_EQUAL(cmd.retcode(), 126);
    });
}


BOOST_AUTO_TEST_CASE(non_exec_throw)
{
    BOOST_CHECK_THROW({SystemCmd cmd( "../helpers/segfaulter.cc", SystemCmd::ThrowBehaviour::DoThrow);},
		      SystemCmdException);
}


BOOST_AUTO_TEST_CASE(env)
{
    vector<string> stdout = {
	"C ++ sure"
    };

    setenv("LC_ALL", "en_US", 1);	// will be overridden (by default in options.env)
    setenv("FUNNY", "++", 1);		// will not be overridden
    setenv("AMUSING", "no", 1);		// will be overridden (by addition to options.env)

    SystemCmd::Options options("echo $LC_ALL $FUNNY $AMUSING");

    options.env.push_back("AMUSING=sure");

    SystemCmd cmd(options);

    BOOST_CHECK(cmd.retcode() == 0);
    BOOST_CHECK_EQUAL(join(cmd.stdout()), join(stdout));
}
