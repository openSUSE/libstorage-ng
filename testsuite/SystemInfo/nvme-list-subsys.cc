
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <numeric>
#include <boost/test/unit_test.hpp>

#include "storage/SystemInfo/CmdNvme.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"


using namespace std;
using namespace storage;


void
check(const vector<string>& input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(NVME_BIN " list-subsys --verbose --output-format json", input);

    CmdNvmeListSubsys cmd_nvme_list_subsys;

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmd_nvme_list_subsys;

    string lhs = parsed.str();
    string rhs = accumulate(output.begin(), output.end(), ""s,
			    [](auto a, auto b) { return a + b + "\n"; });

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    set_logger(get_stdout_logger());

    vector<string> input = {
	"[",
	"  {",
	"    \"HostNQN\":\"nqn.2014-08.org.nvmexpress:uuid:afb211cc-32bb-11b2-a85c-8b99b656b4d1\",",
	"    \"HostID\":\"8aa6c093-7e2f-4b59-93dd-9374345abed8\",",
	"    \"Subsystems\":[",
	"      {",
	"        \"Name\":\"nvme-subsys0\",",
	"        \"NQN\":\"nqn.2014.08.org.nvmexpress:17aa17aa1142267006586\",",
	"        \"Paths\":[",
	"          {",
	"            \"Name\":\"nvme0\",",
	"            \"Transport\":\"pcie\",",
	"            \"Address\":\"0000:3e:00.0\",",
	"            \"State\":\"live\"",
	"          }",
	"        ]",
	"      }",
	"    ]",
	"  }",
	"]"
    };

    vector<string> output = {
	"host-nqn:nqn.2014-08.org.nvmexpress:uuid:afb211cc-32bb-11b2-a85c-8b99b656b4d1, host-id:8aa6c093-7e2f-4b59-93dd-9374345abed8, subsystems: <name:nvme-subsys0nqn:nqn.2014.08.org.nvmexpress:17aa17aa1142267006586, paths:<name:nvme0, transport: PCIe>>"
    };

    check(input, output);
}
