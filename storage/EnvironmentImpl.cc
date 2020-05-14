/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#include <string.h>
#include <ostream>

#include "storage/EnvironmentImpl.h"


namespace storage
{

    Environment::Impl::Impl(bool read_only, ProbeMode probe_mode, TargetMode target_mode)
	: read_only(read_only), probe_mode(probe_mode), target_mode(target_mode)
    {
    }


    Environment::Impl::~Impl()
    {
    }


    void
    Environment::Impl::set_devicegraph_filename(const string& devicegraph_filename)
    {
	Impl::devicegraph_filename = devicegraph_filename;
    }


    void
    Environment::Impl::set_arch_filename(const string& arch_filename)
    {
	Impl::arch_filename = arch_filename;
    }


    void
    Environment::Impl::set_mockup_filename(const string& mockup_filename)
    {
	Impl::mockup_filename = mockup_filename;
    }


    bool
    Environment::Impl::is_do_lock() const
    {
	switch (probe_mode)
	{
	    case ProbeMode::STANDARD:
	    case ProbeMode::STANDARD_WRITE_DEVICEGRAPH:
	    case ProbeMode::STANDARD_WRITE_MOCKUP:
		return true;

	    case ProbeMode::NONE:
	    case ProbeMode::READ_DEVICEGRAPH:
	    case ProbeMode::READ_MOCKUP:
		return false;
	}

	return true;
    }


    std::ostream&
    operator<<(std::ostream& out, const Environment::Impl& environment)
    {
	return out << "read-only:" << environment.read_only << " probe-mode:"
		   << toString(environment.probe_mode) << " target-mode:"
		   << toString(environment.target_mode);
    }


    const vector<string> EnumTraits<ProbeMode>::names({
	"STANDARD", "STANDARD_WRITE_DEVICEGRAPH", "STANDARD_WRITE_MOCKUP", "NONE",
	"READ_DEVICEGRAPH", "READ_MOCKUP"
    });


    const vector<string> EnumTraits<TargetMode>::names({
	"DIRECT", "CHROOT", "IMAGE"
    });


    bool
    support_btrfs_multiple_devices()
    {
	const char* p = getenv("YAST_MULTIPLE_DEVICES_BTRFS");
	return !p || strcmp(p, "yes") == 0;
    }


    bool
    support_btrfs_snapshot_relations()
    {
	const char* p = getenv("YAST_BTRFS_SNAPSHOT_RELATIONS");
	return p && strcmp(p, "yes") == 0;
    }


    bool
    developer_mode()
    {
	const char* p = getenv("LIBSTORAGE_DEVELOPER_MODE");
	return p && strcmp(p, "yes") == 0;
    }

}
