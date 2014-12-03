#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H


namespace storage
{

    enum class ProbeMode {
	PROBE_NORMAL,			// probe system during init
	PROBE_NONE,			// no probing - for testsuite
	PROBE_READ_DEVICE_GRAPH,	// fake probe - for testsuite
	PROBE_READ_SYSTEM_INFO		// fake probe - for testsuite
    };


    enum class TargetMode {
	TARGET_NORMAL,		// normal target
	TARGET_CHROOT,		// the target is chrooted, e.g. inst-sys
	TARGET_IMAGE		// the target is image based
    };


    class Environment
    {
    public:

	Environment(bool read_only);
	Environment(bool read_only, ProbeMode probe_mode, TargetMode target_mode);
	~Environment();

	bool read_only;
	ProbeMode probe_mode;
	TargetMode target_mode;

    };

}

#endif
