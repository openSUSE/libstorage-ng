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
#include "storage/Devices/PartitionImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/XmlFile.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/AlignmentImpl.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<DasdPt>::classname = "DasdPt";


    DasdPt::Impl::Impl(const xmlNode* node)
	: PartitionTable::Impl(node)
    {
    }


    void
    DasdPt::Impl::probe_pass_1c(Prober& prober)
    {
	PartitionTable::Impl::probe_pass_1c(prober);
    }


    void
    DasdPt::Impl::save(xmlNode* node) const
    {
	PartitionTable::Impl::save(node);
    }


    void
    DasdPt::Impl::check(const CheckCallbacks* check_callbacks) const
    {
	PartitionTable::Impl::check(check_callbacks);

	// partitions must be ordered according to start sector on DASD partition tables

	vector<const Partition*> partitions = get_partitions();
	if (std::adjacent_find(partitions.begin(), partitions.end(),
			       [](const Partition* lhs, const Partition* rhs) {
				   return lhs->get_region() >= rhs->get_region();
			       }) != partitions.end())
	    ST_THROW(Exception("partitions not ordered on DASD partition table"));
    }


    Partition*
    DasdPt::Impl::create_partition(const string& name, const Region& region, PartitionType type)
    {
	Partition* partition = PartitionTable::Impl::create_partition(name, region, type);

	// After creating a partition the numbers of partitions with higher starting sector
	// are shifted.

	for (Partition* tmp : get_partitions())
	{
	    if (tmp->get_region() > region)
		tmp->get_impl().set_number(tmp->get_impl().get_number() + 1);
	}

	return partition;
    }


    void
    DasdPt::Impl::delete_partition(Partition* partition)
    {
	Region region = partition->get_region();

	PartitionTable::Impl::delete_partition(partition);

	// After deleting a partition the numbers of partitions with higher starting sector
	// are shifted.

	for (Partition* tmp : get_partitions())
	{
	    if (tmp->get_region() > region)
		tmp->get_impl().set_number(tmp->get_impl().get_number() - 1);
	}
    }


    Region
    DasdPt::Impl::get_usable_region() const
    {
	const unsigned long sectors_per_track = 12; // TODO

	Region device_region = get_partitionable()->get_region();

	// The first two tracks are unusable for partitions.

	unsigned long long first_usable_sector = 2 * sectors_per_track;
	unsigned long long last_usable_sector = device_region.get_end();
	Region usable_region(first_usable_sector, last_usable_sector - first_usable_sector + 1,
			     device_region.get_block_size());

	return device_region.intersection(usable_region);
    }


    Alignment
    DasdPt::Impl::get_alignment(AlignType align_type) const
    {
	const unsigned long sectors_per_track = 12; // TODO

	Region device_region = get_partitionable()->get_region();

	// Also align to tracks.

	Alignment alignment(PartitionTable::Impl::get_alignment(align_type));
	alignment.get_impl().set_extra_grain(sectors_per_track * device_region.get_block_size());
	return alignment;
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
	    ID_UNKNOWN, ID_LINUX, ID_LVM, ID_RAID, ID_SWAP
	};

	return contains(supported_ids, id);
    }


    unsigned int
    DasdPt::Impl::max_primary() const
    {
	return min(3U, get_partitionable()->get_range() - 1);
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

	SystemCmd(UDEVADMBIN_SETTLE);
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
