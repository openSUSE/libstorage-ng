

#include <iostream>

#include "storage/Devices/LvmLvImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    LvmLv*
    LvmLv::create(Devicegraph* devicegraph, const string& name)
    {
	LvmLv* ret = new LvmLv(new LvmLv::Impl(name));
	ret->Device::create(devicegraph);
	return ret;
    }


    LvmLv*
    LvmLv::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	LvmLv* ret = new LvmLv(new LvmLv::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    LvmLv::LvmLv(Impl* impl)
	: BlkDevice(impl)
    {
    }


    LvmLv*
    LvmLv::clone() const
    {
	return new LvmLv(get_impl().clone());
    }


    LvmLv::Impl&
    LvmLv::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const LvmLv::Impl&
    LvmLv::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    void
    LvmLv::check() const
    {
	if (get_name().empty())
	    cerr << "logical volume has no name" << endl;
    }


    void
    LvmLv::print(std::ostream& out) const
    {
	BlkDevice::print(out);
    }

}
