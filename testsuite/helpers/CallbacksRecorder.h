

#include <string>
#include <vector>

#include "storage/Storage.h"


namespace storage
{
    using namespace std;


    class ProbeCallbacksRecorder : public ProbeCallbacks
    {
    public:

	ProbeCallbacksRecorder(vector<string>& messages) : messages(messages) { messages.clear(); }

	virtual void message(const std::string& message) const override {}

	virtual bool error(const string& message, const std::string& what) const override;

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
