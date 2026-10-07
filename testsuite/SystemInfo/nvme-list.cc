
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
	R"({)",
	R"(  "Devices":[)",
	R"(    {)",
	R"(      "HostNQN":"nqn.2014-08.org.nvmexpress:uuid:afb211cc-32bb-11b2-a85c-8b99b656b4d1",)",
	R"(      "HostID":"8aa6c093-7e2f-4b59-93dd-9374345abed8",)",
	R"(      "Subsystems":[)",
	R"(        {)",
	R"(          "Subsystem":"nvme-subsys0",)",
	R"(          "SubsystemNQN":"nqn.2014.08.org.nvmexpress:17aa17aa1142267006586",)",
	R"(          "Controllers":[)",
	R"(            {)",
	R"(              "Controller":"nvme0",)",
	R"(              "SerialNumber":"1142267006586",)",
	R"(              "ModelNumber":"LENSE20512GMSP34MEAT2TA",)",
	R"(              "Firmware":"2.8.8341",)",
	R"(              "Transport":"pcie",)",
	R"(              "Address":"0000:3e:00.0",)",
	R"(              "Namespaces":[)",
	R"(                {)",
	R"(                  "NameSpace":"nvme0n1",)",
	R"(                  "NSID":1,)",
	R"(                  "UsedBytes":0,)",
	R"(                  "MaximumLBA":1000215216,)",
	R"(                  "PhysicalSize":-2147483648,)",
	R"(                  "SectorSize":512)",
	R"(                })",
	R"(              ],)",
	R"(              "Paths":[)",
	R"(              ])",
	R"(            })",
	R"(          ],)",
	R"(          "Namespaces":[)",
	R"(          ])",
	R"(        })",
	R"(      ])",
	R"(    })",
	R"(  ])",
	R"(})"
    };

    vector<string> output = {
    };

    check(input, output);
}
