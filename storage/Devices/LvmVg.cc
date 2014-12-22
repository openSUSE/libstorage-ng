

#include <iostream>

#include "storage/Devices/LvmVgImpl.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    LvmVg*
    LvmVg::create(Devicegraph* devicegraph, const string& name)
    {
	LvmVg* ret = new LvmVg(new LvmVg::Impl(name));
	ret->Device::create(devicegraph);
	return ret;
    }


    LvmVg*
    LvmVg::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	LvmVg* ret = new LvmVg(new LvmVg::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    LvmVg::LvmVg(Impl* impl)
	: Device(impl)
    {
    }


    LvmVg*
    LvmVg::clone() const
    {
	return new LvmVg(get_impl().clone());
    }


    LvmVg::Impl&
    LvmVg::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const LvmVg::Impl&
    LvmVg::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    const string&
    LvmVg::get_name() const
    {
	return get_impl().name;
    }


    void
    LvmVg::set_name(const string& name)
    {
	get_impl().name = name;
    }


    void
    LvmVg::check() const
    {
	if (get_name().empty())
	    cerr << "volume group has no name" << endl;
    }


    LvmLv*
    LvmVg::create_lvm_lv(const string& name)
    {
	Devicegraph* devicegraph = get_impl().get_devicegraph();

	LvmLv* lvm_lv = LvmLv::create(devicegraph, name);
	Subdevice::create(devicegraph, this, lvm_lv);

	return lvm_lv;
    }


    void
    LvmVg::print(std::ostream& out) const
    {
	Device::print(out);
    }

}
