

#include <assert.h>
#include <stdexcept>

#include "storage/Utils/Mockup.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    void
    Mockup::load(const string& filename)
    {
	XmlFile xml(filename);

	const xmlNode* root_node = xml.getRootElement();
	assert(root_node);

	const xmlNode* mockup_node = getChildNode(root_node, "Mockup");
	assert(mockup_node);

	const xmlNode* commands_node = getChildNode(mockup_node, "Commands");
	assert(commands_node);

	for (const xmlNode* command_node : getChildNodes(commands_node))
	{
	    string name;
	    getChildValue(command_node, "name", name);

	    Command command;
	    getChildValue(command_node, "stdout", command.stdout);
	    getChildValue(command_node, "stderr", command.stderr);
	    getChildValue(command_node, "exit-code", command.exit_code);

	    commands[name] = command;
	}

	const xmlNode* files_node = getChildNode(mockup_node, "Files");
	assert(files_node);

	for (const xmlNode* file_node : getChildNodes(files_node))
	{
	    string name;
	    getChildValue(file_node, "name", name);

	    File file;
	    getChildValue(file_node, "content", file.content);

	    files[name] = file;
	}
    }


    void
    Mockup::save(const string& filename)
    {
	XmlFile xml;

	xmlNode* mockup_node = xmlNewNode("Mockup");
	xml.setRootElement(mockup_node);

	xmlNode* commands_node = xmlNewChild(mockup_node, "Commands");

	for (const map<string, Command>::value_type& it : commands)
	{
	    xmlNode* command_node = xmlNewChild(commands_node, "Command");

	    setChildValue(command_node, "name", it.first);
	    setChildValue(command_node, "stdout", it.second.stdout);
	    setChildValue(command_node, "stderr", it.second.stderr);
	    setChildValueIf(command_node, "exit-code", it.second.exit_code, it.second.exit_code != 0);
	}

	xmlNode* files_node = xmlNewChild(mockup_node, "Files");

	for (const map<string, File>::value_type& it : files)
	{
	    xmlNode* file_node = xmlNewChild(files_node, "File");

	    setChildValue(file_node, "name", it.first);
	    setChildValue(file_node, "content", it.second.content);
	}

	xml.save(filename);
    }


    const Mockup::Command&
    Mockup::get_command(const string& command)
    {
	map<string, Command>::const_iterator it = commands.find(command);
	if (it == commands.end())
	    throw std::runtime_error("no mockup found for command '" + command + "'");

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
	    throw std::runtime_error("no mockup found for file '" + file + "'");

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
