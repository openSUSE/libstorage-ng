

#include "storage/StorageImpl.h"
#include "storage/DeviceGraphImpl.h"
#include "storage/Devices/Disk.h"


namespace storage
{

    Storage::Impl::Impl(ProbeMode probe_mode, bool read_only)
	: probe_mode(probe_mode), read_only(read_only)
    {
	switch (probe_mode)
	{
	    case ProbeMode::PROBE_NONE: {

		pair<map<string, DeviceGraph>::iterator, bool> tmp =
		    device_graphs.emplace(piecewise_construct, forward_as_tuple("current"),
					  forward_as_tuple());
		if (!tmp.second)
		    throw logic_error("current device graph already exists");

	    } break;

	    case ProbeMode::PROBE_NORMAL: {

		pair<map<string, DeviceGraph>::iterator, bool> tmp =
		    device_graphs.emplace(piecewise_construct, forward_as_tuple("probed"),
				    forward_as_tuple());
		if (!tmp.second)
		    throw logic_error("probed device graph already exists");

		probe(tmp.first->second);

		copyDeviceGraph("probed", "current");

	    } break;

	    case ProbeMode::PROBE_FAKE: {

		// read();

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


    void
    Storage::Impl::copyDeviceGraph(const string& source_name, const string& dest_name)
    {
	map<string, DeviceGraph>::const_iterator it1 = device_graphs.find(source_name);
	if (it1 == device_graphs.end())
	    throw runtime_error("device graph not found");

	pair<map<string, DeviceGraph>::iterator, bool> tmp =
	    device_graphs.emplace(piecewise_construct, forward_as_tuple(dest_name),
			    forward_as_tuple());
	if (!tmp.second)
	    throw runtime_error("device graph already exists");
	map<string, DeviceGraph>::iterator it2 = tmp.first;

	it1->second.copy(it2->second);
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
    Storage::Impl::checkDeviceGraph(const string& name) const
    {
	return device_graphs.find(name) != device_graphs.end();
    }


    bool
    Storage::Impl::equalDeviceGraph(const string& lhs, const string& rhs) const
    {
	map<string, DeviceGraph>::const_iterator it1 = device_graphs.find(lhs);
	if (it1 == device_graphs.end())
	    throw runtime_error("device graph not found");

	map<string, DeviceGraph>::const_iterator it2 = device_graphs.find(rhs);
	if (it2 == device_graphs.end())
	    throw runtime_error("device graph not fot found");

	// TODO
    }

}
