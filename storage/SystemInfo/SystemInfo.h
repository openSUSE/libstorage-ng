/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) [2016-2019] SUSE LLC
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


#ifndef STORAGE_SYSTEM_INFO_H
#define STORAGE_SYSTEM_INFO_H


#include <boost/noncopyable.hpp>

#include "storage/EtcFstab.h"
#include "storage/EtcCrypttab.h"
#include "storage/EtcMdadm.h"

#include "storage/SystemInfo/Arch.h"
#include "storage/SystemInfo/ProcMounts.h"
#include "storage/SystemInfo/ProcMdstat.h"
#include "storage/SystemInfo/CmdBlkid.h"
#include "storage/SystemInfo/CmdLsattr.h"
#include "storage/SystemInfo/CmdLsscsi.h"
#include "storage/SystemInfo/CmdParted.h"
#include "storage/SystemInfo/CmdStat.h"
#include "storage/SystemInfo/CmdDasdview.h"
#include "storage/SystemInfo/CmdDf.h"
#include "storage/SystemInfo/CmdDmsetup.h"
#include "storage/SystemInfo/CmdCryptsetup.h"
#include "storage/SystemInfo/CmdDmraid.h"
#include "storage/SystemInfo/CmdMultipath.h"
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
    class SystemInfo : private boost::noncopyable
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

	SystemInfo();
	~SystemInfo();

	const EtcFstab& getEtcFstab() { return etc_fstab.get(); }
	const EtcCrypttab& getEtcCrypttab() { return etc_crypttab.get(); }
	const EtcMdadm& getEtcMdadm() { return etc_mdadm.get(); }

	const Arch& getArch() { return arch.get(); }
	const Dir& getDir(const string& path) { return dirs.get(path); }
	const File& getFile(const string& path) { return files.get(path); }
	const CmdStat& getCmdStat(const string& path) { return cmd_stats.get(path); }
	const MdLinks& getMdLinks() { return mdlinks.get(); }
	const ProcMounts& getProcMounts() { return procmounts.get(); }
	const ProcMdstat& getProcMdstat() { return procmdstat.get(); }
	const MdadmDetail& getMdadmDetail(const string& device) { return mdadmdetails.get(device); }
	const MdadmExamine& getMdadmExamine(const vector<string>& devices) { return mdadmexamines.get(devices); }
	const Blkid& getBlkid() { return blkid.get(); }
	const Lsscsi& getLsscsi() { return lsscsi.get(); }
	const Parted& getParted(const string& device) { return parteds.get(device); }
	const Dasdview& getDasdview(const string& device) { return dasdviews.get(device); }
	const CmdDmsetupInfo& getCmdDmsetupInfo() { return cmd_dmsetup_info.get(); }
	const CmdDmsetupTable& getCmdDmsetupTable() { return cmd_dmsetup_table.get(); }
	const CmdCryptsetupLuksDump& getCmdCryptsetupLuksDump(const string& name) { return cmd_cryptsetup_luks_dumps.get(name); }
	const CmdDmraid& getCmdDmraid() { return cmddmraid.get(); }
	const CmdMultipath& getCmdMultipath() { return cmdmultipath.get(); }

	const CmdBtrfsFilesystemShow& getCmdBtrfsFilesystemShow() { return cmdbtrfsfilesystemshow.get(); }

	// The device is only used for the cache-key.
	const CmdBtrfsSubvolumeList& getCmdBtrfsSubvolumeList(const string& device, const string& mountpoint)
	    { return cmdbtrfssubvolumelists.get(CmdBtrfsSubvolumeList::key_t(device), mountpoint); }

	// The device is only used for the cache-key.
	const CmdBtrfsSubvolumeGetDefault& getCmdBtrfsSubvolumeGetDefault(const string& device, const string& mountpoint)
	    { return cmdbtrfssubvolumegetdefaults.get(CmdBtrfsSubvolumeGetDefault::key_t(device), mountpoint); }

	// The device is only used for the cache-key.
	const CmdBtrfsFilesystemDf& getCmdBtrfsFilesystemDf(const string& device, const string& mount_point)
	    { return cmd_btrfs_filesystem_df.get(CmdBtrfsSubvolumeGetDefault::key_t(device), mount_point); }

	const CmdPvs& getCmdPvs() { return cmdpvs.get(); }
	const CmdVgs& getCmdVgs() { return cmdvgs.get(); }
	const CmdLvs& getCmdLvs() { return cmdlvs.get(); }
	const CmdUdevadmInfo& getCmdUdevadmInfo(const string& file) { return cmdudevadminfos.get(file); }
	const CmdDf& getCmdDf(const string& mountpoint) { return cmddfs.get(mountpoint); }

	// The device is only used for the cache-key.
	const CmdLsattr& getCmdLsattr(const string& device, const string& mountpoint, const string& path)
	    { return cmdlsattr.get(CmdLsattr::key_t(device, path), mountpoint, path); }

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
			object.reset(new Object(args...));
		    }
		    catch (const std::exception& e)
		    {
			ep = std::current_exception();
			std::rethrow_exception(ep);
		    }
		}

		return *object;
	    }

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

	LazyObject<EtcFstab> etc_fstab;
	LazyObject<EtcCrypttab> etc_crypttab;
	LazyObject<EtcMdadm> etc_mdadm;

	LazyObject<Arch> arch;
	LazyObjects<Dir> dirs;
	LazyObjects<File> files;
	LazyObjects<CmdStat> cmd_stats;
	LazyObject<MdLinks> mdlinks;
	LazyObject<ProcMounts> procmounts;
	LazyObject<ProcMdstat> procmdstat;
	LazyObjects<MdadmDetail> mdadmdetails;
	LazyObjects<MdadmExamine, vector<string>> mdadmexamines;
	LazyObject<Blkid> blkid;
	LazyObject<Lsscsi> lsscsi;
	LazyObjects<Parted> parteds;
	LazyObjects<Dasdview> dasdviews;
	LazyObject<CmdDmsetupInfo> cmd_dmsetup_info;
	LazyObject<CmdDmsetupTable> cmd_dmsetup_table;
	LazyObjects<CmdCryptsetupLuksDump> cmd_cryptsetup_luks_dumps;
	LazyObject<CmdDmraid> cmddmraid;
	LazyObject<CmdMultipath> cmdmultipath;

	LazyObject<CmdBtrfsFilesystemShow> cmdbtrfsfilesystemshow;
	LazyObjectsWithKey<CmdBtrfsSubvolumeList, string> cmdbtrfssubvolumelists;
	LazyObjectsWithKey<CmdBtrfsSubvolumeGetDefault, string> cmdbtrfssubvolumegetdefaults;
	LazyObjectsWithKey<CmdBtrfsFilesystemDf, string> cmd_btrfs_filesystem_df;

	LazyObject<CmdPvs> cmdpvs;
	LazyObject<CmdVgs> cmdvgs;
	LazyObject<CmdLvs> cmdlvs;

	LazyObjects<CmdUdevadmInfo> cmdudevadminfos;
	LazyObjects<CmdDf> cmddfs;

	LazyObjectsWithKey<CmdLsattr, string, string> cmdlsattr;

    };

}


#endif
