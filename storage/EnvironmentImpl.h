#ifndef ENVIRONMENT_IMPL_H
#define ENVIRONMENT_IMPL_H


#include "storage/Environment.h"


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

    private:

	bool read_only;
	ProbeMode probe_mode;
	TargetMode target_mode;
	string devicegraph_filename;

    };

}

#endif
