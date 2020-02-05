/*
 * Copyright (c) [2004-2015] Novell, Inc.
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


#ifndef STORAGE_SYSTEM_CMD_H
#define STORAGE_SYSTEM_CMD_H


#include <sys/poll.h>
#include <stdio.h>

#include <string>
#include <vector>
#include <functional>
#include <boost/noncopyable.hpp>

#include "storage/Utils/Exception.h"


namespace storage
{
    using std::string;
    using std::vector;


    /**
     * Class to invoke a shell command and capture its exit value and output.
     */
    class SystemCmd : private boost::noncopyable
    {
    public:

	enum ThrowBehaviour { DoThrow, NoThrow };


	/**
	 * Class with detailed options for the SystemCmd class.
	 */
	struct Options
	{
	    Options(const string& command, ThrowBehaviour throw_behaviour = NoThrow)
		: command(command), throw_behaviour(throw_behaviour), stdin_text(),
		  mockup_key(), log_line_limit(1000),
		  verify([](int exit_code){ return exit_code == 0; }) {}

	    /**
	     * The command to be executed.
	     */
	    string command;

	    /**
	     * Should exceptions be thrown or not?
	     */
	    ThrowBehaviour throw_behaviour;

	    /**
	     * The stdin text to be sent via pipe to the command.
	     */
	    string stdin_text;

	    /**
	     * The key used for mockup. If empty the command is used as the key.
	     */
	    string mockup_key;

	    /**
	     * Limit for logged lines.
	     */
	    unsigned int log_line_limit;

	    /**
	     * If throw_behaviour is DoThrow this function is used
	     * additionally to verify if the command succeeded and if not an
	     * exception is thrown.
	     */
	    std::function<bool(int)> verify;

	};

	/**
	 * Constructor. The command is immediately executed with the options
	 * in the Options object. Use 'retcode()' to get the commands exit
	 * code and stdout() and stderr() to get the commands output.
	 */
	SystemCmd(const Options& options);

	/**
	 * Convenience constructor where only the command and the throw
	 * behaviour can be specified. Otherwise identical to 'SystemCmd(const
	 * Options& options)'.
	 */
	SystemCmd(const string& command, ThrowBehaviour throw_behaviour = NoThrow);

	/**
	 * Destructor.
	 */
	virtual ~SystemCmd();

    protected:

	/**
	 * Execute the specified command in the foreground and return its exit code.
	 **/
	int execute();

	/**
	 * Execute the specified command in the background.
	 * The return value is only meaningful if < 0 (fork() or exec() failed).
	 */
	int executeBackground();

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
	int executeRestricted(unsigned long maxTimeSec, unsigned long maxLineOut,
			      bool& timeExceeded_ret, bool& linesExceeded_ret);

    public:

	/**
	 * Return the output lines collected on stdout so far.
	 */
	const vector<string>& stdout() const { return _outputLines[IDX_STDOUT]; }

	/**
	 * Return the output lines collected on stderr so far.
	 */
	const vector<string>& stderr() const { return _outputLines[IDX_STDERR]; }

	/**
	 * Return the command executed.
	 */
	const string& command() const { return options.command; }

	/**
	 * Return the exit code of the command.
	 */
	int retcode() const { return _cmdRet; }

    public:

	/**
	 * Quotes and protects a single string for shell execution.
	 */
	static string quote(const string& str);

	/**
	 * Quotes and protects every single string in the vector for shell execution.
	 */
	static string quote(const vector<string>& strs);

    protected:

	enum OutputStream { IDX_STDOUT, IDX_STDERR };

	void init();
	void cleanup();
	void invalidate();
	void closeOpenFds() const;
	int doExecute();
	bool doWait(bool hang, int& cmdRet_ret);
	void checkOutput();
        void sendStdin();
	void getUntilEOF(FILE* file, std::vector<string>& lines,
			 bool& newLineSeen_ret, bool isStderr) const;
	void extractNewline(const string& buffer, int count, bool& newLineSeen_ret,
			    string& text, std::vector<string>& lines) const;
	void addLine(const string& text, std::vector<string>& lines) const;

	void logOutput() const;

	bool do_throw() const { return options.throw_behaviour == DoThrow; }

	const string& mockup_key() const
	    { return options.mockup_key.empty() ? options.command : options.mockup_key; }

	//
	// Data members
	//

	Options options;

	FILE* _files[2];
        FILE* _childStdin;
	std::vector<string> _outputLines[2];
	bool _newLineSeen[2];
	bool _combineOutput;
	bool _execInBackground;
	int _cmdRet;
	int _cmdPid;
	struct pollfd _pfds[3];

	/**
	 * Constructs the environment for the child process.
	 *
	 * Must not be called after exec since allocating the memory
	 * for the vector is not allowed then (in a multithreaded
	 * program), see fork(2) and signal-safety(7). So simply call
	 * it right before fork.
	 */
	vector<const char*> make_env() const;

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
		_cmd = sysCmd->command();
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
	const string& command() const { return _cmd; }

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
    class CommandNotFoundException : public SystemCmdException
    {
    public:
	CommandNotFoundException( const SystemCmd * sysCmd )
	    : SystemCmdException( sysCmd, "Command not found" )
	    {}

	virtual ~CommandNotFoundException() noexcept
	    {}
    };



    inline string
    quote(const string& str)
    {
	return SystemCmd::quote(str);
    }


    inline string
    quote(const vector<string>& strs)
    {
	return SystemCmd::quote(strs);
    }

}

#endif
