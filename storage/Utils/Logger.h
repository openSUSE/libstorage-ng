/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2020] SUSE LLC
 *
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, contact Novell, Inc.
 *
 * To contact Novell about this file by physical or electronic mail, you may
 * find current contact information at www.novell.com.
 */


#ifndef STORAGE_LOGGER_H
#define STORAGE_LOGGER_H


#include <string>


namespace storage
{

    /**
     * Enum with log levels.
     */
    enum class LogLevel { DEBUG = 0, MILESTONE = 1, WARNING = 2, ERROR = 3 };


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


    /**
     * Get the current logger object.
     */
    Logger* get_logger();


    /**
     * Set the current logger object. The logger object must be valid until
     * replaced by another logger object.
     */
    void set_logger(Logger* logger);


    /**
     * Returns a Logger that logs to stdout/stderr. Do not use this function for
     * production code but only for examples and test-cases.
     */
    Logger* get_stdout_logger();


    /**
     * Returns a Logger that logs to the standard libstorage log file
     * ("/var/log/libstorage.log") or to a given file. Do not use this
     * function for production code but only for examples and test-cases.
     *
     * Note that this method only uses the given filename the first time
     * it is called.
     */
    Logger* get_logfile_logger(const std::string& filename = "/var/log/libstorage.log");


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
