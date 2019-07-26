/*
 * Copyright (c) [2004-2014] Novell, Inc.
 * Copyright (c) 2019 SUSE LLC
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


#include <regex>

#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/AppUtil.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/SystemInfo/CmdCryptsetup.h"
#include "storage/Devices/EncryptionImpl.h"


namespace storage
{
    using namespace std;


    CmdCryptsetupStatus::CmdCryptsetupStatus(const string& name)
	: name(name), encryption_type(EncryptionType::UNKNOWN)
    {
	SystemCmd cmd(CRYPTSETUPBIN " status " + quote(name));
	if (cmd.retcode() == 0 && !cmd.stdout().empty())
	    parse(cmd.stdout());
    }


    void
    CmdCryptsetupStatus::parse(const vector<string>& lines)
    {
	string type, cipher, keysize;
	for (const string& line : lines)
	{
	    string key = extractNthWord(0, line);
	    if (key == "type:")
		type = extractNthWord(1, line);
	    else if (key == "cipher:")
		cipher = extractNthWord(1, line);
	    else if(key == "keysize:")
		keysize = extractNthWord(1, line);
	}

	if (type == "LUKS1")
	    encryption_type = EncryptionType::LUKS1;
	else if (type == "LUKS2")
	    encryption_type = EncryptionType::LUKS2;
	else if (cipher == "twofish-cbc-plain")
	    encryption_type = EncryptionType::TWOFISH;
	else if (cipher == "twofish-cbc-null" && keysize == "192")
	    encryption_type = EncryptionType::TWOFISH_OLD;
	else if (cipher == "twofish-cbc-null" && keysize == "256")
	    encryption_type = EncryptionType::TWOFISH256_OLD;
	else
	{
	    encryption_type = EncryptionType::UNKNOWN;
	    y2err("unknown encryption type:" << type << " cipher:" << cipher << " keysize:" <<
		  keysize);
	}

	y2mil(*this);
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdCryptsetupStatus& cmd_cryptsetup_status)
    {
	s << "name:" << cmd_cryptsetup_status.name << " encryption-type:"
	  << toString(cmd_cryptsetup_status.encryption_type);

	return s;
    }


    CmdCryptsetupLuksDump::CmdCryptsetupLuksDump(const string& name)
	: name(name), encryption_type(EncryptionType::UNKNOWN)
    {
	SystemCmd cmd(CRYPTSETUPBIN " luksDump " + quote(name));
	if (cmd.retcode() == 0 && !cmd.stdout().empty())
	    parse(cmd.stdout());
    }


    void
    CmdCryptsetupLuksDump::parse(const vector<string>& lines)
    {
	static const regex version_regex("Version:[ \t]*([0-9]+)[ \t]*", regex::extended);

	smatch match;

	for (const string& line : lines)
	{
	    if (regex_match(line, match, version_regex) && match.size() == 2)
	    {
		if (match[1] == "1")
		    encryption_type = EncryptionType::LUKS1;
		else if (match[1] == "2")
		    encryption_type = EncryptionType::LUKS2;
		else
		    y2err("unknown encryption type:" << match[1]);
	    }
	}

	y2mil(*this);
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdCryptsetupLuksDump& cmd_cryptsetup_luks_dump)
    {
	s << "name:" << cmd_cryptsetup_luks_dump.name << " encryption-type:"
	  << toString(cmd_cryptsetup_luks_dump.encryption_type);

	return s;
    }

}
