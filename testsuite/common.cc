
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include "common.h"

#include "storage/Utils/AppUtil.h"
#include "storage/Utils/Region.h"
#include "storage/Exception.h"


extern char* program_invocation_short_name;


namespace storage
{
    using namespace std;


    void
    setup_logger()
    {
	string name = program_invocation_short_name;
	string::size_type pos = name.rfind(".");
	if (pos != string::npos)
	{
	    string path = name.substr(pos + 1) + ".out/out";
	    string file = name.substr(0, pos) + ".log";
	    system(string("rm -f " + path + "/" + file).c_str());
	    createLogger(path, file);
	}
    }


    void
    setup_system(const string& name)
    {
	system("mkdir -p tmp");
	system("rm -rf tmp/*");
	system("mkdir tmp/etc");
	system(string("cp data/" + name + "/*.info tmp").c_str());
    }


    void
    write_fstab(const list<string>& lines)
    {
	ofstream fstab("tmp/etc/fstab");

	for (const string& line : lines)
	    fstab << line << endl;
    }


    void
    write_crypttab(const list<string>& lines)
    {
	ofstream fstab("tmp/etc/crypttab");

	for (const string& line : lines)
	    fstab << line << endl;
    }


    void
    print_fstab()
    {
	ifstream fstab("tmp/etc/fstab");

	cout << "begin of fstab" << endl;
	string line;
	while (getline(fstab, line))
	    cout << line << endl;
	cout << "end of fstab" << endl;
    }


    void
    print_crypttab()
    {
	ifstream fstab("tmp/etc/crypttab");

	cout << "begin of crypttab" << endl;
	string line;
	while (getline(fstab, line))
	    cout << line << endl;
	cout << "end of crypttab" << endl;
    }


    void
    print_partitions(StorageInterface* s, const string& disk)
    {
	deque<PartitionInfo> partitioninfos;
	s->getPartitionInfo(disk, partitioninfos);
	for (const PartitionInfo& partitioninfo : partitioninfos)
	{
	    cout << partitioninfo.v.name << ' ';
	    switch (partitioninfo.partitionType)
	    {
		case PRIMARY: cout << "PRIMARY "; break;
		case EXTENDED: cout << "EXTENDED "; break;
		case LOGICAL: cout << "LOGICAL "; break;
		case PTYPE_ANY: cout << "ANY "; break;
	    }
	    cout << Region(partitioninfo.cylRegion) << ' ';
	    switch (partitioninfo.v.fs)
	    {
		case FSUNKNOWN: cout << "UNKNOWN"; break;
		case REISERFS: cout << "REISERFS"; break;
		case EXT2: cout << "EXT2"; break;
		case EXT3: cout << "EXT3"; break;
		case EXT4: cout << "EXT4"; break;
		case BTRFS: cout << "BTRFS"; break;
		case VFAT: cout << "VFAT"; break;
		case XFS: cout << "XFS"; break;
		case JFS: cout << "JFS"; break;
		case HFS: cout << "HFS"; break;
		case NTFS: cout << "NTFS"; break;
		case HFSPLUS: cout << "HFSPLUS"; break;
		case SWAP: cout << "SWAP"; break;
		case NFS: cout << "NFS"; break;
		case NFS4: cout << "NFS4"; break;
		case TMPFS: cout << "TMPFS"; break;
		case ISO9660: cout << "ISO9660"; break;
		case UDF: cout << "UDF"; break;
		case FSNONE: cout << "NONE"; break;
	    }
	    cout << endl;
	}

	cout << endl;
    }
}


std::ostream & operator<<( std::ostream & stream, const storage::ParseException & ex )
{
    stream << "ParseException: " << ex.msg()
	   << "\n  seen:     \"" << ex.seen()     << "\""
	   << "\n  expected: \"" << ex.expected() << "\""
	   << std::endl;

    return stream;
}
