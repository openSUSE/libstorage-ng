/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2022] SUSE LLC
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

#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/Format.h"
#include "storage/Devices/PartitionImpl.h"
#include "storage/Devicegraph.h"
#include "storage/SystemInfo/SystemInfoImpl.h"


namespace storage
{

    using namespace std;


    string
    get_partition_type_name(PartitionType partition_type)
    {
	return toString(partition_type);
    }


    string
    get_partition_id_name(IdNum partition_id)
    {
	return id_to_text(partition_id).translated;
    }


    IdNum
    get_linux_partition_id(LinuxPartitionIdCategory linux_partition_id_category, SystemInfo& system_info)
    {
	static const map<string, vector<IdNum>> arch_to_ids = {
	    { "aarch64", { ID_LINUX_ROOT_AARCH64, ID_LINUX_USR_AARCH64 } },
	    { "i586", { ID_LINUX_ROOT_X86, ID_LINUX_USR_X86 } },
	    { "ppc", { ID_LINUX_ROOT_PPC32,ID_LINUX_USR_PPC32 } },
	    { "ppc64", { ID_LINUX_ROOT_PPC64BE, ID_LINUX_USR_PPC64BE } },
	    { "ppc64le", { ID_LINUX_ROOT_PPC64LE, ID_LINUX_USR_PPC64LE } },
	    { "riscv64", { ID_LINUX_ROOT_RISCV64, ID_LINUX_USR_RISCV64 } },
	    { "s390", { ID_LINUX_ROOT_S390, ID_LINUX_USR_S390 } },
	    { "s390x", { ID_LINUX_ROOT_S390X, ID_LINUX_USR_S390X } },
	    { "x86_64", { ID_LINUX_ROOT_X86_64, ID_LINUX_USR_X86_64 } },
	};

	const string& arch = system_info.get_impl().getArch().get_arch();
	const map<string, vector<IdNum>>::const_iterator it = arch_to_ids.find(arch);
	if (it == arch_to_ids.end())
	    ST_THROW(Exception(sformat("partition id unknown for architecture %s", arch)));

	return it->second[(int)(linux_partition_id_category)];
    }


    Partition*
    Partition::create(Devicegraph* devicegraph, const string& name, const Region& region, PartitionType type)
    {
	if (!boost::starts_with(name, DEV_DIR "/"))
	    ST_THROW(Exception("invalid partition name"));

	Partition* ret = new Partition(new Partition::Impl(name, region, type));
	ret->Device::create(devicegraph);
	return ret;
    }


    Partition*
    Partition::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	Partition* ret = new Partition(new Partition::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    Partition::Partition(Impl* impl)
	: BlkDevice(impl)
    {
    }


    Partition*
    Partition::clone() const
    {
	return new Partition(get_impl().clone());
    }


    Partition::Impl&
    Partition::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Partition::Impl&
    Partition::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    unsigned int
    Partition::get_number() const
    {
	return get_impl().get_number();
    }


    PartitionType
    Partition::get_type() const
    {
	return get_impl().get_type();
    }


    void
    Partition::set_type(PartitionType type)
    {
	get_impl().set_type(type);
    }


    unsigned int
    Partition::get_id() const
    {
	return get_impl().get_id();
    }


    void
    Partition::set_id(unsigned int id)
    {
	get_impl().set_id(id);
    }


    bool
    Partition::is_boot() const
    {
	return get_impl().is_boot();
    }


    void
    Partition::set_boot(bool boot)
    {
	get_impl().set_boot(boot);
    }


    bool
    Partition::is_legacy_boot() const
    {
	return get_impl().is_legacy_boot();
    }


    void
    Partition::set_legacy_boot(bool legacy_boot)
    {
	get_impl().set_legacy_boot(legacy_boot);
    }


    const string&
    Partition::get_label() const
    {
	return get_impl().get_label();
    }


    void
    Partition::set_label(const string& label)
    {
	get_impl().set_label(label);
    }


    const string&
    Partition::get_uuid() const
    {
	return get_impl().get_uuid();
    }


    PartitionTable*
    Partition::get_partition_table()
    {
	return get_impl().get_partition_table();
    }


    const PartitionTable*
    Partition::get_partition_table() const
    {
	return get_impl().get_partition_table();
    }


    Partitionable*
    Partition::get_partitionable()
    {
	return get_impl().get_partitionable();
    }


    const Partitionable*
    Partition::get_partitionable() const
    {
	return get_impl().get_partitionable();
    }


    Region
    Partition::get_unused_surrounding_region() const
    {
	return get_impl().get_unused_surrounding_region();
    }


    Partition*
    Partition::find_by_name(Devicegraph* devicegraph, const string& name)
    {
	return to_partition(BlkDevice::find_by_name(devicegraph, name));
    }


    const Partition*
    Partition::find_by_name(const Devicegraph* devicegraph, const string& name)
    {
	return to_partition(BlkDevice::find_by_name(devicegraph, name));
    }


    bool
    Partition::compare_by_number(const Partition* lhs, const Partition* rhs)
    {
	return lhs->get_number() < rhs->get_number();
    }


    bool
    is_partition(const Device* device)
    {
	return is_device_of_type<const Partition>(device);
    }


    Partition*
    to_partition(Device* device)
    {
	return to_device_of_type<Partition>(device);
    }


    const Partition*
    to_partition(const Device* device)
    {
	return to_device_of_type<const Partition>(device);
    }

}
