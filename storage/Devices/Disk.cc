

#include "storage/Devices/DiskImpl.h"
#include "storage/Devices/Msdos.h"
#include "storage/Devices/Gpt.h"
#include "storage/Holders/User.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/Utils/Enum.h"


namespace storage
{

    using namespace std;


    Disk*
    Disk::create(Devicegraph* devicegraph, const string& name)
    {
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
	: BlkDevice(impl)
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


    PtType
    Disk::get_default_partition_table_type() const
    {
	return get_impl().get_default_partition_table_type();
    }


    vector<Disk*>
    Disk::get_all(Devicegraph* devicegraph)
    {
	auto pred = [](Device* device) {
	    return to_disk(device);
	};

	return devicegraph->get_impl().getDevicesIf<Disk>(pred);
    }


    PartitionTable*
    Disk::create_partition_table(PtType pt_type)
    {
	return get_impl().create_partition_table(pt_type);
    }


    PartitionTable*
    Disk::get_partition_table()
    {
	return get_impl().get_partition_table();
    }


    const PartitionTable*
    Disk::get_partition_table() const
    {
	return get_impl().get_partition_table();
    }


    bool
    is_disk(const Device* device)
    {
	return dynamic_cast<const Disk*>(device);
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
