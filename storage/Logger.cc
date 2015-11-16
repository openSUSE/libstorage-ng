

#include <iostream>

#include "storage/Logger.h"
#include "storage/Utils/AppUtil.h"


namespace storage
{

    Logger* my_logger = nullptr;


    Logger*
    get_logger()
    {
	return my_logger;
    }


    void
    set_logger(Logger* logger)
    {
	my_logger = logger;
    }


    bool
    Logger::test(int level)
    {
	return true;
    }


    class StdoutLogger : public Logger
    {
	virtual void write(int level, const std::string& component, const std::string& file,
			   int line, const std::string& function, const std::string& content);
    };


    void
    StdoutLogger::write(int level, const std::string& component, const std::string& file,
			int line, const std::string& function, const std::string& content)
    {
	std::cout << datetime(time(NULL), true, true) << " <" << level << "> [" << component << "] "
		  << file << "(" << function << "):" << line << " " << content << std::endl;
    }


    Logger*
    get_stdout_logger()
    {
	static StdoutLogger stdout_logger;

	return &stdout_logger;
    }

}
