/*
 * Copyright (c) [2004-2010] Novell, Inc.
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


#ifndef STORAGE_ETC_MDADM_H
#define STORAGE_ETC_MDADM_H

#include <string>
#include <map>

#include "storage/Utils/AsciiFile.h"
#include "storage/Storage.h"


namespace storage
{

    class EtcMdadm
    {

    public:

	EtcMdadm(const Storage* sto, const string& prefix = "");

	// From this structure line 'ARRAY' will be build in config file.
	// Not all fields are mandatory
	// If container is present then container line will be build
	// before volume line.
	struct mdconf_info
	{
	    mdconf_info() : container_present(false) {}

	    string device;
	    string uuid;

	    bool container_present;

	    /* following members only valid if container_present is true */
	    string container_member;
	    string container_metadata;
	    string container_uuid;

	    friend std::ostream& operator<<(std::ostream& s, const mdconf_info& info);
	};

	bool updateEntry(const mdconf_info& info);

	bool removeEntry(const string& uuid);

    protected:

	void setDeviceLine(const string& line);
	void setAutoLine(const string& line);
	void setArrayLine(const string& line, const string& uuid);

	string ContLine(const mdconf_info& info) const;
	string ArrayLine(const mdconf_info& info) const;

	vector<string>::iterator findArray(const string& uuid);

	string getUuid(const string& line) const;

	const Storage* sto;

	AsciiFile mdadm;

    };

}


#endif
