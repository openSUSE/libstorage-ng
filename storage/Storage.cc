/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2018] SUSE LLC
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


#include <vector>

#include "storage/Storage.h"
#include "storage/StorageImpl.h"


namespace storage
{
    using std::vector;


    LuksInfo::LuksInfo()
	: impl(make_unique<Impl>())
    {
    }


    LuksInfo::~LuksInfo()
    {
    }


    const string& LuksInfo::get_device_name() const
    {
	return get_impl().device_name;
    }


    const string& LuksInfo::get_uuid() const
    {
	return get_impl().uuid;
    }


    const string& LuksInfo::get_label() const
    {
	return get_impl().label;
    }


    Storage::Storage(const Environment& environment)
	: impl(new Impl(*this, environment))
    {
    }


    Storage::~Storage()
    {
    }


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
	return get_impl().deactivate();
    }


    void
    Storage::probe(const ProbeCallbacks* probe_callbacks)
    {
	get_impl().probe(probe_callbacks);
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

}
