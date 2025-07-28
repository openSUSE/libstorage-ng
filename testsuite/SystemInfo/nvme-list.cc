
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
    Mockup::set_command(NVME_BIN " list --verbose --output-format json", input);

    CmdNvmeList cmd_nvme_list;

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmd_nvme_list;

    string lhs = parsed.str();
    string rhs = accumulate(output.begin(), output.end(), ""s,
			    [](auto a, auto b) { return a + b + "\n"; });

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    set_logger(get_stdout_logger());

    vector<string> input = {
	"{",
	"  \"Devices\":[",
	"    {",
	"      \"HostNQN\":\"nqn.2014-08.org.nvmexpress:uuid:afb211cc-32bb-11b2-a85c-8b99b656b4d1\",",
	"      \"HostID\":\"8aa6c093-7e2f-4b59-93dd-9374345abed8\",",
	"      \"Subsystems\":[",
	"        {",
	"          \"Subsystem\":\"nvme-subsys0\",",
	"          \"SubsystemNQN\":\"nqn.2014.08.org.nvmexpress:17aa17aa1142267006586\",",
	"          \"Controllers\":[",
	"            {",
	"              \"Controller\":\"nvme0\",",
	"              \"SerialNumber\":\"1142267006586\",",
	"              \"ModelNumber\":\"LENSE20512GMSP34MEAT2TA\",",
	"              \"Firmware\":\"2.8.8341\",",
	"              \"Transport\":\"pcie\",",
	"              \"Address\":\"0000:3e:00.0\",",
	"              \"Namespaces\":[",
	"                {",
	"                  \"NameSpace\":\"nvme0n1\",",
	"                  \"NSID\":1,",
	"                  \"UsedBytes\":0,",
	"                  \"MaximumLBA\":1000215216,",
	"                  \"PhysicalSize\":-2147483648,",
	"                  \"SectorSize\":512",
	"                }",
	"              ],",
	"              \"Paths\":[",
	"              ]",
	"            }",
	"          ],",
	"          \"Namespaces\":[",
	"          ]",
	"        }",
	"      ]",
	"    }",
	"  ]",
	"}"
    };

    vector<string> output = {
    };

    check(input, output);
}
