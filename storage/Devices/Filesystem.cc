

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


    template <typename ListType, typename Type>
    bool contains(const ListType& l, const Type& value)
    {
	return find(l.begin(), l.end(), value) != l.end();
    }


    vector<Filesystem*>
    Filesystem::findByLabel(const DeviceGraph* device_graph, const string& label)
    {
	auto pred = [&label](const Filesystem* filesystem) {
	    return filesystem->getLabel() == label;
	};

	return device_graph->getImpl().getDevicesIf<Filesystem>(pred);
    }


    vector<Filesystem*>
    Filesystem::findByMountPoint(const DeviceGraph* device_graph, const string& mount_point)
    {
	auto pred = [&mount_point](const Filesystem* filesystem) {
	    return contains(filesystem->getMountPoints(), mount_point);
	};

	return device_graph->getImpl().getDevicesIf<Filesystem>(pred);
    }

}
