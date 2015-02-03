

#include "storage/Devices/XfsImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Xfs*
    Xfs::create(Devicegraph* devicegraph)
    {
	Xfs* ret = new Xfs(new Xfs::Impl());
	ret->Device::create(devicegraph);
	return ret;
    }


    Xfs*
    Xfs::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	Xfs* ret = new Xfs(new Xfs::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    Xfs::Xfs(Impl* impl)
	: Filesystem(impl)
    {
    }


    Xfs*
    Xfs::clone() const
    {
	return new Xfs(get_impl().clone());
    }


    Xfs::Impl&
    Xfs::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Xfs::Impl&
    Xfs::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }

}
