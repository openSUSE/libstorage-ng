

#include "storage/Environment.h"


namespace storage
{

    Environment::Environment(bool read_only)
	: Environment(read_only, ProbeMode::PROBE_NORMAL, TargetMode::TARGET_NORMAL)
    {
    }


    Environment::Environment(bool read_only, ProbeMode probe_mode, TargetMode target_mode)
	: read_only(read_only), probe_mode(probe_mode), target_mode(target_mode)
    {
    }


    Environment::~Environment()
    {
    }

}
