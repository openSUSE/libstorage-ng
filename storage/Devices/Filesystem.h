#ifndef FILESYSTEM_H
#define FILESYSTEM_H


#include <vector>

#include "storage/Devices/Device.h"
#include "storage/StorageInterface.h"


namespace storage
{
    using namespace storage_legacy;

    class BlkDevice;


    // abstract class

    class Filesystem : public Device
    {
    public:

	FsType get_type() const;

	virtual bool supports_label() const = 0;
	virtual unsigned int max_labelsize() const = 0;

	const std::string& get_label() const;
	void set_label(const std::string& label);

	virtual bool supports_uuid() const = 0;

	const std::string& get_uuid() const;

	const std::vector<std::string>& get_mountpoints() const;
	void set_mountpoints(const std::vector<std::string>& mountpoints);
	void add_mountpoint(const std::string& mountpoint);

	MountByType get_mount_by() const;
	void set_mount_by(MountByType mount_by);

	const std::list<std::string>& get_fstab_options() const;
	void set_fstab_options(const std::list<std::string>& fstab_options);

	const string& get_mkfs_options() const;
	void set_mkfs_options(const string& mkfs_options);

	const string& get_tune_options() const;
	void set_tune_options(const string& tune_options);

	static std::vector<Filesystem*> find_by_label(const Devicegraph* devicegraph,
						      const std::string& label);

	static std::vector<Filesystem*> find_by_mountpoint(const Devicegraph* devicegraph,
							   const std::string& mountpoint);

	// TODO class BlkFilesystem for not nfs, tmpfs?
	std::vector<const BlkDevice*> get_blkdevices() const;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

    protected:

	Filesystem(Impl* impl);

    };


    inline bool
    is_filesystem(const Device* device)
    {
	return dynamic_cast<const Filesystem*>(device) != 0;
    }


    inline Filesystem*
    to_filesystem(Device* device)
    {
	return dynamic_cast<Filesystem*>(device);
    }


    inline const Filesystem*
    to_filesystem(const Device* device)
    {
	return dynamic_cast<const Filesystem*>(device);
    }

}

#endif
