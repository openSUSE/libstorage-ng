
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include <fcntl.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "storage/Utils/Exception.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"


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


BOOST_AUTO_TEST_CASE(hello_stdout_cmd)
{
    vector<string> stdout = {
	"stdout #1: hello world",
	"stdout #2: stdout"
    };

    SystemCmd cmd("../helpers/echoargs 'hello world' stdout");

    BOOST_CHECK_EQUAL(join(cmd.stdout()), join(stdout));
    BOOST_CHECK(cmd.stderr().empty());
    BOOST_CHECK_EQUAL(cmd.retcode(), 0);
}


BOOST_AUTO_TEST_CASE(hello_stdout_args)
{
    vector<string> stdout = {
	"stdout #1: hello world",
	"stdout #2: stdout"
    };

    SystemCmd cmd({ "../helpers/echoargs", "hello world", "stdout" });

    BOOST_CHECK_EQUAL(join(cmd.stdout()), join(stdout));
    BOOST_CHECK(cmd.stderr().empty());
    BOOST_CHECK_EQUAL(cmd.retcode(), 0);
}


BOOST_AUTO_TEST_CASE(hello_huge_stdout_args)
{
    vector<string> stdout;
    for (int i = 0; i < 1000000; ++i)
	stdout.push_back("Hello world, how are you?");

    SystemCmd::Options cmd_options({ "../helpers/repeat", "1000000", "Hello world, how are you?" });

    SystemCmd cmd(cmd_options);

    BOOST_CHECK_EQUAL(join(cmd.stdout()), join(stdout));
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


BOOST_AUTO_TEST_CASE(pipe_stdin_cmd)
{
    vector<string> stdout = {
        "Hello, cruel world",
        "I'm leaving you today"
    };

    SystemCmd::Options cmd_options(CAT_BIN);
    cmd_options.stdin_text = "Hello, cruel world\nI'm leaving you today";

    SystemCmd cmd(cmd_options);

    BOOST_CHECK_EQUAL(join(cmd.stdout()), join(stdout));
}


BOOST_AUTO_TEST_CASE(pipe_stdin_args)
{
    vector<string> stdout = {
	"Hello, cruel world",
	"I'm leaving you today"
    };

    SystemCmd::Options cmd_options({ CAT_BIN });
    cmd_options.stdin_text = "Hello, cruel world\nI'm leaving you today";

    SystemCmd cmd(cmd_options);

    BOOST_CHECK_EQUAL(join(cmd.stdout()), join(stdout));
}


BOOST_AUTO_TEST_CASE(pipe_huge_stdin_args)
{
    vector<string> stdout;
    for (int i = 0; i < 100000; ++i)
	stdout.push_back("Hello world, always keep on smiling!");

    SystemCmd::Options cmd_options({ CAT_BIN });
    cmd_options.stdin_text = boost::join(stdout, "\n");

    SystemCmd cmd(cmd_options);

    BOOST_CHECK_EQUAL(join(cmd.stdout()), join(stdout));
}


BOOST_AUTO_TEST_CASE(retcode_42_cmd)
{
    SystemCmd cmd("../helpers/retcode 42");

    BOOST_CHECK_EQUAL(cmd.retcode(), 42);
}


BOOST_AUTO_TEST_CASE(retcode_42_args)
{
    SystemCmd cmd({ "../helpers/retcode", "42" });

    BOOST_CHECK_EQUAL(cmd.retcode(), 42);
}


BOOST_AUTO_TEST_CASE(non_existent_no_throw)
{
    BOOST_CHECK_NO_THROW({
	SystemCmd cmd("/bin/wrglbrmpf", SystemCmd::ThrowBehaviour::NoThrow);
	BOOST_CHECK_EQUAL(cmd.retcode(), 127);
    });
}


BOOST_AUTO_TEST_CASE(non_existent_throw_cmd)
{
    BOOST_CHECK_THROW({ SystemCmd cmd("/bin/wrglbrmpf", SystemCmd::ThrowBehaviour::DoThrow); },
		      CommandNotFoundException);
}


BOOST_AUTO_TEST_CASE(non_existent_throw_args)
{
    BOOST_CHECK_THROW({ SystemCmd cmd({ "/bin/wrglbrmpf" }, SystemCmd::ThrowBehaviour::DoThrow); },
		      CommandNotFoundException);
}


BOOST_AUTO_TEST_CASE(segfault_no_throw)
{
    BOOST_CHECK_NO_THROW({
	SystemCmd cmd("../helpers/segfaulter", SystemCmd::ThrowBehaviour::NoThrow);
	BOOST_CHECK_EQUAL(cmd.retcode(), -127);
    });
}


BOOST_AUTO_TEST_CASE(segfault_throw_cmd)
{
    BOOST_CHECK_THROW({SystemCmd cmd("../helpers/segfaulter", SystemCmd::ThrowBehaviour::DoThrow);},
		      SystemCmdException);
}


BOOST_AUTO_TEST_CASE(segfault_throw_args)
{
    BOOST_CHECK_THROW({ SystemCmd cmd({ "../helpers/segfaulter" }, SystemCmd::ThrowBehaviour::DoThrow); },
		      SystemCmdException);
}


BOOST_AUTO_TEST_CASE(non_exec_no_throw)
{
    BOOST_CHECK_NO_THROW({
	SystemCmd cmd("../helpers/segfaulter.cc", SystemCmd::ThrowBehaviour::NoThrow);
	BOOST_CHECK_EQUAL(cmd.retcode(), 126);
    });
}


BOOST_AUTO_TEST_CASE(non_exec_throw_cmd)
{
    BOOST_CHECK_THROW({ SystemCmd cmd("../helpers/segfaulter.cc", SystemCmd::ThrowBehaviour::DoThrow); },
		      SystemCmdException);
}


BOOST_AUTO_TEST_CASE(non_exec_throw_args)
{
    BOOST_CHECK_THROW({ SystemCmd cmd({ "../helpers/segfaulter.cc" }, SystemCmd::ThrowBehaviour::DoThrow); },
		      SystemCmdException);
}


BOOST_AUTO_TEST_CASE(env1)
{
    setenv("TWO", "2", 1);

    SystemCmd::Options options("env");

    BOOST_CHECK(find(options.envs.begin(), options.envs.end(), "TWO=2") != options.envs.end());

    options.setenv("ONE", "1");

    BOOST_CHECK(find(options.envs.begin(), options.envs.end(), "ONE=1") != options.envs.end());

    options.setenv("ONE", "");

    BOOST_CHECK(find(options.envs.begin(), options.envs.end(), "ONE=") != options.envs.end());

    options.unsetenv("ONE");

    BOOST_CHECK(find(options.envs.begin(), options.envs.end(), "ONE") == options.envs.end());
    BOOST_CHECK(find(options.envs.begin(), options.envs.end(), "ONE=") == options.envs.end());
    BOOST_CHECK(find(options.envs.begin(), options.envs.end(), "ONE=1") == options.envs.end());

    // AFAIS an empty environment variable is always stored with a trailing '='. Even if
    // that is not the case unsetenv works.

    options.envs.push_back("ONE");
    options.unsetenv("ONE");

    BOOST_CHECK(find(options.envs.begin(), options.envs.end(), "ONE") == options.envs.end());
    BOOST_CHECK(find(options.envs.begin(), options.envs.end(), "ONE=") == options.envs.end());
}


BOOST_AUTO_TEST_CASE(env2)
{
    vector<string> stdout = {
	"C ++ sure"
    };

    setenv("LC_ALL", "en_US", 1);	// will be overridden (by default in options.env)
    setenv("FUNNY", "++", 1);		// will not be overridden
    setenv("AMUSING", "no", 1);		// will be overridden (by addition to options.env)

    SystemCmd::Options options("echo $LC_ALL $FUNNY $AMUSING");

    options.setenv("AMUSING", "sure");

    SystemCmd cmd(options);

    BOOST_CHECK_EQUAL(cmd.retcode(), 0);
    BOOST_CHECK_EQUAL(join(cmd.stdout()), join(stdout));
}


int
num_open_fds()
{
    const int max_fd = getdtablesize();

    int n = 0;

    for (int fd = 0; fd < max_fd; ++fd)
        if (fcntl(fd, F_GETFD) != -1)
	  ++n;

    return n;
}


BOOST_AUTO_TEST_CASE(close_fds)
{
    // Check that no file descriptors are leaked (are closed even right after the
    // SystemCmd constructor).

    // n can be more than three, e.g. in valgrind or emacs
    const int n = num_open_fds();

    SystemCmd cmd({ LS_BIN, "-1", "/proc/self/fd/" });

    BOOST_CHECK_EQUAL(cmd.retcode(), 0);

    // stdin, stdout, stderr and an fd resulting from the opendir in ls
    BOOST_CHECK_EQUAL(cmd.stdout().size(), 4);

    // same number of open fds as before
    BOOST_CHECK_EQUAL(num_open_fds(), n);
}
