#ifndef STORAGE_H
#define STORAGE_H


#include "storage/DeviceGraph.h"


namespace storage
{

    enum class ProbeMode {
	PROBE_NONE,		// no probing
	PROBE_NORMAL,		// probe system during init
	PROBE_FAKE		// fake probe - read XML instead
    };


    // env: DIRECT, INSTALLATION, IMAGE


    class Storage
    {
    public:

	Storage(ProbeMode probe_mode, bool read_only);
	~Storage();

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

    public:

	class Impl;

	Impl& getImpl() { return *impl; }
	const Impl& getImpl() const { return *impl; }

    private:

	shared_ptr<Impl> impl;

    };

}

#endif
