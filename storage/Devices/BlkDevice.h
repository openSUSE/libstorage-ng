#ifndef BLK_DEVICE_H
#define BLK_DEVICE_H


#include "storage/Devices/Device.h"
#include "storage/Devices/Filesystem.h"


namespace storage_bgl
{

    using namespace std;


    // abstract class

    class BlkDevice : public Device
    {
    public:

	virtual string get_displayname() const override { return get_name(); }

	virtual void check() const override;

	const string& get_name() const;
	void set_name(const string& name);

	unsigned long long get_size_k() const;
	void set_size_k(unsigned long long size_k);

	static BlkDevice* find(const Devicegraph* devicegraph, const string& name);

	Filesystem* create_filesystem(FsType fs_type);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

    protected:

	BlkDevice(Impl* impl);

    };

}

#endif
