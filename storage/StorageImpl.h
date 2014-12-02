#ifndef STORAGE_IMPL_H
#define STORAGE_IMPL_H


#include "storage/Storage.h"


namespace storage
{

    class Storage::Impl
    {
    public:

	Impl(const Environment& environment);
	~Impl();

    public:

	DeviceGraph* createDeviceGraph(const string& name);
	DeviceGraph* copyDeviceGraph(const string& source_name, const string& dest_name);
	void removeDeviceGraph(const string& name);
	void restoreDeviceGraph(const string& name);

	bool equalDeviceGraph(const string& lhs, const string& rhs) const;

	bool existDeviceGraph(const string& name) const;
	vector<string> getDeviceGraphNames() const;

	DeviceGraph* getDeviceGraph(const string& name);
	const DeviceGraph* getDeviceGraph(const string& name) const;

	DeviceGraph* getCurrent();
	const DeviceGraph* getCurrent() const;

	const DeviceGraph* getProbed() const;

    private:

	void probe(DeviceGraph* probed);

	const Environment environment;

	map<string, DeviceGraph> device_graphs;

    };

}

#endif
