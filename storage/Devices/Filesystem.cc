

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
    Filesystem::findByMountPoint(const DeviceGraph* device_graph, const string& mount_point)
    {
	vector <Filesystem*> ret;

	for (DeviceGraph::Impl::vertex_descriptor v : device_graph->getImpl().vertices())
	{
	    Filesystem* filesystem = dynamic_cast<Filesystem*>(device_graph->getImpl().graph[v].get());
	    if (filesystem && contains(filesystem->getMountPoints(), mount_point))
		ret.push_back(filesystem);
	}

	return ret;
    }

}
