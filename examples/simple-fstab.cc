
#include <iostream>
#include <boost/algorithm/string.hpp>

#include "storage/SimpleEtcFstab.h"
#include "storage/Utils/Logger.h"

using namespace std;
using namespace storage;


int
main()
{
    set_logger(get_stdout_logger());

    vector<SimpleEtcFstabEntry> fstab = read_simple_etc_fstab("/etc/fstab");

    for (const SimpleEtcFstabEntry& entry : fstab)
    {
	cout << entry.device << "  " << entry.mount_point << "  "
	     << get_fs_type_name(entry.fs_type) << "  "
	     << boost::join(entry.mount_options, ",") << "  "
	     << entry.fs_freq << "  " << entry.fs_passno << '\n';
    }
}
