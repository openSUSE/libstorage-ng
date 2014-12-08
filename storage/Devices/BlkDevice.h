#ifndef BLK_DEVICE_H
#define BLK_DEVICE_H


#include "storage/Devices/Device.h"
#include "storage/Devices/Filesystem.h"


namespace storage
{

    using namespace std;


    // abstract class

    class BlkDevice : public Device
    {
    public:

	virtual string getDisplayName() const override { return getName(); }

	virtual void check() const override;

	const string& getName() const;
	void setName(const string& name);

	unsigned long long getSizeK() const;
	void setSizeK(unsigned long long size_k);

	static BlkDevice* find(const DeviceGraph* device_graph, const string& name);

	Filesystem* createFilesystem(FsType fs_type);

    public:

	class Impl;

	Impl& getImpl();
	const Impl& getImpl() const;

    protected:

	BlkDevice(Impl* impl);

    };

}

#endif
