

#include "storage/Devices/LvmLvImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


using namespace std;


namespace storage
{

    LvmLv::LvmLv(const string& name)
	: BlkDevice(new LvmLv::Impl(name))
    {
    }


    LvmLv::LvmLv(Impl* impl)
	: BlkDevice(impl)
    {
    }


    LvmLv*
    LvmLv::clone() const
    {
	return new LvmLv(getImpl().clone());
    }


    LvmLv::Impl&
    LvmLv::getImpl()
    {
	return dynamic_cast<Impl&>(Device::getImpl());
    }


    const LvmLv::Impl&
    LvmLv::getImpl() const
    {
	return dynamic_cast<const Impl&>(Device::getImpl());
    }


    void
    LvmLv::check() const
    {
	if (getName().empty())
	    cerr << "logical volume has no name" << endl;
    }

}
