
#include <iostream>
#include <boost/algorithm/string.hpp>

#include "storage/SimpleEtcCrypttab.h"
#include "storage/Utils/Logger.h"

using namespace std;
using namespace storage;


int
main()
{
    set_logger(get_stdout_logger());

    vector<SimpleEtcCrypttabEntry> crypttab = read_simple_etc_crypttab("/etc/crypttab");

    for (const SimpleEtcCrypttabEntry& entry : crypttab)
    {
	cout << entry.name << "  " << entry.device << "  " << entry.password << "  "
	     << boost::join(entry.crypt_options, ",") << '\n';
    }
}
