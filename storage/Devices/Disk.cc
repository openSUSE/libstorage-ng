

#include <boost/algorithm/string.hpp>

#include "storage/Devices/DiskImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/Utils/Enum.h"


namespace storage
{

    using namespace std;


    Disk*
    Disk::create(Devicegraph* devicegraph, const string& name)
    {
	if (!boost::starts_with(name, "/dev/"))
	    ST_THROW(Exception("invalid disk name"));

	Disk* ret = new Disk(new Disk::Impl(name));
	ret->Device::create(devicegraph);
	return ret;
    }


    Disk*
    Disk::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	Disk* ret = new Disk(new Disk::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    Disk::Disk(Impl* impl)
	: Partitionable(impl)
    {
    }


    Disk*
    Disk::clone() const
    {
	return new Disk(get_impl().clone());
    }


    Disk::Impl&
    Disk::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Disk::Impl&
    Disk::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    Disk*
    Disk::find(Devicegraph* devicegraph, const string& name)
    {
	return to_disk(BlkDevice::find(devicegraph, name));
    }


    const Disk*
    Disk::find(const Devicegraph* devicegraph, const string& name)
    {
	return to_disk(BlkDevice::find(devicegraph, name));
    }


    bool
    Disk::get_rotational() const
    {
	return get_impl().get_rotational();
    }


    Transport
    Disk::get_transport() const
    {
	return get_impl().get_transport();
    }


    vector<Disk*>
    Disk::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<Disk>(compare_by_name);
    }


    vector<const Disk*>
    Disk::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const Disk>(compare_by_name);
    }


    bool
    is_disk(const Device* device)
    {
	return is_device_of_type<const Disk>(device);
    }


    Disk*
    to_disk(Device* device)
    {
	return to_device_of_type<Disk>(device);
    }


    const Disk*
    to_disk(const Device* device)
    {
	return to_device_of_type<const Disk>(device);
    }


    const vector<string> EnumTraits<DasdType>::names({
	"NONE", "ECKD", "FBA"
    });


    const vector<string> EnumTraits<DasdFormat>::names({
	"NONE", "LDL", "CDL"
    });

}
