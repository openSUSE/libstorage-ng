

#include "storage/Devices/GptImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Gpt*
    Gpt::create(Devicegraph* devicegraph)
    {
	Gpt* ret = new Gpt(new Gpt::Impl());
	ret->Device::create(devicegraph);
	return ret;
    }


    Gpt*
    Gpt::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	Gpt* ret = new Gpt(new Gpt::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    Gpt::Gpt(Impl* impl)
	: PartitionTable(impl)
    {
    }


    Gpt*
    Gpt::clone() const
    {
	return new Gpt(get_impl().clone());
    }


    Gpt::Impl&
    Gpt::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Gpt::Impl&
    Gpt::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }

}
