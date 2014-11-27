#ifndef STORAGE_IMPL_H
#define STORAGE_IMPL_H


#include "storage/Storage.h"


namespace storage
{

    class Storage::Impl
    {
    public:

	Impl(ProbeMode probe_mode, bool read_only);
	~Impl();

    public:

	void copyDeviceGraph(const string& source_name, const string& dest_name);
	void removeDeviceGraph(const string& name);
	void restoreDeviceGraph(const string& name);

	bool equalDeviceGraph(const string& lhs, const string& rhs) const;

	bool checkDeviceGraph(const string& name) const;
	vector<string> getDeviceGraphNames() const;

	DeviceGraph* getDeviceGraph(const string& name);
	const DeviceGraph* getDeviceGraph(const string& name) const;

	DeviceGraph* getCurrent();
	const DeviceGraph* getCurrent() const;

	const DeviceGraph* getProbed() const;

    private:

	void probe(DeviceGraph& probed);

	const ProbeMode probe_mode;
	const bool read_only;

	map<string, DeviceGraph> device_graphs;

    };

}

#endif
