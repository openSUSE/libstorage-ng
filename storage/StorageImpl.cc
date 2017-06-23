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


#include "config.h"
#include "storage/Utils/AppUtil.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/StorageImpl.h"
#include "storage/DevicegraphImpl.h"
#include "storage/Devices/DiskImpl.h"
#include "storage/Devices/DasdImpl.h"
#include "storage/Devices/MultipathImpl.h"
#include "storage/Devices/MdImpl.h"
#include "storage/Devices/LvmPvImpl.h"
#include "storage/Devices/LvmVgImpl.h"
#include "storage/Devices/LvmLvImpl.h"
#include "storage/Devices/LuksImpl.h"
#include "storage/Devices/BcacheImpl.h"
#include "storage/Devices/BcacheCsetImpl.h"
#include "storage/Filesystems/BlkFilesystemImpl.h"
#include "storage/Filesystems/NfsImpl.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Actiongraph.h"
#include "storage/EtcFstab.h"


namespace storage
{

    Storage::Impl::Impl(const Storage& storage, const Environment& environment)
	: storage(storage), environment(environment), arch(false), default_mount_by(MountByType::UUID),
	  tmp_dir("libstorage-XXXXXX")
    {
	y2mil("constructed Storage with " << environment);
	y2mil("libstorage-ng version " VERSION);

	create_devicegraph("probed");
	copy_devicegraph("probed", "staging");
    }


    Storage::Impl::~Impl()
    {
	// TODO: Make sure logger is destroyed after this object
    }


    void
    Storage::Impl::activate(const ActivateCallbacks* activate_callbacks) const
    {
	ST_CHECK_PTR(activate_callbacks);

	y2mil("activate begin");

	Multipath::Impl::activate_multipaths(activate_callbacks);

	while (true)
	{
	    bool again = false;

	    // TODO Md

	    if (LvmLv::Impl::activate_lvm_lvs(activate_callbacks))
		again = true;

	    if (Luks::Impl::activate_lukses(activate_callbacks))
		again = true;

	    if (!again)
		break;
	}

	y2mil("activate end");
    }


    void
    Storage::Impl::probe()
    {
	y2mil("probe begin");

	remove_devicegraph("probed");
	remove_devicegraph("staging");

	Devicegraph* probed = create_devicegraph("probed");

	switch (environment.get_probe_mode())
	{
	    case ProbeMode::STANDARD: {
		probe_helper(probed);
	    } break;

	    case ProbeMode::STANDARD_WRITE_DEVICEGRAPH: {
		probe_helper(probed);
		probed->save(environment.get_devicegraph_filename());
	    } break;

	    case ProbeMode::STANDARD_WRITE_MOCKUP: {
		Mockup::set_mode(Mockup::Mode::RECORD);
		probe_helper(probed);
		Mockup::save(environment.get_mockup_filename());
	    } break;

	    case ProbeMode::NONE: {
	    } break;

	    case ProbeMode::READ_DEVICEGRAPH: {
		probed->load(environment.get_devicegraph_filename());
	    } break;

	    case ProbeMode::READ_MOCKUP: {
		Mockup::set_mode(Mockup::Mode::PLAYBACK);
		Mockup::load(environment.get_mockup_filename());
		probe_helper(probed);
	    } break;
	}

	y2mil("probe end");

	y2mil("probed devicegraph begin");
	y2mil(*probed);
	y2mil("probed devicegraph end");

	copy_devicegraph("probed", "staging");
    }


    void
    Storage::Impl::probe_helper(Devicegraph* probed)
    {
	SystemInfo systeminfo;

	arch = systeminfo.getArch();

	// TODO

	// Pass 1: Detect all Devices except Filesystems and some Holders,
	// e.g. between Partitionable, PartitionTable and Partitions.

	Disk::Impl::probe_disks(probed, systeminfo);

	Dasd::Impl::probe_dasds(probed, systeminfo);

	Multipath::Impl::probe_multipaths(probed, systeminfo);

	if (systeminfo.getBlkid().any_md())
	{
	    // TODO check whether md tools are installed

	    Md::Impl::probe_mds(probed, systeminfo);
	}

	if (systeminfo.getBlkid().any_lvm())
	{
	    // TODO check whether lvm tools are installed

	    LvmVg::Impl::probe_lvm_vgs(probed, systeminfo);
	    LvmPv::Impl::probe_lvm_pvs(probed, systeminfo);
	    LvmLv::Impl::probe_lvm_lvs(probed, systeminfo);
	}

	if (systeminfo.getBlkid().any_luks())
	{
	    // TODO check whether cryptsetup tools are installed

	    Luks::Impl::probe_lukses(probed, systeminfo);
	}

	if (systeminfo.getBlkid().any_bcache())
	{
	    // TODO check whether bcache-tools are installed

	    Bcache::Impl::probe_bcaches(probed, systeminfo);
	    BcacheCset::Impl::probe_bcache_csets(probed, systeminfo);
	}

	// Pass 2: Detect remaining Holders, e.g. MdUsers. This is not
	// possible in pass 1 since a Md can use other Mds (e.g. md0 using md1
	// and md2).

	for (Devicegraph::Impl::vertex_descriptor vertex : probed->get_impl().vertices())
	{
	    Device* device = probed->get_impl()[vertex];
	    device->get_impl().probe_pass_2(probed, systeminfo);
	}

	// Pass 3: Detect filesystems.

	for (BlkDevice* blk_device : BlkDevice::get_all(probed))
	{
	    if (blk_device->num_children() != 0)
		continue;

	    const Blkid& blkid = systeminfo.getBlkid();
	    Blkid::const_iterator it = blkid.find_by_name(blk_device->get_name(), systeminfo);
	    if (it != blkid.end())
	    {
		if (it->second.is_fs)
		{
		    if (it->second.fs_type != FsType::EXT2 && it->second.fs_type != FsType::EXT3 &&
			it->second.fs_type != FsType::EXT4 && it->second.fs_type != FsType::BTRFS &&
			it->second.fs_type != FsType::REISERFS && it->second.fs_type != FsType::XFS &&
			it->second.fs_type != FsType::SWAP && it->second.fs_type != FsType::NTFS &&
			it->second.fs_type != FsType::VFAT && it->second.fs_type != FsType::ISO9660 &&
			it->second.fs_type != FsType::UDF)
		    {
			y2war("detected unsupported filesystem " << toString(it->second.fs_type) << " on " <<
			      blk_device->get_name());
			continue;
		    }

		    BlkFilesystem* blk_filesystem = blk_device->create_blk_filesystem(it->second.fs_type);
		    blk_filesystem->get_impl().probe_pass_3(probed, systeminfo);
		}
	    }
	}

	Nfs::Impl::probe_nfses(probed, systeminfo);
    }


    Devicegraph*
    Storage::Impl::get_devicegraph(const string& name)
    {
	if (name == "probed")
	    ST_THROW(Exception("invalid name"));

	map<string, Devicegraph>::iterator it = devicegraphs.find(name);
	if (it == devicegraphs.end())
	    ST_THROW(Exception("device graph not found"));

	return &it->second;
    }


    const Devicegraph*
    Storage::Impl::get_devicegraph(const string& name) const
    {
	map<string, Devicegraph>::const_iterator it = devicegraphs.find(name);
	if (it == devicegraphs.end())
	    ST_THROW(Exception("device graph not found"));

	return &it->second;
    }


    Devicegraph*
    Storage::Impl::get_staging()
    {
	return get_devicegraph("staging");
    }


    const Devicegraph*
    Storage::Impl::get_staging() const
    {
	return get_devicegraph("staging");
    }


    const Devicegraph*
    Storage::Impl::get_probed() const
    {
	return get_devicegraph("probed");
    }


    vector<string>
    Storage::Impl::get_devicegraph_names() const
    {
	vector<string> ret;

	for (const map<string, Devicegraph>::value_type& it : devicegraphs)
	    ret.push_back(it.first);

	return ret;
    }


    Devicegraph*
    Storage::Impl::create_devicegraph(const string& name)
    {
	pair<map<string, Devicegraph>::iterator, bool> tmp =
	    devicegraphs.emplace(piecewise_construct, forward_as_tuple(name),
				 forward_as_tuple(&storage));
	if (!tmp.second)
	    ST_THROW(Exception("device graph already exists"));

	map<string, Devicegraph>::iterator it = tmp.first;

	return &it->second;
    }


    Devicegraph*
    Storage::Impl::copy_devicegraph(const string& source_name, const string& dest_name)
    {
	const Devicegraph* tmp1 = static_cast<const Impl*>(this)->get_devicegraph(source_name);

	Devicegraph* tmp2 = create_devicegraph(dest_name);

	tmp1->copy(*tmp2);

	return tmp2;
    }


    void
    Storage::Impl::remove_devicegraph(const string& name)
    {
	map<string, Devicegraph>::const_iterator it1 = devicegraphs.find(name);
	if (it1 == devicegraphs.end())
	    ST_THROW(Exception("device graph not found"));

	devicegraphs.erase(it1);
    }


    void
    Storage::Impl::restore_devicegraph(const string& name)
    {
	map<string, Devicegraph>::iterator it1 = devicegraphs.find(name);
	if (it1 == devicegraphs.end())
	    ST_THROW(Exception("device graph not found"));

	map<string, Devicegraph>::iterator it2 = devicegraphs.find("staging");
	if (it2 == devicegraphs.end())
	    ST_THROW(Exception("device graph not found"));

	it1->second.get_impl().swap(it2->second.get_impl());
	devicegraphs.erase(it1);
    }


    bool
    Storage::Impl::exist_devicegraph(const string& name) const
    {
	return devicegraphs.find(name) != devicegraphs.end();
    }


    bool
    Storage::Impl::equal_devicegraph(const string& lhs, const string& rhs) const
    {
	return *get_devicegraph(lhs) == *get_devicegraph(rhs);
    }


    void
    Storage::Impl::check() const
    {
	// check all devicegraphs

	// check that all objects with the same sid have the same type in all
	// devicegraphs

	map<sid_t, set<string>> all_sids_with_types;

	for (const map<string, Devicegraph>::value_type& key_value : devicegraphs)
	{
	    const Devicegraph& devicegraph = key_value.second;

	    devicegraph.check();

	    for (Devicegraph::Impl::vertex_descriptor vertex : devicegraph.get_impl().vertices())
	    {
		const Device* device = devicegraph.get_impl()[vertex];
		all_sids_with_types[device->get_sid()].insert(device->get_impl().get_classname());
	    }
	}

	for (const map<sid_t, set<string>>::value_type& key_value : all_sids_with_types)
	{
	    if (key_value.second.size() != 1)
	    {
		stringstream tmp;
		tmp << key_value.second;

		ST_THROW(Exception(sformat("objects with sid %d have different types %s", key_value.first,
					   tmp.str().c_str())));
	    }
	}
    }


    string
    Storage::Impl::prepend_rootprefix(const string& mount_point) const
    {
	if (mount_point == "swap" || rootprefix.empty())
	    return mount_point;

	if (mount_point == "/")
	    return rootprefix;
	else
	    return rootprefix + mount_point;
    }


    const Actiongraph*
    Storage::Impl::calculate_actiongraph()
    {
	actiongraph.reset();	// free old actiongraph

	Actiongraph* tmp = new Actiongraph(storage, get_probed(), get_staging());
	tmp->generate_compound_actions();

	actiongraph.reset(tmp);

	return tmp;
    }


    void
    Storage::Impl::commit(const CommitCallbacks* commit_callbacks)
    {
	ST_CHECK_PTR(actiongraph.get());

	actiongraph->get_impl().commit(commit_callbacks);

	// TODO somehow update probed
    }

}
