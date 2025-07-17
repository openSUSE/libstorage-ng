/*
 * Copyright (c) [2004-2010] Novell, Inc.
 * Copyright (c) 2017 SUSE LLC
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

#include "storage/Utils/AsciiFile.h"


#define ETC_MDADM "/etc/mdadm.conf"


namespace storage
{

    class Storage;


    class EtcMdadm
    {

    public:

	EtcMdadm(const string& filename = ETC_MDADM);

	/**
	 * From this structure line 'ARRAY' is build in the config file. The
	 * UUID is mandatory.
	 */
	struct Entry
	{
	    string device;
	    string uuid;
	    string metadata;

	    string container_uuid;
	    string container_member;

	    friend std::ostream& operator<<(std::ostream& s, const Entry& entry);
	};

	void init(const Storage* storage);

	bool has_entry(const string& uuid) const;

	bool update_entry(const Entry& entry);

	bool remove_entry(const string& uuid);

    protected:

	void set_device_line(const string& line);
	void set_auto_line(const string& line);
	void set_array_line(const string& line, const string& uuid);

	string array_line(const Entry& entry) const;

	vector<string>::iterator find_array(const string& uuid);
	vector<string>::const_iterator find_array(const string& uuid) const;

	string get_uuid(const string& line) const;

	bool has_iscsi(const Storage* storage) const;

	AsciiFile mdadm;

    };

}


#endif
