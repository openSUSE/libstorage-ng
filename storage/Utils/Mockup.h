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

	struct Command
	{
	    Command() : exit_code(0) {}
	    Command(const vector<string>& stdout) : stdout(stdout), exit_code(0) {}

	    vector<string> stdout;
	    vector<string> stderr;
	    int exit_code;
	};

	struct File
	{
	    File() {}
	    File(const vector<string>& lines) : lines(lines) {}

	    vector<string> lines;
	};

	enum class Mode
	{
	    NONE, PLAYBACK, RECORD
	};

	static Mode get_mode() { return mode; }
	static void set_mode(Mode mode) { Mockup::mode = mode; }

	static const Command& get_command(const string& command);
	static void set_command(const string& command, const vector<string>& stdout);

	static const File& get_file(const string& file);
	static void set_file(const string& file, const vector<string>& lines);

    private:

	static Mode mode;

	static map<string, Command> commands;
	static map<string, File> files;

    };

}


#endif
