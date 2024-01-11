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


#include "storage/StorageImpl.h"
#include "storage/SystemInfo/SystemInfo.h"


namespace storage
{

    using std::vector;


    LuksInfo::LuksInfo()
	: impl(make_unique<Impl>())
    {
    }


    LuksInfo::~LuksInfo() = default;


    const string&
    LuksInfo::get_device_name() const
    {
	return get_impl().device_name;
    }


    unsigned long long
    LuksInfo::get_size() const
    {
	return get_impl().size;
    }


    const string&
    LuksInfo::get_dm_table_name() const
    {
	return get_impl().dm_table_name;
    }


    bool
    LuksInfo::is_dm_table_name_generated() const
    {
	return get_impl().is_dm_table_name_generated;
    }


    const string&
    LuksInfo::get_uuid() const
    {
	return get_impl().uuid;
    }


    const string&
    LuksInfo::get_label() const
    {
	return get_impl().label;
    }


    BitlockerInfo::BitlockerInfo()
	: impl(make_unique<Impl>())
    {
    }


    BitlockerInfo::~BitlockerInfo() = default;


    const string&
    BitlockerInfo::get_device_name() const
    {
	return get_impl().device_name;
    }


    unsigned long long
    BitlockerInfo::get_size() const
    {
	return get_impl().size;
    }


    const string&
    BitlockerInfo::get_dm_table_name() const
    {
	return get_impl().dm_table_name;
    }


    bool
    BitlockerInfo::is_dm_table_name_generated() const
    {
	return get_impl().is_dm_table_name_generated;
    }


    const string&
    BitlockerInfo::get_uuid() const
    {
	return get_impl().uuid;
    }


    DeactivateStatusV2::DeactivateStatusV2()
	: impl(make_unique<Impl>())
    {
    }


    DeactivateStatusV2::DeactivateStatusV2(const DeactivateStatusV2& deactivate_status)
	: impl(make_unique<Impl>(deactivate_status.get_impl()))
    {
    }


    DeactivateStatusV2::~DeactivateStatusV2() = default;


    DeactivateStatusV2&
    DeactivateStatusV2::operator=(const DeactivateStatusV2& deactivate_status)
    {
	*impl = deactivate_status.get_impl();
	return *this;
    }


    bool
    DeactivateStatusV2::multipath() const
    {
	return get_impl().multipath;
    }


    bool
    DeactivateStatusV2::dm_raid() const
    {
	return get_impl().dm_raid;
    }


    bool
    DeactivateStatusV2::md() const
    {
	return get_impl().md;
    }


    bool
    DeactivateStatusV2::lvm_lv() const
    {
	return get_impl().lvm_lv;
    }


    bool
    DeactivateStatusV2::luks() const
    {
	return get_impl().luks;
    }


    bool
    DeactivateStatusV2::bitlocker() const
    {
	return get_impl().bitlocker;
    }


    Storage::Storage(const Environment& environment)
	: impl(make_unique<Impl>(*this, environment))
    {
    }


    Storage::~Storage() = default;


    const Environment&
    Storage::get_environment() const
    {
	return get_impl().get_environment();
    }


    const Arch&
    Storage::get_arch() const
    {
	return get_impl().get_arch();
    }


    Devicegraph*
    Storage::get_devicegraph(const string& name)
    {
	return get_impl().get_devicegraph(name);
    }


    const Devicegraph*
    Storage::get_devicegraph(const string& name) const
    {
	return get_impl().get_devicegraph(name);
    }


    Devicegraph*
    Storage::get_staging()
    {
	return get_impl().get_staging();
    }


    const Devicegraph*
    Storage::get_staging() const
    {
	return get_impl().get_staging();
    }


    const Devicegraph*
    Storage::get_probed() const
    {
	return get_impl().get_probed();
    }


    Devicegraph*
    Storage::get_system()
    {
	return get_impl().get_system();
    }


    const Devicegraph*
    Storage::get_system() const
    {
	return get_impl().get_system();
    }


    vector<string>
    Storage::get_devicegraph_names() const
    {
	return get_impl().get_devicegraph_names();
    }


    map<string, const Devicegraph*>
    Storage::get_devicegraphs() const
    {
	return get_impl().get_devicegraphs();
    }


    Devicegraph*
    Storage::create_devicegraph(const string& name)
    {
	return get_impl().create_devicegraph(name);
    }


    Devicegraph*
    Storage::copy_devicegraph(const string& source_name, const string& dest_name)
    {
	return get_impl().copy_devicegraph(source_name, dest_name);
    }


    void
    Storage::remove_devicegraph(const string& name)
    {
	get_impl().remove_devicegraph(name);
    }


    void
    Storage::restore_devicegraph(const string& name)
    {
	get_impl().restore_devicegraph(name);
    }


    bool
    Storage::exist_devicegraph(const string& name) const
    {
	return get_impl().exist_devicegraph(name);
    }


    bool
    Storage::equal_devicegraph(const string& lhs, const string& rhs) const
    {
	return get_impl().equal_devicegraph(lhs, rhs);
    }


    void
    Storage::check(const CheckCallbacks* check_callbacks) const
    {
	get_impl().check(check_callbacks);
    }


    MountByType
    Storage::get_default_mount_by() const
    {
	return get_impl().get_default_mount_by();
    }


    void
    Storage::set_default_mount_by(MountByType default_mount_by)
    {
	get_impl().set_default_mount_by(default_mount_by);
    }


    const string&
    Storage::get_rootprefix() const
    {
	return get_impl().get_rootprefix();
    }


    void
    Storage::set_rootprefix(const string& rootprefix)
    {
	get_impl().set_rootprefix(rootprefix);
    }


    string
    Storage::prepend_rootprefix(const std::string& mount_point) const
    {
	return get_impl().prepend_rootprefix(mount_point);
    }


    const Actiongraph*
    Storage::calculate_actiongraph()
    {
	return get_impl().calculate_actiongraph();
    }


    void
    Storage::activate(const ActivateCallbacks* activate_callbacks) const
    {
	get_impl().activate(activate_callbacks);
    }


    DeactivateStatus
    Storage::deactivate() const
    {
	DeactivateStatusV2 v2 = deactivate_v2();

	DeactivateStatus v1;
	v1.multipath = v2.multipath();
	v1.dm_raid = v2.dm_raid();
	v1.md = v2.md();
	v1.lvm_lv = v2.lvm_lv();
	v1.luks = v2.luks();
	return v1;
    }


    DeactivateStatusV2
    Storage::deactivate_v2() const
    {
	return get_impl().deactivate();
    }


    void
    Storage::probe(const ProbeCallbacks* probe_callbacks)
    {
	SystemInfo system_info;

	get_impl().probe(system_info, probe_callbacks);
    }


    void
    Storage::probe(SystemInfo& system_info, const ProbeCallbacksV3* probe_callbacks)
    {
	get_impl().probe(system_info, probe_callbacks);
    }


    void
    Storage::commit(const CommitCallbacks* commit_callbacks)
    {
	CommitOptions commit_options(false);
	commit(commit_options, commit_callbacks);
    }


    void
    Storage::commit(const CommitOptions& commit_options, const CommitCallbacks* commit_callbacks)
    {
	get_impl().commit(commit_options, commit_callbacks);
    }


    void
    Storage::generate_pools(const Devicegraph* devicegraph)
    {
	get_impl().generate_pools(devicegraph);
    }


    Pool*
    Storage::create_pool(const std::string& name)
    {
	return get_impl().create_pool(name);
    }


    void
    Storage::remove_pool(const std::string& name)
    {
	get_impl().remove_pool(name);
    }


    void
    Storage::rename_pool(const std::string& old_name, const std::string& new_name)
    {
	get_impl().rename_pool(old_name, new_name);
    }


    bool
    Storage::exists_pool(const std::string& name) const
    {
	return get_impl().exists_pool(name);
    }


    vector<string>
    Storage::get_pool_names() const
    {
	return get_impl().get_pool_names();
    }


    map<string, Pool*>
    Storage::get_pools()
    {
	return get_impl().get_pools();
    }


    map<string, const Pool*>
    Storage::get_pools() const
    {
	return get_impl().get_pools();
    }


    Pool*
    Storage::get_pool(const string& name)
    {
	return get_impl().get_pool(name);
    }


    const Pool*
    Storage::get_pool(const string& name) const
    {
	return get_impl().get_pool(name);
    }

}
