

#include "storage/Devices/EncryptionImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Encryption::Encryption(DeviceGraph& device_graph, const string& name)
	: BlkDevice(device_graph, new Encryption::Impl(device_graph, name))
    {
    }


/*
    Encryption::Encryption(DeviceGraph& device_graph, Impl* impl)
	: BlkDevice(device_graph, impl)
    {
    }
*/


    Encryption::Encryption(Impl* impl)
	: BlkDevice(impl)
    {
    }


    Encryption*
    Encryption::clone(DeviceGraph& device_graph) const
    {
	return new Encryption(getImpl().clone(device_graph));
    }


    Encryption::Impl&
    Encryption::getImpl()
    {
	return dynamic_cast<Impl&>(Device::getImpl());
    }


    const Encryption::Impl&
    Encryption::getImpl() const
    {
	return dynamic_cast<const Impl&>(Device::getImpl());
    }

}
