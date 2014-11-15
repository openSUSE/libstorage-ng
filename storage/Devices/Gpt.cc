

#include "storage/Devices/GptImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Gpt::Gpt()
	: PartitionTable(new Gpt::Impl())
    {
    }


    Gpt::Gpt(Impl* impl)
	: PartitionTable(impl)
    {
    }


    Gpt*
    Gpt::clone() const
    {
	return new Gpt(getImpl().clone());
    }


    Gpt::Impl&
    Gpt::getImpl()
    {
	return dynamic_cast<Impl&>(Device::getImpl());
    }


    const Gpt::Impl&
    Gpt::getImpl() const
    {
	return dynamic_cast<const Impl&>(Device::getImpl());
    }

}
