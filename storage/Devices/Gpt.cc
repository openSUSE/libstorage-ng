

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


    bool
    Gpt::get_enlarge() const
    {
	return get_impl().get_enlarge();
    }


    void
    Gpt::set_enlarge(bool enlarge)
    {
	get_impl().set_enlarge(enlarge);
    }


    bool
    is_gpt(const Device* device)
    {
	return is_device_of_type<const Gpt>(device);
    }


    Gpt*
    to_gpt(Device* device)
    {
	return to_device_of_type<Gpt>(device);
    }


    const Gpt*
    to_gpt(const Device* device)
    {
	return to_device_of_type<const Gpt>(device);
    }

}
