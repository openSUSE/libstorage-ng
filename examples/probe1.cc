
#include <stdlib.h>
#include <getopt.h>
#include <iostream>

#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Devicegraph.h"


using namespace std;
using namespace storage;


bool display_devicegraph = false;
bool save_devicegraph = false;
bool save_mockup = false;


void
doit()
{
    Environment environment(true, save_mockup ? ProbeMode::STANDARD_WRITE_MOCKUP : ProbeMode::STANDARD,
			    TargetMode::DIRECT);

    environment.set_mockup_filename("probe1-mockup.xml");

    Storage storage(environment);

    const Devicegraph* probed = storage.get_probed();

    cout << *probed << endl;

    probed->check();

    if (save_devicegraph)
	probed->save("probe1-devicegraph.xml");

    if (display_devicegraph)
    {
	probed->write_graphviz("probe1.gv");
	system("dot -Tpng < probe1.gv > probe1.png");
	system("display probe1.png");
    }
}


void usage() __attribute__ ((__noreturn__));

void
usage()
{
    cerr << "probe1 [--display-devicegraph] [--save-devicegraph] [--save-mockup]" << endl;
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
