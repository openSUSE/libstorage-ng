

#include "storage/Devices/PartitionImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Partition*
    Partition::create(Devicegraph* devicegraph, const string& name)
    {
	Partition* ret = new Partition(new Partition::Impl(name));
	ret->Device::create(devicegraph);
	return ret;
    }


    Partition*
    Partition::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	Partition* ret = new Partition(new Partition::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    Partition::Partition(Impl* impl)
	: BlkDevice(impl)
    {
    }


    Partition*
    Partition::clone() const
    {
	return new Partition(get_impl().clone());
    }


    Partition::Impl&
    Partition::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Partition::Impl&
    Partition::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    unsigned int
    Partition::get_number() const
    {
	return get_impl().get_number();
    }


    const Region&
    Partition::get_region() const
    {
	return get_impl().get_region();
    }


    void
    Partition::set_region(const Region& region)
    {
	get_impl().set_region(region);
    }


    PartitionType
    Partition::get_type() const
    {
	return get_impl().get_type();
    }


    void
    Partition::set_type(PartitionType type)
    {
	get_impl().set_type(type);
    }


    unsigned
    Partition::get_id() const
    {
	return get_impl().get_id();
    }


    void
    Partition::set_id(unsigned id)
    {
	get_impl().set_id(id);
    }


    bool
    Partition::get_boot() const
    {
	return get_impl().get_boot();
    }


    void
    Partition::set_boot(bool boot)
    {
	get_impl().set_boot(boot);
    }


    const PartitionTable*
    Partition::get_partition_table() const
    {
	return get_impl().get_partition_table();
    }


    Partition*
    Partition::find(const Devicegraph* devicegraph, const string& name)
    {
	BlkDevice* blkdevice = BlkDevice::find(devicegraph, name);
	if (to_partition(blkdevice))
	    return to_partition(blkdevice);

	throw DeviceNotFound(sformat("device not found, name:%s", name.c_str()));
    }


    void
    Partition::print(std::ostream& out) const
    {
	BlkDevice::print(out);

	out << " region:" << get_region();
    }

}
