/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2023] SUSE LLC
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


#include "storage/Devices/MsdosImpl.h"
#include "storage/Devices/PartitionableImpl.h"
#include "storage/Devices/PartitionImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/Region.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/HumanString.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/Format.h"
#include "storage/SystemInfo/CmdParted.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Msdos>::classname = "Msdos";


    Msdos::Impl::Impl(const xmlNode* node)
	: PartitionTable::Impl(node)
    {
	getChildValue(node, "minimal-mbr-gap", minimal_mbr_gap);
    }


    string
    Msdos::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("MS-DOS Partition Table").translated;
    }


    void
    Msdos::Impl::save(xmlNode* node) const
    {
	PartitionTable::Impl::save(node);

	setChildValueIf(node, "minimal-mbr-gap", minimal_mbr_gap, minimal_mbr_gap !=
			default_minimal_mbr_gap);
    }


    void
    Msdos::Impl::delete_partition(Partition* partition)
    {
	PartitionType old_type = partition->get_type();
	unsigned int old_number = partition->get_number();

	PartitionTable::Impl::delete_partition(partition);

	// After deleting a logical partition the numbers of partitions with
	// higher numbers are shifted.
	if (old_type == PartitionType::LOGICAL)
	{
	    vector<Partition*> partitions = get_partitions();
            for (Partition* tmp : partitions)
	    {
		unsigned int number = tmp->get_number();
		if (number > old_number)
		    tmp->get_impl().set_number(number - 1);
	    }
	}
    }


    void
    Msdos::Impl::set_minimal_mbr_gap(unsigned long minimal_mbr_gap)
    {
	Impl::minimal_mbr_gap = minimal_mbr_gap;
    }


    pair<unsigned long long, unsigned long long>
    Msdos::Impl::unusable_sectors() const
    {
	Region region = get_partitionable()->get_region();

	// Reserve one sector or minimal-mbr-gap (per default 1 MiB) for the
	// MBR and the MBR gap, see
	// https://en.wikipedia.org/wiki/BIOS_boot_partition. Normally the
	// space for the MBR gap is unused anyway due to partition alignment
	// but for disks with an alignment offset it can be required to
	// explicitly reserve it.

	unsigned long long at_front = max(1ULL, region.to_blocks(minimal_mbr_gap));

	// The MS-DOS partition table can only address 2^32 sectors. We allow
	// a maximum of UINT32_MAX for length. As a consequence end is maximal
	// UINT32_MAX - 1 (assuming start can be 0).

	unsigned long long at_back = region.get_length() > UINT32_MAX ?
	    region.get_length() - UINT32_MAX : 0;

	return make_pair(at_front, at_back);
    }


    bool
    Msdos::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!PartitionTable::Impl::equal(rhs))
	    return false;

	return minimal_mbr_gap == rhs.minimal_mbr_gap;
    }


    void
    Msdos::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	PartitionTable::Impl::log_diff(log, rhs);

	storage::log_diff(log, "minimal-mbr-gap", minimal_mbr_gap, rhs.minimal_mbr_gap);
    }


    void
    Msdos::Impl::print(std::ostream& out) const
    {
	PartitionTable::Impl::print(out);

	if (minimal_mbr_gap != default_minimal_mbr_gap)
	    out << " minimal-mbr-gap:" << minimal_mbr_gap;
    }


    bool
    Msdos::Impl::is_partition_id_supported(unsigned int id) const
    {
	static const vector<unsigned int> supported_ids = {
	    ID_UNKNOWN, ID_LINUX, ID_SWAP, ID_LVM, ID_RAID, ID_IRST, ID_ESP, ID_PREP,
	    ID_DIAG
	};

	if (contains(supported_ids, id))
	    return true;

	// For more ids the type command of parted 3.6 or the old suse specific type flag
	// is needed.

	if (!CmdPartedVersion::supports_type_command() && !CmdPartedVersion::supports_old_type_flag())
	    return false;

	return id > 0 && id <= 255;
    }


    unsigned int
    Msdos::Impl::max_primary() const
    {
	return min(4U, get_partitionable()->get_range() - 1);
    }


    unsigned int
    Msdos::Impl::max_logical() const
    {
	unsigned int ret = min(Partitionable::Impl::default_range, get_partitionable()->get_range()) - 1;

	return ret < 5 ? 0 : ret;
    }


    bool
    Msdos::Impl::has_extended() const
    {
	vector<const Partition*> partitions = PartitionTable::Impl::get_partitions();
	return any_of(partitions.begin(), partitions.end(), [](const Partition* partition) {
	    return partition->get_type() == PartitionType::EXTENDED;
	});
    }


    unsigned int
    Msdos::Impl::num_logical() const
    {
	vector<const Partition*> partitions = get_partitions();
	return count_if(partitions.begin(), partitions.end(), [](const Partition* partition) {
	    return partition->get_type() == PartitionType::LOGICAL;
	});
    }


    const Partition*
    Msdos::Impl::get_extended() const
    {
	vector<const Partition*> partitions = PartitionTable::Impl::get_partitions();
	for (const Partition* partition : partitions)
	{
	    if (partition->get_type() == PartitionType::EXTENDED)
		return partition;
	}

	ST_THROW(Exception("has no extended partition"));
    }


    vector<Partition*>
    Msdos::Impl::get_partitions()
    {
	vector<Partition*> partitions = PartitionTable::Impl::get_partitions();

	for (Partition* partition : partitions)
	{
	    if (partition->get_type() == PartitionType::EXTENDED)
	    {
		vector<Partition*> logicals = get_logical_partitions(partition);
		partitions.insert(partitions.end(), logicals.begin(), logicals.end());
		break;
	    }
	}

	return partitions;
    }


    vector<const Partition*>
    Msdos::Impl::get_partitions() const
    {
	vector<const Partition*> partitions = PartitionTable::Impl::get_partitions();

	for (const Partition* partition : partitions)
	{
	    if (partition->get_type() == PartitionType::EXTENDED)
	    {
		vector<const Partition*> logicals = get_logical_partitions(partition);
		partitions.insert(partitions.end(), logicals.begin(), logicals.end());
		break;
	    }
	}

	return partitions;
    }


    vector<Partition*>
    Msdos::Impl::get_logical_partitions(Partition* partition)
    {
	if (partition->get_type() != PartitionType::EXTENDED)
	    ST_THROW(Exception("function called on wrong partition"));

	Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();

	return devicegraph.filter_devices_of_type<Partition>
	    (devicegraph.children(partition->get_impl().get_vertex()));
    }


    vector<const Partition*>
    Msdos::Impl::get_logical_partitions(const Partition* partition) const
    {
	if (partition->get_type() != PartitionType::EXTENDED)
	    ST_THROW(Exception("function called on wrong partition"));

	const Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();

	return devicegraph.filter_devices_of_type<const Partition>
	    (devicegraph.children(partition->get_impl().get_vertex()));
    }


    Text
    Msdos::Impl::do_create_text(Tense tense) const
    {
	const Partitionable* partitionable = get_partitionable();

	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda)
			   _("Create MS-DOS partition table on %1$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda)
			   _("Creating MS-DOS partition table on %1$s"));

	return sformat(text, partitionable->get_displayname());
    }


    void
    Msdos::Impl::do_create()
    {
	const Partitionable* partitionable = get_partitionable();

	partitionable->get_impl().discard_device();

	SystemCmd::Args cmd_args = { PARTED_BIN, "--script", partitionable->get_name(), "mklabel", "msdos" };

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);

	udev_settle();
    }


    Text
    Msdos::Impl::do_delete_text(Tense tense) const
    {
	const Partitionable* partitionable = get_partitionable();

	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda)
			   _("Delete MS-DOS partition table on %1$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda)
			   _("Deleting MS-DOS partition table on %1$s"));

	return sformat(text, partitionable->get_displayname());
    }

}
