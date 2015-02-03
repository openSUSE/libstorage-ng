

#include "storage/Devices/BtrfsImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Btrfs*
    Btrfs::create(Devicegraph* devicegraph)
    {
	Btrfs* ret = new Btrfs(new Btrfs::Impl());
	ret->Device::create(devicegraph);
	return ret;
    }


    Btrfs*
    Btrfs::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	Btrfs* ret = new Btrfs(new Btrfs::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    Btrfs::Btrfs(Impl* impl)
	: Filesystem(impl)
    {
    }


    Btrfs*
    Btrfs::clone() const
    {
	return new Btrfs(get_impl().clone());
    }


    Btrfs::Impl&
    Btrfs::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Btrfs::Impl&
    Btrfs::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }

}
