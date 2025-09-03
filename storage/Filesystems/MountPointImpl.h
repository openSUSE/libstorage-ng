/*
 * Copyright (c) [2017-2025] SUSE LLC
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


    namespace Action
    {
	class RenameInEtcFstab;
    }


    template <> struct DeviceTraits<MountPoint> { static const char* classname; };


    /**
     * Class to identify an entry in /etc/fstab including the corresponding
     * blk device (via the id of FilesystemUser).
     */
    class FstabAnchor
    {
    public:

	FstabAnchor() = default;

	FstabAnchor(const string& spec, unsigned int id, const string& mount_point)
	    : spec(spec), id(id), mount_point(mount_point) {}

	/**
	 * The spec as found in /etc/fstab.
	 */
	string spec;

	/**
	 * The id of the corresponding blk filesystem. So far only used for btrfs and
	 * not for mount by UUID or LABEL.
	 */
	unsigned int id = 0;

	/**
	 * The path of the fstab entry.
	 */
	string mount_point;

    };


    class MountPoint::Impl : public Device::Impl
    {
    public:

	Impl(const string& path);

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<MountPoint>::classname; }

	virtual string get_pretty_classname() const override;

	virtual string get_displayname() const override;

	virtual unique_ptr<Device::Impl> clone() const override { return make_unique<Impl>(*this); }

	virtual ResizeInfo detect_resize_info(const BlkDevice* blk_device = nullptr) const override;

	const string& get_path() const { return path; }
	void set_path(const string& path);

	bool is_rootprefixed() const { return rootprefixed; }
	void set_rootprefixed(bool rootprefixed) { Impl::rootprefixed = rootprefixed; }

	MountByType get_mount_by() const { return mount_by; }
	void set_mount_by(MountByType mount_by) { Impl::mount_by = mount_by; }

	vector<MountByType> possible_mount_bys() const;

	void set_default_mount_by();

	const MountOpts& get_mount_options() const { return mount_options; }
	void set_mount_options(const MountOpts& mount_options);
	void set_mount_options(const vector<string>& mount_options);

	MountOpts default_mount_options() const;
	void set_default_mount_options();

	/**
	 * Checks for "ro" in the mount options.
	 */
	bool is_read_only() const;

	int get_freq() const { return freq; }
	void set_freq(int freq) { Impl::freq = freq; }

	int get_passno() const { return passno; }
	void set_passno(int passno) { Impl::passno = passno; }

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

	FsType get_mount_type() const { return mount_type; }
	void set_mount_type(FsType mount_type);

	void set_default_mount_type();

	string get_rootprefixed_path() const;

	virtual uf_t used_features(UsedFeaturesDependencyType used_features_dependency_type) const override;

	virtual MountPoint* get_non_impl() override { return to_mount_point(Device::Impl::get_non_impl()); }
	virtual const MountPoint* get_non_impl() const override { return to_mount_point(Device::Impl::get_non_impl()); }

	/**
	 * Get the FstabAnchor containing the name and id that was
	 * used in /etc/fstab. The name of the fstab anchor is empty
	 * if this filesystem was not in /etc/fstab during probing.
	 */
	const FstabAnchor& get_fstab_anchor() const { return fstab_anchor; }

	void set_fstab_anchor(const FstabAnchor& fstab_anchor) { Impl::fstab_anchor = fstab_anchor; }

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;
	virtual void print(std::ostream& out) const override;

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs) const override;
	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual Text do_mount_text(Tense tense) const;
	virtual void do_mount(CommitData& commit_data, const CommitOptions& commit_options);
	virtual uf_t do_mount_used_features() const;

	virtual Text do_unmount_text(Tense tense) const;
	virtual void do_unmount(CommitData& commit_data);
	virtual uf_t do_unmount_used_features() const;

	virtual Text do_add_to_etc_fstab_text(Tense tense) const;
	virtual void do_add_to_etc_fstab(CommitData& commit_data) const;

	virtual Text do_update_in_etc_fstab_text(const Device* lhs, Tense tense) const;
	virtual void do_update_in_etc_fstab(CommitData& commit_data, const Device* lhs) const;

	virtual Text do_remove_from_etc_fstab_text(Tense tense) const;
	virtual void do_remove_from_etc_fstab(CommitData& commit_data) const;

	virtual Text do_rename_in_etc_fstab_text(const CommitData& commit_data, const Action::RenameInEtcFstab* action) const;
	virtual void do_rename_in_etc_fstab(CommitData& commit_data, const Action::RenameInEtcFstab* action) const;

	virtual void immediate_activate();
	virtual void immediate_deactivate();

    protected:

	virtual void save(xmlNode* node) const override;

	virtual void check(const CheckCallbacks* check_callbacks) const override;

    private:

	string path;
	bool rootprefixed = true;

	MountByType mount_by = MountByType::DEVICE;

	FsType mount_type = FsType::UNKNOWN;

	MountOpts mount_options;

	int freq = 0;
	int passno = 0;

	// TODO: maybe move to Device since also BlkDevice has the flag
	bool active = true;

	bool in_etc_fstab = true;

	FstabAnchor fstab_anchor;

    };

}

#endif
