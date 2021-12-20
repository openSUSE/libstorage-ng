

#include <algorithm>

#include "testsuite/helpers/CallbacksRecorder.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Filesystems/Mountable.h"


namespace storage
{

    void
    ProbeCallbacksRecorder::begin() const
    {
	messages.push_back("begin:");
    }


    void
    ProbeCallbacksRecorder::end() const
    {
	messages.push_back("end:");
    }


    bool
    ProbeCallbacksRecorder::error(const string& message, const string& what) const
    {
	messages.push_back("error: message = '" + message + "', what = '" + what + "'");

	return true;
    }


    bool
    ProbeCallbacksRecorder::missing_command(const string& message, const string& what,
					    const string& command, uint64_t used_features) const
    {
	messages.push_back("missing-command: message = '" + message + "', what = '" + what + "', "
			   "command = '" + command + "', used-features = " + to_string(used_features));

	return true;
    }


    bool
    ProbeCallbacksRecorder::ambiguity_partition_table_and_filesystem(const string& message, const string& what,
								     const string& name, PtType pt_type,
								     FsType fs_type) const
    {
	messages.push_back("ambiguity-partition-table-and-filesystem: message = '" + message + "', what = '" + what +
			   "', name = '" + name + "', pt-type = " + get_pt_type_name(pt_type) + ", fs-type = " +
			   get_fs_type_name(fs_type));

	return true;
    }


    bool
    ProbeCallbacksRecorder::ambiguity_partition_table_and_luks(const string& message, const string& what,
							       const string& name, PtType pt_type) const
    {
	messages.push_back("ambiguity-partition-table-and-luks: message = '" + message + "', what = '" + what +
			   "', name = '" + name + "', pt-type = " + get_pt_type_name(pt_type));

	return true;

    }


    bool
    ProbeCallbacksRecorder::ambiguity_partition_table_and_lvm_pv(const string& message, const string& what,
								 const string& name, PtType pt_type) const
    {
	messages.push_back("ambiguity-partition-table-and-lvm-pv: message = '" + message + "', what = '" + what +
			   "', name = '" + name + "', pt-type = " + get_pt_type_name(pt_type));

	return true;
    }


    bool
    ProbeCallbacksRecorder::unsupported_partition_table(const string& message, const string& what, const string& name,
							PtType pt_type) const
    {
	messages.push_back("unsupported-partition-table: message = '" + message + "', what = '" + what + "', "
			   "name = '" + name + "', pt-type = " + get_pt_type_name(pt_type));

	return true;
    }


    bool
    ProbeCallbacksRecorder::unsupported_filesystem(const string& message, const string& what, const string& name,
						   FsType fs_type) const
    {
	messages.push_back("unsupported-filesystem: message = '" + message + "', what = '" + what + "', "
			   "name = '" + name + "', fs-type = " + get_fs_type_name(fs_type));

	return true;
    }


    void
    CheckCallbacksRecorder::error(const string& message) const
    {
	messages.push_back("error: message = '" + message + "'");

	sort(messages.begin(), messages.end());
    }

}
