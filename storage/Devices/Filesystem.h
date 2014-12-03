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

	virtual bool supportsLabel() const = 0;
	virtual unsigned int maxLabelSize() const = 0;

	const string& getLabel() const;
	void setLabel(const string& label);

	virtual bool supportsUuid() const = 0;

	const string& getUuid() const;

	const vector<string>& getMountPoints() const;
	void addMountPoint(const string& mount_point);

	static vector<Filesystem*> findByLabel(const DeviceGraph* device_graph,
					       const string& label);

	static vector<Filesystem*> findByMountPoint(const DeviceGraph* device_graph,
						    const string& mount_point);

    public:

	class Impl;

	Impl& getImpl();
	const Impl& getImpl() const;

    protected:

	Filesystem(Impl* impl);

    };

}

#endif
