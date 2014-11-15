

#include "storage/Devices/DiskImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Disk::Disk(const string& name)
	: BlkDevice(new Disk::Impl(name))
    {
    }


    Disk::Disk(Impl* impl)
	: BlkDevice(impl)
    {
    }


    Disk*
    Disk::clone() const
    {
	return new Disk(getImpl().clone());
    }


    Disk::Impl&
    Disk::getImpl()
    {
	return dynamic_cast<Impl&>(Device::getImpl());
    }


    const Disk::Impl&
    Disk::getImpl() const
    {
	return dynamic_cast<const Impl&>(Device::getImpl());
    }

}
