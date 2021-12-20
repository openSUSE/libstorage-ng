

#include <string>
#include <vector>

#include "storage/Storage.h"


namespace storage
{
    using namespace std;


    class ProbeCallbacksRecorder : public ProbeCallbacksV4
    {
    public:

	ProbeCallbacksRecorder(vector<string>& messages) : messages(messages) { messages.clear(); }

	virtual void begin() const override;

	virtual void end() const override;

	virtual void message(const std::string& message) const override {}

	virtual bool error(const string& message, const std::string& what) const override;

	virtual bool missing_command(const string& message, const string& what,
				     const string& command, uint64_t used_features) const override;

	virtual bool ambiguity_partition_table_and_filesystem(const string& message, const string& what,
							      const string& name, PtType pt_type,
							      FsType fs_type) const override;

	virtual bool ambiguity_partition_table_and_luks(const string& message, const string& what,
							const string& name, PtType pt_type) const override;

	virtual bool ambiguity_partition_table_and_lvm_pv(const string& message, const string& what,
							  const string& name, PtType pt_type) const override;

	virtual bool unsupported_partition_table(const string& message, const string& what, const string& name,
						 PtType pt_type) const override;

	virtual bool unsupported_filesystem(const string& message, const string& what, const string& name,
					    FsType fs_type) const override;

	vector<string>& messages;

    };


    class CheckCallbacksRecorder : public CheckCallbacks
    {
    public:

	CheckCallbacksRecorder(vector<string>& messages) : messages(messages) { messages.clear(); }

	virtual void error(const string& message) const override;

	vector<string>& messages;

    };

}
