
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <iostream>

#include "storage/StorageImpl.h"
#include "storage/Environment.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/Logger.h"


using namespace std;
using namespace storage;


bool display_devicegraph = false;
bool save_devicegraph = false;
bool save_mockup = false;


void
doit()
{
    set_logger(get_logfile_logger());

    Environment environment(true, save_mockup ? ProbeMode::STANDARD_WRITE_MOCKUP : ProbeMode::STANDARD,
			    TargetMode::DIRECT);

    environment.set_mockup_filename("mockup.xml");

    Storage storage(environment);
    storage.probe();

    const Devicegraph* probed = storage.get_probed();

    cout.setf(ios::boolalpha);
    cout.setf(ios::showbase);

    cout << *probed << endl;

    probed->check();

    if (save_devicegraph)
	probed->save("devicegraph.xml");

    if (display_devicegraph)
    {
	const TmpDir& tmp_dir = storage.get_impl().get_tmp_dir();

	probed->write_graphviz(tmp_dir.get_fullname() + "/probe.gv", GraphvizFlags::CLASSNAME |
			       GraphvizFlags::SID | GraphvizFlags::SIZE);
	system(string("dot -Tsvg < " + quote(tmp_dir.get_fullname() + "/probe.gv") + " > " +
		      quote(tmp_dir.get_fullname() + "/probe.svg")).c_str());
	unlink(string(tmp_dir.get_fullname() + "/probe.gv").c_str());
	system(string("display " + quote(tmp_dir.get_fullname() + "/probe.svg")).c_str());
	unlink(string(tmp_dir.get_fullname() + "/probe.svg").c_str());
    }
}


void usage() __attribute__ ((__noreturn__));

void
usage()
{
    cerr << "probe [--display-devicegraph] [--save-devicegraph] [--save-mockup]" << endl;
    exit(EXIT_FAILURE);
}


int
main(int argc, char **argv)
{
    const struct option options[] = {
	{ "display-devicegraph",	no_argument,	0,	1 },
	{ "save-devicegraph",		no_argument,	0,	2 },
	{ "save-mockup",		no_argument,	0,	3 },
	{ 0, 0, 0, 0 }
    };

    while (true)
    {
	int option_index = 0;
	int c = getopt_long(argc, argv, "", options, &option_index);
	if (c == -1)
	    break;

	if (c == '?')
	    usage();

	switch (c)
	{
	    case 1:
		display_devicegraph = true;
		break;

	    case 2:
		save_devicegraph = true;
		break;

	    case 3:
		save_mockup = true;
		break;

	    default:
		usage();
	}
    }

    if (optind < argc)
	usage();

    doit();

    exit(EXIT_SUCCESS);
}
