/*
 * Copyright (c) [2017-2022] SUSE LLC
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

#include "storage/Utils/XmlFile.h"
#include "storage/Filesystems/NfsImpl.h"
#include "storage/Filesystems/MountPointImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/HumanString.h"
#include "storage/EtcFstab.h"
#include "storage/SystemInfo/SystemInfoImpl.h"
#include "storage/StorageImpl.h"
#include "storage/UsedFeatures.h"
#include "storage/Prober.h"
#include "storage/Actions/CreateImpl.h"
#include "storage/Actions/DeleteImpl.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Nfs>::classname = "Nfs";


    Nfs::Impl::Impl(const xmlNode* node)
	: Filesystem::Impl(node), server(), path()
    {
	if (!getChildValue(node, "server", server))
	    ST_THROW(Exception("no server"));

	if (!getChildValue(node, "path", path))
	    ST_THROW(Exception("no path"));
    }


    string
    Nfs::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("NFS").translated;
    }


    void
    Nfs::Impl::save(xmlNode* node) const
    {
	Filesystem::Impl::save(node);

	setChildValue(node, "server", server);
	setChildValue(node, "path", path);
    }


    bool
    Nfs::Impl::is_valid_name(const string& name)
    {
	return name.size() >= 3 && name[0] != '/' && name.find(':') != string::npos;
    }

    bool
    Nfs::Impl::is_valid_vfstype(FsType& fs_type)
    {
       return fs_type == FsType::NFS || fs_type == FsType::NFS4 || fs_type == FsType::AUTO;
    }


    pair<string, string>
    Nfs::Impl::split_name(const string& name)
    {
	string::size_type pos = name.find(":");
	if (pos == string::npos)
	    ST_THROW(Exception("invalid Nfs name"));

	return make_pair(name.substr(0, pos), canonical_path(name.substr(pos + 1)));
    }


    string
    Nfs::Impl::canonical_path(const string& path)
    {
	string ret = boost::replace_all_copy(path, "//", "/");

	if (ret.size() >= 2 && ret[ret.size() - 1] == '/')
	    ret.erase(ret.size() - 1);

	return ret;
    }


    void
    Nfs::Impl::probe_nfses(Prober& prober)
    {
	// TODO the old library filters the mount options

	SystemInfo::Impl& system_info = prober.get_system_info();
	const Storage& storage = prober.get_storage();
	const string& rootprefix = storage.get_rootprefix();
	const EtcFstab& etc_fstab = system_info.getEtcFstab(storage.prepend_rootprefix(ETC_FSTAB));

	/*
	 * The key of the map is a pair of server and path of the NFS mounts.
	 * The value of the map is a pair of vectors with the FstabEntries
	 * from /etc/fstab and /proc/mounts.
	 */
	typedef map<pair<string, string>, pair<vector<ExtendedFstabEntry>, vector<ExtendedFstabEntry>>> entries_t;

	entries_t entries;

	for (int i = 0; i < etc_fstab.get_entry_count(); ++i)
	{
	    const FstabEntry* fstab_entry = etc_fstab.get_entry(i);
	    string device = fstab_entry->get_spec();
	    FsType vfstype = fstab_entry->get_fs_type();

	    if (is_valid_name(device) && is_valid_vfstype(vfstype))
	    {
		MountPointPath mount_point_path(fstab_entry->get_mount_point(), true);
		entries[Nfs::Impl::split_name(device)].first.emplace_back(mount_point_path, fstab_entry);
	    }
	}

	vector<const FstabEntry*> mount_entries = system_info.getProcMounts().get_all_nfs();
	for (const FstabEntry* mount_entry : mount_entries)
	{
	    string device = mount_entry->get_spec();

	    if (is_valid_name(device))
	    {
		MountPointPath mount_point_path(mount_entry->get_mount_point(), rootprefix);
		entries[Nfs::Impl::split_name(device)].second.emplace_back(mount_point_path, mount_entry);
	    }
	}

	// The code here works only with one mount point per
	// mountable. Anything else is not supported since rejected by the
	// product owner.

	for (const entries_t::value_type& entry : entries)
	{
	    pair<string, string> name_parts = entry.first;

	    vector<JointEntry> joint_entries = join_entries(entry.second.first, entry.second.second);
	    if (!joint_entries.empty())
	    {
		Nfs* nfs = Nfs::create(prober.get_system(), name_parts.first, name_parts.second);
		MountPoint* mount_point = joint_entries[0].add_to(nfs);

		if (mount_point->is_active())
		{
		    const CmdDf& cmd_df = system_info.getCmdDf(mount_point->get_path());
		    nfs->set_space_info(cmd_df.get_space_info());
		}
	    }
	}
    }


    Text
    Nfs::Impl::get_message_name() const
    {
	return UntranslatedText(get_server() + ":" + get_path());
    }


    string
    Nfs::Impl::get_mount_name() const
    {
	return get_server() + ":" + get_path();
    }


    string
    Nfs::Impl::get_mount_by_name(const MountPoint* mount_point) const
    {
	return get_mount_name();
    }


    MountByType
    Nfs::Impl::get_default_mount_by() const
    {
	return MountByType::DEVICE;
    }


    vector<MountByType>
    Nfs::Impl::possible_mount_bys() const
    {
	return { MountByType::DEVICE };
    }


    bool
    Nfs::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Filesystem::Impl::equal(rhs))
	    return false;

	return server == rhs.server && path == rhs.path;
    }


    void
    Nfs::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Filesystem::Impl::log_diff(log, rhs);

	storage::log_diff(log, "server", server, rhs.server);
	storage::log_diff(log, "path", path, rhs.path);
    }


    void
    Nfs::Impl::print(std::ostream& out) const
    {
	Filesystem::Impl::print(out);

	out << " server:" << server;
	out << " path:" << path;
    }


    ResizeInfo
    Nfs::Impl::detect_resize_info(const BlkDevice* blk_device) const
    {
	return ResizeInfo(false, RB_RESIZE_NOT_SUPPORTED_BY_DEVICE);
    }


    void
    Nfs::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<shared_ptr<Action::Base>> actions;

	actions.push_back(make_shared<Action::Create>(get_sid(), true));

	actiongraph.add_chain(actions);
    }


    void
    Nfs::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<shared_ptr<Action::Base>> actions;

	actions.push_back(make_shared<Action::Delete>(get_sid(), true));

	actiongraph.add_chain(actions);
    }


    void
    Nfs::Impl::do_pre_mount() const
    {
	// rpcbind might be needed for remote locking

	static bool rpcbind_started = false;

	if (rpcbind_started)
	    return;

	SystemCmd cmd({ RPCBIND_BIN });

	rpcbind_started = true;
    }


    vector<ExtendedFstabEntry>
    Nfs::Impl::find_proc_mounts_entries_unfiltered(SystemInfo::Impl& system_info) const
    {
	const string& rootprefix = get_storage()->get_rootprefix();
	const ProcMounts& proc_mounts = system_info.getProcMounts();

	vector<ExtendedFstabEntry> ret;

	for (const FstabEntry* mount_entry : proc_mounts.get_all_nfs())
	{
	    string spec = canonical_path(mount_entry->get_spec());
	    pair<string, string> tmp = split_name(spec);

	    if (tmp.first == server && tmp.second == path)
	    {
		MountPointPath mount_point_path(mount_entry->get_mount_point(), rootprefix);
		ret.emplace_back(mount_point_path, mount_entry);
	    }
	}

	return ret;
    }

}
