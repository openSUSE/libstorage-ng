#ifndef STORAGE_LOGGER_H
#define STORAGE_LOGGER_H


#include <string>


namespace storage
{

    class Logger
    {
    public:

	virtual ~Logger() {}

	virtual bool test(int level);

	virtual void write(int level, const std::string& component, const std::string& file,
			   int line, const std::string& function, const std::string& content) = 0;
    };


    Logger* get_logger();
    void set_logger(Logger* logger);


    Logger* get_stdout_logger();

}


#endif
