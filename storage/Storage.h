#ifndef STORAGE_H
#define STORAGE_H


#include "storage/Environment.h"
#include "storage/DeviceGraph.h"


namespace storage
{

    class Storage
    {
    public:

	Storage(const Environment& environment);
	~Storage();

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

    public:

	class Impl;

	Impl& getImpl() { return *impl; }
	const Impl& getImpl() const { return *impl; }

    private:

	shared_ptr<Impl> impl;

    };

}

#endif
