

#include "storage/Holders/UsingImpl.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    Using*
    Using::create(DeviceGraph* device_graph, const Device* source, const Device* target)
    {
	Using* ret = new Using(new Using::Impl());
	ret->addToDeviceGraph(device_graph, source, target);
	return ret;
    }


    Using*
    Using::load(DeviceGraph* device_graph, const xmlNode* node)
    {
	Using* ret = new Using(new Using::Impl(node));

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


    Using::Using(Impl* impl)
	: Holder(impl)
    {
    }


    Using*
    Using::clone() const
    {
	return new Using(getImpl().clone());
    }


    Using::Impl&
    Using::getImpl()
    {
	return dynamic_cast<Impl&>(Holder::getImpl());
    }


    const Using::Impl&
    Using::getImpl() const
    {
	return dynamic_cast<const Impl&>(Holder::getImpl());
    }

}
