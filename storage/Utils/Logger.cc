/*
 * Copyright (c) 2015 Novell, Inc.
 * Copyright (c) [2016-2022] SUSE LLC
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


#include <stdio.h>
#include <fcntl.h>
/* Not technically required, but needed on some UNIX distributions */
#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>

#include "storage/Utils/Logger.h"
#include "storage/Utils/AppUtil.h"


namespace storage
{

    typedef typename std::underlying_type<LogLevel>::type log_level_underlying_type;


    static Logger* current_logger = nullptr;


    Logger*
    get_logger()
    {
	return current_logger;
    }


    void
    set_logger(Logger* logger)
    {
	current_logger = logger;
    }


    bool
    Logger::test(LogLevel log_level, const string& component)
    {
	return log_level != LogLevel::DEBUG;
    }


    class StdoutLogger : public Logger
    {

    public:

	virtual void write(LogLevel log_level, const string& component, const string& file,
			   int line, const string& function, const string& content) override;

    };


    void
    StdoutLogger::write(LogLevel log_level, const string& component, const string& file,
			int line, const string& function, const string& content)
    {
	std::cerr << datetime(time(nullptr)) << " <" << static_cast<log_level_underlying_type>(log_level)
		  << "> [" << component << "] " << file << "(" << function << "):" << line << " "
		  << content << std::endl;
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

	LogfileLogger(const string& filename, int permissions = DEFAULT_PERMISSIONS);

	virtual void write(LogLevel log_level, const string& component, const string& file,
			   int line, const string& function, const string& content) override;

    private:

	// log file should not be world-readable
	static const int DEFAULT_PERMISSIONS = 0640;

	const string filename;
	const int permissions;
    };



    LogfileLogger::LogfileLogger(const string& filename, int permissions)
	: filename(filename), permissions(permissions)
    {
    }


    void
    LogfileLogger::write(LogLevel log_level, const string& component, const string& file,
			 int line, const string& function, const string& content)
    {
	int fd = open(filename.c_str(), O_WRONLY | O_APPEND | O_CREAT | O_CLOEXEC, permissions);
	if (fd < 0)
	    return;

	FILE* f = fdopen(fd, "ae");
	if (!f)
	{
	    close(fd);
	    return;
	}

	fprintf(f, "%s <%d> [%s] %s(%s):%d %s\n", datetime(time(nullptr)).c_str(),
		static_cast<log_level_underlying_type>(log_level), component.c_str(),
		file.c_str(), function.c_str(), line, content.c_str());

	fclose(f);
    }


    Logger*
    get_logfile_logger(const string& filename)
    {
	static LogfileLogger logfile_logger(filename);

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
