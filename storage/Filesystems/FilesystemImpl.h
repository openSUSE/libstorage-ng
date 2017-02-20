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
#include "storage/Filesystems/Filesystem.h"
#include "storage/Devices/DeviceImpl.h"


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

	virtual Text do_mount_text(const string& mountpoint, Tense tense) const = 0;
	virtual void do_mount(const Actiongraph::Impl& actiongraph, const string& mountpoint) const = 0;

	virtual Text do_umount_text(const string& mountpoint, Tense tense) const = 0;
	virtual void do_umount(const Actiongraph::Impl& actiongraph, const string& mountpoint) const = 0;

	virtual Text do_add_to_etc_fstab_text(const string& mountpoint, Tense tense) const = 0;
	virtual void do_add_to_etc_fstab(const Actiongraph::Impl& actiongraph, const string& mountpoint) const = 0;

	virtual Text do_remove_from_etc_fstab_text(const string& mountpoint, Tense tense) const = 0;
	virtual void do_remove_from_etc_fstab(const Actiongraph::Impl& actiongraph, const string& mountpoint) const = 0;

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

}

#endif
