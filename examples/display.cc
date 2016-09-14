
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


void
doit(const string& filename)
{
    set_logger(get_logfile_logger());

    Environment environment(true, ProbeMode::READ_DEVICEGRAPH, TargetMode::DIRECT);

    environment.set_devicegraph_filename(filename);

    Storage storage(environment);

    const Devicegraph* probed = storage.get_probed();

    cout << *probed << endl;

    const TmpDir& tmp_dir = storage.get_impl().get_tmp_dir();

    probed->write_graphviz(tmp_dir.get_fullname() + "/display.gv", GraphvizFlags::CLASSNAME |
			   GraphvizFlags::SID | GraphvizFlags::SIZE);
    system(string("dot -Tsvg < " + quote(tmp_dir.get_fullname() + "/display.gv") + " > " +
		  quote(tmp_dir.get_fullname() + "/display.svg")).c_str());
    unlink(string(tmp_dir.get_fullname() + "/display.gv").c_str());
    system(string("display " + quote(tmp_dir.get_fullname() + "/display.svg")).c_str());
    unlink(string(tmp_dir.get_fullname() + "/display.svg").c_str());
}


void usage() __attribute__ ((__noreturn__));

void
usage()
{
    cerr << "display filename" << endl;
    exit(EXIT_FAILURE);
}


int
main(int argc, char **argv)
{
    if (argc != 2)
	usage();

    doit(argv[1]);

    exit(EXIT_SUCCESS);
}
