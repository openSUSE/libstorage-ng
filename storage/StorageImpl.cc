

#include "config.h"
#include "storage/StorageImpl.h"
#include "storage/DevicegraphImpl.h"
#include "storage/Devices/DiskImpl.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Actiongraph.h"
#include "storage/Utils/AppUtil.h"


namespace storage
{

    Storage::Impl::Impl(const Environment& environment)
	: environment(environment)
    {
	y2mil("constructed Storage with " << environment);
	y2mil("libstorage version " VERSION);

	Devicegraph* probed = create_devicegraph("probed");

	switch (environment.get_probe_mode())
	{
	    case ProbeMode::PROBE_NORMAL: {
		probe(probed);
	    } break;

	    case ProbeMode::PROBE_NONE: {
	    } break;

	    case ProbeMode::PROBE_READ_DEVICE_GRAPH: {
		probed->load(environment.get_devicegraph_filename());
	    } break;

	    case ProbeMode::PROBE_READ_SYSTEM_INFO: {
		// TODO
	    } break;
	}

	y2mil("probed devicegraph begin");
	y2mil(*probed);
	y2mil("probed devicegraph end");

	copy_devicegraph("probed", "current");
    }


    Storage::Impl::~Impl()
    {
    }


    void
    Storage::Impl::probe(Devicegraph* probed)
    {
	SystemInfo systeminfo;

	// TODO

	for (const string& name : systeminfo.getDir(SYSFSDIR))
	{
	    if (!boost::starts_with(name, "sd"))
		continue;

	    Disk* disk = Disk::create(probed, "/dev/" + name);
	    disk->get_impl().probe(systeminfo);
	}
    }


    Devicegraph*
    Storage::Impl::get_devicegraph(const string& name)
    {
	if (name == "probed")
	    throw runtime_error("invalid name");

	map<string, Devicegraph>::iterator it = devicegraphs.find(name);
	if (it == devicegraphs.end())
	    throw runtime_error("device graph not found");

	return &it->second;
    }


    const Devicegraph*
    Storage::Impl::get_devicegraph(const string& name) const
    {
	map<string, Devicegraph>::const_iterator it = devicegraphs.find(name);
	if (it == devicegraphs.end())
	    throw runtime_error("device graph not found");

	return &it->second;
    }


    Devicegraph*
    Storage::Impl::get_current()
    {
	return get_devicegraph("current");
    }


    const Devicegraph*
    Storage::Impl::get_current() const
    {
	return get_devicegraph("current");
    }


    const Devicegraph*
    Storage::Impl::get_probed() const
    {
	return get_devicegraph("probed");
    }


    vector<string>
    Storage::Impl::get_devicegraph_names() const
    {
	vector<string> ret;

	for (const map<string, Devicegraph>::value_type& it : devicegraphs)
	    ret.push_back(it.first);

	return ret;
    }


    Devicegraph*
    Storage::Impl::create_devicegraph(const string& name)
    {
	pair<map<string, Devicegraph>::iterator, bool> tmp =
	    devicegraphs.emplace(piecewise_construct, forward_as_tuple(name),
				  forward_as_tuple());
	if (!tmp.second)
	    throw logic_error("device graph already exists");

	map<string, Devicegraph>::iterator it = tmp.first;

	return &it->second;
    }


    Devicegraph*
    Storage::Impl::copy_devicegraph(const string& source_name, const string& dest_name)
    {
	const Devicegraph* tmp1 = static_cast<const Impl*>(this)->get_devicegraph(source_name);

	Devicegraph* tmp2 = create_devicegraph(dest_name);

	tmp1->copy(*tmp2);

	return tmp2;
    }


    void
    Storage::Impl::remove_devicegraph(const string& name)
    {
	map<string, Devicegraph>::const_iterator it1 = devicegraphs.find(name);
	if (it1 == devicegraphs.end())
	    throw runtime_error("device graph not found");

	devicegraphs.erase(it1);
    }


    void
    Storage::Impl::restore_devicegraph(const string& name)
    {
	map<string, Devicegraph>::iterator it1 = devicegraphs.find(name);
	if (it1 == devicegraphs.end())
	    throw runtime_error("device graph not found");

	map<string, Devicegraph>::iterator it2 = devicegraphs.find("current");
	if (it2 == devicegraphs.end())
	    throw runtime_error("device graph not found");

	it1->second.get_impl().swap(it2->second.get_impl());
	devicegraphs.erase(it1);
    }


    bool
    Storage::Impl::exist_devicegraph(const string& name) const
    {
	return devicegraphs.find(name) != devicegraphs.end();
    }


    bool
    Storage::Impl::equal_devicegraph(const string& lhs, const string& rhs) const
    {
	const Devicegraph* tmp1 = static_cast<const Impl*>(this)->get_devicegraph(lhs);

	const Devicegraph* tmp2 = static_cast<const Impl*>(this)->get_devicegraph(rhs);

	// TODO really needed? just calculate Actiongraph instead? not always
	// same result, e.g. removing a Ext4 object (not mounted, not in
	// fstab) results in no action - but the graphs differ

	return true;
    }


    list<string>
    Storage::Impl::get_commit_steps() const
    {
	Actiongraph actiongraph(get_probed(), get_current());

	return actiongraph.get_commit_steps();
    }


    void
    Storage::Impl::commit()
    {
	Actiongraph actiongraph(get_probed(), get_current());

	actiongraph.commit();

	// TODO somehow update probed
    }

}
