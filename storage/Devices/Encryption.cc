

#include "storage/Devices/EncryptionImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Encryption*
    Encryption::create(DeviceGraph* device_graph, const string& name)
    {
	Encryption* ret = new Encryption(new Encryption::Impl(name));
	ret->addToDeviceGraph(device_graph);
	return ret;
    }


    Encryption*
    Encryption::load(DeviceGraph* device_graph, const xmlNode* node)
    {
	Encryption* ret = new Encryption(new Encryption::Impl(node));
	ret->addToDeviceGraph(device_graph);
	return ret;
    }


    Encryption::Encryption(Impl* impl)
	: BlkDevice(impl)
    {
    }


    Encryption*
    Encryption::clone() const
    {
	return new Encryption(getImpl().clone());
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
