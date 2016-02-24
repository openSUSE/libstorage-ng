#ifndef STORAGE_LOGGER_IMPL_H
#define STORAGE_LOGGER_IMPL_H


#include <sstream>

#include "storage/Utils/Logger.h"


namespace storage
{

    bool query_log_level(LogLevel log_level);

    std::ostringstream* open_log_stream();

    void close_log_stream(LogLevel log_level, const char* file, unsigned line,
			  const char* func, std::ostringstream*);

#define y2deb(op) y2log_op(storage::DEBUG, __FILE__, __LINE__, __FUNCTION__, op)
#define y2mil(op) y2log_op(storage::MILESTONE, __FILE__, __LINE__, __FUNCTION__, op)
#define y2war(op) y2log_op(storage::WARNING, __FILE__, __LINE__, __FUNCTION__, op)
#define y2err(op) y2log_op(storage::ERROR, __FILE__, __LINE__, __FUNCTION__, op)

#define y2log_op(log_level, file, line, func, op)				\
    do {									\
	if (storage::query_log_level(log_level))				\
	{									\
	    std::ostringstream* __buf = storage::open_log_stream();		\
	    *__buf << op;							\
	    storage::close_log_stream(log_level, file, line, func, __buf);	\
	}									\
    } while (0)

}


#endif
