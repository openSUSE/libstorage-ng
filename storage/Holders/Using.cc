

#include "storage/Holders/UsingImpl.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    Using*
    Using::create(Devicegraph* devicegraph, const Device* source, const Device* target)
    {
	Using* ret = new Using(new Using::Impl());
	ret->Holder::create(devicegraph, source, target);
	return ret;
    }


    Using*
    Using::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	Using* ret = new Using(new Using::Impl(node));
	ret->Holder::load(devicegraph, node);
	return ret;
    }


    Using::Using(Impl* impl)
	: Holder(impl)
    {
    }


    Using*
    Using::clone() const
    {
	return new Using(get_impl().clone());
    }


    Using::Impl&
    Using::get_impl()
    {
	return dynamic_cast<Impl&>(Holder::get_impl());
    }


    const Using::Impl&
    Using::get_impl() const
    {
	return dynamic_cast<const Impl&>(Holder::get_impl());
    }

}
