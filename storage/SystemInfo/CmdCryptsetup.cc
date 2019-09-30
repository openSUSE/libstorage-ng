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
#include "storage/Utils/StorageTmpl.h"
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
	: name(name), encryption_type(EncryptionType::UNKNOWN), cipher(), key_size(0)
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

	switch (encryption_type)
	{
	    case EncryptionType::LUKS1:
		parse_version1(lines);
		break;

	    case EncryptionType::LUKS2:
		parse_version2(lines);
		break;

	    default:
		y2err("failed to parse luks version in cryptsetup output");
	}

	y2mil(*this);
    }


    void
    CmdCryptsetupLuksDump::parse_version1(const vector<string>& lines)
    {
	static const regex cipher_name_regex("Cipher name:[ \t]*([^ \t]+)[ \t]*", regex::extended);
	static const regex cipher_mode_regex("Cipher mode:[ \t]*([^ \t]+)[ \t]*", regex::extended);
	static const regex mk_bits_regex("MK bits:[ \t]*([0-9]+)[ \t]*", regex::extended);

	string cipher_name, cipher_mode;

	smatch match;

	for (const string& line : lines)
	{
	    if (regex_match(line, match, cipher_name_regex) && match.size() == 2)
		cipher_name = match[1];

	    if (regex_match(line, match, cipher_mode_regex) && match.size() == 2)
		cipher_mode = match[1];

	    if (regex_match(line, match, mk_bits_regex) && match.size() == 2)
	    {
		match[1] >> key_size;
		key_size /= 8;
	    }
	}

	if (cipher_name.empty() || cipher_mode.empty())
	    y2err("failed to parse cipher in cryptsetup output");
	else
	    cipher = cipher_name + "-" + cipher_mode;
    }


    void
    CmdCryptsetupLuksDump::parse_version2(const vector<string>& lines)
    {
	/*
	 * For the meaning of the output of luksDump for LUKS2 have a
	 * look at the on-disk format, e.g.
	 * https://gitlab.com/cryptsetup/cryptsetup/blob/master/docs/on-disk-format-luks2.pdf.
	 */

	static const regex data_section_regex("Data segments:[ \t]*", regex::extended);
	static const regex keyslot_section_regex("Keyslots:[ \t]*", regex::extended);
	static const regex token_section_regex("Tokens:[ \t]*", regex::extended);
	static const regex digest_section_regex("Digests:[ \t]*", regex::extended);

	static const regex cipher_regex("[ \t]*cipher:[ \t]*([^ \t]+)[ \t]*", regex::extended);

	static const regex cipher_key_regex("[ \t]*Key:[ \t]*([0-9]+) bits[ \t]*", regex::extended);

	enum { DATA_SECTION, KEYSLOT_SECTION, UNUSED_SECTION } section = UNUSED_SECTION;

	smatch match;

	for (const string& line : lines)
	{
	    if (line.empty())
		section = UNUSED_SECTION;
	    else if (regex_match(line, match, data_section_regex))
		section = DATA_SECTION;
	    else if (regex_match(line, match, keyslot_section_regex))
		section = KEYSLOT_SECTION;
	    else if (regex_match(line, match, token_section_regex))
		section = UNUSED_SECTION;
	    else if (regex_match(line, match, digest_section_regex))
		section = UNUSED_SECTION;

	    switch (section)
	    {
		case DATA_SECTION:
		{
		    if (regex_match(line, match, cipher_regex) && match.size() == 2)
			cipher = match[1];
		}
		break;

		case KEYSLOT_SECTION:
		{
		    if (regex_match(line, match, cipher_key_regex) && match.size() == 2)
		    {
			match[1] >> key_size;
			key_size /= 8;
		    }
		}
		break;

		case UNUSED_SECTION:
		    break;
	    }
	}

	if (cipher.empty())
	    y2err("failed to parse cipher in cryptsetup output");
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdCryptsetupLuksDump& cmd_cryptsetup_luks_dump)
    {
	s << "name:" << cmd_cryptsetup_luks_dump.name << " encryption-type:"
	  << toString(cmd_cryptsetup_luks_dump.encryption_type) << " cipher:"
	  << cmd_cryptsetup_luks_dump.cipher << " key-size:"
	  << cmd_cryptsetup_luks_dump.key_size;

	return s;
    }

}
