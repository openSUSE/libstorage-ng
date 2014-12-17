

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
    Environment::get_read_only() const
    {
	return get_impl().get_read_only();
    }


    ProbeMode
    Environment::get_probe_mode() const
    {
	return get_impl().get_probe_mode();
    }


    TargetMode
    Environment::get_target_mode() const
    {
	return get_impl().get_target_mode();
    }


    const string&
    Environment::get_devicegraph_filename() const
    {
	return get_impl().get_devicegraph_filename();
    }


    void
    Environment::set_devicegraph_filename(const string& devicegraph_filename)
    {
	get_impl().set_devicegraph_filename(devicegraph_filename);
    }

}
