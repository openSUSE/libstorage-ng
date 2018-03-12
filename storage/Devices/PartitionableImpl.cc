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


#include <boost/algorithm/string.hpp>

#include "storage/Devices/PartitionableImpl.h"
#include "storage/Devices/PartitionTableImpl.h"
#include "storage/Devices/Msdos.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/DasdPt.h"
#include "storage/Devices/ImplicitPt.h"
#include "storage/Holders/User.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/Enum.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/StorageImpl.h"
#include "storage/Prober.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Partitionable>::classname = "Partitionable";

    const unsigned int Partitionable::Impl::default_range;


    Partitionable::Impl::Impl(const xmlNode* node)
	: BlkDevice::Impl(node), topology(), range(0)
    {
	getChildValue(node, "topology", topology);
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

	setChildValue(node, "topology", topology);
	setChildValue(node, "range", range);
    }


    void
    Partitionable::Impl::probe_pass_1a(Prober& prober)
    {
	BlkDevice::Impl::probe_pass_1a(prober);

	const File size_file = prober.get_system_info().getFile(SYSFS_DIR + get_sysfs_path() + "/size");
	const File logical_block_size_file = prober.get_system_info().getFile(SYSFS_DIR + get_sysfs_path() +
									      "/queue/logical_block_size");

	// size is always in 512 byte blocks
	unsigned long long a = size_file.get<unsigned long long>();
	unsigned long long b = logical_block_size_file.get<unsigned long long>();
	unsigned long long c = a * 512 / b;
	set_region(Region(0, c, b));

	const File alignment_offset_file = prober.get_system_info().getFile(SYSFS_DIR + get_sysfs_path() +
									    "/alignment_offset");
	topology.set_alignment_offset(alignment_offset_file.get<int>());

	const File optimal_io_size_file = prober.get_system_info().getFile(SYSFS_DIR + get_sysfs_path() +
									   "/queue/optimal_io_size");
	topology.set_optimal_io_size(optimal_io_size_file.get<int>());

	if (get_dm_table_name().empty())
	{
	    const File range_file = prober.get_system_info().getFile(SYSFS_DIR + get_sysfs_path() +
								     "/ext_range");
	    range = range_file.get<int>();
	}
    }


    void
    Partitionable::Impl::probe_pass_1c(Prober& prober)
    {
	if (has_children() || !is_active() || get_size() == 0)
	    return;

	const Parted& parted = prober.get_system_info().getParted(get_name());
	if (parted.get_label() == PtType::MSDOS || parted.get_label() == PtType::GPT ||
	    parted.get_label() == PtType::DASD)
	{
	    if (get_region().get_block_size() != parted.get_region().get_block_size())
		ST_THROW(Exception("different block size reported by kernel and parted"));

	    if (get_region().get_length() != parted.get_region().get_length())
		ST_THROW(Exception("different size reported by kernel and parted"));

	    PtType label = parted.get_label();

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
	if (boost::starts_with(get_name(), DEV_MAPPER_DIR "/"))
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

	return topology == rhs.topology && range == rhs.range;
    }


    void
    Partitionable::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	BlkDevice::Impl::log_diff(log, rhs);

	storage::log_diff(log, "topology", topology, rhs.topology);
	storage::log_diff(log, "range", range, rhs.range);
    }


    void
    Partitionable::Impl::print(std::ostream& out) const
    {
	BlkDevice::Impl::print(out);

	out << " topology:" << topology
	    << " range:" << range;
    }

}
