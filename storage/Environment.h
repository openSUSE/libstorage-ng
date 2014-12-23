#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H


#include <string>
#include <memory>


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

	bool get_read_only() const;

	ProbeMode get_probe_mode() const;

	TargetMode get_target_mode() const;

	const std::string& get_devicegraph_filename() const;
	void set_devicegraph_filename(const std::string& devicegraph_filename);

	friend std::ostream& operator<<(std::ostream& out, const Environment& environment);

    public:

        class Impl;

        Impl& get_impl() { return *impl; }
        const Impl& get_impl() const { return *impl; }

    private:

	std::shared_ptr<Impl> impl;

    };

}

#endif
