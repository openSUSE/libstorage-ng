#ifndef BLK_DEVICE_H
#define BLK_DEVICE_H


#include "storage/Devices/Device.h"
#include "storage/StorageInterface.h"


namespace storage
{

    using namespace storage_legacy;

    class Filesystem;


    // abstract class

    class BlkDevice : public Device
    {
    public:

	virtual void check() const override;

	const std::string& get_name() const;
	void set_name(const std::string& name);

	unsigned long long get_size_k() const;
	void set_size_k(unsigned long long size_k);

	std::string get_size_string() const;

	dev_t get_majorminor() const;
	unsigned int get_major() const;
	unsigned int get_minor() const;

	const std::string& get_udev_path() const;
	const std::vector<std::string>& get_udev_ids() const;

	static BlkDevice* find(const Devicegraph* devicegraph, const std::string& name);

	Filesystem* create_filesystem(FsType fs_type);

	Filesystem* get_filesystem();
	const Filesystem* get_filesystem() const;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

    protected:

	BlkDevice(Impl* impl);

    };


    inline bool
    is_blkdevice(Device* device)
    {
	return dynamic_cast<BlkDevice*>(device) != nullptr;
    }


    inline BlkDevice*
    to_blkdevice(Device* device)
    {
	return dynamic_cast<BlkDevice*>(device);
    }


    inline const BlkDevice*
    to_blkdevice(const Device* device)
    {
	return dynamic_cast<const BlkDevice*>(device);
    }

}

#endif
