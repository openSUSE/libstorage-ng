
#include <iostream>

#include "storage/StorageImpl.h"
#include "storage/Environment.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/Logger.h"


using namespace std;
using namespace storage;


void
doit(const string& filename, const vector<string>& commands)
{
    set_logger(get_logfile_logger());

    Environment environment(true, ProbeMode::READ_DEVICEGRAPH, TargetMode::DIRECT);

    environment.set_devicegraph_filename(filename);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    size_t index = 0;
    while (index < commands.size())
    {
	string command = commands[index++];

	// remove device with sid
	if (command == "rm")
	{
	    Device* device = staging->find_device(std::stoi(commands[index++]));
	    staging->remove_device(device);
	}
	// remove device with sid and its descendants
	else if (command == "rm+")
        {
	    Device* device = staging->find_device(std::stoi(commands[index++]));
	    device->remove_descendants();
	    staging->remove_device(device);
        }
	// only remove descendants of device with sid
	else if (command == "rm+-")
        {
	    Device* device = staging->find_device(std::stoi(commands[index++]));
	    device->remove_descendants();
        }
	else
	{
	    cerr << "unknown command" << endl;
	}
    }

    staging->save(filename);
}


void usage() __attribute__ ((__noreturn__));

void
usage()
{
    cerr << "transmogrify filename [commands...]" << endl;
    exit(EXIT_FAILURE);
}


int
main(int argc, char **argv)
{
    if (argc < 2)
	usage();

    doit(argv[1], vector<string>(argv + 2, argv + argc));

    exit(EXIT_SUCCESS);
}
