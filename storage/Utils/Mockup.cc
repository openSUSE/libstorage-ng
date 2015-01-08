

#include <stdexcept>

#include "storage/Utils/Mockup.h"


namespace storage
{

    const Mockup::Command&
    Mockup::get_command(const string& command)
    {
	map<string, Command>::const_iterator it = commands.find(command);
	if (it == commands.end())
	    throw std::runtime_error("no command mockup found for " + command);

	return it->second;
    }


    void
    Mockup::set_command(const string& command, const vector<string>& stdout)
    {
	commands[command] = Command(stdout);
    }


    const Mockup::File&
    Mockup::get_file(const string& file)
    {
	map<string, File>::const_iterator it = files.find(file);
	if (it == files.end())
	    throw std::runtime_error("no file mockup found for " + file);

	return it->second;
    }


    void
    Mockup::set_file(const string& file, const vector<string>& lines)
    {
	files[file] = File(lines);
    }


    Mockup::Mode Mockup::mode = Mockup::Mode::NONE;

    map<string, Mockup::Command> Mockup::commands;
    map<string, Mockup::File> Mockup::files;

}
