/*
 * Copyright (c) 2015 Novell, Inc.
 * Copyright (c) 2016 SUSE LLC
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
    public:

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


    class LogfileLogger : public Logger
    {
    public:

	virtual void write(LogLevel log_level, const std::string& component, const std::string& file,
			   int line, const std::string& function, const std::string& content) override;

    };


    void
    LogfileLogger::write(LogLevel log_level, const std::string& component, const std::string& file,
			 int line, const std::string& function, const std::string& content)
    {
	FILE* f = fopen("/var/log/libstorage-ng.log", "ae");
	if (f)
	{
	    fprintf(f, "%s <%d> [%s] %s(%s):%d %s\n", datetime(time(NULL), true, true).c_str(),
		    log_level, component.c_str(), file.c_str(), function.c_str(), line,
		    content.c_str());

	    fclose(f);
	}
    }


    Logger*
    get_logfile_logger()
    {
	static LogfileLogger logfile_logger;

	return &logfile_logger;
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
