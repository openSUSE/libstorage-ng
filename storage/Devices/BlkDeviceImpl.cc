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
#include <boost/range/adaptor/reversed.hpp>

#include "storage/Utils/XmlFile.h"
#include "storage/Utils/HumanString.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/Mockup.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Devices/PlainEncryptionImpl.h"
#include "storage/Devices/LuksImpl.h"
#include "storage/Devices/BcacheImpl.h"
#include "storage/Devices/BcacheCsetImpl.h"
#include "storage/Devices/LvmPv.h"
#include "storage/Holders/FilesystemUser.h"
#include "storage/Filesystems/BlkFilesystemImpl.h"
#include "storage/Filesystems/Ext2.h"
#include "storage/Filesystems/Ext3.h"
#include "storage/Filesystems/Ext4.h"
#include "storage/Filesystems/Btrfs.h"
#include "storage/Filesystems/Reiserfs.h"
#include "storage/Filesystems/Xfs.h"
#include "storage/Filesystems/Jfs.h"
#include "storage/Filesystems/F2fs.h"
#include "storage/Filesystems/Swap.h"
#include "storage/Filesystems/Ntfs.h"
#include "storage/Filesystems/Vfat.h"
#include "storage/Filesystems/Exfat.h"
#include "storage/Filesystems/Iso9660.h"
#include "storage/Filesystems/Udf.h"
#include "storage/Filesystems/Bitlocker.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/FreeInfo.h"
#include "storage/Prober.h"
#include "storage/Utils/Format.h"


namespace storage
{


    const char* DeviceTraits<BlkDevice>::classname = "BlkDevice";


    BlkDevice::Impl::Impl(const string& name)
	: Impl(name, Region(0, 0, 512))
    {
    }


    BlkDevice::Impl::Impl(const string& name, const Region& region)
	: Device::Impl(), name(name), active(true), read_only(false), region(region), topology(),
	  udev_paths(), udev_ids(), dm_table_name()
    {
	if (!is_valid_name(name))
	    ST_THROW(Exception("invalid BlkDevice name"));
    }


    BlkDevice::Impl::Impl(const xmlNode* node)
	: Device::Impl(node), name(), active(true), read_only(false), region(0, 0, 512), topology(),
	  udev_paths(), udev_ids(), dm_table_name()
    {
	if (!getChildValue(node, "name", name))
	    ST_THROW(Exception("no name"));

	getChildValue(node, "sysfs-name", sysfs_name);
	getChildValue(node, "sysfs-path", sysfs_path);

	getChildValue(node, "active", active);
	getChildValue(node, "read-only", read_only);

	getChildValue(node, "region", region);

	getChildValue(node, "topology", topology);

	getChildValue(node, "udev-path", udev_paths);
	getChildValue(node, "udev-id", udev_ids);

	getChildValue(node, "dm-table-name", dm_table_name);
    }


    void
    BlkDevice::Impl::probe_pass_1a(Prober& prober)
    {
	Device::Impl::probe_pass_1a(prober);

	if (active)
	{
	    SystemInfo& system_info = prober.get_system_info();

	    const CmdUdevadmInfo& cmdudevadminfo = system_info.getCmdUdevadmInfo(name);

	    sysfs_name = cmdudevadminfo.get_name();
	    sysfs_path = cmdudevadminfo.get_path();

	    const File& ro_file = get_sysfs_file(system_info, "ro");
	    read_only = ro_file.get<bool>();

	    // region is probed in subclasses

	    if (!cmdudevadminfo.get_by_path_links().empty())
	    {
		udev_paths = cmdudevadminfo.get_by_path_links();
		process_udev_paths(udev_paths);
	    }

	    if (!cmdudevadminfo.get_by_id_links().empty())
	    {
		udev_ids = cmdudevadminfo.get_by_id_links();
		process_udev_ids(udev_ids);
	    }
	}
    }


    void
    BlkDevice::Impl::probe_size(Prober& prober)
    {
	SystemInfo& system_info = prober.get_system_info();

	const File& size_file = get_sysfs_file(system_info, "size");
	const File& logical_block_size_file = get_sysfs_file(system_info, "queue/logical_block_size");

	// size is always in 512 byte blocks
	unsigned long long a = size_file.get<unsigned long long>();

	unsigned long long b = logical_block_size_file.get<unsigned long long>();
	if (b < 512)
	    ST_THROW(Exception("invalid logical block size"));

	unsigned long long c = a * 512 / b;
	set_region(Region(0, c, b));
    }


    void
    BlkDevice::Impl::probe_topology(Prober& prober)
    {
	SystemInfo& system_info = prober.get_system_info();

	const File& alignment_offset_file = get_sysfs_file(system_info, "alignment_offset");
	const File& optimal_io_size_file = get_sysfs_file(system_info, "queue/optimal_io_size");

	unsigned long long alignment_offset = alignment_offset_file.get<int>();
	unsigned long long optimal_io_size = optimal_io_size_file.get<int>();

	set_topology(Topology(alignment_offset, optimal_io_size));
    }


    void
    BlkDevice::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValue(node, "name", name);

	setChildValueIf(node, "sysfs-name", sysfs_name, !sysfs_name.empty());
	setChildValueIf(node, "sysfs-path", sysfs_path, !sysfs_path.empty());

	setChildValueIf(node, "active", active, !active);
	setChildValueIf(node, "read-only", read_only, read_only);

	setChildValue(node, "region", region);

	setChildValue(node, "topology", topology);

	setChildValueIf(node, "udev-path", udev_paths, !udev_paths.empty());
	setChildValueIf(node, "udev-id", udev_ids, !udev_ids.empty());

	setChildValueIf(node, "dm-table-name", dm_table_name, !dm_table_name.empty());
    }


    void
    BlkDevice::Impl::check(const CheckCallbacks* check_callbacks) const
    {
	Device::Impl::check(check_callbacks);

	if (region.get_block_size() == 0)
	    ST_THROW(Exception(sformat("block size is zero for %s", get_name())));

	if (!is_valid_name(get_name()))
	    ST_THROW(Exception("BlkDevice has invalid name"));
    }


    void
    BlkDevice::Impl::set_name(const string& name)
    {
	Impl::name = name;
    }


    void
    BlkDevice::Impl::set_region(const Region& region)
    {
	Impl::region = region;

	for (Device* child : get_non_impl()->get_children())
	    child->get_impl().parent_has_new_region(get_non_impl());
    }


    unsigned long long
    BlkDevice::Impl::get_size() const
    {
	return region.to_bytes(region.get_length());
    }


    void
    BlkDevice::Impl::set_size(unsigned long long size)
    {
	// Direct to virtual set_region so that derived classes can perform
	// checks and that children can be informed.

	set_region(Region(region.get_start(), region.to_blocks(size), region.get_block_size()));
    }


    Text
    BlkDevice::Impl::get_size_text() const
    {
	// TODO but maybe ByteCount

	return Text(byte_to_humanstring(get_size(), true, 2, false),
		    byte_to_humanstring(get_size(), false, 2, false));
    }


    const File&
    BlkDevice::Impl::get_sysfs_file(SystemInfo& system_info, const char* filename) const
    {
	return system_info.getFile(SYSFS_DIR + get_sysfs_path() + "/" + filename);
    }


    vector<MountByType>
    BlkDevice::Impl::possible_mount_bys() const
    {
	vector<MountByType> ret = { MountByType::DEVICE };

	if (!udev_paths.empty())
	    ret.push_back(MountByType::PATH);

	if (!udev_ids.empty())
	    ret.push_back(MountByType::ID);

	return ret;
    }


    string
    BlkDevice::Impl::get_mount_by_name(MountByType mount_by_type) const
    {
	string ret;

	switch (mount_by_type)
	{
	    case MountByType::UUID:
		y2war("no uuid possible, using fallback");
		break;

	    case MountByType::LABEL:
		y2war("no label possible, using fallback");
		break;

	    case MountByType::ID:
		if (!get_udev_ids().empty())
		    ret = DEV_DISK_BY_ID_DIR "/" + get_udev_ids().front();
		else
		    y2war("no udev-id defined, using fallback");
		break;

	    case MountByType::PATH:
		if (!get_udev_paths().empty())
		    ret = DEV_DISK_BY_PATH_DIR "/" + get_udev_paths().front();
		else
		    y2war("no udev-path defined, using fallback");
		break;

	    case MountByType::DEVICE:
		break;
	}

	if (ret.empty())
	{
	    ret = get_name();
	}

	return ret;
    }


    ResizeInfo
    BlkDevice::Impl::detect_resize_info(const BlkDevice* blk_device) const
    {
	ResizeInfo resize_info(true, 0);

	for (const Device* child : get_non_impl()->get_children())
	    resize_info.combine(child->get_impl().detect_resize_info(blk_device));

	return resize_info;
    }


    bool
    BlkDevice::Impl::exists_by_any_name(const Devicegraph* devicegraph, const string& name,
					SystemInfo& system_info)
    {
	if (!devicegraph->get_impl().is_system() && !devicegraph->get_impl().is_probed())
	    ST_THROW(Exception("function called on wrong devicegraph"));

	for (Devicegraph::Impl::vertex_descriptor vertex : devicegraph->get_impl().vertices())
	{
	    const BlkDevice* blk_device = dynamic_cast<const BlkDevice*>(devicegraph->get_impl()[vertex]);
	    if (blk_device)
	    {
		if (blk_device->get_name() == name)
		    return true;
	    }
	}

	try
	{
	    string sysfs_path = system_info.getCmdUdevadmInfo(name).get_path();

	    for (Devicegraph::Impl::vertex_descriptor vertex : devicegraph->get_impl().vertices())
	    {
		const BlkDevice* blk_device = dynamic_cast<const BlkDevice*>(devicegraph->get_impl()[vertex]);
		if (blk_device && blk_device->get_impl().active)
		{
		    if (blk_device->get_sysfs_path() == sysfs_path)
			return true;
		}
	    }
	}
	catch (const Exception& exception)
	{
	    ST_CAUGHT(exception);
	}

	return false;
    }


    BlkDevice*
    BlkDevice::Impl::find_by_any_name(Devicegraph* devicegraph, const string& name,
				      SystemInfo& system_info)
    {
	if (!devicegraph->get_impl().is_system() && !devicegraph->get_impl().is_probed())
	    ST_THROW(Exception("function called on wrong devicegraph"));

	for (Devicegraph::Impl::vertex_descriptor vertex : devicegraph->get_impl().vertices())
	{
	    BlkDevice* blk_device = dynamic_cast<BlkDevice*>(devicegraph->get_impl()[vertex]);
	    if (blk_device)
	    {
		if (blk_device->get_name() == name)
		    return blk_device;
	    }
	}

	try
	{
	    string sysfs_path = system_info.getCmdUdevadmInfo(name).get_path();

	    for (Devicegraph::Impl::vertex_descriptor vertex : devicegraph->get_impl().vertices())
	    {
		BlkDevice* blk_device = dynamic_cast<BlkDevice*>(devicegraph->get_impl()[vertex]);
		if (blk_device && blk_device->get_impl().active)
		{
		    if (blk_device->get_sysfs_path() == sysfs_path)
			return blk_device;
		}
	    }
	}
	catch (const Exception& exception)
	{
	    ST_CAUGHT(exception);
	}

	ST_THROW(DeviceNotFoundByName(name));
    }


    const BlkDevice*
    BlkDevice::Impl::find_by_any_name(const Devicegraph* devicegraph, const string& name,
				      SystemInfo& system_info)
    {
	if (!devicegraph->get_impl().is_system() && !devicegraph->get_impl().is_probed())
	    ST_THROW(Exception("function called on wrong devicegraph"));

	for (Devicegraph::Impl::vertex_descriptor vertex : devicegraph->get_impl().vertices())
	{
	    const BlkDevice* blk_device = dynamic_cast<const BlkDevice*>(devicegraph->get_impl()[vertex]);
	    if (blk_device)
	    {
		if (blk_device->get_name() == name)
		    return blk_device;
	    }
	}

	try
	{
	    string sysfs_path = system_info.getCmdUdevadmInfo(name).get_path();

	    for (Devicegraph::Impl::vertex_descriptor vertex : devicegraph->get_impl().vertices())
	    {
		const BlkDevice* blk_device = dynamic_cast<const BlkDevice*>(devicegraph->get_impl()[vertex]);
		if (blk_device && blk_device->get_impl().active)
		{
		    if (blk_device->get_sysfs_path() == sysfs_path)
			return blk_device;
		}
	    }
	}
	catch (const Exception& exception)
	{
	    ST_CAUGHT(exception);
	}

	ST_THROW(DeviceNotFoundByName(name));
    }


    namespace
    {

	/**
	 * Auxiliary struct to represent a direct parent-child relationship
	 */
	struct DirectRelation
	{
	    DirectRelation(const BlkDevice* parent, const Device* child)
		: parent(parent), child(child) {}

	    const BlkDevice* parent;
	    const Device* child;
	};


	/**
	 * Auxiliary method to get all parent-child relationships: from the device that
	 * is being resized to the most high-level device that also needs to be resized.
	 *
	 * TODO handle extended partitions (unsupported so far)
	 */
	vector<DirectRelation>
	devices_to_resize(const BlkDevice* blk_device)
	{
	    vector<DirectRelation> ret;

	    for (const Device* child : blk_device->get_children())
	    {
		if (is_blk_device(child) && !is_md(child))
		{
		    ret.emplace_back(blk_device, child);

		    vector<DirectRelation> tmp = devices_to_resize(to_blk_device(child));
		    ret.insert(ret.end(), tmp.begin(), tmp.end());
		}

		if (is_lvm_pv(child))
		{
		    ret.emplace_back(blk_device, child);
		}

		if (is_blk_filesystem(child))
		{
		    ret.emplace_back(blk_device, child);
		}
	    }

	    return ret;
	}

    }


    void
    BlkDevice::Impl::add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs_base) const
    {
	Device::Impl::add_modify_actions(actiongraph, lhs_base);

	const Impl& lhs = dynamic_cast<const Impl&>(lhs_base->get_impl());

	// The lowest underlying blk device handles the resize, so Partitions
	// and LvmLvs but not Luks.

	if (lhs.get_size() != get_size() && (is_partition(get_non_impl()) || is_lvm_lv(get_non_impl())))
	{
	    ResizeMode resize_mode = get_size() < lhs.get_size() ? ResizeMode::SHRINK :
		ResizeMode::GROW;

	    vector<DirectRelation> devices_to_resize_lhs = devices_to_resize(lhs.get_non_impl());
	    vector<DirectRelation> devices_to_resize_rhs = devices_to_resize(get_non_impl());

	    const BlkFilesystem* blk_filesystem_lhs = nullptr;
	    const BlkFilesystem* blk_filesystem_rhs = nullptr;

	    for (const DirectRelation& device_to_resize : devices_to_resize_lhs)
	    {
		if (is_blk_filesystem(device_to_resize.child))
		    blk_filesystem_lhs = to_blk_filesystem(device_to_resize.child);
	    }

	    for (const DirectRelation& device_to_resize : devices_to_resize_rhs)
	    {
		if (is_blk_filesystem(device_to_resize.child))
		    blk_filesystem_rhs = to_blk_filesystem(device_to_resize.child);
	    }

	    // Only tmp unmounts are inserted in the actiongraph. tmp mounts
	    // are simply handled in the do_resize() functions.

	    bool need_tmp_unmount = false;

	    if (blk_filesystem_rhs)
	    {
		if (!blk_filesystem_rhs->get_impl().supports_mounted_resize(resize_mode))
		    need_tmp_unmount = true;
	    }

	    // Only insert mount and resize actions if the devices exist in
	    // LHS and RHS.

	    vector<Action::Base*> actions;

	    if (need_tmp_unmount && blk_filesystem_lhs)
	    {
		if (blk_filesystem_lhs->exists_in_devicegraph(actiongraph.get_devicegraph(RHS)))
		    blk_filesystem_lhs->get_impl().insert_unmount_action(actions);
	    }

	    if (resize_mode == ResizeMode::SHRINK)
	    {
		for (const DirectRelation& device_to_resize : boost::adaptors::reverse(devices_to_resize_lhs))
		{
		    if (device_to_resize.child->exists_in_devicegraph(actiongraph.get_devicegraph(RHS)))
		    {
			actions.push_back(new Action::Resize(device_to_resize.child->get_sid(), resize_mode,
							     device_to_resize.parent));
		    }
		}
	    }

	    actions.push_back(new Action::Resize(get_sid(), resize_mode, nullptr));

	    if (resize_mode == ResizeMode::GROW)
	    {
		for (const DirectRelation& device_to_resize : devices_to_resize_rhs)
		{
		    if (device_to_resize.child->exists_in_devicegraph(actiongraph.get_devicegraph(LHS)))
		    {
			actions.push_back(new Action::Resize(device_to_resize.child->get_sid(), resize_mode,
							     device_to_resize.parent));
		    }
		}
	    }

	    if (need_tmp_unmount && blk_filesystem_rhs)
	    {
		if (blk_filesystem_rhs->exists_in_devicegraph(actiongraph.get_devicegraph(LHS)))
		    blk_filesystem_rhs->get_impl().insert_mount_action(actions);
	    }

	    actiongraph.add_chain(actions);
	}

	if (!lhs.is_active() && is_active())
	{
	    Action::Base* action = new Action::Activate(get_sid());
	    actiongraph.add_vertex(action);
	}
	else if (lhs.is_active() && !is_active())
	{
	    Action::Base* action = new Action::Deactivate(get_sid());
	    actiongraph.add_vertex(action);
	}
    }


    bool
    BlkDevice::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Device::Impl::equal(rhs))
	    return false;

	return name == rhs.name && sysfs_name == rhs.sysfs_name && sysfs_path == rhs.sysfs_path &&
	    region == rhs.region && topology == rhs.topology && active == rhs.active &&
	    read_only == rhs.read_only && udev_paths == rhs.udev_paths && udev_ids == rhs.udev_ids &&
	    dm_table_name == rhs.dm_table_name;
    }


    void
    BlkDevice::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Device::Impl::log_diff(log, rhs);

	storage::log_diff(log, "name", name, rhs.name);

	storage::log_diff(log, "sysfs-name", sysfs_name, rhs.sysfs_name);
	storage::log_diff(log, "sysfs-path", sysfs_path, rhs.sysfs_path);

	storage::log_diff(log, "active", active, rhs.active);
	storage::log_diff(log, "read-only", read_only, rhs.read_only);

	storage::log_diff(log, "region", region, rhs.region);

	storage::log_diff(log, "topology", topology, rhs.topology);

	storage::log_diff(log, "udev-paths", udev_paths, rhs.udev_paths);
	storage::log_diff(log, "udev-ids", udev_ids, rhs.udev_ids);

	storage::log_diff(log, "dm-table-name", dm_table_name, rhs.dm_table_name);
    }


    void
    BlkDevice::Impl::print(std::ostream& out) const
    {
	Device::Impl::print(out);

	out << " name:" << get_name();

	if (!sysfs_name.empty())
	    out << " sysfs-name:" << sysfs_name;

	if (!sysfs_path.empty())
	    out << " sysfs-path:" << sysfs_path;

	if (!active)
	    out << " active:" << active;

	if (read_only)
	    out << " read-only:" << read_only;

	out << " region:" << get_region()
	    << " topology:" << topology;

	if (!udev_paths.empty())
	    out << " udev-paths:" << udev_paths;

	if (!udev_ids.empty())
	    out << " udev-ids:" << udev_ids;

	if (!dm_table_name.empty())
	    out << " dm-table-name:" << dm_table_name;
    }


    typedef std::function<BlkFilesystem* (Devicegraph* devicegraph)> blk_filesystem_create_fnc;

    const map<FsType, blk_filesystem_create_fnc> blk_filesystem_create_registry = {
	{ FsType::BTRFS, &Btrfs::create },
	{ FsType::EXT2, &Ext2::create },
	{ FsType::EXT3, &Ext3::create },
	{ FsType::EXT4, &Ext4::create },
	{ FsType::ISO9660, &Iso9660::create },
	{ FsType::NTFS, &Ntfs::create },
	{ FsType::REISERFS, &Reiserfs::create },
	{ FsType::SWAP, &Swap::create },
	{ FsType::UDF, &Udf::create },
	{ FsType::VFAT, &Vfat::create },
	{ FsType::EXFAT, &Exfat::create },
	{ FsType::XFS, &Xfs::create },
	{ FsType::JFS, &Jfs::create },
	{ FsType::F2FS, &F2fs::create },
	{ FsType::BITLOCKER, &Bitlocker::create }
    };


    BlkFilesystem*
    BlkDevice::Impl::create_blk_filesystem(FsType fs_type)
    {
	if (num_children() != 0)
	    ST_THROW(WrongNumberOfChildren(num_children(), 0));

	map<FsType, blk_filesystem_create_fnc>::const_iterator it = blk_filesystem_create_registry.find(fs_type);
	if (it == blk_filesystem_create_registry.end())
	{
	    if (fs_type == FsType::NFS || fs_type == FsType::NFS4)
		ST_THROW(UnsupportedException("cannot create Nfs on BlkDevice"));

	    ST_THROW(UnsupportedException("unsupported filesystem type " + toString(fs_type)));
	}

	Devicegraph* devicegraph = get_devicegraph();

	BlkFilesystem* blk_filesystem = it->second(devicegraph);

	FilesystemUser::create(devicegraph, get_non_impl(), blk_filesystem);

	return blk_filesystem;
    }


    bool
    BlkDevice::Impl::has_blk_filesystem() const
    {
	return has_single_child_of_type<const BlkFilesystem>();
    }


    BlkFilesystem*
    BlkDevice::Impl::get_blk_filesystem()
    {
	return get_single_child_of_type<BlkFilesystem>();
    }


    const BlkFilesystem*
    BlkDevice::Impl::get_blk_filesystem() const
    {
	return get_single_child_of_type<const BlkFilesystem>();
    }


    Encryption*
    BlkDevice::Impl::create_encryption(const string& dm_name, EncryptionType type)
    {
	Devicegraph* devicegraph = get_devicegraph();

	vector<Devicegraph::Impl::edge_descriptor> out_edges =
	    devicegraph->get_impl().out_edges(get_vertex());

	Encryption* encryption = nullptr;

	switch (type)
	{
	    case EncryptionType::PLAIN:
		encryption = PlainEncryption::create(devicegraph, dm_name);
		break;

	    case EncryptionType::LUKS1:
	    case EncryptionType::LUKS2:
		encryption = Luks::create(devicegraph, dm_name);
		break;

	    default:
		ST_THROW(Exception("invalid encryption type"));
	}

	encryption->get_impl().Encryption::Impl::set_type(type);

	Devicegraph::Impl::vertex_descriptor encryption_vertex = encryption->get_impl().get_vertex();

	User::create(devicegraph, get_non_impl(), encryption);

	for (Devicegraph::Impl::edge_descriptor out_edge : out_edges)
	{
	    devicegraph->get_impl().set_source(out_edge, encryption_vertex);
	}

	encryption->set_default_mount_by();

	// TODO maybe add parent_added() next to parent_has_new_region() for this?
	encryption->get_impl().parent_has_new_region(get_non_impl());

	return encryption;
    }


    void
    BlkDevice::Impl::remove_encryption()
    {
	Devicegraph* devicegraph = get_devicegraph();

	Encryption* encryption = get_encryption();
	Devicegraph::Impl::vertex_descriptor encryption_vertex = encryption->get_impl().get_vertex();

	vector<Devicegraph::Impl::edge_descriptor> out_edges =
	    devicegraph->get_impl().out_edges(encryption_vertex);

	for (Devicegraph::Impl::edge_descriptor out_edge : out_edges)
	{
	    devicegraph->get_impl().set_source(out_edge, get_vertex());
	}

	devicegraph->get_impl().remove_vertex(encryption_vertex);
    }


    bool
    BlkDevice::Impl::has_encryption() const
    {
	return has_single_child_of_type<const Encryption>();
    }


    Encryption*
    BlkDevice::Impl::get_encryption()
    {
	return get_single_child_of_type<Encryption>();
    }


    const Encryption*
    BlkDevice::Impl::get_encryption() const
    {
	return get_single_child_of_type<const Encryption>();
    }


    Bcache*
    BlkDevice::Impl::create_bcache(const string& name)
    {
	Devicegraph* devicegraph = get_devicegraph();

	// TODO reuse code with create_encryption

	vector<Devicegraph::Impl::edge_descriptor> out_edges =
	    devicegraph->get_impl().out_edges(get_vertex());

	Bcache* bcache = Bcache::create(devicegraph, name);
	Devicegraph::Impl::vertex_descriptor bcache_vertex = bcache->get_impl().get_vertex();

	User::create(devicegraph, get_non_impl(), bcache);

	for (Devicegraph::Impl::edge_descriptor out_edge : out_edges)
	{
	    devicegraph->get_impl().set_source(out_edge, bcache_vertex);
	}

	bcache->get_impl().parent_has_new_region(get_non_impl());

	return bcache;
    }


    bool
    BlkDevice::Impl::has_bcache() const
    {
	return has_single_child_of_type<const Bcache>();
    }


    Bcache*
    BlkDevice::Impl::get_bcache()
    {
	return get_single_child_of_type<Bcache>();
    }


    const Bcache*
    BlkDevice::Impl::get_bcache() const
    {
	return get_single_child_of_type<const Bcache>();
    }


    BcacheCset*
    BlkDevice::Impl::create_bcache_cset()
    {
	if (num_children() != 0)
	    ST_THROW(WrongNumberOfChildren(num_children(), 0));

	Devicegraph* devicegraph = get_devicegraph();

	BcacheCset* bcache_cset = BcacheCset::create(devicegraph);

	User::create(devicegraph, get_non_impl(), bcache_cset);

	return bcache_cset;
    }


    bool
    BlkDevice::Impl::has_bcache_cset() const
    {
	return has_single_child_of_type<const BcacheCset>();
    }


    BcacheCset*
    BlkDevice::Impl::get_bcache_cset()
    {
	return get_single_child_of_type<BcacheCset>();
    }


    const BcacheCset*
    BlkDevice::Impl::get_bcache_cset() const
    {
	return get_single_child_of_type<const BcacheCset>();
    }


    void
    BlkDevice::Impl::wipe_device() const
    {
	string cmd_line = WIPEFS_BIN " --all " + quote(get_name());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    bool
    BlkDevice::Impl::is_valid_name(const string& name)
    {
	return boost::starts_with(name, DEV_DIR "/");
    }


    void
    wait_for_devices(const vector<const BlkDevice*>& blk_devices)
    {
	SystemCmd(UDEVADM_BIN_SETTLE);

	if (Mockup::get_mode() == Mockup::Mode::PLAYBACK)
	    return;

	for (const BlkDevice* blk_device : blk_devices)
	{
	    string name = blk_device->get_name();

	    bool exists = access(name.c_str(), R_OK) == 0;
	    y2mil("name:" << name << " exists:" << exists);

	    if (!exists)
	    {
		for (int count = 0; count < 500; ++count)
		{
		    usleep(10000);
		    exists = access(name.c_str(), R_OK) == 0;
		    if (exists)
			break;
		}
		y2mil("name:" << name << " exists:" << exists);
	    }

	    if (!exists)
		ST_THROW(Exception("wait_for_devices failed " + name));
	}
    }


    void
    wait_for_detach_devices(const vector<const BlkDevice*>& blk_devices)
    {
	vector<string> dev_names;

	for(const BlkDevice* dev : blk_devices)
	{
	    dev_names.push_back(dev->get_name());
	}

	wait_for_detach_devices(dev_names);
    }


    void
    wait_for_detach_devices(const vector<string>& dev_names)
    {
	SystemCmd(UDEVADM_BIN_SETTLE);

	if (Mockup::get_mode() == Mockup::Mode::PLAYBACK)
	    return;

	for (auto name : dev_names)
	{
	    bool exists = access(name.c_str(), R_OK) == 0;

	    y2mil("name:" << name << " exists:" << exists);

	    if (exists)
	    {
		// Waits a max of 5 seconds
		for (int count = 0; count < 500; ++count)
		{
		    if((count % 100) == 0)
			y2mil("waiting for detach " << name);

		    usleep(10000);
		    exists = access(name.c_str(), R_OK) == 0;
		    if (!exists)
			break;
		}

		y2mil("name:" << name << " exists:" << exists);
	    }

	    if (exists)
		ST_THROW(Exception("wait_for_detach_devices failed " + name));
	}
    }


    Text
    join(const vector<const BlkDevice*>& blk_devices, JoinMode join_mode, size_t limit)
    {
	vector<const BlkDevice*> sorted_blk_devices = blk_devices;
	sort(sorted_blk_devices.begin(), sorted_blk_devices.end(), BlkDevice::compare_by_name);

	vector<Text> tmp;

	for (const BlkDevice* blk_device : sorted_blk_devices)
	{
	    // TRANSLATORS:
	    // %1$s is replaced with the device name (e.g. /dev/sdc1),
	    // %2$s is replaced with the size (e.g. 60.00 GiB)
	    tmp.push_back(sformat(_("%1$s (%2$s)"), blk_device->get_name(),
				  blk_device->get_size_string()));
	}

	return join(tmp, join_mode, limit);
    }

}
