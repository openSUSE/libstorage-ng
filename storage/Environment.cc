

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


    const string&
    Environment::get_arch_filename() const
    {
	return get_impl().get_arch_filename();
    }


    void
    Environment::set_arch_filename(const string& arch_filename)
    {
	get_impl().set_arch_filename(arch_filename);
    }


    const string&
    Environment::get_mockup_filename() const
    {
	return get_impl().get_mockup_filename();
    }


    void
    Environment::set_mockup_filename(const string& mockup_filename)
    {
	get_impl().set_mockup_filename(mockup_filename);
    }


    std::ostream&
    operator<<(std::ostream& out, const Environment& environment)
    {
	return operator<<(out, environment.get_impl());
    }

}
