/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) [2016-2023] SUSE LLC
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


#include <string>
#include <vector>
#include <functional>
#include <initializer_list>
#include <boost/noncopyable.hpp>

#include "storage/Utils/Exception.h"


namespace storage
{
    using std::string;
    using std::vector;


    /**
     * Class to invoke a shell command and capture its exit value and output.
     */
    class SystemCmd final : private boost::noncopyable
    {
    public:

	/**
	 * Class holding command with arguments.
	 */
	class Args
	{
	public:

	    Args(std::initializer_list<string> init)
		: values(init) {}

	    const vector<string>& get_values() const { return values; }

	    Args& operator<<(const char* arg) { values.push_back(arg); return *this; }
	    Args& operator<<(const string& arg) { values.push_back(arg); return *this; }

	private:

	    vector<string> values;

	};


	enum ThrowBehaviour { DoThrow, NoThrow };


	/**
	 * Class with detailed options for the SystemCmd class.
	 */
	struct Options
	{
	    Options(const string& command, ThrowBehaviour throw_behaviour = NoThrow);
	    Options(const Args& args, ThrowBehaviour throw_behaviour = NoThrow);
	    Options(std::initializer_list<string> init, ThrowBehaviour throw_behaviour = NoThrow);

	    /**
	     * The command to be executed. Either command or args must be empty.
	     */
	    string command;

	    /**
	     * The command with arguments to be executed. Either command or args must be empty.
	     */
	    vector<string> args;

	    /**
	     * Should exceptions be thrown or not?
	     *
	     * For fundamental fatal errors, e.g. pipe(), fork(), or dup() failures, an
	     * exception is thrown in any case. The system seems to be in Dutch anyway.
	     */
	    ThrowBehaviour throw_behaviour = NoThrow;

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
	    unsigned int log_line_limit = 1000;

	    /**
	     * If throw_behaviour is DoThrow this function is used
	     * additionally to verify if the command succeeded and if not an
	     * exception is thrown.
	     */
	    std::function<bool(int)> verify = [](int exit_code){ return exit_code == 0; };

	    /**
	     * Environment variables to override existing environment variables. Per
	     * default this includes LC_ALL=C[.UTF-8] and LANGUAGE=C[.UTF-8].
	     */
	    vector<string> env;

	private:

	    void init_env();

	};


	/**
	 * Constructor. The command is immediately executed with the options in the
	 * Options object. Use 'retcode()' to get the commands exit code and stdout() and
	 * stderr() to get the commands output.
	 */
	SystemCmd(const Options& options);

	/**
	 * Convenience constructor where only the command and the throw behaviour can be
	 * specified. Otherwise identical to 'SystemCmd(const Options& options)'.
	 */
	SystemCmd(const string& command, ThrowBehaviour throw_behaviour = NoThrow);

	/**
	 * Convenience constructor where only the command with arguments and the throw
	 * behaviour can be specified. Otherwise identical to 'SystemCmd(const Options&
	 * options)'.
	 */
	SystemCmd(const Args& args, ThrowBehaviour throw_behaviour = NoThrow);

	/**
	 * Convenience constructor where only the command with arguments and the throw
	 * behaviour can be specified. Otherwise identical to 'SystemCmd(const Options&
	 * options)'.
	 */
	SystemCmd(std::initializer_list<string> init, ThrowBehaviour throw_behaviour = NoThrow);

	/**
	 * Return the output lines collected on stdout.
	 */
	const vector<string>& stdout() const { return stdout_lines; }

	/**
	 * Return the output lines collected on stderr.
	 */
	const vector<string>& stderr() const { return stderr_lines; }

	/**
	 * Return the command executed.
	 */
	const string& command() const { return options.command; }

	/**
	 * Return the command with arguments executed.
	 */
	const vector<string>& args() const { return options.args; }

	/**
	 * The command for displaying. Only for logging since quoting is gone.
	 */
	string display_command() const;

	/**
	 * Return the exit code of the command.
	 */
	int retcode() const { return child_retcode; }

	/**
	 * Quotes and protects a single string for shell execution.
	 */
	static string quote(const string& str);

    private:

	class Executor;

	void execute_with_mockup();
	void execute();

	bool do_throw() const { return options.throw_behaviour == DoThrow; }

	string mockup_key() const;

	const Options options;

	vector<string> stdout_lines;
	vector<string> stderr_lines;

	int child_retcode = -1;

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

	SystemCmdException(const SystemCmd* system_cmd, const string& msg)
	    : Exception("")
	{
	    if (system_cmd)
	    {
		// Copy relevant information from sysCmd because it might go
		// out of scope while this exception still exists.
		_cmd = system_cmd->display_command();
		setMsg(msg + ": \"" + _cmd + "\"");
		_cmd_ret = system_cmd->retcode();
		_stderr = system_cmd->stderr();
	    }
	    else
	    {
		setMsg( msg );
	    }
	}

	/**
	 * Return the command that was to be called with all arguments.
	 */
	const string& command() const { return _cmd; }

	/**
	 * Return the return code (the exit code) of the command. This might
	 * not always be meaningful, e.g. if fork() failed or if the command
	 * could not even be started.
	 */
	int cmdRet() const { return _cmd_ret; }

	/**
	 * Return the stderr output of the command. If the command could not be
	 * started, this will be empty.
	 */
	const vector<string> & stderr() const { return _stderr; }

    protected:

	string _cmd;
	int _cmd_ret = -1;
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
     */
    class CommandNotFoundException : public SystemCmdException
    {
    public:

	CommandNotFoundException(const SystemCmd* system_cmd)
	    : SystemCmdException(system_cmd, "Command not found")
	    {}

    };


    inline string
    quote(const string& str)
    {
	return SystemCmd::quote(str);
    }

}

#endif
