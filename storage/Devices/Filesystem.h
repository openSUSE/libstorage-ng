#ifndef FILESYSTEM_H
#define FILESYSTEM_H


#include <vector>

#include "storage/Devices/Device.h"
#include "storage/StorageInterface.h"


namespace storage
{
    class BlkDevice;


    // abstract class

    class Filesystem : public Device
    {
    public:

	virtual bool supports_label() const = 0;
	virtual unsigned int max_labelsize() const = 0;

	const std::string& get_label() const;
	void set_label(const std::string& label);

	virtual bool supports_uuid() const = 0;

	const std::string& get_uuid() const;

	const std::vector<std::string>& get_mountpoints() const;
	void add_mountpoint(const std::string& mountpoint);

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

	void print(std::ostream& out) const override = 0;

    };


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
