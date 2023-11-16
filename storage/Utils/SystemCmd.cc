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
#include <poll.h>
#include <langinfo.h>
#include <sys/types.h>
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


#define SYSCALL_FAILED(SYSCALL_MSG) \
    ST_THROW(Exception(Exception::strErrno(errno, SYSCALL_MSG)))


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

	execute_with_mockup();

	if (do_throw() && !options.verify(child_retcode))
	{
	    string s = "command '" + display_command() + "' failed:\n\n";

	    if (!stdout().empty())
		s += "stdout:\n" + boost::join(stdout(), "\n") + "\n\n";

	    if (!stderr().empty())
		s += "stderr:\n" + boost::join(stderr(), "\n") + "\n\n";

	    s += "exit code:\n" + to_string(child_retcode);

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
    SystemCmd::execute_with_mockup()
    {
	// TODO the command handling could need a better concept

	if (Mockup::get_mode() == Mockup::Mode::PLAYBACK)
	{
	    const Mockup::Command mockup_command = Mockup::get_command(mockup_key());
	    stdout_lines = mockup_command.stdout;
	    stderr_lines = mockup_command.stderr;
	    child_retcode = mockup_command.exit_code;

	    if (child_retcode == 127 && do_throw())
		ST_THROW(CommandNotFoundException(this));

	    return;
	}

	if (get_remote_callbacks())
	{
	    const RemoteCallbacks* remote_callbacks = get_remote_callbacks();

	    if (args().empty())
	    {
		const RemoteCommand remote_command = remote_callbacks->get_command(command());
		stdout_lines = remote_command.stdout;
		stderr_lines = remote_command.stderr;
		child_retcode = remote_command.exit_code;
	    }
	    else
	    {
		const RemoteCallbacksV2* remote_callbacks_v2 = dynamic_cast<const RemoteCallbacksV2*>(remote_callbacks);
		if (!remote_callbacks_v2)
		    ST_THROW(Exception("old RemoteCallback"));

		const RemoteCommand remote_command = remote_callbacks_v2->get_command_v2(args());
		stdout_lines = remote_command.stdout;
		stderr_lines = remote_command.stderr;
		child_retcode = remote_command.exit_code;
	    }
	}
	else
	{
	    execute();
	}

	if (Mockup::get_mode() == Mockup::Mode::RECORD)
	{
	    Mockup::set_command(mockup_key(), Mockup::Command(stdout(), stderr(), retcode()));
	}
    }


    namespace
    {

	class FileDescriptor : boost::noncopyable
	{
	public:

	    ~FileDescriptor() { if (fd >= 0) close(); }

	    int close();

	    int fd = -1;

	};


	int
	FileDescriptor::close()
	{
	    int ret = ::close(fd);
	    fd = -1;
	    return ret;
	}


	class Pipe : boost::noncopyable
	{
	public:

	    Pipe(int flags = O_CLOEXEC);

	    FileDescriptor read_end;
	    FileDescriptor write_end;

	};


	Pipe::Pipe(int flags)
	{
	    int pipefd[2];

	    if (pipe2(pipefd, flags) != 0)
		SYSCALL_FAILED("pipe2 failed");

	    read_end.fd = pipefd[0];
	    write_end.fd = pipefd[1];
	}


	/**
	 * Class to tempararily hold copies of args or env for execle() and execvpe().
	 */
	class TmpForExec : boost::noncopyable
	{
	public:

	    TmpForExec(const vector<char*>& values) : values(values) {}
	    ~TmpForExec();

	    char* const * get() const { return &values[0]; }

	private:

	    vector<char*> values;

	};


	TmpForExec::~TmpForExec()
	{
	    for (char* v : values)
		free(v);
	}


	/**
	 * Struct to hold information about failures of the child between fork and exec. The
	 * struct is passed via a pipe from the client to the parent. The information is so
	 * far only used for logging.
	 */
	struct ChildFailureInfo
	{
	    int cmdnum = 0;
	    int errnum = 0;
	};

    }


    /**
     * This extra class allows us to 1. reduce the size of SystemCmd.h esp. the number of
     * includes and 2. ensure that the resources associated with it are released
     * automatically early (before the SystemCmd destructor).
     */
    class SystemCmd::Executor
    {
    public:

	Executor(SystemCmd& system_cmd);

    private:

	void step_fork_and_exec();
	void step_poll();
	void step_wait();

	void write_stdin();
	void read_stdout();
	void read_stderr();

	void fill_buffer(const char* name, int fd, string& buffer, vector<string>& lines) const;
	void split_buffer(const char* name, string& buffer, vector<string>& lines, bool finito = false) const;
	void add_line(const char* name, const string& line, vector<string>& lines) const;

	/**
	 * Constructs the args for the child process.
	 *
	 * Not async‐signal‐safe, see fork(2) and signal-safety(7).
	 */
	TmpForExec make_args() const;

	/**
	 * Constructs the environment for the child process.
	 *
	 * Not async‐signal‐safe, see fork(2) and signal-safety(7).
	 */
	TmpForExec make_env() const;

	SystemCmd& system_cmd;

	pid_t child_pid = -1;

	Pipe stdin_pipe;
	Pipe stdout_pipe;
	Pipe stderr_pipe;

	struct pollfd pollfds[3];

	string::size_type stdin_pos = 0;

	string stdout_buffer;
	string stderr_buffer;

    };


    void
    SystemCmd::execute()
    {
	y2mil("SystemCmd Executing:\"" << display_command() << "\"");
	y2mil("timestamp " << timestamp());

	Stopwatch stopwatch;

	Executor executor(*this);

	y2mil("stopwatch " << stopwatch << " for \"" << display_command() << "\"");
    }


    SystemCmd::Executor::Executor(SystemCmd& system_cmd)
	: system_cmd(system_cmd)
    {
	step_fork_and_exec();
	step_poll();
	step_wait();
    }


    void
    SystemCmd::Executor::step_fork_and_exec()
    {
	y2deb("step fork and exec");

	if (fcntl(stdin_pipe.write_end.fd, F_SETFL, O_NONBLOCK) != 0)
	    SYSCALL_FAILED("pipe stdin O_NONBLOCK failed");

	if (fcntl(stdout_pipe.read_end.fd, F_SETFL, O_NONBLOCK) != 0)
	    SYSCALL_FAILED("pipe stdout O_NONBLOCK failed");

	if (fcntl(stderr_pipe.read_end.fd, F_SETFL, O_NONBLOCK) != 0)
	    SYSCALL_FAILED("pipe stderr O_NONBLOCK failed");

	Pipe child_failure_info_pipe;

	const int max_fd = getdtablesize();

	const TmpForExec args_p(make_args());
	const TmpForExec env_p(make_env());

	child_pid = fork();
	if (child_pid == -1)
	    SYSCALL_FAILED("fork failed");

	if (child_pid == 0)
	{
	    // child process

	    // Do not use exit() here. Use _exit() instead.

	    // Only use async‐signal‐safe functions here, see fork(2) and
	    // signal-safety(7).

	    if (dup2(stdin_pipe.read_end.fd, STDIN_FILENO) < 0)
		_exit(125);
	    if (stdin_pipe.write_end.close() != 0)
		_exit(125);

	    if (dup2(stdout_pipe.write_end.fd, STDOUT_FILENO) < 0)
		_exit(125);
	    if (stdout_pipe.read_end.close() != 0)
		_exit(125);

	    if (dup2(stderr_pipe.write_end.fd, STDERR_FILENO) < 0)
		_exit(125);
	    if (stderr_pipe.read_end.close() != 0)
		_exit(125);

	    if (child_failure_info_pipe.read_end.close() != 0)
		_exit(125);

	    // Unfortunaltely close_range(2) is still too new. It is also not mentioned in
	    // signal-safety(7). We use CLOEXEC here since child_failure_info_pipe must
	    // not yet be closed.

	    for (int fd = 3; fd < max_fd; ++fd)
		fcntl(fd, F_SETFD, FD_CLOEXEC);

	    if (system_cmd.args().empty())
		execle(SH_BIN, SH_BIN, "-c", system_cmd.command().c_str(), nullptr, env_p.get());
	    else
		execvpe(system_cmd.args()[0].c_str(), args_p.get(), env_p.get());

	    // If we get here the exec has failed. Depending on errno we return an
	    // error code like the shell does ('sh -c ...').

	    ChildFailureInfo child_failure_info { 1, errno };
	    int write_ret = TEMP_FAILURE_RETRY(write(child_failure_info_pipe.write_end.fd,
						     &child_failure_info, sizeof(child_failure_info)));
	    if (write_ret != sizeof(child_failure_info))
		_exit(125);
	    if (child_failure_info_pipe.write_end.close() != 0)
		_exit(125);

	    if (errno == ENOENT)
		_exit(SHELL_RET_COMMAND_NOT_FOUND);
	    if (errno == ENOEXEC || errno == EACCES || errno == EISDIR)
		_exit(SHELL_RET_COMMAND_NOT_EXECUTABLE);
	    _exit(125);
	}

	// parent process

	y2mil("child_pid:" << child_pid);

	if (stdin_pipe.read_end.close() != 0)
	    SYSCALL_FAILED("close stdin in parent failed");

	if (stdout_pipe.write_end.close() != 0)
	    SYSCALL_FAILED("close stdout in parent failed");

	if (stderr_pipe.write_end.close() != 0)
	    SYSCALL_FAILED("close stderr in parent failed" );

	if (child_failure_info_pipe.write_end.close() != 0)
	    SYSCALL_FAILED("close child_failure_info_pipe failed");

	// Read ChildFailureInfo. If the exec in the child succeeded the pipe
	// was closed (due to CLOEXEC) and reading fails. If the exec in the
	// child failed the information was written to the pipe and can be
	// used here in the parent.

	ChildFailureInfo child_failure_info;
	int read_ret = TEMP_FAILURE_RETRY(read(child_failure_info_pipe.read_end.fd, &child_failure_info,
					       sizeof(child_failure_info)));
	if (read_ret < 0)
	    SYSCALL_FAILED("read child_failure_info");

	if (read_ret == sizeof(child_failure_info))
	{
	    // so far only used for logging
	    y2err("exec failed: " << child_failure_info.errnum);
	}

	if (child_failure_info_pipe.read_end.close() != 0)
	    SYSCALL_FAILED("close child_failure_info_pipe failed");
    }


    void
    SystemCmd::Executor::step_poll()
    {
	pollfds[0].events = POLLOUT;
	pollfds[0].fd = stdin_pipe.write_end.fd;

	pollfds[1].events = POLLIN;
	pollfds[1].fd = stdout_pipe.read_end.fd;

	pollfds[2].events = POLLIN;
	pollfds[2].fd = stderr_pipe.read_end.fd;

	while (pollfds[0].fd != -1 || pollfds[1].fd != -1 || pollfds[2].fd != -1)
	{
	    y2deb("pollfds[0].fd:" << pollfds[0].fd << "pollfds[1].fd:" << pollfds[1].fd <<
		  "pollfds[2].fd:" << pollfds[2].fd);

	    int poll_ret = TEMP_FAILURE_RETRY(poll(pollfds, 3, -1));
	    if (poll_ret < 0)
		SYSCALL_FAILED("poll failed");

	    if (poll_ret > 0)
	    {
		y2deb("pollfds[0].revents:" << pollfds[0].revents << "pollfds[1].revents:" <<
		      pollfds[1].revents << "pollfds[2].revents:" << pollfds[2].revents);

		if (pollfds[0].revents & POLLOUT)
		    write_stdin();

		if (pollfds[1].revents & POLLIN)
		    read_stdout();
		if (pollfds[1].revents & POLLHUP)
		    pollfds[1].fd = -1;

		if (pollfds[2].revents & POLLIN)
		    read_stderr();
		if (pollfds[2].revents & POLLHUP)
		    pollfds[2].fd = -1;
	    }
	}

	stdout_pipe.read_end.close();
	split_buffer("stdout", stdout_buffer, system_cmd.stdout_lines, true);
	if (system_cmd.stdout_lines.size() >= system_cmd.options.log_line_limit)
	    y2mil("stdout lines:" << system_cmd.stdout_lines.size());

	stderr_pipe.read_end.close();
	split_buffer("stderr", stderr_buffer, system_cmd.stderr_lines, true);
	if (system_cmd.stderr_lines.size() >= system_cmd.options.log_line_limit)
	    y2mil("stderr lines:" << system_cmd.stderr_lines.size());
    }


    void
    SystemCmd::Executor::step_wait()
    {
	y2deb("step wait");

	int wstatus;
	int waitpid_ret = TEMP_FAILURE_RETRY(waitpid(child_pid, &wstatus, 0));
	if (waitpid_ret < 0)
	    SYSCALL_FAILED("waitpid failed");

	y2deb("waitpid_ret:" << waitpid_ret << " wstatus:" << wstatus);

	if (WIFEXITED(wstatus))
	{
	    system_cmd.child_retcode = WEXITSTATUS(wstatus);
	    if (system_cmd.child_retcode == SHELL_RET_COMMAND_NOT_EXECUTABLE)
		ST_MAYBE_THROW(SystemCmdException(&system_cmd, "Command not executable"), system_cmd.do_throw());
	    else if (system_cmd.child_retcode == SHELL_RET_COMMAND_NOT_FOUND)
		ST_MAYBE_THROW(CommandNotFoundException(&system_cmd), system_cmd.do_throw());
	    else if (system_cmd.child_retcode > SHELL_RET_SIGNAL)
	    {
		std::stringstream msg;
		msg << "Caught signal #" << (system_cmd.child_retcode - SHELL_RET_SIGNAL);
		ST_MAYBE_THROW(SystemCmdException(&system_cmd, msg.str()), system_cmd.do_throw());
	    }
	}
	else if (WIFSIGNALED(wstatus))
	{
	    system_cmd.child_retcode = -127;
	    int sig = WTERMSIG(wstatus);
	    y2err("child caught signal " << sig /* << " (" << sigabbrev_np(sig) << ")" */);
	    ST_MAYBE_THROW(SystemCmdException(&system_cmd, "Command failed (signaled)"), system_cmd.do_throw());
	}
	else
	{
	    system_cmd.child_retcode = -127;
	    ST_MAYBE_THROW(SystemCmdException(&system_cmd, "Command failed"), system_cmd.do_throw());
	}

	y2mil("child_retcode:" << system_cmd.child_retcode);
    }


    void
    SystemCmd::Executor::write_stdin()
    {
	const char* p = system_cmd.options.stdin_text.data();

	while (true)
	{
	    const size_t count = min(system_cmd.options.stdin_text.size() - stdin_pos, (string::size_type)(1024));
	    if (count == 0)
	    {
		stdin_pipe.write_end.close();
		pollfds[0].fd = -1;
		break;
	    }

	    ssize_t write_ret = TEMP_FAILURE_RETRY(write(stdin_pipe.write_end.fd, &p[stdin_pos], count));
	    if (write_ret < 0)
	    {
		if (errno == EAGAIN)
		    break;

		SYSCALL_FAILED("write");
	    }

	    stdin_pos += write_ret;
	}
    }


    void
    SystemCmd::Executor::read_stdout()
    {
	fill_buffer("stdout", stdout_pipe.read_end.fd, stdout_buffer, system_cmd.stdout_lines);
    }


    void
    SystemCmd::Executor::read_stderr()
    {
	fill_buffer("stderr", stderr_pipe.read_end.fd, stderr_buffer, system_cmd.stderr_lines);
    }


    void
    SystemCmd::Executor::fill_buffer(const char* name, int fd, string& buffer, vector<string>& lines) const
    {
	// We need a loop here to read all data from fd since poll can set POLLIN and
	// POLLHUP at the same time. Otherwise we can loose data.

	char buffer2[1024];

	while (true)
	{
	    ssize_t read_ret = TEMP_FAILURE_RETRY(read(fd, buffer2, sizeof(buffer2)));
	    if (read_ret < 0)
	    {
		if (errno == EAGAIN)
		    break;

		SYSCALL_FAILED("read");
	    }

	    buffer.append(buffer2, read_ret);

	    split_buffer(name, buffer, lines);

	    if ((size_t)(read_ret) < sizeof(buffer2))
		break;
	}
    }


    void
    SystemCmd::Executor::split_buffer(const char* name, string& buffer, vector<string>& lines, bool finito) const
    {
	while (true)
	{
	    string::size_type pos = buffer.find('\n');
	    if (pos == string::npos)
		break;

	    add_line(name, buffer.substr(0, pos), lines);
	    buffer.erase(0, pos + 1);
	}

	if (finito && !buffer.empty())
	    add_line(name, buffer, lines);
    }


    void
    SystemCmd::Executor::add_line(const char* name, const string& line, vector<string>& lines) const
    {
	if (lines.size() < system_cmd.options.log_line_limit)
	    y2mil("line " << name << "[" << lines.size() << "] '" << line << "'");
	else
	    y2deb("line " << name << "[" << lines.size() << "] '" << line << "'");

	lines.push_back(line);
    }


    TmpForExec
    SystemCmd::Executor::make_args() const
    {
	vector<char*> args;

	for (const string& v : system_cmd.options.args)
	{
	    args.push_back(strdup(v.c_str()));
	}

	args.push_back(nullptr);

	return args;
    }


    TmpForExec
    SystemCmd::Executor::make_env() const
    {
	// Environment variables should be present only once in the environment.
	// https://pubs.opengroup.org/onlinepubs/009695399/basedefs/xbd_chap08.html

	vector<char*> env;

	for (char** v = environ; *v != NULL; ++v)
	    env.push_back(strdup(*v));

	for (const string& v : system_cmd.options.env)
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
