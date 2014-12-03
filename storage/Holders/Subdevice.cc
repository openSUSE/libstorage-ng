

#include "storage/Holders/SubdeviceImpl.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    Subdevice*
    Subdevice::create(DeviceGraph* device_graph, const Device* source, const Device* target)
    {
	Subdevice* ret = new Subdevice(new Subdevice::Impl());
	ret->addToDeviceGraph(device_graph, source, target);
	return ret;
    }


    Subdevice*
    Subdevice::load(DeviceGraph* device_graph, const xmlNode* node)
    {
	Subdevice* ret = new Subdevice(new Subdevice::Impl(node));

	// TODO

	sid_t source_sid = 0;
	getChildValue(node, "source-sid", source_sid);

	sid_t target_sid = 0;
	getChildValue(node, "target-sid", target_sid);

	const Device* source_device = device_graph->find_device(source_sid);
	const Device* target_device = device_graph->find_device(target_sid);

	ret->addToDeviceGraph(device_graph, source_device, target_device);

	return ret;
    }


    Subdevice::Subdevice(Impl* impl)
	: Holder(impl)
    {
    }


    Subdevice*
    Subdevice::clone() const
    {
	return new Subdevice(getImpl().clone());
    }


    Subdevice::Impl&
    Subdevice::getImpl()
    {
	return dynamic_cast<Impl&>(Holder::getImpl());
    }


    const Subdevice::Impl&
    Subdevice::getImpl() const
    {
	return dynamic_cast<const Impl&>(Holder::getImpl());
    }

}
