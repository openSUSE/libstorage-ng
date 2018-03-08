

#include <algorithm>

#include "testsuite/helpers/CheckCallbacksRecorder.h"


namespace storage
{

    void
    CheckCallbacksRecorder::error(const string& message) const
    {
	messages.push_back(message);

	sort(messages.begin(), messages.end());
    }

}
