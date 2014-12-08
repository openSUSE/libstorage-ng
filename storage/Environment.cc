

#include "storage/EnvironmentImpl.h"


namespace storage
{

    Environment::Environment(bool read_only)
	: Environment(read_only, ProbeMode::PROBE_NORMAL, TargetMode::TARGET_NORMAL)
    {
    }


    Environment::Environment(bool read_only, ProbeMode probe_mode, TargetMode target_mode)
	: impl(new Impl(read_only, probe_mode, target_mode))
    {
    }


    Environment::~Environment()
    {
    }


    bool
    Environment::getReadOnly() const
    {
	return getImpl().getReadOnly();
    }


    ProbeMode
    Environment::getProbeMode() const
    {
	return getImpl().getProbeMode();
    }


    TargetMode
    Environment::getTargetMode() const
    {
	return getImpl().getTargetMode();
    }


    const string&
    Environment::getDeviceGraphFilename() const
    {
	return getImpl().getDeviceGraphFilename();
    }


    void
    Environment::setDeviceGraphFilename(const string& device_graph_filename)
    {
	getImpl().setDeviceGraphFilename(device_graph_filename);
    }

}
