

#include <iostream>

#include "storage/Utils/Logger.h"
#include "storage/Utils/AppUtil.h"


namespace storage
{

    static Logger* my_logger = nullptr;


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
    Logger::test(LogLevel log_level, const std::string& component)
    {
	return true;
    }


    class StdoutLogger : public Logger
    {
	virtual void write(LogLevel log_level, const std::string& component, const std::string& file,
			   int line, const std::string& function, const std::string& content) override;
    };


    void
    StdoutLogger::write(LogLevel log_level, const std::string& component, const std::string& file,
			int line, const std::string& function, const std::string& content)
    {
	std::cout << datetime(time(NULL), true, true) << " <" << log_level << "> [" << component << "] "
		  << file << "(" << function << "):" << line << " " << content << std::endl;
    }


    Logger*
    get_stdout_logger()
    {
	static StdoutLogger stdout_logger;

	return &stdout_logger;
    }


    Silencer::Silencer()
	: active(false)
    {
	turn_on();
    }


    Silencer::~Silencer()
    {
	turn_off();
    }


    void
    Silencer::turn_on()
    {
	if (active)
	    return;

	active = true;
	++count;
    }


    void
    Silencer::turn_off()
    {
	if (!active)
	    return;

	active = false;
	--count;
    }


    int Silencer::count = 0;

}
