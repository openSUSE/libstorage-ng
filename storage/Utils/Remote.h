#ifndef REMOTE_H
#define REMOTE_H


#include <string>
#include <vector>


// TODO the classnames are bad since the classes for also used in Mockup
// TODO swig does not support nested classes so here are currently ugly names

namespace storage
{

    struct RemoteCommand
    {
	RemoteCommand() : stdout(), stderr(), exit_code(0) {}
	RemoteCommand(const std::vector<std::string>& stdout)
	    : stdout(stdout), stderr(), exit_code(0) {}
	RemoteCommand(const std::vector<std::string>& stdout,
		      const std::vector<std::string>& stderr, int exit_code)
	    : stdout(stdout), stderr(stderr), exit_code(exit_code) {}

	std::vector<std::string> stdout;
	std::vector<std::string> stderr;
	int exit_code;
    };


    struct RemoteFile
    {
	RemoteFile() : content() {}
	RemoteFile(const std::vector<std::string>& content) : content(content) {}

	std::vector<std::string> content;
    };


    class RemoteCallbacks
    {
    public:

	virtual ~RemoteCallbacks() {}

	virtual RemoteCommand get_command(const std::string& name) const = 0;
	virtual RemoteFile get_file(const std::string& name) const = 0;

    };

    const RemoteCallbacks* get_remote_callbacks();
    void set_remote_callbacks(const RemoteCallbacks* remote_callbacks);

}


#endif
