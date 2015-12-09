/*
 * Copyright (c) [2004-2015] Novell, Inc.
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


#ifndef STORAGE_SYSTEM_CMD_H
#define STORAGE_SYSTEM_CMD_H


#include <sys/poll.h>
#include <stdio.h>

#include <string>
#include <vector>
#include <list>
#include <boost/noncopyable.hpp>

#include "storage/Utils/Exception.h"


namespace storage
{
    using std::string;
    using std::vector;

    class OutputProcessor;

    /**
     * Class to invoke a shell command and capture its output.
     * By default, stdout and stderr output are kept separate.
     * Use 'setCombine()' to merge them.
     */
    class SystemCmd : private boost::noncopyable
    {
    public:

	enum OutputStream { IDX_STDOUT, IDX_STDERR };
	enum ThrowBehaviour { DoThrow, NoThrow };

	/**
	 * Constructor. Invoke the specified command immediately in the foreground.
	 * If 'throwBehaviour' is 'DoThrow', this might throw exceptions.
	 * Use 'retcode()' to get the command's exit code.
	 */
	SystemCmd(const string& command, ThrowBehaviour throwBehaviour = NoThrow );

	/**
	 * Default constructor for the more advanced cases or where immediate
	 * execution is not desired.
	 */
	SystemCmd();

	/**
	 * Destructor.
	 */
	virtual ~SystemCmd();

	/**
	 * Execute the specified command in the foreground and return its exit code.
	 **/
	int execute(const string& command);

	/**
	 * Execute the specified command in the background.
	 * The return value is only meaningful if < 0 (fork() or exec() failed).
	 */
	int executeBackground(const string& command);

	/**
	 * Execute the specified command in the foreground with some restrictions:
	 *
	 * If 'maxTimeSec' is > 0, the command is killed (with SIGKILL) after
	 * 'maxTimeSec' seconds, and 'timeExceeded_ret' is set to 'true'.
	 *
	 * If 'maxLineOut' is > 0, the command is killed (with SIGKILL) if more
	 * than 'maxLineOut' lines have been collected in both stdout and
	 * stderr.
	 *
	 * This function returns the command's exit code if none of the limits
	 * was exceeded, and -257 otherwise.
	 */
	int executeRestricted(const string& command,
			      unsigned long maxTimeSec, unsigned long maxLineOut,
			      bool& timeExceeded_ret, bool& linesExceeded_ret);

	/**
	 * Set an OutputProcessor. This is mostly useful for visual feedback to
	 * the user (progress bar, busy animation). The OutputProcessor is
	 * called when an output line is read. See OutputProcessor.h.
	 */
	void setOutputProcessor(OutputProcessor* proc) { _outputProc = proc; }

	/**
	 * Return the output lines collected on stdout so far.
	 */
	const vector<string>& stdout() const { return _outputLines[IDX_STDOUT]; }

	/**
	 * Return the output lines collected on stderr so far.
	 */
	const vector<string>& stderr() const { return _outputLines[IDX_STDERR]; }

	/**
	 * Return the (last) command executed.
	 */
	string cmd() const { return _lastCmd; }

	/**
	 * Return the exit code of the command.
	 */
	int retcode() const { return _cmdRet; }

	/**
	 * Select which output lines to use in the specified output stream (stdout or stderr).
	 * 'pattern' is a fixed string with minimal regexp functionality:
	 * '^' start of line
	 * '$' end of line
	 * Returns how many lines matched.
	 */
	int select(const string& pattern, OutputStream streamIndex = IDX_STDOUT);

	/**
	 * Return the number of lines collected so far in the specified output
	 * stream (stdout or stderr). If 'selected' is 'true', return only the
	 * ones matched by the last 'select()' call.
	 */
	unsigned numLines(bool selected = false, OutputStream streamIndex = IDX_STDOUT) const;

	/**
	 * Return the output line number 'lineNo' from the specified output stream
	 * (stdout or stderr). If 'selected' is 'true', restrict this to the
	 * lines matched by the last 'select()' call.
	 */
	string getLine(unsigned lineNo, bool selected = false, OutputStream streamIndex = IDX_STDOUT) const;

	/**
	 * Combine stdout and stderr in output lines?
	 */
	void setCombine(bool combine = true);

	/**
	 * Set the throw behaviour: Should exceptions be thrown or not?
	 */
	void setThrowBehaviour( ThrowBehaviour val );

	/**
	 * Set test mode, i.e. don't actually invoke the command.
	 */
	static void setTestmode(bool testmode = true);

	/**
	 * Quotes and protects a single string for shell execution.
	 */
	static string quote(const string& str);

	/**
	 * Quotes and protects every single string in the list for shell execution.
	 */
	static string quote(const std::list<string>& strs);

    protected:

	void init();
	void cleanup();
	void invalidate();
	void closeOpenFds() const;
	int doExecute(const string& command);
	bool doWait(bool hang, int& cmdRet_ret);
	void checkOutput();
	void getUntilEOF(FILE* file, std::vector<string>& lines,
			 bool& newLineSeen_ret, bool isStderr) const;
	void extractNewline(const string& buffer, int count, bool& newLineSeen_ret,
			    string& text, std::vector<string>& lines) const;
	void addLine(const string& text, std::vector<string>& lines) const;

	void logOutput() const;

	//
	// Data members
	//

	FILE* _files[2];
	std::vector<string> _outputLines[2];
	std::vector<string*> _selectedOutputLines[2];
	bool _newLineSeen[2];
	bool _combineOutput;
	bool _execInBackground;
	string _lastCmd;
	int _cmdRet;
	int _cmdPid;
	bool _doThrow;
	OutputProcessor* _outputProc;
	struct pollfd _pfds[2];

	static bool _testmode;

	static const unsigned LINE_LIMIT = 50;
    };


    /**
     * Exception class for SystemCmd. This is used both to really throw
     * exceptions (if the 'DoThrow' behaviour was set for the SystemCmd) as
     * well as for just logging the problem (done in both cases, 'DoThrow' and
     * 'NoThrow'.
     */
    class SystemCmdException : public Exception
    {
    public:
	SystemCmdException( const SystemCmd * sysCmd, const string & msg )
	    : Exception( "" )
	{
	    _cmdRet = -1;

	    if ( sysCmd )
	    {
		// Copy relevant information from sysCmd because it might go
		// out of scope while this exception still exists.
		_cmd = sysCmd->cmd();
		setMsg( msg + ": \"" + _cmd + "\"" );
		_cmdRet = sysCmd->retcode();
		_stderr = sysCmd->stderr();
	    }
	    else
	    {
		setMsg( msg );
	    }
	}

	virtual ~SystemCmdException() noexcept
	    {}

	/**
	 * Return the command that was to be called with all arguments.
	 */
	const string & cmd() const { return _cmd; }

	/**
	 * Return the return code (the exit code) of the command. This might
	 * not always be meaningful, e.g. if fork() failed or if the command
	 * could not even be started.
	 */
	int cmdRet() const { return _cmdRet; }

	/**
	 * Return the stderr output of the command. If the command could not be
	 * started, this will be empty.
	 */
	const vector<string> & stderr() const { return _stderr; }

    protected:
	string _cmd;
	int    _cmdRet;
	vector<string> _stderr;
    };


    /**
     * Specialized exception class for the very common case "command not found".
     * This might legally happen while probing for storage features with a
     * command that might or might not be installed.
     *
     * The purpose of this class is to make this special case easily
     * distinguishable from other error cases (command installed, but failed
     * for some reason).
     **/
    class CommandNotFoundException: public SystemCmdException
    {
    public:
	CommandNotFoundException( const SystemCmd * sysCmd )
	    : SystemCmdException( sysCmd, "Command not found" )
	    {}

	virtual ~CommandNotFoundException() noexcept
	    {}
    };



    inline string quote(const string& str)
    {
	return SystemCmd::quote(str);
    }

    inline string quote(const std::list<string>& strs)
    {
	return SystemCmd::quote(strs);
    }

}

#endif
