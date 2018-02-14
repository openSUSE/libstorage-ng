
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
#include "storage/Actiongraph.h"


using namespace std;
using namespace storage;


enum Mode { DEVICEGRAPH, ACTIONGRAPH };
Mode mode = DEVICEGRAPH;

bool keep_gv = false;
bool keep_svg = false;


void
helper(const string& filename_gv, const string& filename_svg)
{
    system(string("dot -Tsvg < " + quote(filename_gv) + " > " + quote(filename_svg)).c_str());

    if (!keep_gv)
	unlink(filename_gv.c_str());

    system(string("display " + quote(filename_svg)).c_str());

    if (!keep_svg)
	unlink(filename_svg.c_str());
}


void
doit_devicegraph(const string& filename)
{
    Environment environment(true, ProbeMode::READ_DEVICEGRAPH, TargetMode::DIRECT);

    environment.set_devicegraph_filename(filename);

    Storage storage(environment);
    storage.probe();

    const Devicegraph* probed = storage.get_probed();
    cout << *probed << '\n';

    const TmpDir& tmp_dir = storage.get_impl().get_tmp_dir();

    string name = basename(filename);
    if (boost::ends_with(name, ".xml"))
	name.erase(name.size() - 4);

    string filename_gv = keep_gv ? name + ".gv" : tmp_dir.get_fullname() + "/" + name + ".gv";
    string filename_svg = keep_svg ? name + ".svg" : tmp_dir.get_fullname() + "/" + name + ".svg";

    probed->write_graphviz(filename_gv, GraphvizFlags::CLASSNAME | GraphvizFlags::NAME |
			   GraphvizFlags::SID | GraphvizFlags::SIZE);

    helper(filename_gv, filename_svg);
}


void
doit_actiongraph(const string& filename_lhs, const string& filename_rhs)
{
    Environment environment(true, ProbeMode::READ_DEVICEGRAPH, TargetMode::DIRECT);
    environment.set_devicegraph_filename(filename_lhs);

    Storage storage(environment);
    storage.probe();

    Devicegraph* system = storage.get_system();
    cout << *system << '\n';

    Devicegraph* staging = storage.get_staging();
    staging->load(filename_rhs);
    cout << *staging << '\n';

    Actiongraph actiongraph(storage, system, staging);

    const TmpDir& tmp_dir = storage.get_impl().get_tmp_dir();

    string name = "actions";

    string filename_gv = keep_gv ? name + ".gv" : tmp_dir.get_fullname() + "/" + name + ".gv";
    string filename_svg = keep_svg ? name + ".svg" : tmp_dir.get_fullname() + "/" + name + ".svg";

    actiongraph.write_graphviz(filename_gv, GraphvizFlags::NAME | GraphvizFlags::SID);

    helper(filename_gv, filename_svg);
}


void usage() __attribute__ ((__noreturn__));

void
usage()
{
    cerr << "display [--devicegraph] [--keep-gv] [--keep-svg] filename\n"
	 << "display --actiongraph [--keep-gv] [--keep-svg] filename filename\n";

    exit(EXIT_FAILURE);
}


int
main(int argc, char **argv)
{
    const struct option options[] = {
	{ "devicegraph",	no_argument,	0,	1 },
	{ "actiongraph",	no_argument,	0,	2 },
	{ "keep-gv",		no_argument,	0,	3 },
	{ "keep-svg",		no_argument,	0,	4 },
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
		mode = DEVICEGRAPH;
		break;

	    case 2:
		mode = ACTIONGRAPH;
		break;

	    case 3:
		keep_gv = true;
		break;

	    case 4:
		keep_svg = true;
		break;

	    default:
		usage();
	}
    }

    switch (mode)
    {
	case DEVICEGRAPH:
	    if (optind + 1 != argc)
		usage();
	    break;

	case ACTIONGRAPH:
	    if (optind + 2 != argc)
		usage();
	    break;
    }

    try
    {
	set_logger(get_logfile_logger());

	switch (mode)
	{
	    case DEVICEGRAPH:
		doit_devicegraph(argv[optind]);
		break;

	    case ACTIONGRAPH:
		doit_actiongraph(argv[optind], argv[optind + 1]);
		break;
	}
    }
    catch (const exception& e)
    {
	cerr << "exception occured: " << e.what() << '\n';
	exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
