/*
 * Copyright (c) [2017-2018] SUSE LLC
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


#ifndef STORAGE_MOUNT_POINT_IMPL_H
#define STORAGE_MOUNT_POINT_IMPL_H


#include "storage/Filesystems/MountPoint.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/EtcFstab.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<MountPoint> { static const char* classname; };


    class MountPoint::Impl : public Device::Impl
    {
    public:

	Impl(const string& path);

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<MountPoint>::classname; }

	virtual string get_pretty_classname() const override;

	virtual string get_displayname() const override;

	virtual Impl* clone() const override { return new Impl(*this); }

	static bool valid_path(const string& path);
	static string normalize_path(const string& path);

	const string& get_path() const { return path; }
	void set_path(const string& path);

	MountByType get_mount_by() const { return mount_by; }
	void set_mount_by(const MountByType mount_by) { Impl::mount_by = mount_by; }

	vector<MountByType> possible_mount_bys() const;

	void set_default_mount_by();

	const MountOpts& get_mount_options() const { return mount_options; }
	void set_mount_options(const MountOpts& mount_options);
	void set_mount_options(const vector<string>& mount_options);

	void set_default_mount_options();

	int get_freq() const { return freq; }

	int get_passno() const { return passno; }

	bool is_in_etc_fstab() const { return in_etc_fstab; }
	void set_in_etc_fstab(bool in_etc_fstab) { Impl::in_etc_fstab = in_etc_fstab; }

	bool is_active() const { return active; }
	void set_active(bool active) { Impl::active = active; }

	bool has_mountable() const;

	Mountable* get_mountable();
	const Mountable* get_mountable() const;

	Filesystem* get_filesystem();
	const Filesystem* get_filesystem() const;

	string get_mount_name() const;

	string get_mount_by_name() const;

	FsType get_mount_type() const;

	virtual MountPoint* get_non_impl() override { return to_mount_point(Device::Impl::get_non_impl()); }
	virtual const MountPoint* get_non_impl() const override { return to_mount_point(Device::Impl::get_non_impl()); }

	/**
	 * Get the device name that was used in /etc/fstab.
	 * This is empty if this filesystem was not in /etc/fstab during probing.
	 **/
	const string& get_fstab_device_name() const { return fstab_device_name; }
	void set_fstab_device_name(const string& name) { Impl::fstab_device_name = name; }

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs) const override;
	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual Text do_mount_text(Tense tense) const;
	virtual void do_mount(CommitData& commit_data, const CommitOptions& commit_options);

	virtual Text do_umount_text(Tense tense) const;
	virtual void do_umount(CommitData& commit_data);

	virtual Text do_add_to_etc_fstab_text(Tense tense) const;
	virtual void do_add_to_etc_fstab(CommitData& commit_data) const;

	virtual Text do_update_in_etc_fstab_text(const Device* lhs, Tense tense) const;
	virtual void do_update_in_etc_fstab(CommitData& commit_data, const Device* lhs) const;

	virtual Text do_remove_from_etc_fstab_text(Tense tense) const;
	virtual void do_remove_from_etc_fstab(CommitData& commit_data) const;

	virtual void immediate_activate();
	virtual void immediate_deactivate();

    protected:

	void save(xmlNode* node) const override;

	virtual void check(const CheckCallbacks* check_callbacks) const override;

    private:

	string path;

	MountByType mount_by;

	MountOpts mount_options;

	int freq;
	int passno;

	// TODO: maybe move to Device since also BlkDevice has the flag
	bool active;

	bool in_etc_fstab;

	string fstab_device_name; // device name as found in /etc/fstab

    };


    namespace Action
    {

	class Mount : public Create
	{
	public:

	    Mount(sid_t sid) : Create(sid) {}

	    virtual Text text(const CommitData& commit_data) const override;
	    virtual void commit(CommitData& commit_data, const CommitOptions& commit_options) const override;

	    const string& get_path(Actiongraph::Impl& actiongraph) const;

	    const MountPoint* get_mount_point(Actiongraph::Impl& actiongraph) const;
	};


	class Umount : public Delete
	{
	public:

	    Umount(sid_t sid) : Delete(sid) {}

	    virtual Text text(const CommitData& commit_data) const override;
	    virtual void commit(CommitData& commit_data, const CommitOptions& commit_options) const override;

	};


	class AddToEtcFstab : public Modify
	{
	public:

	    AddToEtcFstab(sid_t sid) : Modify(sid) {}

	    virtual Text text(const CommitData& commit_data) const override;
	    virtual void commit(CommitData& commit_data, const CommitOptions& commit_options) const override;

	    const string& get_path(Actiongraph::Impl& actiongraph) const;

	    virtual void add_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
					  Actiongraph::Impl& actiongraph) const override;

	};


	class UpdateInEtcFstab : public Modify
	{
	public:

	    UpdateInEtcFstab(sid_t sid) : Modify(sid) {}

	    virtual Text text(const CommitData& commit_data) const override;
	    virtual void commit(CommitData& commit_data, const CommitOptions& commit_options) const override;

	};


	class RemoveFromEtcFstab : public Modify
	{
	public:

	    RemoveFromEtcFstab(sid_t sid) : Modify(sid) {}

	    virtual Text text(const CommitData& commit_data) const override;
	    virtual void commit(CommitData& commit_data, const CommitOptions& commit_options) const override;

	};

    }

}

#endif
