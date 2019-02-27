/*
 * Copyright (c) 2015 Novell, Inc.
 * Copyright (c) [2016-2019] SUSE LLC
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


#include "storage/Utils/Mockup.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/ExceptionImpl.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/Format.h"


namespace storage
{

    void
    Mockup::load(const string& filename)
    {
	XmlFile xml(filename);

	const xmlNode* root_node = xml.getRootElement();
	if (!root_node)
	    ST_THROW(Exception("root node not found"));

	const xmlNode* mockup_node = getChildNode(root_node, "Mockup");
	if (!mockup_node)
	    ST_THROW(Exception("Mockup node not found"));

	const xmlNode* commands_node = getChildNode(mockup_node, "Commands");
	if (commands_node)
	{
	    for (const xmlNode* command_node : getChildNodes(commands_node))
	    {
		string name;
		getChildValue(command_node, "name", name);

		Command command;
		getChildValue(command_node, "stdout", command.stdout);
		getChildValue(command_node, "stderr", command.stderr);
		getChildValue(command_node, "exit-code", command.exit_code);

		if (!commands.emplace(name, command).second)
		    ST_THROW(Exception(sformat("command \"%s\" already loaded for mockup", name)));
	    }
	}

	const xmlNode* files_node = getChildNode(mockup_node, "Files");
	if (files_node)
	{
	    for (const xmlNode* file_node : getChildNodes(files_node))
	    {
		string name;
		getChildValue(file_node, "name", name);

		File file;
		getChildValue(file_node, "content", file.content);

		if (!files.emplace(name, file).second)
		    ST_THROW(Exception(sformat("file \"%s\" already loaded for mockup", name)));
	    }
	}
    }


    void
    Mockup::save(const string& filename)
    {
	XmlFile xml;

	xmlNode* mockup_node = xmlNewNode("Mockup");
	xml.setRootElement(mockup_node);

	xmlNode* comment = xmlNewComment(string(" " + generated_string() + " ").c_str());
	xmlAddPrevSibling(mockup_node, comment);

	if (!commands.empty())
	{
	    xmlNode* commands_node = xmlNewChild(mockup_node, "Commands");

	    for (const map<string, Command>::value_type& it : commands)
	    {
		xmlNode* command_node = xmlNewChild(commands_node, "Command");

		setChildValue(command_node, "name", it.first);
		setChildValue(command_node, "stdout", it.second.stdout);
		setChildValue(command_node, "stderr", it.second.stderr);
		setChildValueIf(command_node, "exit-code", it.second.exit_code, it.second.exit_code != 0);
	    }
	}

	if (!files.empty())
	{
	    xmlNode* files_node = xmlNewChild(mockup_node, "Files");

	    for (const map<string, File>::value_type& it : files)
	    {
		xmlNode* file_node = xmlNewChild(files_node, "File");

		setChildValue(file_node, "name", it.first);
		setChildValue(file_node, "content", it.second.content);
	    }
	}

	xml.save(filename);
    }


    bool
    Mockup::has_command(const string& name)
    {
	return commands.find(name) != commands.end();
    }


    const Mockup::Command&
    Mockup::get_command(const string& name)
    {
	map<string, Command>::const_iterator it = commands.find(name);
	if (it == commands.end())
	    ST_THROW(Exception("no mockup found for command '" + name + "'"));

#ifdef OCCAMS_RAZOR
	used_commands.insert(name);
#endif

	return it->second;
    }


    void
    Mockup::set_command(const string& name, const Command& command)
    {
	commands[name] = command;
    }


    void
    Mockup::erase_command(const string& name)
    {
	commands.erase(name);
    }


    bool
    Mockup::has_file(const string& name)
    {
	return files.find(name) != files.end();
    }


    const Mockup::File&
    Mockup::get_file(const string& name)
    {
	map<string, File>::const_iterator it = files.find(name);
	if (it == files.end())
	    ST_THROW(Exception("no mockup found for file '" + name + "'"));

#ifdef OCCAMS_RAZOR
	used_files.insert(name);
#endif

	return it->second;
    }


    void
    Mockup::set_file(const string& name, const File& file)
    {
	files[name] = file;
    }


    void
    Mockup::erase_file(const string& name)
    {
	files.erase(name);
    }


    void
    Mockup::occams_razor()
    {
#ifdef OCCAMS_RAZOR

	bool ok = true;

	for (const map<string, Command>::value_type& tmp : commands)
	{
	    if (used_commands.count(tmp.first) == 0)
	    {
		y2err("unused command mockup '" << tmp.first << "'");
		ok = false;
	    }
	}

	for (const map<string, File>::value_type& tmp : files)
	{
	    if (used_files.count(tmp.first) == 0)
	    {
		y2err("unused file mockup '" << tmp.first << "'");
		ok = false;
	    }
	}

	if (!ok)
	    ST_THROW(Exception("Occam's Razor"));

#endif
    }


    Mockup::Mode Mockup::mode = Mockup::Mode::NONE;

    map<string, Mockup::Command> Mockup::commands;
    map<string, Mockup::File> Mockup::files;

#ifdef OCCAMS_RAZOR
    set<string> Mockup::used_commands;
    set<string> Mockup::used_files;
#endif

}
