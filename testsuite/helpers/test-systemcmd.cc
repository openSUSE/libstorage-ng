
#include <iostream>

#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/Format.h"

using namespace std;
using namespace storage;


unsigned int
num_children()
{
    const pid_t pid = getpid();

    string path = sformat("/proc/%d/task/%d/children", pid, pid);

    FILE* f = fopen(path.c_str(), "r");
    if (!f)
    {
        cerr << "failed to open proc children file\n";
	throw;
    }

    unsigned int count = 0;

    pid_t child_pid;
    while (fscanf(f, "%d", &child_pid) == 1)
	++count;

    fclose(f);

    return count;
}


int
main()
{
    try
    {
	SystemCmd cmd({ "/usr/bin/echo", "hello", "world" }, SystemCmd::ThrowBehaviour::DoThrow);
    }
    catch (const Exception& e)
    {
	cerr << "failed\n"
	     << e.what() << '\n'
	     << "num children: " << num_children() << '\n';

	return EXIT_FAILURE;
    }

    cerr << "success\n"
	 << "num children: " << num_children() << '\n';

    return EXIT_SUCCESS;
}
