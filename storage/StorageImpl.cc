

#include "storage/StorageImpl.h"
#include "storage/DeviceGraphImpl.h"
#include "storage/Devices/Disk.h"


namespace storage
{

    Storage::Impl::Impl(const Environment& environment)
	: environment(environment)
    {
	switch (environment.probe_mode)
	{
	    case ProbeMode::PROBE_NORMAL: {

		DeviceGraph* probed = createDeviceGraph("probed");

		probe(*probed);

		copyDeviceGraph("probed", "current");

	    } break;

	    case ProbeMode::PROBE_NONE: {

		createDeviceGraph("current");

	    } break;

	    case ProbeMode::PROBE_READ_DEVICE_GRAPH: {

		createDeviceGraph("current");

		// TODO

	    } break;

	    case ProbeMode::PROBE_READ_SYSTEM_INFO: {

		createDeviceGraph("current");

		// TODO

	    } break;

	}
    }


    Storage::Impl::~Impl()
    {
    }


    void
    Storage::Impl::probe(DeviceGraph& probed)
    {
	// TODO

	Disk::create(probed, "/dev/sda");
    }


    DeviceGraph*
    Storage::Impl::getDeviceGraph(const string& name)
    {
	if (name == "probed")
	    throw runtime_error("invalid name");

	map<string, DeviceGraph>::iterator it = device_graphs.find(name);
	if (it == device_graphs.end())
	    throw runtime_error("device graph not found");

	return &it->second;
    }


    const DeviceGraph*
    Storage::Impl::getDeviceGraph(const string& name) const
    {
	map<string, DeviceGraph>::const_iterator it = device_graphs.find(name);
	if (it == device_graphs.end())
	    throw runtime_error("device graph not found");

	return &it->second;
    }


    DeviceGraph*
    Storage::Impl::getCurrent()
    {
	return getDeviceGraph("current");
    }


    const DeviceGraph*
    Storage::Impl::getCurrent() const
    {
	return getDeviceGraph("current");
    }


    const DeviceGraph*
    Storage::Impl::getProbed() const
    {
	return getDeviceGraph("probed");
    }


    vector<string>
    Storage::Impl::getDeviceGraphNames() const
    {
	vector<string> ret;

	for (const map<string, DeviceGraph>::value_type& it : device_graphs)
	    ret.push_back(it.first);

	return ret;
    }


    DeviceGraph*
    Storage::Impl::createDeviceGraph(const string& name)
    {
	pair<map<string, DeviceGraph>::iterator, bool> tmp =
	    device_graphs.emplace(piecewise_construct, forward_as_tuple(name),
				  forward_as_tuple());
	if (!tmp.second)
	    throw logic_error("device graph already exists");

	map<string, DeviceGraph>::iterator it = tmp.first;

	return &it->second;
    }


    DeviceGraph*
    Storage::Impl::copyDeviceGraph(const string& source_name, const string& dest_name)
    {
	const DeviceGraph* tmp1 = static_cast<const Impl*>(this)->getDeviceGraph(source_name);

	DeviceGraph* tmp2 = createDeviceGraph(dest_name);

	tmp1->copy(*tmp2);

	return tmp2;
    }


    void
    Storage::Impl::removeDeviceGraph(const string& name)
    {
	map<string, DeviceGraph>::const_iterator it1 = device_graphs.find(name);
	if (it1 == device_graphs.end())
	    throw runtime_error("device graph not found");

	device_graphs.erase(it1);
    }


    void
    Storage::Impl::restoreDeviceGraph(const string& name)
    {
	map<string, DeviceGraph>::iterator it1 = device_graphs.find(name);
	if (it1 == device_graphs.end())
	    throw runtime_error("device graph not found");

	map<string, DeviceGraph>::iterator it2 = device_graphs.find("current");
	if (it2 == device_graphs.end())
	    throw runtime_error("device graph not found");

	it1->second.getImpl().swap(it2->second.getImpl());
	device_graphs.erase(it1);
    }


    bool
    Storage::Impl::existDeviceGraph(const string& name) const
    {
	return device_graphs.find(name) != device_graphs.end();
    }


    bool
    Storage::Impl::equalDeviceGraph(const string& lhs, const string& rhs) const
    {
	const DeviceGraph* tmp1 = static_cast<const Impl*>(this)->getDeviceGraph(lhs);

	const DeviceGraph* tmp2 = static_cast<const Impl*>(this)->getDeviceGraph(rhs);

	// TODO really needed? just calculate ActionGraph instead? not always
	// same result, e.g. removing a Ext4 object (not mounted, not in
	// fstab) results in no action - but the graphs differ
    }

}
