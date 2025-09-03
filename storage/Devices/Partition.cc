/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2024] SUSE LLC
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


#include <regex>
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


    struct IdInfo
    {
	IdInfo(IdNum id)
	    : id(id), linux_partition_id_category((LinuxPartitionIdCategory)(-1)) {}
	IdInfo(IdNum id, LinuxPartitionIdCategory linux_partition_id_category, const char* arch)
	    : id(id), linux_partition_id_category(linux_partition_id_category), arch(arch, regex::extended) {}

	IdNum id;
	LinuxPartitionIdCategory linux_partition_id_category;
	regex arch;
    };


    const vector<IdInfo> id_info_registry = {
	{ ID_LINUX },
	{ ID_LINUX_HOME },
	{ ID_LINUX_ROOT_AARCH64, LinuxPartitionIdCategory::ROOT, "aarch64" },
	{ ID_LINUX_ROOT_ARM,     LinuxPartitionIdCategory::ROOT, "arm.*" },
	{ ID_LINUX_ROOT_PPC32,   LinuxPartitionIdCategory::ROOT, "ppc" },
	{ ID_LINUX_ROOT_PPC64BE, LinuxPartitionIdCategory::ROOT, "ppc64" },
	{ ID_LINUX_ROOT_PPC64LE, LinuxPartitionIdCategory::ROOT, "ppc64le" },
	{ ID_LINUX_ROOT_RISCV32, LinuxPartitionIdCategory::ROOT, "riscv32" },
	{ ID_LINUX_ROOT_RISCV64, LinuxPartitionIdCategory::ROOT, "riscv64" },
	{ ID_LINUX_ROOT_S390,    LinuxPartitionIdCategory::ROOT, "s390" },
	{ ID_LINUX_ROOT_S390X,   LinuxPartitionIdCategory::ROOT, "s390x" },
	{ ID_LINUX_ROOT_X86,     LinuxPartitionIdCategory::ROOT, "i[5-7]86" },
	{ ID_LINUX_ROOT_X86_64,  LinuxPartitionIdCategory::ROOT, "x86_64" },
	{ ID_LINUX_SERVER_DATA },
	{ ID_LINUX_USR_AARCH64,  LinuxPartitionIdCategory::USR,  "aarch64" },
	{ ID_LINUX_USR_ARM,      LinuxPartitionIdCategory::USR,  "arm.*" },
	{ ID_LINUX_USR_PPC32,    LinuxPartitionIdCategory::USR,  "ppc" },
	{ ID_LINUX_USR_PPC64BE,  LinuxPartitionIdCategory::USR,  "ppc64" },
	{ ID_LINUX_USR_PPC64LE,  LinuxPartitionIdCategory::USR,  "ppc64le" },
	{ ID_LINUX_USR_RISCV32,  LinuxPartitionIdCategory::USR,  "riscv32" },
	{ ID_LINUX_USR_RISCV64,  LinuxPartitionIdCategory::USR,  "riscv64" },
	{ ID_LINUX_USR_S390,     LinuxPartitionIdCategory::USR,  "s390" },
	{ ID_LINUX_USR_S390X,    LinuxPartitionIdCategory::USR,  "s390x" },
	{ ID_LINUX_USR_X86,      LinuxPartitionIdCategory::USR,  "i[5-7]86" },
	{ ID_LINUX_USR_X86_64,   LinuxPartitionIdCategory::USR,  "x86_64" },
	{ ID_LVM },
	{ ID_RAID },
	{ ID_SWAP },
    };


    bool
    is_linux_partition_id(IdNum id)
    {
	return any_of(id_info_registry.begin(), id_info_registry.end(), [id](const IdInfo& id_info) {
	    return id_info.id == id;
	});
    }


    bool
    is_linux_partition_id(IdNum id, LinuxPartitionIdCategory linux_partition_id_category)
    {
	return any_of(id_info_registry.begin(), id_info_registry.end(), [id, linux_partition_id_category](const IdInfo& id_info) {
	    return id_info.id == id && id_info.linux_partition_id_category == linux_partition_id_category;
	});
    }


    IdNum
    get_linux_partition_id(LinuxPartitionIdCategory linux_partition_id_category, const Arch& arch)
    {
	for (const IdInfo& id_info : id_info_registry)
	{
	    if (id_info.linux_partition_id_category == linux_partition_id_category &&
		regex_match(arch.get_arch(), id_info.arch))
		return id_info.id;
	}

	ST_THROW(Exception(sformat("partition id unknown for architecture %s", arch)));
    }


    IdNum
    get_linux_partition_id(LinuxPartitionIdCategory linux_partition_id_category, SystemInfo& system_info)
    {
	return get_linux_partition_id(linux_partition_id_category, system_info.get_arch());
    }


    Partition*
    Partition::create(Devicegraph* devicegraph, const string& name, const Region& region, PartitionType type)
    {
	if (!boost::starts_with(name, DEV_DIR "/"))
	    ST_THROW(Exception("invalid partition name"));

	shared_ptr<Partition> partition = make_shared<Partition>(make_unique<Partition::Impl>(name, region, type));
	Device::Impl::create(devicegraph, partition);
	return partition.get();
    }


    Partition*
    Partition::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<Partition> partition = make_shared<Partition>(make_unique<Partition::Impl>(node));
	Device::Impl::load(devicegraph, partition);
	return partition.get();
    }


    Partition::Partition(Impl* impl)
	: BlkDevice(impl)
    {
    }


    Partition::Partition(unique_ptr<Device::Impl>&& impl)
	: BlkDevice(std::move(impl))
    {
    }


    Partition*
    Partition::clone() const
    {
	return new Partition(get_impl().clone());
    }


    std::unique_ptr<Device>
    Partition::clone_v2() const
    {
	return make_unique<Partition>(get_impl().clone());
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


    bool
    Partition::is_no_automount() const
    {
	return get_impl().is_no_automount();
    }


    void
    Partition::set_no_automount(bool no_automount)
    {
	get_impl().set_no_automount(no_automount);
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
