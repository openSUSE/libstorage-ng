/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2017] SUSE LLC
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


#include <iostream>

#include "storage/Utils/HumanString.h"
#include "storage/Devices/DasdPtImpl.h"
#include "storage/Devices/Partitionable.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/XmlFile.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<DasdPt>::classname = "DasdPt";


    DasdPt::Impl::Impl(const xmlNode* node)
	: PartitionTable::Impl(node)
    {
    }


    void
    DasdPt::Impl::probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo)
    {
	PartitionTable::Impl::probe_pass_1(probed, systeminfo);
    }


    void
    DasdPt::Impl::save(xmlNode* node) const
    {
	PartitionTable::Impl::save(node);
    }


    Region
    DasdPt::Impl::get_usable_region() const
    {
	Region device_region = get_partitionable()->get_region();

	// TODO

	unsigned long long first_usable_sector = 24;
	unsigned long long last_usable_sector = device_region.get_end();
	Region usable_region(first_usable_sector, last_usable_sector - first_usable_sector + 1,
			     device_region.get_block_size());

	return device_region.intersection(usable_region);
    }


    bool
    DasdPt::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!PartitionTable::Impl::equal(rhs))
	    return false;

	return true;
    }


    void
    DasdPt::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	PartitionTable::Impl::log_diff(log, rhs);
    }


    void
    DasdPt::Impl::print(std::ostream& out) const
    {
	PartitionTable::Impl::print(out);
    }


    bool
    DasdPt::Impl::is_partition_type_supported(PartitionType type) const
    {
	return type == PartitionType::PRIMARY;
    }


    bool
    DasdPt::Impl::is_partition_id_supported(unsigned int id) const
    {
	static const vector<unsigned int> supported_ids = {
	    ID_UNKNOWN, ID_LINUX, ID_LVM, ID_RAID
	};

	return contains(supported_ids, id);
    }


    unsigned int
    DasdPt::Impl::max_primary() const
    {
	return min(3U, get_partitionable()->get_range());
    }


    Text
    DasdPt::Impl::do_create_text(Tense tense) const
    {
	const Partitionable* partitionable = get_partitionable();

	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda)
			   _("Create DASD partition table on %1$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda)
			   _("Creating DASD partition table on %1$s"));

	return sformat(text, partitionable->get_displayname().c_str());
    }


    void
    DasdPt::Impl::do_create()
    {
	const Partitionable* partitionable = get_partitionable();

	string cmd_line = PARTEDBIN " --script " + quote(partitionable->get_name()) + " mklabel dasd";
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create DasdPt failed"));
    }


    Text
    DasdPt::Impl::do_delete_text(Tense tense) const
    {
	const Partitionable* partitionable = get_partitionable();

	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda)
			   _("Delete DASD partition table on %1$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda)
			   _("Deleting DASD partition table on %1$s"));

	return sformat(text, partitionable->get_displayname().c_str());
    }

}
