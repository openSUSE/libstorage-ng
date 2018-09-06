/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) [2016-2018] SUSE LLC
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


#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <ostream>
#include <fstream>
#include <sys/wait.h>
#include <string>
#include <sstream>
#include <boost/algorithm/string.hpp>

extern char **environ;

#include "storage/Utils/ExceptionImpl.h"
#include "storage/Utils/Stopwatch.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/OutputProcessor.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/AppUtil.h"


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


    SystemCmd::SystemCmd(const Options& options)
	: options(options), _combineOutput(false), _execInBackground(false), _cmdRet(0),
	  _cmdPid(0), _outputProc(nullptr)
    {
	y2mil("constructor SystemCmd(\"" << command() << "\")");

	if (command().empty())
            ST_THROW(SystemCmdException(this, "No command specified"));

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
	    string s = "command '" + command() + "' failed:\n\n";

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


    void
    SystemCmd::init()
    {
        _childStdin = NULL;
	_files[0] = _files[1] = NULL;
	_pfds[0].events = POLLOUT; // stdin
	_pfds[1].events = POLLIN;  // stdout
	_pfds[2].events = POLLIN;  // stderr
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


    void
    SystemCmd::closeOpenFds() const
    {
	int max_fd = getdtablesize();

	for ( int fd = 3; fd < max_fd; fd++ )
	{
	    close(fd);
	}
    }


    int
    SystemCmd::execute()
    {
	// TODO the command handling could need a better concept

	if (Mockup::get_mode() == Mockup::Mode::PLAYBACK)
	{
	    const Mockup::Command& mockup_command = Mockup::get_command(mockup_key());
	    _outputLines[IDX_STDOUT] = mockup_command.stdout;
	    _outputLines[IDX_STDERR] = mockup_command.stderr;
	    _cmdRet = mockup_command.exit_code;
	    return 0;
	}

	int ret;

	if (get_remote_callbacks())
	{
	    const RemoteCommand remote_command = get_remote_callbacks()->get_command(command());
	    _outputLines[IDX_STDOUT] = remote_command.stdout;
	    _outputLines[IDX_STDERR] = remote_command.stderr;
	    _cmdRet = remote_command.exit_code;
	    ret = 0;
	}
	else
	{
	    y2mil("SystemCmd Executing:\"" << command() << "\"");
	    y2mil("timestamp " << timestamp());
	    _execInBackground = false;
	    ret = doExecute();
	}

	if (Mockup::get_mode() == Mockup::Mode::RECORD)
	{
	    Mockup::set_command(mockup_key(), Mockup::Command(stdout(), stderr(), retcode()));
	}

	return ret;
    }


    int
    SystemCmd::executeBackground()
    {
	y2mil("SystemCmd Executing (Background): \"" << command() << "\"");
	_execInBackground = true;
	return doExecute();
    }


    int
    SystemCmd::executeRestricted(long unsigned maxTimeSec, long unsigned maxLineOut,
				 bool& timeExceeded_ret, bool& linesExceeded_ret)
    {
	y2mil("cmd:" << command() << " MaxTime:" << maxTimeSec << " MaxLines:" << maxLineOut);
	timeExceeded_ret = linesExceeded_ret = false;
	int ret = executeBackground();
	unsigned long ts = 0;
	unsigned long ls = 0;
	unsigned long start_time = time(NULL);
	while ( !timeExceeded_ret && !linesExceeded_ret && !doWait( false, ret ) )
	{
	    if ( maxTimeSec>0 )
	    {
		ts = time(NULL)-start_time;
		y2mil( "time used:" << ts );
	    }
	    if ( maxLineOut>0 )
	    {
		ls = stdout().size() + stderr().size();
		y2mil( "lines out:" << ls );
	    }
	    timeExceeded_ret = maxTimeSec>0 && ts>maxTimeSec;
	    linesExceeded_ret = maxLineOut>0 && ls>maxLineOut;
	    sleep( 1 );
	}
	if ( timeExceeded_ret || linesExceeded_ret )
	{
	    int r = kill( _cmdPid, SIGKILL );
	    y2mil( "kill pid:" << _cmdPid << " ret:" << r );
	    unsigned count=0;
	    int cmdStatus;
	    int waitpidRet = -1;
	    while ( count<5 && waitpidRet<=0 )
	    {
		waitpidRet = waitpid( _cmdPid, &cmdStatus, WNOHANG );
		y2mil( "waitpid:" << waitpidRet );
		count++;
		sleep( 1 );
	    }
	    /*
	      r = kill( _cmdPid, SIGKILL );
	      y2mil( "kill pid:" << _cmdPid << " ret:" << r );
	      count=0;
	      waitDone = false;
	      while ( count<8 && !waitDone )
	      {
		  y2mil( "doWait:" << count );
		  waitDone = doWait( false, ret );
		  count++;
		  sleep( 1 );
	      }
	    */
	    _cmdRet = -257;
	}
	else
	    _cmdRet = ret;
	y2mil("ret:" << ret << " timeExceeded:" << timeExceeded_ret
	      << " linesExceeded_ret:" << linesExceeded_ret);
	return ret;
    }


    int
    SystemCmd::doExecute()
    {
	if ( _outputProc )
	{
	    _outputProc->reset();
	}
	y2deb("command:" << command());

	Stopwatch stopwatch;

        _childStdin = NULL;
	_files[IDX_STDERR] = _files[IDX_STDOUT] = NULL;
	invalidate();
	int sin[2];
	int sout[2];
	int serr[2];
	bool ok = true;
	if ( !_testmode && pipe(sin)<0 )
	{
	    SYSCALL_FAILED( "pipe stdin creation failed" );
	    ok = false;
	}
	if ( !_testmode && pipe(sout)<0 )
	{
	    SYSCALL_FAILED( "pipe stdout creation failed" );
	    ok = false;
	}
	if ( !_testmode && !_combineOutput && pipe(serr)<0 )
	{
	    SYSCALL_FAILED( "pipe stderr creation failed" );
	    ok = false;
	}
	if ( !_testmode && ok )
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
	    if ( !_combineOutput )
	    {
		_pfds[2].fd = serr[0];
		if ( fcntl( _pfds[2].fd, F_SETFL, O_NONBLOCK )<0 )
		{
		    SYSCALL_FAILED( "fcntl O_NONBLOCK failed for stderr" );
		}
	    }
	    y2deb("sout:" << _pfds[1].fd << " serr:" << (_combineOutput?-1:_pfds[2].fd));

	    const vector<const char*> env = make_env();

	    switch( (_cmdPid=fork()) )
	    {
		case 0: // child process

		    if ( dup2( sin[0], STDIN_FILENO )<0 )
		    {
			SYSCALL_FAILED_NOTHROW( "dup2 stdin failed in child process" );
		    }
		    if ( dup2( sout[1], STDOUT_FILENO )<0 )
		    {
			SYSCALL_FAILED_NOTHROW( "dup2 stdout failed in child process" );
		    }
		    if ( !_combineOutput && dup2( serr[1], STDERR_FILENO )<0 )
		    {
			SYSCALL_FAILED_NOTHROW( "dup2 stderr failed in child process" );
		    }
		    if ( _combineOutput && dup2( STDOUT_FILENO, STDERR_FILENO )<0 )
		    {
			SYSCALL_FAILED_NOTHROW( "dup2 stderr failed in child process" );
		    }
		    if ( close( sin[1] )<0 )
		    {
			SYSCALL_FAILED_NOTHROW( "close( stdin ) failed in child process" );
		    }
		    if ( close( sout[0] )<0 )
		    {
			SYSCALL_FAILED_NOTHROW( "close( stdout ) failed in child process" );
		    }
		    if ( !_combineOutput && close( serr[0] )<0 )
		    {
			SYSCALL_FAILED_NOTHROW( "close( stderr ) failed in child process" );
		    }
		    closeOpenFds();
		    _cmdRet = execle(SHBIN, SHBIN, "-c", command().c_str(), nullptr, &env[0]);

		    // execle() should not return. If we get here, it failed.
		    // Throwing an exception here would not make any sense, however:
		    // We are in the forked child process, and there is nothing
		    // to return to that could make use of an exception.
		    y2err("execle() failed: THIS SHOULD NOT HAPPEN \"SHBIN\" Ret:" <<
			  _cmdRet << " errno: " << errno);
		    y2err( "Exiting child process" );
		    exit(127); // same as "command not found" in the shell
		    break;

		case -1:
		    _cmdRet = -1;
		    SYSCALL_FAILED( "fork() failed" );
		    break;

		default: // parent process
		    if ( close( sin[0] ) < 0 )
		    {
			SYSCALL_FAILED_NOTHROW( "close( stdin ) in parent failed" );
		    }
		    if ( close( sout[1] )<0 )
		    {
			SYSCALL_FAILED_NOTHROW( "close( stdout ) in parent failed" );
		    }
		    if ( !_combineOutput && close( serr[1] )<0 )
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
		    if ( !_combineOutput )
		    {
			_files[IDX_STDERR] = fdopen( serr[0], "r" );
			if ( _files[IDX_STDERR] == NULL )
			{
			    SYSCALL_FAILED_NOTHROW( "fdopen( stderr ) failed" );
			}
		    }
		    if ( !_execInBackground )
		    {
			doWait( true, _cmdRet );
			y2mil("stopwatch " << stopwatch << " for \"" << command() << "\"");
		    }
		    break;
	    }
	}
	else if ( !_testmode )
	{
	    _cmdRet = -1;
	}
	else
	{
	    _cmdRet = 0;
	    y2mil("TESTMODE would execute \"" << command() << "\"");
	}
	if ( _cmdRet==-127 || _cmdRet==-1 )
	{
	    y2err("system (\"" << command() << "\") = " << _cmdRet);
	}
	if ( !_testmode )
	    checkOutput();
	y2mil("system() Returns:" << _cmdRet);
	if ( _cmdRet!=0 )
	    logOutput();
	return _cmdRet;
    }


    bool
    SystemCmd::doWait( bool hang, int& cmdRet_ret )
    {
	int waitpidRet;
	int cmdStatus;

	do
	{
	    y2deb("[0] id:" <<	_pfds[1].fd << " ev:" << hex << (unsigned)_pfds[1].events << dec << " [1] fs:" <<
		  (_combineOutput?-1:_pfds[2].fd) << " ev:" << hex << (_combineOutput?0:(unsigned)_pfds[2].events));
	    int sel = poll( _pfds, _combineOutput?2:3, 1000 );
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
	while ( hang && waitpidRet == 0 );

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
	    if ( !_combineOutput )
	    {
		fclose( _files[IDX_STDERR] );
		_files[IDX_STDERR] = NULL;
	    }
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
	    if ( _outputProc )
	    {
		_outputProc->finish();
	    }
	}

	y2deb("Wait:" << waitpidRet << " pid:" << _cmdPid << " stat:" << cmdStatus <<
	      " Hang:" << hang << " Ret:" << cmdRet_ret);
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
		if ( _outputProc )
		{
		    _outputProc->process( buffer, isStderr );
		}
	    }
	    c = EOF;
	}
	if ( count>0 )
	{
	    buffer[count] = 0;
	    extractNewline( buffer, count, newLineSeen_ret, text, lines );
	    if ( _outputProc )
	    {
		_outputProc->process( buffer, isStderr );
	    }
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


    vector<const char*>
    SystemCmd::make_env() const
    {
	vector<const char*> env;

	for (char** v = environ; *v != NULL; ++v)
	{
	    if (strncmp(*v, "LC_ALL=", strlen("LC_ALL=")) != 0 &&
		strncmp(*v, "LANGUAGE=", strlen("LANGUAGE=")) != 0)
		env.push_back(*v);
	}

	env.push_back("LC_ALL=C");
	env.push_back("LANGUAGE=C");

	env.push_back(nullptr);

	return env;
    }


    string
    SystemCmd::quote(const string& str)
    {
	return "'" + boost::replace_all_copy(str, "'", "'\\''") + "'";
    }


    string
    SystemCmd::quote(const vector<string>& strs)
    {
	string ret;

	for (std::vector<string>::const_iterator it = strs.begin(); it != strs.end(); ++it)
	{
	    if (it != strs.begin())
		ret.append(" ");
	    ret.append(quote(*it));
	}

	return ret;
    }


    bool SystemCmd::_testmode = false;

}
