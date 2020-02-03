/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2020] SUSE LLC
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

#include "storage/Prober.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/CallbacksImpl.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/StorageImpl.h"
#include "storage/DevicegraphImpl.h"
#include "storage/Devices/DiskImpl.h"
#include "storage/Devices/DasdImpl.h"
#include "storage/Devices/MultipathImpl.h"
#include "storage/Devices/DmRaidImpl.h"
#include "storage/Devices/MdImpl.h"
#include "storage/Devices/LvmPvImpl.h"
#include "storage/Devices/LvmVgImpl.h"
#include "storage/Devices/LvmLvImpl.h"
#include "storage/Devices/PlainEncryptionImpl.h"
#include "storage/Devices/LuksImpl.h"
#include "storage/Devices/BcacheImpl.h"
#include "storage/Devices/BcacheCsetImpl.h"
#include "storage/Devices/StrayBlkDeviceImpl.h"
#include "storage/Filesystems/BlkFilesystemImpl.h"
#include "storage/Filesystems/BtrfsImpl.h"
#include "storage/Filesystems/NfsImpl.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/UsedFeatures.h"


namespace storage
{


    SysBlockEntries
    probe_sys_block_entries(SystemInfo& system_info)
    {
	const Arch& arch = system_info.getArch();

	SysBlockEntries sys_block_entries;

	for (const string& short_name : system_info.getDir(SYSFS_DIR "/block"))
	{
	    if (boost::starts_with(short_name, "loop") || boost::starts_with(short_name, "dm-"))
		continue;

	    string name = DEV_DIR "/" + short_name;

	    // skip devices without node in /dev (bsc #1076971) - check must
	    // happen before 'udevadm info' call
	    const CmdStat& cmd_stat = system_info.getCmdStat(name);
	    if (!cmd_stat.is_blk())
		continue;

	    if (Md::Impl::is_valid_sysfs_name(name))
	    {
		// workaround for bsc #1030896
		const ProcMdstat& proc_mdstat = system_info.getProcMdstat();
		if (!proc_mdstat.has_entry(short_name))
		    continue;

		sys_block_entries.mds.push_back(short_name);

		continue;
	    }

	    if (Bcache::Impl::is_valid_name(name))
	    {
		sys_block_entries.bcaches.push_back(short_name);

		continue;
	    }

	    const CmdUdevadmInfo& udevadminfo = system_info.getCmdUdevadmInfo(name);

	    const File& range_file = system_info.getFile(SYSFS_DIR + udevadminfo.get_path() +
							 "/ext_range");

	    if (boost::starts_with(short_name, "dasd"))
	    {
		if (range_file.get<int>() <= 1)
		    continue;

		sys_block_entries.dasds.push_back(short_name);

		continue;
	    }

	    if (boost::starts_with(short_name, "xvd"))
	    {
		if (range_file.get<int>() > 1)
		    sys_block_entries.disks.push_back(short_name);
		else
		    sys_block_entries.stray_blk_devices.push_back(short_name);

		continue;
	    }

	    // On S/390 disks using virtio-blk (name /dev/vd*) and
	    // with a DASD partition table are considered DASDs. See
	    // bsc #1112037. Might be fragile.

	    if (arch.is_s390() && boost::starts_with(short_name, "vd"))
	    {
		const Parted& parted = system_info.getParted(name);
		if (parted.get_label() == PtType::DASD)
		{
		    sys_block_entries.dasds.push_back(short_name);

		    continue;
		}
	    }

	    if (true)		// for disks all remaining names are allowed
	    {
		if (range_file.get<int>() <= 1)
		    continue;

		sys_block_entries.disks.push_back(short_name);

		continue;
	    }
	}

	y2mil("sys_block_entries.disks " << sys_block_entries.disks);
	y2mil("sys_block_entries.dasds " << sys_block_entries.dasds);
	y2mil("sys_block_entries.stray_blk_devices " << sys_block_entries.stray_blk_devices);
	y2mil("sys_block_entries.mds " << sys_block_entries.mds);
	y2mil("sys_block_entries.bcaches " << sys_block_entries.bcaches);

	return sys_block_entries;
    }


    Prober::Prober(const ProbeCallbacks* probe_callbacks, Devicegraph* system, SystemInfo& system_info)
	: probe_callbacks(probe_callbacks), system(system), system_info(system_info)
    {
	/**
	 * Difficulties:
	 *
	 * - No static probe order is possible. E.g. LUKS can be on LVM or vica versa.
	 *
	 * - Do not create partitions on partitionables used by something
	 *   else, e.g. used by Multipath, LVM, MD or LUKS. If possible not
	 *   even call parted for those partitionables.
	 *
	 * Solution:
	 *
	 * Pass 1a: Probe partitionables (Disks, DASDs, Multipath and MDs) (without their
	 *          partitions), LVM, bcache, ...
	 *
	 *          Includes most attributes, e.g. name, size
	 *
	 * Pass 1b: Probe holders. Since not all block devices are known some
	 *          holders are saved in a list of pending holders.
	 *
	 *          After this step it is known if partitionables are used for
	 *          something else than partitions (except of LUKS and
	 *          filesystems).
	 *
	 * Pass 1c: Probe partitions of partitionables. Includes attributes of
	 *          pass 1a for partitions.
	 *
	 * Pass 1d: Probe LUKS
	 *
	 *          After this step all BlkDevices, LvmVgs, LvmPvs, ... are
	 *          known.
	 *
	 * Pass 1e: The list of pendings holders is flushed.
	 *
	 * Pass 1f: Probe some additional attributes.
	 *
	 * Pass 2:  Probe filesystems and mount points.
	 */

	try
	{
	    sys_block_entries = probe_sys_block_entries(system_info);
	}
	catch (const Exception& exception)
	{
	    // TRANSLATORS: error message
	    handle(exception, _("Probing failed"), 0);
	}

	// Pass 1a

	y2mil("prober pass 1a");

	// TRANSLATORS: progress message
	message_callback(probe_callbacks, _("Probing disks"));

	try
	{
	    Disk::Impl::probe_disks(*this);
	}
	catch (const Exception& exception)
	{
	    // TRANSLATORS: error message
	    handle(exception, _("Probing disks failed"), 0);
	}

	// TRANSLATORS: progress message
	message_callback(probe_callbacks, _("Probing DASDs"));

	try
	{
	    Dasd::Impl::probe_dasds(*this);
	}
	catch (const Exception& exception)
	{
	    // TRANSLATORS: error message
	    handle(exception, _("Probing DASDs failed"), UF_DASD);
	}

	// TRANSLATORS: progress message
	message_callback(probe_callbacks, _("Probing stray block devices"));

	try
	{
	    StrayBlkDevice::Impl::probe_stray_blk_devices(*this);
	}
	catch (const Exception& exception)
	{
	    // TRANSLATORS: error message
	    handle(exception, _("Probing stray block devices failed"), 0);
	}

	// TRANSLATORS: progress message
	message_callback(probe_callbacks, _("Probing multipath"));

	try
	{
	    Multipath::Impl::probe_multipaths(*this);
	}
	catch (const Exception& exception)
	{
	    // TRANSLATORS: error message
	    handle(exception, _("Probing multipath failed"), UF_MULTIPATH);
	}

	// TRANSLATORS: progress message
	message_callback(probe_callbacks, _("Probing DM RAIDs"));

	try
	{
	    DmRaid::Impl::probe_dm_raids(*this);
	}
	catch (const Exception& exception)
	{
	    ST_CAUGHT(exception);

	    // TRANSLATORS: error message
	    handle(exception, _("Probing DM RAIDs failed"), UF_DMRAID);
	}

	// TRANSLATORS: progress message
	message_callback(probe_callbacks, _("Probing MD RAIDs"));

	try
	{
	    if (system_info.getBlkid().any_md())
	    {
		Md::Impl::probe_mds(*this);
	    }
	}
	catch (const Exception& exception)
	{
	    // TRANSLATORS: error message
	    handle(exception, _("Probing MD RAIDs failed"), UF_MDRAID);
	}

	// TRANSLATORS: progress message
	message_callback(probe_callbacks, _("Probing LVM"));

	try
	{
	    if (system_info.getBlkid().any_lvm())
	    {
		LvmVg::Impl::probe_lvm_vgs(*this);
		LvmPv::Impl::probe_lvm_pvs(*this);
		LvmLv::Impl::probe_lvm_lvs(*this);

		for (LvmVg* lvm_vg : LvmVg::get_all(system))
		    lvm_vg->get_impl().calculate_reserved_extents(*this);
	    }
	}
	catch (const Exception& exception)
	{
	    // TRANSLATORS: error message
	    handle(exception, _("Probing LVM failed"), UF_LVM);
	}

	// TRANSLATORS: progress message
	message_callback(probe_callbacks, _("Probing bcache"));

	try
	{
	    if (system_info.getBlkid().any_bcache())
	    {
		Bcache::Impl::probe_bcaches(*this);
		BcacheCset::Impl::probe_bcache_csets(*this);
	    }
	}
	catch (const Exception& exception)
	{
	    // TRANSLATORS: error message
	    handle(exception, _("Probing bcache failed"), UF_BCACHE);
	}

	// Pass 1b

	y2mil("prober pass 1b");

	// TRANSLATORS: progress message
	message_callback(probe_callbacks, _("Probing device relationships"));

	try
	{
	    for (Devicegraph::Impl::vertex_descriptor vertex : system->get_impl().vertices())
	    {
		Device* device = system->get_impl()[vertex];
		device->get_impl().probe_pass_1b(*this);
	    }
	}
	catch (const Exception& exception)
	{
	    // TRANSLATORS: error message
	    handle(exception, _("Probing device relationships failed"), 0);
	}

	// Pass 1c

	y2mil("prober pass 1c");

	// TRANSLATORS: progress message
	message_callback(probe_callbacks, _("Probing partitions"));

	try
	{
	    for (Devicegraph::Impl::vertex_descriptor vertex : system->get_impl().vertices())
	    {
		Device* device = system->get_impl()[vertex];
		if (is_partitionable(device))
		{
		    Partitionable* partitionable = to_partitionable(device);
		    partitionable->get_impl().probe_pass_1c(*this);
		}
	    }
	}
	catch (const Exception& exception)
	{
	    // TRANSLATORS: error message
	    handle(exception, _("Probing partitions failed"), 0);
	}

	// Pass 1d

	y2mil("prober pass 1d");

	// TRANSLATORS: progress message
	message_callback(probe_callbacks, _("Probing plain encryptions"));

	try
	{
	    PlainEncryption::Impl::probe_plain_encryptions(*this);
	}
	catch (const Exception& exception)
	{
	    // TRANSLATORS: error message
	    handle(exception, _("Probing plain encryptions failed"), UF_PLAIN_ENCRYPTION);
	}

	// TRANSLATORS: progress message
	message_callback(probe_callbacks, _("Probing LUKS"));

	try
	{
	    if (system_info.getBlkid().any_luks())
	    {
		Luks::Impl::probe_lukses(*this);
	    }
	}
	catch (const Exception& exception)
	{
	    // TRANSLATORS: error message
	    handle(exception, _("Probing LUKS failed"), UF_LUKS);
	}

	// Pass 1e

	y2mil("prober pass 1e");

	// TRANSLATORS: progress message
	message_callback(probe_callbacks, _("Probing device relationships"));

	try
	{
	    flush_pending_holders();
	}
	catch (const Exception& exception)
	{
	    // TRANSLATORS: error message
	    handle(exception, _("Probing device relationships failed"), 0);
	}

	// Pass 1f

	y2mil("prober pass 1f");

	// TRANSLATORS: progress message
	message_callback(probe_callbacks, _("Probing additional attributes"));

	try
	{
	    for (Devicegraph::Impl::vertex_descriptor vertex : system->get_impl().vertices())
	    {
		Device* device = system->get_impl()[vertex];
		device->get_impl().probe_pass_1f(*this);
	    }
	}
	catch (const Exception& exception)
	{
	    // TRANSLATORS: error message
	    handle(exception, _("Probing additional attributes failed"), 0);
	}

	// Pass 2

	y2mil("prober pass 2");

	// TRANSLATORS: progress message
	message_callback(probe_callbacks, _("Probing file systems"));

	try
	{
	    BlkFilesystem::Impl::Impl::probe_blk_filesystems(*this);

	    if (system_info.getBlkid().any_btrfs())
	    {
		Btrfs::Impl::Impl::probe_btrfses(*this);
	    }
	}
	catch (const Exception& exception)
	{
	    // TRANSLATORS: error message
	    handle(exception, _("Probing file systems failed"), UF_BTRFS);
	}

	// TRANSLATORS: progress message
	message_callback(probe_callbacks, _("Probing NFS"));

	try
	{
	    Nfs::Impl::probe_nfses(*this);
	}
	catch (const Exception& exception)
	{
	    // TRANSLATORS: error message
	    handle(exception, _("Probing NFS failed"), UF_NFS);
	}

	y2mil("prober done");
    }


    void
    Prober::add_holder(const string& name, Device* b, add_holder_func_t add_holder_func)
    {
	if (BlkDevice::Impl::exists_by_any_name(system, name, system_info))
	{
	    BlkDevice* a = BlkDevice::Impl::find_by_any_name(system, name, system_info);
	    add_holder_func(system, a, b);
	}
	else
	{
	    pending_holders.emplace_back(name, b, add_holder_func);
	}
    }


    void
    Prober::flush_pending_holders()
    {
	for (const pending_holder_t& pending_holder : pending_holders)
	{
	    try
	    {
		BlkDevice* a = BlkDevice::Impl::find_by_any_name(system, pending_holder.name, system_info);
		pending_holder.add_holder_func(system, a, pending_holder.b);
	    }
	    catch (const Exception& exception)
	    {
		ST_CAUGHT(exception);

		y2err("failed to find " << pending_holder.name << " for "
		      << pending_holder.b->get_displayname());

		ST_RETHROW(exception);
	    }
	}

	pending_holders.clear();
    }


    void
    Prober::handle(const Exception& exception, const Text& message, uint64_t used_features) const
    {
	const ProbeCallbacksV2* probe_callbacks_v2 = dynamic_cast<const ProbeCallbacksV2*>(probe_callbacks);

	if (probe_callbacks_v2 && typeid(exception) == typeid(CommandNotFoundException))
	{
	    missing_command_callback(probe_callbacks_v2, message,
				     dynamic_cast<const CommandNotFoundException&>(exception).command(),
				     used_features, exception);
	}
	else
	{
	    error_callback(probe_callbacks, message, exception);
	}
    }

}
