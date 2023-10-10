
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/CmdDmsetup.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"


using namespace std;
using namespace storage;


void
check(const vector<string>& input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(DMSETUP_BIN " --columns --separator / --noheadings -o name,major,minor,"
			"segments,subsystem,uuid info", input);

    CmdDmsetupInfo cmddmsetupinfo;

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmddmsetupinfo;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"system-abuild/253/0/3/LVM/LVM-OMPzXFm3am1zIlAVdQi5WxtmyNcevmRn25elwrmng2K9f1Vnfex9VFkN4tEXD8xI",
	"system-test/253/7/1/LVM/LVM-OMPzXFm3am1zIlAVdQi5WxtmyNcevmRnAL4A6NByky35grIbJsJ8RYCQP0NqLsF7",
	"system-testsuite/253/6/1/LVM/LVM-OMPzXFm3am1zIlAVdQi5WxtmyNcevmRn47gLDXzm2P9srqwHhaiuUlNFmLlLJH9V",
	"system-btrfs/253/5/1/LVM/LVM-OMPzXFm3am1zIlAVdQi5WxtmyNcevmRnFSGfCGnRIJQ5lNx59k7JP7uK3fkyQl48",
	"system-arvin/253/1/2/LVM/LVM-OMPzXFm3am1zIlAVdQi5WxtmyNcevmRngOeeB8at0WP8Z3sAyZv5BtKT49j6TwN6",
	"system-swap/253/3/1/LVM/LVM-OMPzXFm3am1zIlAVdQi5WxtmyNcevmRnKKC5tfbWLpsF2toVKQtE0wxeQpUp8bV0",
	"system-root/253/2/1/LVM/LVM-OMPzXFm3am1zIlAVdQi5WxtmyNcevmRn89Crg8K5dO0VvjVwurvCLK4efhWCtRfN",
	"system-giant--xfs/253/4/3/LVM/LVM-OMPzXFm3am1zIlAVdQi5WxtmyNcevmRnHdH1l4B0LUptYisBQuf33vP5rGosS1e2"
    };

    vector<string> output = {
	"data[system-abuild] -> major:253 minor:0 segments:3 subsystem:LVM uuid:LVM-OMPzXFm3am1zIlAVdQi5WxtmyNcevmRn25elwrmng2K9f1Vnfex9VFkN4tEXD8xI",
	"data[system-arvin] -> major:253 minor:1 segments:2 subsystem:LVM uuid:LVM-OMPzXFm3am1zIlAVdQi5WxtmyNcevmRngOeeB8at0WP8Z3sAyZv5BtKT49j6TwN6",
	"data[system-btrfs] -> major:253 minor:5 segments:1 subsystem:LVM uuid:LVM-OMPzXFm3am1zIlAVdQi5WxtmyNcevmRnFSGfCGnRIJQ5lNx59k7JP7uK3fkyQl48",
	"data[system-giant--xfs] -> major:253 minor:4 segments:3 subsystem:LVM uuid:LVM-OMPzXFm3am1zIlAVdQi5WxtmyNcevmRnHdH1l4B0LUptYisBQuf33vP5rGosS1e2",
	"data[system-root] -> major:253 minor:2 segments:1 subsystem:LVM uuid:LVM-OMPzXFm3am1zIlAVdQi5WxtmyNcevmRn89Crg8K5dO0VvjVwurvCLK4efhWCtRfN",
	"data[system-swap] -> major:253 minor:3 segments:1 subsystem:LVM uuid:LVM-OMPzXFm3am1zIlAVdQi5WxtmyNcevmRnKKC5tfbWLpsF2toVKQtE0wxeQpUp8bV0",
	"data[system-test] -> major:253 minor:7 segments:1 subsystem:LVM uuid:LVM-OMPzXFm3am1zIlAVdQi5WxtmyNcevmRnAL4A6NByky35grIbJsJ8RYCQP0NqLsF7",
	"data[system-testsuite] -> major:253 minor:6 segments:1 subsystem:LVM uuid:LVM-OMPzXFm3am1zIlAVdQi5WxtmyNcevmRn47gLDXzm2P9srqwHhaiuUlNFmLlLJH9V"
    };

    check(input, output);
}
