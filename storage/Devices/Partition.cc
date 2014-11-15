

#include "storage/Devices/PartitionImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Partition::Partition(const string& name)
	: BlkDevice(new Partition::Impl(name))
    {
    }


    Partition::Partition(Impl* impl)
	: BlkDevice(impl)
    {
    }


    Partition*
    Partition::clone() const
    {
	return new Partition(getImpl().clone());
    }


    Partition::Impl&
    Partition::getImpl()
    {
	return dynamic_cast<Impl&>(Device::getImpl());
    }


    const Partition::Impl&
    Partition::getImpl() const
    {
	return dynamic_cast<const Impl&>(Device::getImpl());
    }


    unsigned int
    Partition::getNumber() const
    {
	string::size_type pos = getName().find_last_not_of("0123456789");
	return atoi(getName().substr(pos + 1).c_str());
    }

}
