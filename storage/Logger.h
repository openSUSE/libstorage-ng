#ifndef STORAGE_LOGGER_H
#define STORAGE_LOGGER_H


#include <string>


namespace storage
{

    /**
     * The Logger class.
     */
    class Logger
    {
    public:

	Logger() {}
	virtual ~Logger() {}

	/**
	 * Function to control whether a log line with level and component
	 * should be logged.
	 */
	virtual bool test(int level, const std::string& component);

	/**
	 * Function to log a line.
	 */
	virtual void write(int level, const std::string& component, const std::string& file,
			   int line, const std::string& function, const std::string& content) = 0;

    };


    Logger* get_logger();
    void set_logger(Logger* logger);


    Logger* get_stdout_logger();

}


#endif
