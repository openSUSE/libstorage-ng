

#include "storage/Devices/SwapImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Swap::Swap()
	: Filesystem(new Swap::Impl())
    {
    }


    Swap::Swap(Impl* impl)
	: Filesystem(impl)
    {
    }


    Swap*
    Swap::clone() const
    {
	return new Swap(getImpl().clone());
    }


    Swap::Impl&
    Swap::getImpl()
    {
	return dynamic_cast<Impl&>(Device::getImpl());
    }


    const Swap::Impl&
    Swap::getImpl() const
    {
	return dynamic_cast<const Impl&>(Device::getImpl());
    }

}
