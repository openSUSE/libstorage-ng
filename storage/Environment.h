#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H


#include <string>
#include <memory>


namespace storage
{

    //! How to probe the system
    enum class ProbeMode {
	STANDARD,		// probe system during init
	STANDARD_WRITE_MOCKUP,	// probe system during init
	NONE,			// no probing - for testsuite
	READ_DEVICEGRAPH,	// fake probe - for testsuite
	READ_MOCKUP		// fake probe - for testsuite
    };

    //! Is the target a disk, chroot, or image?
    enum class TargetMode {
	DIRECT,			// direct target
	CHROOT,			// the target is chrooted, e.g. inst-sys
	IMAGE			// the target is image based
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

	const std::string& get_arch_filename() const;
	void set_arch_filename(const std::string& arch_filename);

	const std::string& get_mockup_filename() const;
	void set_mockup_filename(const std::string& mockup_filename);

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
