#ifndef ENVIRONMENT_IMPL_H
#define ENVIRONMENT_IMPL_H


#include "storage/Environment.h"


namespace storage
{

    class Environment::Impl
    {
    public:

	Impl(bool read_only, ProbeMode probe_mode, TargetMode target_mode);
	~Impl();

	bool getReadOnly() const { return read_only; }

	ProbeMode getProbeMode() const { return probe_mode; }

	TargetMode getTargetMode() const { return target_mode; }

	const string& getDeviceGraphFilename() const { return device_graph_filename; }
	void setDeviceGraphFilename(const string& device_graph_filename);

    private:

	bool read_only;
	ProbeMode probe_mode;
	TargetMode target_mode;
	string device_graph_filename;

    };

}

#endif
