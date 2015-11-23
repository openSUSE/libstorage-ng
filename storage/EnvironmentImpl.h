#ifndef STORAGE_ENVIRONMENT_IMPL_H
#define STORAGE_ENVIRONMENT_IMPL_H


#include "storage/Environment.h"
#include "storage/Utils/Enum.h"


namespace storage
{
    using std::string;


    class Environment::Impl
    {
    public:

	Impl(bool read_only, ProbeMode probe_mode, TargetMode target_mode);
	~Impl();

	bool get_read_only() const { return read_only; }

	ProbeMode get_probe_mode() const { return probe_mode; }

	TargetMode get_target_mode() const { return target_mode; }

	const string& get_devicegraph_filename() const { return devicegraph_filename; }
	void set_devicegraph_filename(const string& devicegraph_filename);

	const string& get_arch_filename() const { return arch_filename; }
	void set_arch_filename(const string& arch_filename);

	const string& get_mockup_filename() const { return mockup_filename; }
	void set_mockup_filename(const string& mockup_filename);

	friend std::ostream& operator<<(std::ostream& out, const Impl& environment);

    private:

	bool read_only;
	ProbeMode probe_mode;
	TargetMode target_mode;
	string devicegraph_filename;
	string arch_filename;
	string mockup_filename;

    };


    template <> struct EnumTraits<ProbeMode> { static const vector<string> names; };
    template <> struct EnumTraits<TargetMode> { static const vector<string> names; };

}

#endif
