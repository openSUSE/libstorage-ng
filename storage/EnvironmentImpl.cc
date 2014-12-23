

#include <ostream>

#include "storage/EnvironmentImpl.h"


namespace storage
{

    Environment::Impl::Impl(bool read_only, ProbeMode probe_mode, TargetMode target_mode)
	: read_only(read_only), probe_mode(probe_mode), target_mode(target_mode)
    {
    }


    Environment::Impl::~Impl()
    {
    }


    void
    Environment::Impl::set_devicegraph_filename(const string& devicegraph_filename)
    {
	Impl::devicegraph_filename = devicegraph_filename;
    }


    std::ostream&
    operator<<(std::ostream& out, const Environment::Impl& environment)
    {
	return out << "read-only:" << environment.read_only << " probe-mode:"
		   << toString(environment.probe_mode) << " target-mode:"
		   << toString(environment.target_mode);
    }


    const vector<string> EnumInfo<ProbeMode>::names({
        "NORMAL", "NONE", "READ_DEVICE_GRAPH", "READ_SYSTEM_INFO"
    });


    const vector<string> EnumInfo<TargetMode>::names({
        "NORMAL", "CHROOT", "IMAGE"
    });

}
