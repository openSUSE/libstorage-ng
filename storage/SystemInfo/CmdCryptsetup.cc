/*
 * Copyright (c) [2004-2014] Novell, Inc.
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


#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/AppUtil.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/SystemInfo/CmdCryptsetup.h"
#include "storage/Devices/EncryptionImpl.h"


namespace storage
{
    using namespace std;


    CmdCryptsetup::CmdCryptsetup(const string& name)
	: encrypt_type(EncryptType::UNKNOWN), name(name)
    {
	SystemCmd c(CRYPTSETUPBIN " status " + quote(name));
	if (c.retcode() == 0 && !c.stdout().empty())
	    parse(c.stdout());
    }


    void
    CmdCryptsetup::parse(const vector<string>& lines)
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
	    encrypt_type = EncryptType::LUKS;
	else if (cipher == "twofish-cbc-plain")
	    encrypt_type = EncryptType::TWOFISH;
	else if (cipher == "twofish-cbc-null" && keysize == "192")
	    encrypt_type = EncryptType::TWOFISH_OLD;
	else if (cipher == "twofish-cbc-null" && keysize == "256")
	    encrypt_type = EncryptType::TWOFISH256_OLD;
	else
	{
	    encrypt_type = EncryptType::UNKNOWN;
	    y2err("unknown encryption type:" << type << " cipher:" << cipher << " keysize:" <<
		  keysize);
	}
    }


    std::ostream& operator<<(std::ostream& s, const CmdCryptsetup& cmdcryptsetup)
    {
	s << "name:" << cmdcryptsetup.name << " encrypt_type:"
	  << toString(cmdcryptsetup.encrypt_type);

	return s;
    }

}
