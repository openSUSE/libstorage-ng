#ifndef FILESYSTEM_H
#define FILESYSTEM_H


#include <vector>

#include "storage/Devices/Device.h"


namespace storage
{

    using namespace std;


    enum class FsType {
	REISERFS, EXT2, EXT3, EXT4, BTRFS, VFAT, XFS, JFS, HFS, NTFS, SWAP,
	HFSPLUS, NFS, NFS4, TMPFS, ISO9660, UDF
    };


    // abstract class

    class Filesystem : public Device
    {
    public:

	virtual bool supports_label() const = 0;
	virtual unsigned int max_labelsize() const = 0;

	const string& get_label() const;
	void set_label(const string& label);

	virtual bool supports_uuid() const = 0;

	const string& get_uuid() const;

	const vector<string>& get_mountpoints() const;
	void add_mountpoint(const string& mountpoint);

	static vector<Filesystem*> find_by_label(const Devicegraph* devicegraph,
						 const string& label);

	static vector<Filesystem*> find_by_mountpoint(const Devicegraph* devicegraph,
						      const string& mountpoint);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

    protected:

	Filesystem(Impl* impl);

    };

}

#endif
