/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) [2016-2025] SUSE LLC
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


#ifndef STORAGE_SYSTEM_INFO_IMPL_H
#define STORAGE_SYSTEM_INFO_IMPL_H


#include "storage/EtcFstab.h"
#include "storage/EtcCrypttab.h"
#include "storage/EtcMdadm.h"

#include "storage/Utils/Udev.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/SystemInfo/Arch.h"
#include "storage/SystemInfo/ProcMounts.h"
#include "storage/SystemInfo/ProcMdstat.h"
#include "storage/SystemInfo/CmdBlkid.h"
#include "storage/SystemInfo/CmdLsattr.h"
#include "storage/SystemInfo/CmdLsscsi.h"
#include "storage/SystemInfo/CmdParted.h"
#include "storage/SystemInfo/CmdStat.h"
#include "storage/SystemInfo/CmdBlockdev.h"
#include "storage/SystemInfo/CmdDasdview.h"
#include "storage/SystemInfo/CmdDf.h"
#include "storage/SystemInfo/CmdDmsetup.h"
#include "storage/SystemInfo/CmdCryptsetup.h"
#include "storage/SystemInfo/CmdDmraid.h"
#include "storage/SystemInfo/CmdMdadm.h"
#include "storage/SystemInfo/CmdMultipath.h"
#include "storage/SystemInfo/CmdNvme.h"
#include "storage/SystemInfo/CmdBtrfs.h"
#include "storage/SystemInfo/CmdLvm.h"
#include "storage/SystemInfo/CmdUdevadm.h"
#include "storage/SystemInfo/DevAndSys.h"


namespace storage
{
    using std::map;


    /**
     * Encapsulates system access, also for testsuite mocking
     */
    class SystemInfo::Impl : private boost::noncopyable
    {

    public:

	/* For most commands the command-line can be used as the key for a
	   cache. For some commands this does not work since the command-line
	   is not stable, e.g. when a temporary mountpoint is used.
	   E.g. 'parted /dev/sda' is fine but 'btrfs subvolume list
	   /tmp/libstorage-H6lvbw' is not.

	   If the command-line is not stable a key is introduced, e.g. the
	   device name. So for getCmdBtrfsSubvolumeList() the device name and
	   the mountpoint have to be specified while the device is only used
	   as key and the mountpoint only for the command. */

	Impl();
	~Impl();

	Udevadm udevadm;

	const EtcFstab& getEtcFstab(const string& path) { return etc_fstab.get(path); }
	const EtcCrypttab& getEtcCrypttab(const string& path) { return etc_crypttab.get(path); }
	const EtcMdadm& getEtcMdadm(const string& path) { return etc_mdadm.get(path); }

	const Arch& getArch() { return arch.get(); }
	const Dir& getDir(const string& path) { return dirs.get2(udevadm, path); }
	const File& getFile(const string& path) { return files.get(path); }
	const CmdStat& getCmdStat(const string& path) { return cmd_stats.get(path); }
	const CmdBlockdev& getCmdBlockdev(const string& path) { return cmd_blockdev.get(path); }
	const MdLinks& getMdLinks() { return md_links.get2(udevadm); }
	const ProcMounts& getProcMounts() { return proc_mounts.get(); }
	const ProcMdstat& getProcMdstat() { return proc_mdstat.get(); }
	const CmdMdadmDetail& getCmdMdadmDetail(const string& device) { return cmd_mdadm_details.get(device); }
	const Blkid& getBlkid() { return blkid.get2(udevadm); }
	const CmdLsscsi& getCmdLsscsi() { return cmd_lsscsi.get(); }
	const CmdNvmeList& getCmdNvmeList() { return cmd_nvme_list.get(); }
	const CmdNvmeListSubsys& getCmdNvmeListSubsys() { return cmd_nvme_list_subsys.get(); }
	const Parted& getParted(const string& device) { return parteds.get2(udevadm, device); }
	const Dasdview& getDasdview(const string& device) { return dasdviews.get(device); }
	const CmdDmsetupInfo& getCmdDmsetupInfo() { return cmd_dmsetup_info.get(); }
	const CmdDmsetupTable& getCmdDmsetupTable() { return cmd_dmsetup_table.get(); }
	const CmdCryptsetupLuksDump& getCmdCryptsetupLuksDump(const string& name) { return cmd_cryptsetup_luks_dumps.get(name); }
	const CmdCryptsetupBitlkDump& getCmdCryptsetupBitlkDump(const string& name) { return cmd_cryptsetup_bitlk_dumps.get(name); }
	const CmdDmraid& getCmdDmraid() { return cmd_dmraid.get(); }
	const CmdMultipath& getCmdMultipath() { return cmd_multipath.get(); }

	const CmdBtrfsFilesystemShow& getCmdBtrfsFilesystemShow() { return cmd_btrfs_filesystem_show.get2(udevadm); }

	// The device is only used for the cache-key.
	const CmdBtrfsSubvolumeList& getCmdBtrfsSubvolumeList(const string& device, const string& mount_point)
	    { return cmd_btrfs_subvolume_lists.get(CmdBtrfsSubvolumeList::key_t(device), mount_point); }

	// The device is only used for the cache-key.
	const CmdBtrfsSubvolumeShow& getCmdBtrfsSubvolumeShow(const string& device, const string& mount_point)
	    { return cmd_btrfs_subvolume_shows.get(CmdBtrfsSubvolumeShow::key_t(device), mount_point); }

	// The device is only used for the cache-key.
	const CmdBtrfsSubvolumeGetDefault& getCmdBtrfsSubvolumeGetDefault(const string& device, const string& mount_point)
	    { return cmd_btrfs_subvolume_get_defaults.get(CmdBtrfsSubvolumeGetDefault::key_t(device), mount_point); }

	// The device is only used for the cache-key.
	const CmdBtrfsFilesystemDf& getCmdBtrfsFilesystemDf(const string& device, const string& mount_point)
	    { return cmd_btrfs_filesystem_df.get(CmdBtrfsSubvolumeGetDefault::key_t(device), mount_point); }

	// The device is only used for the cache-key.
	const CmdBtrfsQgroupShow& getCmdBtrfsQgroupShow(const string& device, const string& mount_point)
	    { return cmd_btrfs_qgroup_show.get(CmdBtrfsQgroupShow::key_t(device), mount_point); }

	const CmdPvs& getCmdPvs() { return cmd_pvs.get(); }
	const CmdVgs& getCmdVgs() { return cmd_vgs.get(); }
	const CmdLvs& getCmdLvs() { return cmd_lvs.get(); }

	/**
	 * This function is special in that it checks for some aliases.
	 */
	const CmdUdevadmInfo& getCmdUdevadmInfo(const string& file);

	const CmdDf& getCmdDf(const string& mount_point) { return cmd_dfs.get(mount_point); }

	// The device is only used for the cache-key.
	const CmdLsattr& getCmdLsattr(const string& device, const string& mount_point, const string& path)
	    { return cmd_lsattr.get(CmdLsattr::key_t(device, path), mount_point, path); }

    private:

	/* LazyObject, LazyObjects and LazyObjectsWithKey cache the object and
	   a potential exception during object construction. HelperBase does
	   the common part. */

	template <class Object, typename... Args>
	class HelperBase
	{
	public:

	    const Object& get(Args... args)
	    {
		if (ep)
		    std::rethrow_exception(ep);

		if (!object)
		{
		    try
		    {
			object = make_shared<Object>(args...);
		    }
		    catch (const std::exception& e)
		    {
			ep = std::current_exception();
			std::rethrow_exception(ep);
		    }
		}

		return *object;
	    }

	    /**
	     * Just like get() above but also pass udevadm to constructor.
	     */
	    const Object& get2(Udevadm& udevadm, Args... args)
	    {
		if (ep)
		    std::rethrow_exception(ep);

		if (!object)
		{
		    try
		    {
			object = make_shared<Object>(udevadm, args...);
		    }
		    catch (const std::exception& e)
		    {
			ep = std::current_exception();
			std::rethrow_exception(ep);
		    }
		}

		return *object;
	    }

	    bool has_object() const { return (bool)(object); }
	    const Object& get_object() const { return *object; }

	private:

	    std::shared_ptr<Object> object;
	    std::exception_ptr ep;

	};


	template <class Object>
	class LazyObject : public HelperBase<Object>, private boost::noncopyable
	{
	};


	template <class Object, class Arg = string>
	class LazyObjects : private boost::noncopyable
	{
	public:

	    typedef HelperBase<Object, Arg> Helper;

	    const Object& get(const Arg& arg)
	    {
		typename map<Arg, Helper>::iterator pos = data.lower_bound(arg);
		if (pos == data.end() || typename map<Arg, Helper>::key_compare()(arg, pos->first))
		    pos = data.insert(pos, typename map<Arg, Helper>::value_type(arg, Helper()));
		return pos->second.get(arg);
	    }

	    const Object& get2(Udevadm& udevadm, const Arg& arg)
	    {
		typename map<Arg, Helper>::iterator pos = data.lower_bound(arg);
		if (pos == data.end() || typename map<Arg, Helper>::key_compare()(arg, pos->first))
		    pos = data.insert(pos, typename map<Arg, Helper>::value_type(arg, Helper()));
		return pos->second.get2(udevadm, arg);
	    }

	    const map<Arg, Helper>& get_data() const { return data; }

	private:

	    map<Arg, Helper> data;

	};


	template <class Object, typename... Args>
	class LazyObjectsWithKey : private boost::noncopyable
	{
	public:

	    typedef typename Object::key_t Key;

	    typedef HelperBase<Object, Key, Args...> Helper;

	    bool includes(const Key& key) const
	    {
		typename map<Key, Helper>::const_iterator pos = data.lower_bound(key);
		return pos != data.end() && !typename map<Key, Helper>::key_compare()(key, pos->first);
	    }

	    const Object& get(const Key& key, Args... args)
	    {
		typename map<Key, Helper>::iterator pos = data.lower_bound(key);
		if (pos == data.end() || typename map<Key, Helper>::key_compare()(key, pos->first))
		    pos = data.insert(pos, typename map<Key, Helper>::value_type(key, Helper()));
		return pos->second.get(key, args...);
	    }

	private:

	    map<Key, Helper> data;

	};


	LazyObjects<EtcFstab> etc_fstab;
	LazyObjects<EtcCrypttab> etc_crypttab;
	LazyObjects<EtcMdadm> etc_mdadm;

	LazyObject<Arch> arch;
	LazyObjects<Dir> dirs;
	LazyObjects<File> files;
	LazyObjects<CmdStat> cmd_stats;
	LazyObjects<CmdBlockdev> cmd_blockdev;
	LazyObject<MdLinks> md_links;
	LazyObject<ProcMounts> proc_mounts;
	LazyObject<ProcMdstat> proc_mdstat;
	LazyObjects<CmdMdadmDetail> cmd_mdadm_details;
	LazyObject<Blkid> blkid;
	LazyObject<CmdLsscsi> cmd_lsscsi;
	LazyObject<CmdNvmeList> cmd_nvme_list;
	LazyObject<CmdNvmeListSubsys> cmd_nvme_list_subsys;
	LazyObjects<Parted> parteds;
	LazyObjects<Dasdview> dasdviews;
	LazyObject<CmdDmsetupInfo> cmd_dmsetup_info;
	LazyObject<CmdDmsetupTable> cmd_dmsetup_table;
	LazyObjects<CmdCryptsetupLuksDump> cmd_cryptsetup_luks_dumps;
	LazyObjects<CmdCryptsetupBitlkDump> cmd_cryptsetup_bitlk_dumps;
	LazyObject<CmdDmraid> cmd_dmraid;
	LazyObject<CmdMultipath> cmd_multipath;

	LazyObject<CmdBtrfsFilesystemShow> cmd_btrfs_filesystem_show;
	LazyObjectsWithKey<CmdBtrfsSubvolumeList, string> cmd_btrfs_subvolume_lists;
	LazyObjectsWithKey<CmdBtrfsSubvolumeShow, string> cmd_btrfs_subvolume_shows;
	LazyObjectsWithKey<CmdBtrfsSubvolumeGetDefault, string> cmd_btrfs_subvolume_get_defaults;
	LazyObjectsWithKey<CmdBtrfsFilesystemDf, string> cmd_btrfs_filesystem_df;
	LazyObjectsWithKey<CmdBtrfsQgroupShow, string> cmd_btrfs_qgroup_show;

	LazyObject<CmdPvs> cmd_pvs;
	LazyObject<CmdVgs> cmd_vgs;
	LazyObject<CmdLvs> cmd_lvs;

	LazyObjects<CmdUdevadmInfo> cmd_udevadm_infos;
	LazyObjects<CmdDf> cmd_dfs;

	LazyObjectsWithKey<CmdLsattr, string, string> cmd_lsattr;

    };

}


#endif
