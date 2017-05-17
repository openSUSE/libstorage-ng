
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <iostream>
#include <boost/algorithm/string.hpp>

#include "storage/StorageImpl.h"
#include "storage/Environment.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/Logger.h"
#include "storage/Utils/AppUtil.h"


using namespace std;
using namespace storage;


bool keep_gv = false;
bool keep_svg = false;


void
doit(const string& filename)
{
    set_logger(get_logfile_logger());

    Environment environment(true, ProbeMode::READ_DEVICEGRAPH, TargetMode::DIRECT);

    environment.set_devicegraph_filename(filename);

    Storage storage(environment);
    storage.probe();

    const Devicegraph* probed = storage.get_probed();

    cout << *probed << endl;

    const TmpDir& tmp_dir = storage.get_impl().get_tmp_dir();

    string name = basename(filename);
    if (boost::ends_with(name, ".xml"))
	name.erase(name.size() - 4);

    string filename_gv = keep_gv ? name + ".gv" : tmp_dir.get_fullname() + "/" + name + ".gv";

    string filename_svg = keep_svg ? name + ".svg" : tmp_dir.get_fullname() + "/" + name + ".svg";

    probed->write_graphviz(filename_gv, GraphvizFlags::CLASSNAME | GraphvizFlags::SID |
			   GraphvizFlags::SIZE);

    system(string("dot -Tsvg < " + quote(filename_gv) + " > " + quote(filename_svg)).c_str());

    if (!keep_gv)
	unlink(filename_gv.c_str());

    system(string("display " + quote(filename_svg)).c_str());

    if (!keep_svg)
	unlink(filename_svg.c_str());
}


void usage() __attribute__ ((__noreturn__));

void
usage()
{
    cerr << "display [--keep-gv] [--keep-svg] filename" << endl;
    exit(EXIT_FAILURE);
}


int
main(int argc, char **argv)
{
    const struct option options[] = {
	{ "keep-gv",		no_argument,	0,	1 },
	{ "keep-svg",		no_argument,	0,	2 },
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
		keep_gv = true;
		break;

	    case 2:
		keep_svg = true;
		break;

	    default:
		usage();
	}
    }

    if (optind + 1 != argc)
	usage();

    try
    {
	doit(argv[optind]);
    }
    catch (const exception& e)
    {
	cerr << "exception occured: " << e.what() << endl;
	exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
