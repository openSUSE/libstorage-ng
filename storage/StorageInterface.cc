

#include "storage/StorageInterface.h"
#include "storage/Utils/AppUtil.h"


namespace storage
{

    CallbackLogDo logger_do_fnc = storage_bgl::defaultLogDo;
    CallbackLogQuery logger_query_fnc = storage_bgl::defaultLogQuery;

    void setLogDoCallback(CallbackLogDo pfnc) { logger_do_fnc = pfnc; }
    CallbackLogDo getLogDoCallback() { return logger_do_fnc; }
    void setLogQueryCallback(CallbackLogQuery pfnc) { logger_query_fnc = pfnc; }
    CallbackLogQuery getLogQueryCallback() { return logger_query_fnc; }

}
