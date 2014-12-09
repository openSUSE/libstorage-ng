

#include "storage/Devices/EncryptionImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


namespace storage_bgl
{

    using namespace std;


    Encryption*
    Encryption::create(Devicegraph* devicegraph, const string& name)
    {
	Encryption* ret = new Encryption(new Encryption::Impl(name));
	ret->Device::create(devicegraph);
	return ret;
    }


    Encryption*
    Encryption::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	Encryption* ret = new Encryption(new Encryption::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    Encryption::Encryption(Impl* impl)
	: BlkDevice(impl)
    {
    }


    Encryption*
    Encryption::clone() const
    {
	return new Encryption(get_impl().clone());
    }


    Encryption::Impl&
    Encryption::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Encryption::Impl&
    Encryption::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }

}
