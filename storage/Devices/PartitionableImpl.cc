/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2021] SUSE LLC
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


#include <boost/algorithm/string.hpp>

#include "storage/Devices/PartitionableImpl.h"
#include "storage/Devices/PartitionTableImpl.h"
#include "storage/Devices/Msdos.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/DasdPt.h"
#include "storage/Devices/ImplicitPt.h"
#include "storage/Holders/User.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/Enum.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/CallbacksImpl.h"
#include "storage/SystemInfo/SystemInfoImpl.h"
#include "storage/StorageImpl.h"
#include "storage/EnvironmentImpl.h"
#include "storage/Prober.h"
#include "storage/Utils/Format.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Partitionable>::classname = "Partitionable";

    const unsigned int Partitionable::Impl::default_range;


    Partitionable::Impl::Impl(const xmlNode* node)
	: BlkDevice::Impl(node)
    {
	getChildValue(node, "range", range);
    }


    void
    Partitionable::Impl::check(const CheckCallbacks* check_callbacks) const
    {
	BlkDevice::Impl::check(check_callbacks);

	if (get_region().get_start() != 0)
	    ST_THROW(Exception("start of region of partitionable not zero"));
    }


    void
    Partitionable::Impl::save(xmlNode* node) const
    {
	BlkDevice::Impl::save(node);

	setChildValue(node, "range", range);
    }


    void
    Partitionable::Impl::probe_pass_1a(Prober& prober)
    {
	BlkDevice::Impl::probe_pass_1a(prober);

	BlkDevice::Impl::probe_size(prober);
	BlkDevice::Impl::probe_topology(prober);

	if (get_dm_table_name().empty())
	{
	    SystemInfo::Impl& system_info = prober.get_system_info();

	    const File& range_file = get_sysfs_file(system_info, "ext_range");
	    range = range_file.get<int>();
	}
    }


    void
    Partitionable::Impl::check_unambiguity(Prober& prober, PtType pt_type) const
    {
	const Blkid& blkid = prober.get_system_info().getBlkid();

	Blkid::const_iterator it = blkid.find_by_any_name(get_name(), prober.get_system_info());
	if (it == blkid.end())
	    return;

	if (it->second.is_fs)
	{
	    y2err("found filesystem next to partition table on " << get_name());

	    // TRANSLATORS: Error message displayed during probing,
	    // %1$s is replaced by the filesystem type (e.g. ext4),
	    // %2$s is replaced by the partition table type (e.g. GPT),
	    // %3$s is replaced by the device name (e.g. /dev/sda)
	    Text text = sformat(_("Detected a %1$s file system next to a partition table of type %2$s on the\n"
				  "device %3$s. The file system will be ignored."), toString(it->second.fs_type),
				toString(pt_type), get_name());

	    ambiguity_partition_table_and_filesystem_callback(prober.get_probe_callbacks(), text, get_name(), pt_type,
							      it->second.fs_type);
	}

	if (it->second.is_luks)
	{
	    y2err("found LUKS next to partition table on " << get_name());

	    // TRANSLATORS: Error message displayed during probing,
	    // %1$s is replaced by the partition table type (e.g. GPT),
	    // %2$s is replaced by the device name (e.g. /dev/sda)
	    Text text = sformat(_("Detected a LUKS device next to a partition table of type %1$s on the\n"
				  "device %2$s. The LUKS device will be ignored."), get_name());

	    ambiguity_partition_table_and_luks_callback(prober.get_probe_callbacks(), text, get_name(), pt_type);
	}

	if (it->second.is_lvm)
	{
	    y2err("found LVM logical volume next to partition table on " << get_name());

	    // TRANSLATORS: Error message displayed during probing,
	    // %1$s is replaced by the partition table type (e.g. GPT),
	    // %2$s is replaced by the device name (e.g. /dev/sda)
	    Text text = sformat(_("Detected a LVM logical volume next to a partition table of type %1$s on the\n"
				  "device %2$s. The LVM logical volume will be ignored as good as possible."),
				get_name());

	    ambiguity_partition_table_and_lvm_pv_callback(prober.get_probe_callbacks(), text, get_name(), pt_type);
	}
    }


    void
    Partitionable::Impl::probe_pass_1c(Prober& prober)
    {
	if (has_children() || !is_active() || get_size() == 0)
	    return;

	// do not run parted on host-managed zoned disks
	if (!is_usable_as_partitionable())
	    return;

	try
	{
	    SystemInfo::Impl& system_info = prober.get_system_info();

	    const Parted& parted = system_info.getParted(get_name());
	    PtType label = parted.get_label();

	    // Ignore MS-DOS partition table without any partitions if there is also an
	    // filesystem - likely the partition table is in fact only the MBR signature
	    // from instelled from grub (#see bsc #) 1186823.
	    if (label == PtType::MSDOS && parted.get_entries().empty() &&
		prefer_filesystem_over_empty_msdos())
	    {
		const Blkid& blkid = system_info.getBlkid();
		Blkid::const_iterator it = blkid.find_by_any_name(get_name(), system_info);
		if (it != blkid.end() && it->second.is_fs)
		    return;
	    }

	    if (label == PtType::MSDOS || label == PtType::GPT || label == PtType::DASD)
	    {
		if (get_region().get_block_size() != parted.get_region().get_block_size())
		    ST_THROW(Exception(sformat("different block size reported by kernel and parted for %s",
					       get_name())));

		if (get_region().get_length() != parted.get_region().get_length())
		    ST_THROW(Exception(sformat("different size reported by kernel and parted for %s",
					       get_name())));

		// parted reports DASD partition table for implicit partition
		// tables. Convert that to implicit partition table.

		if (is_dasd(get_non_impl()))
		{
		    const Dasd* dasd = to_dasd(get_non_impl());

		    if (dasd->get_type() == DasdType::ECKD && dasd->get_format() == DasdFormat::LDL)
			label = PtType::IMPLICIT;

		    if (dasd->get_type() == DasdType::FBA && parted.is_implicit())
			label = PtType::IMPLICIT;
		}

		PartitionTable* partition_table = create_partition_table(label);
		partition_table->get_impl().probe_pass_1c(prober);

		check_unambiguity(prober, label);
	    }

	    if (label != PtType::UNKNOWN && label != PtType::LOOP && label != PtType::MSDOS &&
		label != PtType::GPT && label != PtType::DASD && label != PtType::IMPLICIT)
	    {
		y2err("detected unsupported partition table " << toString(label) << " on " <<
		      get_name());

		// TRANSLATORS: Error message displayed during probing,
		// %1$s is replaced by the partition table type (e.g. BSD),
		// %2$s is replaced by the device name (e.g. /dev/sda)
		Text text = sformat(_("Detected an unsupported partition table of type %1$s on the\n"
				      "device %2$s."), toString(label), get_name());

		unsupported_partition_table_callback(prober.get_probe_callbacks(), text, get_name(), label);
	    }
	}
	catch (const Exception& exception)
	{
	    // TRANSLATORS: error message
	    prober.handle(exception, sformat(_("Probing partitions on %s failed"), get_name()), 0);
	}
    }


    PtType
    Partitionable::Impl::get_default_partition_table_type() const
    {
	vector<PtType> possible = get_possible_partition_table_types();

	if (possible.empty())
	    ST_THROW(Exception("no partition table possible"));

	PtType ret = possible.front();

	y2mil("ret:" << toString(ret));

	return ret;
    }


    vector<PtType>
    Partitionable::Impl::get_possible_partition_table_types() const
    {
	// ECKD DASDs are handled completely and FBA DASDs partly in Dasd
	// class.

	unsigned long long int num_sectors = get_region().get_length();
	bool size_ok_for_msdos = num_sectors <= UINT32_MAX;
	y2mil("num-sectors:" << num_sectors << " size-ok-for-msdos:" << size_ok_for_msdos);

	PtType best = PtType::GPT;
	vector<PtType> ret = { best };

	if (size_ok_for_msdos)
	    ret.push_back(PtType::MSDOS);

	return ret;
    }


    PartitionTable*
    Partitionable::Impl::create_partition_table(PtType pt_type)
    {
	if (num_children() != 0)
	    ST_THROW(WrongNumberOfChildren(num_children(), 0));

	PartitionTable* ret = nullptr;

	switch (pt_type)
	{
	    case PtType::MSDOS:
		ret = Msdos::create(get_devicegraph());
		break;

	    case PtType::GPT:
		ret = Gpt::create(get_devicegraph());
		break;

	    case PtType::DASD:
		ret = DasdPt::create(get_devicegraph());
		break;

	    case PtType::IMPLICIT:
		ret = ImplicitPt::create(get_devicegraph());
		break;

	    default:
		ST_THROW(UnsupportedException("unsupported partition table type " + toString(pt_type)));
	}

	User::create(get_devicegraph(), get_non_impl(), ret);

	if (pt_type == PtType::IMPLICIT)
	{
	    to_implicit_pt(ret)->create_implicit_partition();
	}

	return ret;
    }


    bool
    Partitionable::Impl::has_partition_table() const
    {
	return has_single_child_of_type<const PartitionTable>();
    }


    PartitionTable*
    Partitionable::Impl::get_partition_table()
    {
	return get_single_child_of_type<PartitionTable>();
    }


    const PartitionTable*
    Partitionable::Impl::get_partition_table() const
    {
	return get_single_child_of_type<const PartitionTable>();
    }


    string
    Partitionable::Impl::partition_name(int number) const
    {
	if (boost::starts_with(get_name(), DEV_MAPPER_DIR "/") &&
	    os_flavour() == OsFlavour::SUSE)
	    return get_name() + "-part" + to_string(number);
	else if (isdigit(get_name().back()))
	    return get_name() + "p" + to_string(number);
	else
	    return get_name() + to_string(number);
    }


    bool
    Partitionable::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!BlkDevice::Impl::equal(rhs))
	    return false;

	return range == rhs.range;
    }


    void
    Partitionable::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	BlkDevice::Impl::log_diff(log, rhs);

	storage::log_diff(log, "range", range, rhs.range);
    }


    void
    Partitionable::Impl::print(std::ostream& out) const
    {
	BlkDevice::Impl::print(out);

	out << " range:" << range;
    }

}
