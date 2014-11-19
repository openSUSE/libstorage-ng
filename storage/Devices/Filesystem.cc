

#include "storage/Devices/FilesystemImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Filesystem::Filesystem(Impl* impl)
	: Device(impl)
    {
    }


    Filesystem::Filesystem(DeviceGraph& device_graph, Impl* impl)
	: Device(device_graph, impl)
    {
    }


    Filesystem::Impl&
    Filesystem::getImpl()
    {
	return dynamic_cast<Impl&>(Device::getImpl());
    }


    const Filesystem::Impl&
    Filesystem::getImpl() const
    {
	return dynamic_cast<const Impl&>(Device::getImpl());
    }


    const string&
    Filesystem::getLabel() const
    {
	return getImpl().label;
    }


    void
    Filesystem::setLabel(const string& label)
    {
	getImpl().label = label;
    }


    const string&
    Filesystem::getUuid() const
    {
	return getImpl().uuid;
    }


    const vector<string>&
    Filesystem::getMountPoints() const
    {
	return getImpl().mount_points;
    }


    void
    Filesystem::addMountPoint(const string& mount_point)
    {
	return getImpl().mount_points.push_back(mount_point);
    }

}
