

#include "storage/Holders/SubdeviceImpl.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    Subdevice*
    Subdevice::create(DeviceGraph* device_graph, const Device* source, const Device* target)
    {
	Subdevice* ret = new Subdevice(new Subdevice::Impl());
	ret->Holder::create(device_graph, source, target);
	return ret;
    }


    Subdevice*
    Subdevice::load(DeviceGraph* device_graph, const xmlNode* node)
    {
	Subdevice* ret = new Subdevice(new Subdevice::Impl(node));
	ret->Holder::load(device_graph, node);
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
