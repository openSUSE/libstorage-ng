#ifndef MOCKUP_H
#define MOCKUP_H


#include <string>
#include <vector>
#include <map>


namespace storage
{
    using std::string;
    using std::vector;
    using std::map;


    class Mockup
    {
    public:

	enum class Mode
	{
	    NONE, PLAYBACK, RECORD
	};

	struct Command
	{
	    Command() : stdout(), stderr(), exit_code(0) {}
	    Command(const vector<string>& stdout) : stdout(stdout), stderr(), exit_code(0) {}
	    Command(const vector<string>& stdout, const vector<string>& stderr, int exit_code)
		: stdout(stdout), stderr(stderr), exit_code(exit_code) {}

	    vector<string> stdout;
	    vector<string> stderr;
	    int exit_code;
	};

	struct File
	{
	    File() : content() {}
	    File(const vector<string>& content) : content(content) {}

	    vector<string> content;
	};

	static Mode get_mode() { return mode; }
	static void set_mode(Mode mode) { Mockup::mode = mode; }

	static void load(const string& filename);
	static void save(const string& filename);

	static const Command& get_command(const string& name);
	static void set_command(const string& name, const Command& command);

	static const File& get_file(const string& name);
	static void set_file(const string& name, const File& file);

    private:

	static Mode mode;

	static map<string, Command> commands;
	static map<string, File> files;

    };

}


#endif
