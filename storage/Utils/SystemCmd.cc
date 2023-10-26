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


#include <cstdlib>
#include <unistd.h>
#include <cerrno>
#include <fcntl.h>
#include <langinfo.h>
#include <sys/wait.h>
#include <string>
#include <sstream>
#include <boost/algorithm/string.hpp>

#include "storage/Utils/ExceptionImpl.h"
#include "storage/Utils/Stopwatch.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/AppUtil.h"
#include "storage/Utils/StorageTmpl.h"


#define SYSCALL_FAILED( SYSCALL_MSG ) \
    ST_MAYBE_THROW( Exception( Exception::strErrno( errno, SYSCALL_MSG ) ), do_throw() )

#define SYSCALL_FAILED_NOTHROW( SYSCALL_MSG ) \
    ST_MAYBE_THROW( Exception( Exception::strErrno( errno, SYSCALL_MSG ) ), false )

// See man bash
// Since all commands are started via a shell, only the shell's return value is returned.
#define SHELL_RET_COMMAND_NOT_EXECUTABLE	126
#define SHELL_RET_COMMAND_NOT_FOUND		127
#define SHELL_RET_SIGNAL			128


namespace storage
{
    using namespace std;


    SystemCmd::Options::Options(const string& command, ThrowBehaviour throw_behaviour)
	: command(command), throw_behaviour(throw_behaviour)
    {
	init_env();
    }


    SystemCmd::Options::Options(const Args& args, ThrowBehaviour throw_behaviour)
	: args(args.get_values()), throw_behaviour(throw_behaviour)
    {
	init_env();
    }


    SystemCmd::Options::Options(std::initializer_list<string> init, ThrowBehaviour throw_behaviour)
	: args(init), throw_behaviour(throw_behaviour)
    {
	init_env();
    }


    void
    SystemCmd::Options::init_env()
    {
	// parted needs UTF-8 to decode partition names with non-ASCII characters. Might
	// be the case for other programs as well. Running in non-UTF-8 is not really
	// supported.

	if (strcmp(nl_langinfo(CODESET), "UTF-8") == 0)
	    env = { "LC_ALL=C.UTF-8", "LANGUAGE=C.UTF-8" };
	else
	    env = { "LC_ALL=C", "LANGUAGE=C" };
    }


    SystemCmd::SystemCmd(const Options& options)
	: options(options)
    {
	if (!command().empty())
	    y2mil("constructor SystemCmd(\"" << command() << "\")");

	if (!args().empty())
	    y2mil("constructor SystemCmd(" << args() << ")");

	if (command().empty() && args().empty())
            ST_THROW(SystemCmdException(this, "Neither command nor args specified"));

	if (!command().empty() && !args().empty())
            ST_THROW(SystemCmdException(this, "Both command and args specified"));

	init();

	try
	{
	    execute();
	}
	catch ( const Exception &exception )
	{
	    ST_CAUGHT( exception );
	    cleanup();
	    ST_RETHROW( exception );
	}

	if (do_throw() && !options.verify(_cmdRet))
	{
	    string s = "command '" + display_command() + "' failed:\n\n";

	    if (!stdout().empty())
		s += "stdout:\n" + boost::join(stdout(), "\n") + "\n\n";

	    if (!stderr().empty())
		s += "stderr:\n" + boost::join(stderr(), "\n") + "\n\n";

	    s += "exit code:\n" + to_string(_cmdRet);

	    ST_THROW(Exception(s));
	}
    }


    SystemCmd::SystemCmd(const string& command, ThrowBehaviour throw_behaviour)
	: SystemCmd(SystemCmd::Options(command, throw_behaviour))
    {
    }


    SystemCmd::SystemCmd(const Args& args, ThrowBehaviour throw_behaviour)
	: SystemCmd(SystemCmd::Options(args, throw_behaviour))
    {
    }


    SystemCmd::SystemCmd(std::initializer_list<string> init, ThrowBehaviour throw_behaviour)
	: SystemCmd(SystemCmd::Options(init, throw_behaviour))
    {
    }


    string
    SystemCmd::display_command() const
    {
	return command().empty() ? boost::join(args(), " ") : command();
    }


    void
    SystemCmd::init()
    {
        _childStdin = NULL;
	_files[0] = _files[1] = NULL;
	_pfds[0].events = POLLOUT; // stdin
	_pfds[1].events = POLLIN;  // stdout
	_pfds[2].events = POLLIN;  // stderr
    }


    string
    SystemCmd::mockup_key() const
    {
	if (!options.mockup_key.empty())
	    return options.mockup_key;

	if (!options.args.empty())
	    return boost::join(options.args, " ");
	else
	    return options.command;
    }


    void
    SystemCmd::cleanup()
    {
        if ( _childStdin )
        {
            fclose( _childStdin );
            _childStdin = NULL;
        }

	if ( _files[IDX_STDOUT] )
	{
	    fclose( _files[IDX_STDOUT] );
	    _files[IDX_STDOUT] = NULL;
	}

	if ( _files[IDX_STDERR] )
	{
	    fclose( _files[IDX_STDERR] );
	    _files[IDX_STDERR] = NULL;
	}
    }


    SystemCmd::~SystemCmd()
    {
	cleanup();
    }


    int
    SystemCmd::execute()
    {
	// TODO the command handling could need a better concept

	if (Mockup::get_mode() == Mockup::Mode::PLAYBACK)
	{
	    const Mockup::Command mockup_command = Mockup::get_command(mockup_key());
	    _outputLines[IDX_STDOUT] = mockup_command.stdout;
	    _outputLines[IDX_STDERR] = mockup_command.stderr;
	    _cmdRet = mockup_command.exit_code;

	    if (_cmdRet == 127 && do_throw())
		ST_THROW(CommandNotFoundException(this));

	    return 0;
	}

	int ret;

	if (get_remote_callbacks())
	{
	    const RemoteCallbacks* remote_callbacks = get_remote_callbacks();

	    if (args().empty())
	    {
		const RemoteCommand remote_command = remote_callbacks->get_command(command());
		_outputLines[IDX_STDOUT] = remote_command.stdout;
		_outputLines[IDX_STDERR] = remote_command.stderr;
		_cmdRet = remote_command.exit_code;
		ret = 0;
	    }
	    else
	    {
		const RemoteCallbacksV2* remote_callbacks_v2 = dynamic_cast<const RemoteCallbacksV2*>(remote_callbacks);
		if (!remote_callbacks_v2)
		    ST_THROW(Exception("old RemoteCallback"));

		const RemoteCommand remote_command = remote_callbacks_v2->get_command_v2(args());
		_outputLines[IDX_STDOUT] = remote_command.stdout;
		_outputLines[IDX_STDERR] = remote_command.stderr;
		_cmdRet = remote_command.exit_code;
		ret = 0;
	    }
	}
	else
	{
	    y2mil("SystemCmd Executing:\"" << display_command() << "\"");
	    y2mil("timestamp " << timestamp());
	    ret = doExecute();
	}

	if (Mockup::get_mode() == Mockup::Mode::RECORD)
	{
	    Mockup::set_command(mockup_key(), Mockup::Command(stdout(), stderr(), retcode()));
	}

	return ret;
    }


    int
    SystemCmd::doExecute()
    {
	y2deb("command:" << display_command());

	Stopwatch stopwatch;

        _childStdin = NULL;
	_files[IDX_STDERR] = _files[IDX_STDOUT] = NULL;
	invalidate();
	int sin[2];
	int sout[2];
	int serr[2];
	bool ok = true;
	if ( pipe(sin)<0 )
	{
	    SYSCALL_FAILED( "pipe stdin creation failed" );
	    ok = false;
	}
	if ( pipe(sout)<0 )
	{
	    SYSCALL_FAILED( "pipe stdout creation failed" );
	    ok = false;
	}
	if ( pipe(serr)<0 )
	{
	    SYSCALL_FAILED( "pipe stderr creation failed" );
	    ok = false;
	}
	if ( ok )
	{
	    _pfds[0].fd = sin[1];
	    if ( fcntl( _pfds[0].fd, F_SETFL, O_NONBLOCK )<0 )
	    {
		SYSCALL_FAILED( "fcntl O_NONBLOCK failed for stdin" );
	    }
	    _pfds[1].fd = sout[0];
	    if ( fcntl( _pfds[1].fd, F_SETFL, O_NONBLOCK )<0 )
	    {
		SYSCALL_FAILED( "fcntl O_NONBLOCK failed for stdout" );
	    }
	    _pfds[2].fd = serr[0];
	    if ( fcntl( _pfds[2].fd, F_SETFL, O_NONBLOCK )<0 )
	    {
		SYSCALL_FAILED( "fcntl O_NONBLOCK failed for stderr" );
	    }
	    y2deb("sout:" << _pfds[1].fd << " serr:" << _pfds[2].fd);

	    const int max_fd = getdtablesize();

	    const TmpForExec args_p(make_args());
	    const TmpForExec env_p(make_env());

	    switch( (_cmdPid=fork()) )
	    {
		case 0: // child process
		{
		    // Do not use exit() here. Use _exit() instead.

		    // Only use async‐signal‐safe functions here, see fork(2) and
		    // signal-safety(7).

		    if ( dup2( sin[0], STDIN_FILENO )<0 )
			_exit(125);
		    if ( dup2( sout[1], STDOUT_FILENO )<0 )
			_exit(125);
		    if ( dup2( serr[1], STDERR_FILENO )<0 )
			_exit(125);
		    if ( close( sin[1] )<0 )
			_exit(125);
		    if ( close( sout[0] )<0 )
			_exit(125);
		    if ( close( serr[0] )<0 )
			_exit(125);

		    // Unfortunaltely close_range(2) is still too new. It is also not
		    // mentioned in signal-safety(7).

		    for (int fd = 3; fd < max_fd; ++fd)
			fcntl(fd, F_SETFD, FD_CLOEXEC);

		    if (args().empty())
		    {
			_cmdRet = execle(SH_BIN, SH_BIN, "-c", command().c_str(), nullptr, env_p.get());
		    }
		    else
		    {
			_cmdRet = execvpe(args()[0].c_str(), args_p.get(), env_p.get());
		    }

		    // If we get here the exec has failed. Depending on errno we return an
		    // error code like the shell does ('sh -c ...'). Unfortunately it is
		    // not possible in the parent to distinguish whether exec or the
		    // program returned the code (e.g. 127). So far that does not seem to
		    // be needed. If ever needed the "self-pipe trick" could be used.

		    if (errno == ENOENT)
			_exit(SHELL_RET_COMMAND_NOT_FOUND);
		    if (errno == ENOEXEC || errno == EACCES || errno == EISDIR)
			_exit(SHELL_RET_COMMAND_NOT_EXECUTABLE);
		    _exit(125);
		}
		break;

		case -1:
		{
		    _cmdRet = -1;
		    SYSCALL_FAILED( "fork() failed" );
		}
		break;

		default: // parent process
		{
		    if ( close( sin[0] ) < 0 )
		    {
			SYSCALL_FAILED_NOTHROW( "close( stdin ) in parent failed" );
		    }
		    if ( close( sout[1] )<0 )
		    {
			SYSCALL_FAILED_NOTHROW( "close( stdout ) in parent failed" );
		    }
		    if ( close( serr[1] )<0 )
		    {
			SYSCALL_FAILED_NOTHROW( "close( stderr ) in parent failed" );
		    }
		    _cmdRet = 0;

                    _childStdin = fdopen( sin[1], "a" );
		    if ( _childStdin == NULL )
		    {
			SYSCALL_FAILED_NOTHROW( "fdopen( stdin ) failed" );
		    }

		    _files[IDX_STDOUT] = fdopen( sout[0], "r" );
		    if ( _files[IDX_STDOUT] == NULL )
		    {
			SYSCALL_FAILED_NOTHROW( "fdopen( stdout ) failed" );
		    }
		    _files[IDX_STDERR] = fdopen( serr[0], "r" );
		    if ( _files[IDX_STDERR] == NULL )
		    {
			SYSCALL_FAILED_NOTHROW( "fdopen( stderr ) failed" );
		    }

		    doWait( _cmdRet );
		    y2mil("stopwatch " << stopwatch << " for \"" << display_command() << "\"");
		}
		break;
	    }
	}
	else
	{
	    _cmdRet = -1;
	}
	if ( _cmdRet==-127 || _cmdRet==-1 )
	{
	    y2err("system (\"" << display_command() << "\") = " << _cmdRet);
	}
	checkOutput();
	y2mil("system() Returns:" << _cmdRet);
	if ( _cmdRet!=0 )
	    logOutput();
	return _cmdRet;
    }


    bool
    SystemCmd::doWait( int& cmdRet_ret )
    {
	int waitpidRet;
	int cmdStatus;

	do
	{
	    y2deb("[1] fd:" << _pfds[1].fd << " ev:" << hex << (unsigned)(_pfds[1].events) << dec << " "
		  "[2] fd:" << _pfds[2].fd << " ev:" << hex << (unsigned)(_pfds[2].events));
	    int sel = poll( _pfds, 3, 1000 );
	    if (sel < 0)
	    {
		SYSCALL_FAILED_NOTHROW( "poll() failed" );
	    }
	    y2deb("poll ret:" << sel);
	    if ( sel>0 )
	    {
                if ( _pfds[0].revents )
                    sendStdin();
                if ( _pfds[1].revents || _pfds[2].revents )
                    checkOutput();
	    }
	    waitpidRet = waitpid( _cmdPid, &cmdStatus, WNOHANG );
	    y2deb("Wait ret:" << waitpidRet);
	}
	while ( waitpidRet == 0 );

	if ( waitpidRet != 0 )
	{
	    checkOutput();
            if ( _childStdin )
            {
                fclose( _childStdin );
                _childStdin = NULL;
            }
	    fclose( _files[IDX_STDOUT] );
	    _files[IDX_STDOUT] = NULL;
	    fclose( _files[IDX_STDERR] );
	    _files[IDX_STDERR] = NULL;
	    if (WIFEXITED(cmdStatus))
	    {
		cmdRet_ret = WEXITSTATUS(cmdStatus);
		if ( cmdRet_ret == SHELL_RET_COMMAND_NOT_EXECUTABLE )
		    ST_MAYBE_THROW(SystemCmdException(this, "Command not executable"), do_throw());
		else if ( cmdRet_ret == SHELL_RET_COMMAND_NOT_FOUND )
		    ST_MAYBE_THROW(CommandNotFoundException(this), do_throw());
		else if ( cmdRet_ret > SHELL_RET_SIGNAL )
		{
		    std::stringstream msg;
		    msg << "Caught signal #" << ( cmdRet_ret - SHELL_RET_SIGNAL );
		    ST_MAYBE_THROW( SystemCmdException(this, msg.str()), do_throw());
		}
	    }
	    else
	    {
		cmdRet_ret = -127;
		ST_MAYBE_THROW(SystemCmdException(this, "Command failed"), do_throw());
	    }
	}

	y2deb("Wait:" << waitpidRet << " pid:" << _cmdPid << " stat:" << cmdStatus <<
	      " Ret:" << cmdRet_ret);
	return waitpidRet != 0;
    }


    void
    SystemCmd::invalidate()
    {
	for (int streamIndex = 0; streamIndex < 2; streamIndex++)
	{
	    _outputLines[streamIndex].clear();
	    _newLineSeen[streamIndex] = true;
	}
    }


    void
    SystemCmd::checkOutput()
    {
	y2deb("NewLine out:" << _newLineSeen[IDX_STDOUT] << " err:" << _newLineSeen[IDX_STDERR]);
	if (_files[IDX_STDOUT])
	    getUntilEOF(_files[IDX_STDOUT], _outputLines[IDX_STDOUT], _newLineSeen[IDX_STDOUT], false);
	if (_files[IDX_STDERR])
	    getUntilEOF(_files[IDX_STDERR], _outputLines[IDX_STDERR], _newLineSeen[IDX_STDERR], true);
	y2deb("NewLine out:" << _newLineSeen[IDX_STDOUT] << " err:" << _newLineSeen[IDX_STDERR]);
    }


    void
    SystemCmd::sendStdin()
    {
        if ( ! _childStdin )
            return;

        if (!options.stdin_text.empty())
        {
            string::size_type count = 0;
            string::size_type len   = options.stdin_text.size();
            int result = 1;

            while ( count < len && result > 0 )
                result = fputc( options.stdin_text[ count++ ], _childStdin );

            options.stdin_text.erase( 0, count );
            // y2deb( count << " characters written; left over: \"" << _stdinText << "\"" );
        }

        if (options.stdin_text.empty())
        {
            fclose( _childStdin );
            _childStdin = NULL;
            _pfds[0].fd = -1; // ignore for poll() from now on
        }
    }


#define BUF_LEN 256

    void
    SystemCmd::getUntilEOF( FILE* file, vector<string>& lines,
			    bool& newLineSeen_ret, bool isStderr ) const
    {
	size_t oldSize = lines.size();
	char buffer[BUF_LEN];
	int count;
	int c;
	string text;

	clearerr( file );
	count = 0;
	c = EOF;
	while ( (c=fgetc(file)) != EOF )
	{
	    buffer[count++] = c;
	    if ( count==sizeof(buffer)-1 )
	    {
		buffer[count] = 0;
		extractNewline( buffer, count, newLineSeen_ret, text, lines );
		count = 0;
	    }
	    c = EOF;
	}
	if ( count>0 )
	{
	    buffer[count] = 0;
	    extractNewline( buffer, count, newLineSeen_ret, text, lines );
	}
	if ( text.length() > 0 )
	{
	    if ( newLineSeen_ret )
	    {
		addLine( text, lines );
	    }
	    else
	    {
		lines[lines.size()-1] += text;
	    }
	    newLineSeen_ret = false;
	}
	else
	{
	    newLineSeen_ret = true;
	}
	y2deb("text:" << text << " NewLine:" << newLineSeen_ret);
	if ( oldSize != lines.size() )
	{
	    y2mil("pid:" << _cmdPid << " added lines:" << lines.size() - oldSize << " stderr:" << isStderr);
	}
    }


    void
    SystemCmd::extractNewline(const string& buffer, int count, bool& newLineSeen_ret,
			      string& text, vector<string>& lines) const
    {
	string::size_type index;

	text += buffer;
	while ( (index=text.find( '\n' )) != string::npos )
	{
	    if ( !newLineSeen_ret )
	    {
		lines[lines.size()-1] += text.substr( 0, index );
	    }
	    else
	    {
		addLine( text.substr( 0, index ), lines );
	    }
	    text.erase( 0, index+1 );
	    newLineSeen_ret = true;
	}
	y2deb("text: \"" << text << "\" newLineSeen: " << newLineSeen_ret);
    }


    void
    SystemCmd::addLine(const string& text, vector<string>& lines) const
    {
	if (lines.size() < options.log_line_limit)
	{
	    y2mil("Adding Line " << lines.size() + 1 << " \"" << text << "\"");
	}
	else
	{
	    y2deb("Adding Line " << lines.size() + 1 << " \"" << text << "\"");
	}

	lines.push_back(text);
    }


    void
    SystemCmd::logOutput() const
    {
	unsigned lineCount = stderr().size();
	if (lineCount <= options.log_line_limit)
	{
	    for (const string& line : stderr())
		y2mil("stderr:" << line);
	}
	else
	{
	    for (unsigned i = 0; i < options.log_line_limit / 2; ++i)
		y2mil("stderr:" << stderr()[i]);

	    y2mil("stderr omitting lines");

	    for (unsigned i = lineCount - options.log_line_limit / 2; i < lineCount; ++i)
		y2mil("stderr:" << stderr()[i]);
	}

	lineCount = stdout().size();
	if (lineCount <= options.log_line_limit)
	{
	    for (const string& line : stdout())
                y2mil("stdout:" << line);
	}
	else
	{
	    for (unsigned i = 0; i < options.log_line_limit / 2; ++i)
		y2mil("stdout:" << stdout()[i]);

	    y2mil("stdout omitting lines");

	    for (unsigned i = lineCount - options.log_line_limit / 2; i < lineCount; ++i)
		y2mil("stdout:" << stdout()[i]);
	}
    }


    SystemCmd::TmpForExec::~TmpForExec()
    {
	for (char* v : values)
	    free(v);
    }


    SystemCmd::TmpForExec
    SystemCmd::make_args() const
    {
	vector<char*> args;

	for (const string& v : options.args)
	{
	    args.push_back(strdup(v.c_str()));
	}

	args.push_back(nullptr);

	return args;
    }


    SystemCmd::TmpForExec
    SystemCmd::make_env() const
    {
	// Environment variables should be present only once in the environment.
	// https://pubs.opengroup.org/onlinepubs/009695399/basedefs/xbd_chap08.html

	vector<char*> env;

	for (char** v = environ; *v != NULL; ++v)
	    env.push_back(strdup(*v));

	for (const string& v : options.env)
	{
	    string::size_type pos = v.find("=");
	    if (pos == string::npos)
		continue;

	    string key = v.substr(0, pos + 1); // key including '=' sign

	    vector<char*>::iterator it = find_if(env.begin(), env.end(),
		[&key](const char* tmp) { return boost::starts_with(tmp, key); });
	    if (it != env.end())
		*it = strdup(v.c_str());
	    else
		env.push_back(strdup(v.c_str()));
	}

	env.push_back(nullptr);

	return env;
    }


    string
    SystemCmd::quote(const string& str)
    {
	return "'" + boost::replace_all_copy(str, "'", "'\\''") + "'";
    }

}
