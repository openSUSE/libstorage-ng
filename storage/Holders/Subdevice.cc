

#include "storage/Holders/SubdeviceImpl.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    Subdevice*
    Subdevice::create(Devicegraph* devicegraph, const Device* source, const Device* target)
    {
	Subdevice* ret = new Subdevice(new Subdevice::Impl());
	ret->Holder::create(devicegraph, source, target);
	return ret;
    }


    Subdevice*
    Subdevice::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	Subdevice* ret = new Subdevice(new Subdevice::Impl(node));
	ret->Holder::load(devicegraph, node);
	return ret;
    }


    Subdevice::Subdevice(Impl* impl)
	: Holder(impl)
    {
    }


    Subdevice*
    Subdevice::clone() const
    {
	return new Subdevice(get_impl().clone());
    }


    Subdevice::Impl&
    Subdevice::get_impl()
    {
	return dynamic_cast<Impl&>(Holder::get_impl());
    }


    const Subdevice::Impl&
    Subdevice::get_impl() const
    {
	return dynamic_cast<const Impl&>(Holder::get_impl());
    }


    bool
    is_subdevice(const Holder* holder)
    {
	return is_holder_of_type<const Subdevice>(holder);
    }


    Subdevice*
    to_subdevice(Holder* holder)
    {
	return to_holder_of_type<Subdevice>(holder);
    }


    const Subdevice*
    to_subdevice(const Holder* holder)
    {
	return to_holder_of_type<const Subdevice>(holder);
    }

}
