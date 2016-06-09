/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) 2016 SUSE LLC
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
#include "storage/Devices/MdImpl.h"
#include "storage/Devices/LvmPvImpl.h"
#include "storage/Devices/LvmVgImpl.h"
#include "storage/Devices/LvmLvImpl.h"
#include "storage/Devices/LuksImpl.h"
#include "storage/Devices/FilesystemImpl.h"
#include "storage/Holders/Subdevice.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Actiongraph.h"


namespace storage
{

    Storage::Impl::Impl(const Storage& storage, const Environment& environment)
	: storage(storage), environment(environment), arch(false), tmp_dir("libstorage-XXXXXX")
    {
	y2mil("constructed Storage with " << environment);
	y2mil("libstorage version " VERSION);

	Devicegraph* probed = create_devicegraph("probed");

	switch (environment.get_probe_mode())
	{
	    case ProbeMode::STANDARD: {
		probe(probed);
	    } break;

	    case ProbeMode::STANDARD_WRITE_DEVICEGRAPH: {
		probe(probed);
		probed->save(environment.get_devicegraph_filename());
	    } break;

	    case ProbeMode::STANDARD_WRITE_MOCKUP: {
		Mockup::set_mode(Mockup::Mode::RECORD);
		probe(probed);
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
		probe(probed);
	    } break;
	}

	y2mil("probed devicegraph begin");
	y2mil(*probed);
	y2mil("probed devicegraph end");

	copy_devicegraph("probed", "staging");
    }


    Storage::Impl::~Impl()
    {
	// TODO: Make sure logger is destroyed after this object
    }


    void
    Storage::Impl::probe(Devicegraph* probed)
    {
	SystemInfo systeminfo;

	arch = systeminfo.getArch();

	EtcFstab fstab("/etc");

	// TODO

	// Pass 1: Detect all Devices except Filesystems and some Holders,
	// e.g. between Partitionable, PartitionTable and Partitions.

	for (const string& name : Disk::Impl::probe_disks(systeminfo))
	{
	    Disk* disk = Disk::create(probed, name);
	    disk->get_impl().probe_pass_1(probed, systeminfo);
	}

	if (systeminfo.getBlkid().any_md())
	{
	    // TODO check whether md tools are installed

	    for (const string& name : Md::Impl::probe_mds(systeminfo))
	    {
		Md* md = Md::create(probed, name);
		md->get_impl().probe_pass_1(probed, systeminfo);
	    }
	}

	if (systeminfo.getBlkid().any_lvm())
	{
	    // TODO check whether lvm tools are installed

	    for (const CmdVgs::Vg& vg : systeminfo.getCmdVgs().get_vgs())
	    {
		LvmVg* lvm_vg = LvmVg::create(probed, vg.vg_name);
		lvm_vg->get_impl().set_uuid(vg.vg_uuid);
	    }

	    for (const CmdPvs::Pv& pv : systeminfo.getCmdPvs().get_pvs())
	    {
		LvmPv* lvm_pv = LvmPv::create(probed);
		lvm_pv->get_impl().set_uuid(pv.pv_uuid);

		// TODO the pv may not be included in any vg
		LvmVg* lvm_vg = LvmVg::find_by_uuid(probed, pv.vg_uuid);
		Subdevice::create(probed, lvm_pv, lvm_vg);
	    }

	    for (const CmdLvs::Lv& lv : systeminfo.getCmdLvs().get_lvs())
	    {
		LvmVg* lvm_vg = LvmVg::find_by_uuid(probed, lv.vg_uuid);
		LvmLv* lvm_lv = lvm_vg->create_lvm_lv(lv.lv_name);
		lvm_lv->get_impl().set_uuid(lv.lv_uuid);
	    }
	}

	if (systeminfo.getBlkid().any_luks())
        {
            // TODO check whether cryptsetup tools are installed

            for (const string& dm_name : Luks::Impl::probe_luks(systeminfo))
            {
                Luks* luks = Luks::create(probed, dm_name);
                luks->get_impl().probe_pass_1(probed, systeminfo);
            }
        }

	// Pass 2: Detect remaining Holders, e.g. MdUsers. This is not
	// possible in pass 1 since a Md can use other Mds (e.g. md0 using md1
	// and md2).

	for (Md* md : Md::get_all(probed))
	{
	    md->get_impl().probe_pass_2(probed, systeminfo);
	}

	for (LvmPv* lvm_pv : LvmPv::get_all(probed))
	{
	    lvm_pv->get_impl().probe_pass_2(probed, systeminfo);
	}

        for (Luks* luks : Luks::get_all(probed))
        {
            luks->get_impl().probe_pass_2(probed, systeminfo);
        }

	// Pass 3: Detect filesystems.

	for (BlkDevice* blk_device : BlkDevice::get_all(probed))
	{
	    if (blk_device->num_children() != 0)
		continue;

	    Blkid::Entry entry;
	    if (systeminfo.getBlkid().find_by_name(blk_device->get_name(), entry, systeminfo))
	    {
		if (entry.is_fs)
		{
		    // TODO temporary until all fs are implemented
		    if (entry.fs_type != FsType::EXT4 && entry.fs_type != FsType::BTRFS &&
			entry.fs_type != FsType::XFS && entry.fs_type != FsType::SWAP &&
			entry.fs_type != FsType::NTFS && entry.fs_type != FsType::VFAT)
			continue;

		    Filesystem* filesystem = blk_device->create_filesystem(entry.fs_type);
		    filesystem->get_impl().probe_pass_3(probed, systeminfo, fstab);
		}
	    }
	}
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
	for (const map<string, Devicegraph>::value_type& key_value : devicegraphs)
	{
	    key_value.second.check();
	}

	// TODO check that a object has the same type in every devicegraph
    }


    string
    Storage::Impl::prepend_rootprefix(const string& mountpoint) const
    {
	if (mountpoint == "swap" || rootprefix.empty())
	    return mountpoint;

	if (mountpoint == "/")
	    return rootprefix;
	else
	    return rootprefix + mountpoint;
    }


    const Actiongraph*
    Storage::Impl::calculate_actiongraph()
    {
	actiongraph.reset(new Actiongraph(storage, get_probed(), get_staging()));

	return actiongraph.get();
    }


    void
    Storage::Impl::commit(const CommitCallbacks* commit_callbacks)
    {
	ST_CHECK_PTR(actiongraph.get());

	actiongraph->get_impl().commit(commit_callbacks);

	// TODO somehow update probed
    }

}
