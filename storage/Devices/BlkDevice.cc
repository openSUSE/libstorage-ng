

#include <iostream>

#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Devices/Ext4.h"
#include "storage/Devices/Swap.h"


namespace storage
{

    using namespace std;


    BlkDevice::BlkDevice(Impl* impl)
	: Device(impl)
    {
    }



    BlkDevice::BlkDevice(DeviceGraph& device_graph, Impl* impl)
	: Device(device_graph, impl)
    {
    }


    BlkDevice::Impl&
    BlkDevice::getImpl()
    {
	return dynamic_cast<Impl&>(Device::getImpl());
    }


    const BlkDevice::Impl&
    BlkDevice::getImpl() const
    {
	return dynamic_cast<const Impl&>(Device::getImpl());
    }


    BlkDevice*
    BlkDevice::find(DeviceGraph& device_graph, const string& name)
    {
	for (DeviceGraph::Impl::vertex_descriptor v : device_graph.getImpl().vertices())
	{
	    BlkDevice* blk_device = dynamic_cast<BlkDevice*>(device_graph.getImpl().graph[v].get());
	    if (blk_device && blk_device->getName() == name)
		return blk_device;
	}

	ostringstream str;
	str << "device not found, name = " << name;
	throw device_not_found(str.str());
    }


    void
    BlkDevice::setName(const string& name)
    {
	getImpl().name = name;
    }


    const string&
    BlkDevice::getName() const
    {
	return getImpl().name;
    }


    void
    BlkDevice::check() const
    {
	if (getName().empty())
	    cerr << "blk device has no name" << endl;
    }


    Filesystem*
    BlkDevice::createFilesystem(FsType fs_type)
    {
	if (numChildren() != 0)
	    throw runtime_error("BlkDevice has children");

	Filesystem* ret = nullptr;

	switch (fs_type)
	{
	    case EXT4:
		ret = Ext4::create(getImpl().getDeviceGraph());
		break;

	    case SWAP:
		ret = Swap::create(getImpl().getDeviceGraph());
		break;
	}

	if (!ret)
	    throw runtime_error("unknown filesystem type");

	Using::create(getImpl().getDeviceGraph(), this, ret);

	return ret;
    }

}
