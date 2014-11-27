

#include "storage/Storage.h"
#include "storage/StorageImpl.h"


namespace storage
{

    Storage::Storage(ProbeMode probe_mode, bool read_only)
	: impl(new Impl(probe_mode, read_only))
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


    void
    Storage::copyDeviceGraph(const string& source_name, const string& dest_name)
    {
	getImpl().copyDeviceGraph(source_name, dest_name);
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
    Storage::checkDeviceGraph(const string& name) const
    {
	return getImpl().checkDeviceGraph(name);
    }


    bool
    Storage::equalDeviceGraph(const string& lhs, const string& rhs) const
    {
	return getImpl().equalDeviceGraph(lhs, rhs);
    }

}
