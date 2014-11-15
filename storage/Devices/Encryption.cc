

#include "storage/Devices/EncryptionImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Encryption::Encryption(const string& name)
	: BlkDevice(new Encryption::Impl(name))
    {
    }


    Encryption::Encryption(Impl* impl)
	: BlkDevice(impl)
    {
    }


    Encryption*
    Encryption::clone() const
    {
	return new Encryption(getImpl().clone());
    }


    Encryption::Impl&
    Encryption::getImpl()
    {
	return dynamic_cast<Impl&>(Device::getImpl());
    }


    const Encryption::Impl&
    Encryption::getImpl() const
    {
	return dynamic_cast<const Impl&>(Device::getImpl());
    }

}
