#ifndef STORAGE_LOGGER_H
#define STORAGE_LOGGER_H


#include <string>


namespace storage
{


    enum LogLevel { DEBUG = 0, MILESTONE = 1, WARNING = 2, ERROR = 3 };


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
	virtual bool test(LogLevel log_level, const std::string& component);

	/**
	 * Function to log a line.
	 */
	virtual void write(LogLevel log_level, const std::string& component, const std::string& file,
			   int line, const std::string& function, const std::string& content) = 0;

    };


    Logger* get_logger();
    void set_logger(Logger* logger);


    Logger* get_stdout_logger();


    /**
     * Class to make some exceptions log-level DEBUG instead of WARNING.
     */
    class Silencer
    {

    public:

	Silencer();
	~Silencer();

	void turn_on();
	void turn_off();

	static bool is_any_active() { return count > 0; }

    private:

	bool active;

	static int count;

    };

}


#endif
