#ifndef STORAGE_MOCKUP_H
#define STORAGE_MOCKUP_H


#include <string>
#include <vector>
#include <map>

#include "storage/Utils/Remote.h"


namespace storage
{
    using std::string;
    using std::vector;
    using std::map;


    class Mockup
    {
    public:

	typedef RemoteCommand Command;
	typedef RemoteFile File;

	enum class Mode
	{
	    NONE, PLAYBACK, RECORD
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
