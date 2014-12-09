

#include "storage/Devices/DiskImpl.h"
#include "storage/Devices/Gpt.h"
#include "storage/Holders/Using.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


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


    PartitionTable*
    Disk::create_partition_table(PtType pt_type)
    {
	if (num_children() != 0)
	    throw runtime_error("has children");

	PartitionTable* ret = Gpt::create(get_impl().get_devicegraph());
	Using::create(get_impl().get_devicegraph(), this, ret);
	return ret;
    }

}
