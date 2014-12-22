

#include "storage/Devices/MsdosImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Msdos*
    Msdos::create(Devicegraph* devicegraph)
    {
	Msdos* ret = new Msdos(new Msdos::Impl());
	ret->Device::create(devicegraph);
	return ret;
    }


    Msdos*
    Msdos::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	Msdos* ret = new Msdos(new Msdos::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    Msdos::Msdos(Impl* impl)
	: PartitionTable(impl)
    {
    }


    Msdos*
    Msdos::clone() const
    {
	return new Msdos(get_impl().clone());
    }


    Msdos::Impl&
    Msdos::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Msdos::Impl&
    Msdos::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    void
    Msdos::print(std::ostream& out) const
    {
	PartitionTable::print(out);
    }

}
