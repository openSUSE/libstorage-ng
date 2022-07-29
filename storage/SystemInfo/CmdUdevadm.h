/*
 * Copyright (c) 2015 Novell, Inc.
 * Copyright (c) [2018-2020] SUSE LLC
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


#ifndef STORAGE_CMD_UDEVADM_H
#define STORAGE_CMD_UDEVADM_H


#include <sys/sysmacros.h>

#include <string>
#include <vector>

#include "storage/Utils/Enum.h"


namespace storage
{
    using std::string;
    using std::vector;


    class CmdUdevadmInfo
    {

    public:

	enum class DeviceType { UNKNOWN, DISK, PARTITION };

	CmdUdevadmInfo(const string& file);

	const string& get_path() const { return path; }
	const string& get_name() const { return name; }

	dev_t get_majorminor() const { return majorminor; }
	unsigned int get_major() const { return major(majorminor); }
	unsigned int get_minor() const { return minor(majorminor); }

	DeviceType get_device_type() const { return device_type; }

	const vector<string>& get_by_path_links() const { return by_path_links; }
	const vector<string>& get_by_id_links() const { return by_id_links; }
	const vector<string>& get_by_partuuid_links() const { return by_partuuid_links; }

	friend std::ostream& operator<<(std::ostream& s, const CmdUdevadmInfo& cmd_udevadm_info);

    private:

	void parse(const vector<string>& stdout);

	string file;

	string path;
	string name;

	dev_t majorminor = 0;

	DeviceType device_type = DeviceType::UNKNOWN;

	vector<string> by_path_links;
	vector<string> by_id_links;
	vector<string> by_partlabel_links;
	vector<string> by_partuuid_links;

    };

    template <> struct EnumTraits<CmdUdevadmInfo::DeviceType> { static const vector<string> names; };

}


#endif
