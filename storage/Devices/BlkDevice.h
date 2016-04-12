#ifndef STORAGE_BLK_DEVICE_H
#define STORAGE_BLK_DEVICE_H


#include "storage/Devices/Device.h"
#include "storage/Devices/Filesystem.h"


namespace storage
{

    //! An abstract Block Device.
    class BlkDevice : public Device
    {
    public:

	virtual void check() const override;

	const std::string& get_name() const;
	void set_name(const std::string& name);

	unsigned long long get_size_k() const;
	void set_size_k(unsigned long long size_k);

	std::string get_size_string() const;

	const std::string& get_sysfs_name() const;
	const std::string& get_sysfs_path() const;

	const std::string& get_udev_path() const;
	const std::vector<std::string>& get_udev_ids() const;

	ResizeInfo detect_resize_info() const;

	static BlkDevice* find(Devicegraph* devicegraph, const std::string& name);
	static const BlkDevice* find(const Devicegraph* devicegraph, const std::string& name);

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


    bool is_blk_device(const Device* device);

    BlkDevice* to_blk_device(Device* device);
    const BlkDevice* to_blk_device(const Device* device);

}

#endif
