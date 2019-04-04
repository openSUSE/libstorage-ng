
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
bool load_mockup = false;
bool ignore_probe_errors = false;


class MyProbeCallbacks : public ProbeCallbacks
{
public:

    virtual void
    message(const std::string& message) const override
    {
	cerr << message << endl;
    }


    virtual bool
    error(const std::string& message, const std::string& what) const override
    {
	cerr << message << endl;

	return ignore_probe_errors;
    }

};


void
doit()
{
    set_logger(get_logfile_logger());

    ProbeMode probe_mode = ProbeMode::STANDARD;
    if (save_mockup)
	probe_mode = ProbeMode::STANDARD_WRITE_MOCKUP;
    else if (load_mockup)
	probe_mode = ProbeMode::READ_MOCKUP;

    Environment environment(true, probe_mode, TargetMode::DIRECT);

    environment.set_mockup_filename("mockup.xml");

    MyProbeCallbacks my_probe_callbacks;

    Storage storage(environment);
    storage.probe(&my_probe_callbacks);

    const Devicegraph* probed = storage.get_probed();

    cout.setf(ios::boolalpha);
    cout.setf(ios::showbase);

    cout << *probed << '\n';

    probed->check();

    if (save_devicegraph)
	probed->save("devicegraph.xml");

    if (display_devicegraph)
    {
	const TmpDir& tmp_dir = storage.get_impl().get_tmp_dir();

	probed->write_graphviz(tmp_dir.get_fullname() + "/probe.gv", GraphvizFlags::CLASSNAME |
			       GraphvizFlags::NAME | GraphvizFlags::SID | GraphvizFlags::SIZE);
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
    cerr << "probe [--display-devicegraph] [--save-devicegraph] [--save-mockup] [--load-mockup] "
	"[--ignore-probe-errors]\n";
    exit(EXIT_FAILURE);
}


int
main(int argc, char **argv)
{
    const struct option options[] = {
	{ "display-devicegraph",	no_argument,	0,	1 },
	{ "save-devicegraph",		no_argument,	0,	2 },
	{ "save-mockup",		no_argument,	0,	3 },
	{ "load-mockup",		no_argument,	0,	4 },
	{ "ignore-probe-errors",	no_argument,	0,	5 },
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

	    case 4:
		load_mockup = true;
		break;

	    case 5:
		ignore_probe_errors = true;
		break;

	    default:
		usage();
	}
    }

    if (optind < argc)
	usage();

    try
    {
	doit();
    }
    catch (const exception& e)
    {
	cerr << "exception occured: " << e.what() << '\n';
	exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
