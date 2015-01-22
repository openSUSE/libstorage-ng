

#include "storage/Utils/Remote.h"


namespace storage
{

    static const RemoteCallbacks* remote_callbacks = nullptr;


    const RemoteCallbacks*
    get_remote_callbacks()
    {
	return remote_callbacks;
    }


    void
    set_remote_callbacks(const RemoteCallbacks* remote_callbacks)
    {
	storage::remote_callbacks = remote_callbacks;
    }

}
