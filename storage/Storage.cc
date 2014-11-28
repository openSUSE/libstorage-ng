

#include "storage/Storage.h"
#include "storage/StorageImpl.h"


namespace storage
{

    Storage::Storage(const Environment& environment)
	: impl(new Impl(environment))
    {
    }


    Storage::~Storage()
    {
    }


    DeviceGraph*
    Storage::getDeviceGraph(const string& name)
    {
	return getImpl().getDeviceGraph(name);
    }


    const DeviceGraph*
    Storage::getDeviceGraph(const string& name) const
    {
	return getImpl().getDeviceGraph(name);
    }


    DeviceGraph*
    Storage::getCurrent()
    {
	return getImpl().getCurrent();
    }


    const DeviceGraph*
    Storage::getCurrent() const
    {
	return getImpl().getCurrent();
    }


    const DeviceGraph*
    Storage::getProbed() const
    {
	return getImpl().getProbed();
    }


    vector<string>
    Storage::getDeviceGraphNames() const
    {
	return getImpl().getDeviceGraphNames();
    }

    DeviceGraph*
    Storage::createDeviceGraph(const string& name)
    {
	return getImpl().createDeviceGraph(name);
    }


    DeviceGraph*
    Storage::copyDeviceGraph(const string& source_name, const string& dest_name)
    {
	return getImpl().copyDeviceGraph(source_name, dest_name);
    }


    void
    Storage::removeDeviceGraph(const string& name)
    {
	getImpl().removeDeviceGraph(name);
    }


    void
    Storage::restoreDeviceGraph(const string& name)
    {
	getImpl().restoreDeviceGraph(name);
    }


    bool
    Storage::existDeviceGraph(const string& name) const
    {
	return getImpl().existDeviceGraph(name);
    }


    bool
    Storage::equalDeviceGraph(const string& lhs, const string& rhs) const
    {
	return getImpl().equalDeviceGraph(lhs, rhs);
    }

}
