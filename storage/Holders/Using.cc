

#include "storage/Holders/UsingImpl.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    Using*
    Using::create(DeviceGraph* device_graph, const Device* source, const Device* target)
    {
	Using* ret = new Using(new Using::Impl());
	ret->Holder::create(device_graph, source, target);
	return ret;
    }


    Using*
    Using::load(DeviceGraph* device_graph, const xmlNode* node)
    {
	Using* ret = new Using(new Using::Impl(node));
	ret->Holder::load(device_graph, node);
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
