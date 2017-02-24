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


#ifndef STORAGE_FILESYSTEM_IMPL_H
#define STORAGE_FILESYSTEM_IMPL_H


#include "storage/Utils/Enum.h"
#include "storage/Utils/CDgD.h"
#include "storage/Utils/FileUtils.h"
#include "storage/Filesystems/Filesystem.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/FreeInfo.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Filesystem> { static const char* classname; };

    template <> struct EnumTraits<FsType> { static const vector<string> names; };
    template <> struct EnumTraits<MountByType> { static const vector<string> names; };


    // abstract class

    class Filesystem::Impl : public Device::Impl
    {
    public:

	typedef Filesystem non_impl_type;

	virtual FsType get_type() const = 0;

	const vector<string>& get_mountpoints() const { return mountpoints; }
	void set_mountpoints(const std::vector<std::string>& mountpoints);
	void add_mountpoint(const string& mountpoint);

	MountByType get_mount_by() const { return mount_by; }
	void set_mount_by(MountByType mount_by);

	const list<string>& get_fstab_options() const { return fstab_options; }
	void set_fstab_options(const list<string>& fstab_options);

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	/**
	 * A string useable for mounting the filesystem. E.g. for
	 * BlkFilesystems the name of a device and for Nfs server plus
	 * path. Used for messages and mount command.
	 */
	virtual string get_mount_string() const = 0;

	/**
	 * A string useable for mounting the filesystem. E.g. for
	 * BlkFilesystems the name of a device and for Nfs server plus
	 * path. Used for entries in /etc/fstab.
	 */
	virtual string get_mount_by_string() const = 0;

	virtual SpaceInfo detect_space_info() const;
	virtual SpaceInfo detect_space_info_pure() const;
	void set_space_info(const SpaceInfo& space_info);
	bool has_space_info() const { return space_info.has_value(); }

	const Filesystem* get_filesystem() const { return to_filesystem(get_device()); }

	virtual Text do_mount_text(const string& mountpoint, Tense tense) const;
	virtual void do_mount(const Actiongraph::Impl& actiongraph, const string& mountpoint) const;

	virtual Text do_umount_text(const string& mountpoint, Tense tense) const;
	virtual void do_umount(const Actiongraph::Impl& actiongraph, const string& mountpoint) const;

	virtual Text do_add_to_etc_fstab_text(const string& mountpoint, Tense tense) const;
	virtual void do_add_to_etc_fstab(const Actiongraph::Impl& actiongraph, const string& mountpoint) const;

	virtual Text do_remove_from_etc_fstab_text(const string& mountpoint, Tense tense) const;
	virtual void do_remove_from_etc_fstab(const Actiongraph::Impl& actiongraph, const string& mountpoint) const;

    protected:

	Impl()
	    : Device::Impl(), mountpoints({}), mount_by(MountByType::DEVICE), fstab_options({})
	    {}

	Impl(const xmlNode* node);

	void save(xmlNode* node) const override;

    private:

	// TODO this should be a list of a struct with mountpoint, mount-by and fstab-options
	vector<string> mountpoints;
	MountByType mount_by;
	list<string> fstab_options;

	/**
	 * mutable to allow updating cache from const functions. Otherwise
	 * caching would not be possible when working with the probed
	 * devicegraph.
	 */
	mutable CDgD<SpaceInfo> space_info;

    };


    namespace Action
    {

	class Mount : public Modify
	{
	public:

	    Mount(sid_t sid, const string& mountpoint)
		: Modify(sid), mountpoint(mountpoint) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, Tense tense) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	    const string mountpoint;

	};


	class Umount : public Modify
	{
	public:

	    Umount(sid_t sid, const string& mountpoint)
		: Modify(sid), mountpoint(mountpoint) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, Tense tense) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	    const string mountpoint;

	};


	class AddToEtcFstab : public Modify
	{
	public:

	    AddToEtcFstab(sid_t sid, const string& mountpoint)
		: Modify(sid), mountpoint(mountpoint) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, Tense tense) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	    virtual void add_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
					  Actiongraph::Impl& actiongraph) const override;

	    const string mountpoint;

	};


	class RemoveFromEtcFstab : public Modify
	{
	public:

	    RemoveFromEtcFstab(sid_t sid, const string& mountpoint)
		: Modify(sid), mountpoint(mountpoint) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, Tense tense) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	    const string mountpoint;

	};

    }


    class EnsureMounted : boost::noncopyable
    {

    public:

	/**
	 * Ensures that the blk filesystem is mounted somewhere.
	 *
	 * The mode is not enforced.
	 */
	EnsureMounted(const Filesystem* filesystem);

	/**
	 * Returns any mountpoint of the filesystem.
	 */
	string get_any_mountpoint() const;

    private:

	const Filesystem* filesystem;

	unique_ptr<TmpMount> tmp_mount;

    };

}

#endif
