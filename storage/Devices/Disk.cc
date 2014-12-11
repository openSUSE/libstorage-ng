

#include "storage/Devices/DiskImpl.h"
#include "storage/Devices/Msdos.h"
#include "storage/Devices/Gpt.h"
#include "storage/Holders/Using.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/Utils/Enum.h"


namespace storage_bgl
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
	return get_impl().create_partition_table(pt_type);
    }


    const vector<string> EnumInfo<DasdType>::names({
	"NONE", "ECKD", "FBA"
    });


    const vector<string> EnumInfo<DasdFormat>::names({
	"NONE", "LDL", "CDL"
    });

}
