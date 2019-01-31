

#include <algorithm>

#include "testsuite/helpers/CallbacksRecorder.h"


namespace storage
{

    bool
    ProbeCallbacksRecorder::error(const string& message, const string& what) const
    {
	if (!what.empty())
	    messages.push_back(message + " (" + what + ")");
	else
	    messages.push_back(message);

	sort(messages.begin(), messages.end());

	return true;
    }


    void
    CheckCallbacksRecorder::error(const string& message) const
    {
	messages.push_back(message);

	sort(messages.begin(), messages.end());
    }

}
