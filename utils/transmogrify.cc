
#include <iostream>
#include <algorithm>

#include "storage/Storage.h"
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
    storage.probe();

    Devicegraph* staging = storage.get_staging();

    size_t index = 0;
    while (index < commands.size())
    {
	string command = commands[index++];

	// remove device with sid
	if (command == "remove")
	{
	    Device* device = staging->find_device(std::stoi(commands[index++]));
	    staging->remove_device(device);
	}
	// remove device with sid and its descendants
	else if (command == "remove+")
        {
	    Device* device = staging->find_device(std::stoi(commands[index++]));
	    device->remove_descendants();
	    staging->remove_device(device);
        }
	// only remove descendants of device with sid
	else if (command == "remove+-")
        {
	    Device* device = staging->find_device(std::stoi(commands[index++]));
	    device->remove_descendants();
        }
	// keep device with sid and its descendants
	else if (command == "keep+")
	{
	    Device* device = staging->find_device(std::stoi(commands[index++]));
	    vector<Device*> devices_to_keep = device->get_descendants(true);

	    for (Device* tmp : Device::get_all(staging))
		if (find(devices_to_keep.begin(), devices_to_keep.end(), tmp) == devices_to_keep.end())
		    staging->remove_device(tmp);
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

    try
    {
	doit(argv[1], vector<string>(argv + 2, argv + argc));
    }
    catch (const exception& e)
    {
	cerr << "exception occured: " << e.what() << endl;
	exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
