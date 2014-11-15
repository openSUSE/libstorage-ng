

#include "storage/Devices/PartitionTableImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    PartitionTable::PartitionTable(Impl* impl)
	: Device(impl)
    {
    }


    PartitionTable::Impl&
    PartitionTable::getImpl()
    {
	return dynamic_cast<Impl&>(Device::getImpl());
    }


    const PartitionTable::Impl&
    PartitionTable::getImpl() const
    {
	return dynamic_cast<const Impl&>(Device::getImpl());
    }

}
