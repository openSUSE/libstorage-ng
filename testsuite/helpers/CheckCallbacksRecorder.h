

#include <string>
#include <vector>

#include "storage/Storage.h"


namespace storage
{
    using namespace std;


    class CheckCallbacksRecorder : public CheckCallbacks
    {
    public:

	CheckCallbacksRecorder(vector<string>& messages) : messages(messages) { messages.clear(); }

	virtual void error(const string& message) const override;

	vector<string>& messages;

    };

}
