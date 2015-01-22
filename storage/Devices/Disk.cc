

#include "storage/Devices/DiskImpl.h"
#include "storage/Devices/Msdos.h"
#include "storage/Devices/Gpt.h"
#include "storage/Holders/Using.h"
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
    Disk::find(const Devicegraph* devicegraph, const string& name)
    {
	BlkDevice* blkdevice = BlkDevice::find(devicegraph, name);
	if (to_disk(blkdevice))
	    return to_disk(blkdevice);

	throw DeviceNotFound(sformat("device not found, name:%s", name.c_str()));
    }


    bool
    Disk::get_rotational() const
    {
	return get_impl().get_rotational();
    }


    void
    Disk::set_rotational(bool rotational)
    {
	get_impl().set_rotational(rotational);
    }


    Transport
    Disk::get_transport() const
    {
	return get_impl().get_transport();
    }


    void
    Disk::set_transport(Transport transport)
    {
	get_impl().set_transport(transport);
    }


    vector<Disk*>
    Disk::get_all(const Devicegraph* devicegraph)
    {
	auto pred = [](const Device* device) {
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


    const vector<string> EnumInfo<DasdType>::names({
	"NONE", "ECKD", "FBA"
    });


    const vector<string> EnumInfo<DasdFormat>::names({
	"NONE", "LDL", "CDL"
    });

}
