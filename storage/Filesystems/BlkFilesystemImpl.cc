/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2026] SUSE LLC
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


#include <glob.h>
#include <algorithm>
#include <boost/algorithm/string.hpp>

#include "storage/Utils/XmlFile.h"
#include "storage/Utils/Enum.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/HumanString.h"
#include "storage/Utils/CallbacksImpl.h"
#include "storage/Utils/Udev.h"
#include "storage/Filesystems/BlkFilesystemImpl.h"
#include "storage/Filesystems/MountPointImpl.h"
#include "storage/Holders/FilesystemUserImpl.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/Luks.h"
#include "storage/Devicegraph.h"
#include "storage/SystemInfo/SystemInfoImpl.h"
#include "storage/StorageImpl.h"
#include "storage/FreeInfo.h"
#include "storage/Prober.h"
#include "storage/Redirect.h"
#include "storage/Utils/Format.h"
#include "storage/EnvironmentImpl.h"
#include "storage/Actions/SetUuidImpl.h"
#include "storage/Actions/SetLabelImpl.h"
#include "storage/Actions/SetTuneOptionsImpl.h"
#include "storage/Actions/CreateImpl.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<BlkFilesystem>::classname = "BlkFilesystem";


    BlkFilesystem::Impl::Impl(const xmlNode* node)
	: Filesystem::Impl(node)
    {
	getChildValue(node, "label", label);
	getChildValue(node, "uuid", uuid);

	getChildValue(node, "mkfs-options", mkfs_options);
	getChildValue(node, "tune-options", tune_options);

	const xmlNode* resize_info_node = getChildNode(node, "ResizeInfo");
	if (resize_info_node)
	    resize_info.set_value(ResizeInfo(resize_info_node));

	const xmlNode* content_info_node = getChildNode(node, "ContentInfo");
	if (content_info_node)
	    content_info.set_value(ContentInfo(content_info_node));
    }


    bool
    BlkFilesystem::Impl::supports_mounted_resize(ResizeMode resize_mode) const
    {
	switch (resize_mode)
	{
	    case ResizeMode::SHRINK:
		return supports_mounted_shrink();

	    case ResizeMode::GROW:
		return supports_mounted_grow();
	}

	ST_THROW(Exception("invalid resize_mode"));
    }


    bool
    BlkFilesystem::Impl::supports_unmounted_resize(ResizeMode resize_mode) const
    {
	switch (resize_mode)
	{
	    case ResizeMode::SHRINK:
		return supports_unmounted_shrink();

	    case ResizeMode::GROW:
		return supports_unmounted_grow();
	}

	ST_THROW(Exception("invalid resize_mode"));
    }


    MountByType
    BlkFilesystem::Impl::get_default_mount_by() const
    {
	const BlkDevice* blk_device = get_blk_device();

	/*
	 * If the device name of the block device is fully stable, then use it.
	 * That's true by definition for LVM LVs. It's also true for LUKS
	 * devices as long as the name used in /etc/crypttab remains stable.
	 * According to bsc#1181196 and the subsequent jsc#SLE-20416, using
	 * that name is the safest option for LUKS devices when systemd is used.
	 */
	if (is_lvm_lv(blk_device) || is_luks(blk_device))
	    return MountByType::DEVICE;
	else
	    return get_storage()->get_default_mount_by();
    }


    vector<MountByType>
    BlkFilesystem::Impl::possible_mount_bys() const
    {
	vector<MountByType> ret;

	if (supports_uuid() && is_permanent())
	    ret.push_back(MountByType::UUID);

	if (supports_label() && is_permanent())
	    ret.push_back(MountByType::LABEL);

	return ret;
    }


    void
    BlkFilesystem::Impl::save(xmlNode* node) const
    {
	Filesystem::Impl::save(node);

	setChildValueIf(node, "label", label, !label.empty());
	setChildValueIf(node, "uuid", uuid, !uuid.empty());

	setChildValueIf(node, "mkfs-options", mkfs_options, !mkfs_options.empty());
	setChildValueIf(node, "tune-options", tune_options, !tune_options.empty());

	if (resize_info.has_value())
	{
	    xmlNode* resize_info_node = xmlNewChild(node, "ResizeInfo");
	    resize_info.get_value().save(resize_info_node);
	}

	if (content_info.has_value())
	{
	    xmlNode* content_info_node = xmlNewChild(node, "ContentInfo");
	    content_info.get_value().save(content_info_node);
	}
    }


    void
    BlkFilesystem::Impl::probe_blk_filesystems(Prober& prober)
    {
	SystemInfo::Impl& system_info = prober.get_system_info();
	const Blkid& blkid = system_info.getBlkid();

	for (BlkDevice* blk_device : BlkDevice::get_all(prober.get_system()))
	{
	    if (blk_device->has_children())
		continue;

	    if (!blk_device->get_impl().is_active())
		continue;

	    Blkid::const_iterator it = blkid.find_by_any_name(blk_device->get_name(), system_info);
	    if (it == blkid.end() || !it->second.is_fs)
		continue;

	    const FsType fs_type = it->second.fs_type;

	    if (fs_type != FsType::EXT2 && fs_type != FsType::EXT3 && fs_type != FsType::EXT4 &&
		fs_type != FsType::BTRFS && fs_type != FsType::REISERFS && fs_type != FsType::XFS &&
		fs_type != FsType::SWAP && fs_type != FsType::NTFS && fs_type != FsType::VFAT &&
		fs_type != FsType::ISO9660 && fs_type != FsType::UDF && fs_type != FsType::JFS &&
		fs_type != FsType::F2FS && fs_type != FsType::NILFS2 && fs_type != FsType::EXFAT &&
		fs_type != FsType::BITLOCKER && fs_type != FsType::BCACHEFS && fs_type != FsType::SQUASHFS &&
		fs_type != FsType::EROFS1)
	    {
		y2err("detected unsupported filesystem " << toString(fs_type) << " on " <<
		      blk_device->get_name());

		// TRANSLATORS: Error message displayed during probing,
		// %1$s is replaced by the filesystem type (e.g. minix),
		// %2$s is replaced by the device name (e.g. /dev/sda1)
		Text text = sformat(_("Detected an unsupported file system of type %1$s on the\n"
				      "device %2$s."), toString(fs_type), blk_device->get_name());

		unsupported_filesystem_callback(prober.get_probe_callbacks(), text, blk_device->get_name(), fs_type);

		continue;
	    }

	    // btrfs is probed independently in probe_btrfses().
	    if (fs_type == FsType::BTRFS && support_btrfs_multiple_devices())
		continue;

	    try
	    {
		BlkFilesystem* blk_filesystem = blk_device->create_blk_filesystem(fs_type);
		blk_filesystem->get_impl().probe_pass_2a(prober);
		blk_filesystem->get_impl().probe_pass_2b(prober);
	    }
	    catch (const Exception& exception)
	    {
		// TRANSLATORS: error message
		prober.handle(exception, sformat(_("Probing file system on %s failed"),
						 blk_device->get_name()), 0);
	    }
	}
    }


    void
    BlkFilesystem::Impl::probe_pass_2a(Prober& prober)
    {
	SystemInfo::Impl& system_info = prober.get_system_info();

	const BlkDevice* blk_device = get_blk_device();

	const Blkid& blkid = system_info.getBlkid();
	Blkid::const_iterator it = blkid.find_by_any_name(blk_device->get_name(), system_info);
	if (it != blkid.end())
	{
	    label = it->second.fs_label;
	    uuid = it->second.fs_uuid;
	}
    }


    void
    BlkFilesystem::Impl::probe_uuid()
    {
	Udevadm udevadm;

	const BlkDevice* blk_device = get_blk_device();

	const Blkid blkid(udevadm, blk_device->get_name());
	Blkid::const_iterator it = blkid.get_sole_entry();
	if (it != blkid.end())
	    uuid = it->second.fs_uuid;
    }


    ResizeInfo
    BlkFilesystem::Impl::detect_resize_info(const BlkDevice* blk_device) const
    {
	/*
	 * If the filesystem is not on-disk, ResizeInfo(true, min_size,
	 * max_size) is returned.
	 *
	 * Otherwise detect_resize_info_on_disk() is called (on the probed
	 * devicegraph), the value cached and returned.
	 *
	 * The on-disk ResizeInfo is cached since it is invariant. E.g. the
	 * ResizeInfo of a partition changes when the following partition is
	 * removed.
	 *
	 * Filesystems that do not support create nor resize, e.g. ISO9660,
	 * override detect_resize_info() and just return ResizeInfo(false).
	 *
	 * Filesystems can also override detect_resize_info_on_disk() if they
	 * need special handling to detect the values, e.g. Ntfs calls a
	 * external program to do so. In general a tools for every filesystem
	 * reporting min and max size would be nice (LOL).
	 */

	if (!exists_in_system())
	{
	    ResizeInfo resize_info(true, 0, min_size(), max_size());

	    unsigned long long blk_device_size = get_blk_device()->get_size();

	    if (blk_device_size <= min_size())
		resize_info.reasons |= RB_MIN_SIZE_FOR_FILESYSTEM;

	    if (blk_device_size >= max_size())
		resize_info.reasons |= RB_MAX_SIZE_FOR_FILESYSTEM;

	    return resize_info;
	}

	if (!resize_info.has_value())
	{
	    const BlkFilesystem* tmp_blk_filesystem = redirect_to_system(get_non_impl());

	    if (tmp_blk_filesystem->has_mount_point())
	    {
		const MountPoint* mount_point = tmp_blk_filesystem->get_mount_point();
		if (mount_point->is_active() && mount_point->get_impl().is_read_only())
		    return ResizeInfo(false, RB_FILESYSTEM_MOUNT_READ_ONLY);
	    }

	    ResizeInfo tmp_resize_info = tmp_blk_filesystem->get_impl().detect_resize_info_on_disk(blk_device);

	    y2mil("on-disk resize-info:" << tmp_resize_info);

	    resize_info.set_value(tmp_resize_info);
	}

	return resize_info.get_value();
    }


    ResizeInfo
    BlkFilesystem::Impl::detect_resize_info_on_disk(const BlkDevice* blk_device) const
    {
	// TODO only in real probe mode allowed

	if (!get_devicegraph()->get_impl().is_system() && !get_devicegraph()->get_impl().is_probed())
	    ST_THROW(Exception("function called on wrong device"));

	ResizeInfo resize_info(true, 0, min_size(), max_size());

	unsigned long long blk_device_size = get_blk_device()->get_size();

	// Even if shrink or grow is not supported it is still possible to
	// shrink or grow to the original size.

	if (!supports_shrink())
	{
	    resize_info.min_size = blk_device_size;
	    resize_info.reasons |= RB_SHRINK_NOT_SUPPORTED_BY_FILESYSTEM;
	}
	else
	{
	    // The min-size is calculated by the block device size
	    // minus the free size reported by statvfs. Additional the
	    // min-size must not be less than the generic min-size of
	    // the filesystem.

	    // Calculating the min-size by the total size minus free
	    // size (both reported by statvfs) does not work since the
	    // total size in this case does not include the size for
	    // e.g. for superblocks, metadata and journal.

	    resize_info.min_size = max(resize_info.min_size, blk_device_size - free_size_on_disk());

	    if (resize_info.min_size >= blk_device_size)
		resize_info.reasons |= RB_FILESYSTEM_FULL;

	    // But the min-size must never be bigger than the blk device size.

	    resize_info.min_size = min(resize_info.min_size, blk_device_size);

	    if (blk_device_size <= min_size())
		resize_info.reasons |= RB_MIN_SIZE_FOR_FILESYSTEM;
	}

	if (!supports_grow())
	{
	    resize_info.max_size = blk_device_size;
	    resize_info.reasons |= RB_GROW_NOT_SUPPORTED_BY_FILESYSTEM;
	}
	else
	{
	    if (blk_device_size >= max_size())
		resize_info.reasons |= RB_MAX_SIZE_FOR_FILESYSTEM;
	}

	if (resize_info.min_size >= resize_info.max_size)
	    resize_info.reasons |= RB_FILESYSTEM_FULL;

	return resize_info;
    }


    unsigned long long
    BlkFilesystem::Impl::used_size_on_disk() const
    {
	// TODO only in real probe mode allowed

	EnsureMounted ensure_mounted(get_filesystem());

	StatVfs stat_vfs = detect_stat_vfs(ensure_mounted.get_any_mount_point());

	return stat_vfs.size - stat_vfs.free;
    }


    unsigned long long
    BlkFilesystem::Impl::free_size_on_disk() const
    {
	// TODO only in real probe mode allowed

	EnsureMounted ensure_mounted(get_filesystem());

	StatVfs stat_vfs = detect_stat_vfs(ensure_mounted.get_any_mount_point());

	return stat_vfs.free;
    }


    void
    BlkFilesystem::Impl::set_resize_info(const ResizeInfo& tmp)
    {
	resize_info.set_value(tmp);
    }


    ContentInfo
    BlkFilesystem::Impl::detect_content_info() const
    {
	if (!content_info.has_value())
	{
	    content_info.set_value(detect_content_info_on_disk());
	}

	return content_info.get_value();
    }


    ContentInfo
    BlkFilesystem::Impl::detect_content_info_on_disk() const
    {
	// TODO only in real probe mode allowed

	EnsureMounted ensure_mounted(get_filesystem());

	ContentInfo content_info;
	content_info.is_windows = false;
	content_info.is_efi = false;
	content_info.num_homes = detect_num_homes(ensure_mounted.get_any_mount_point());

	return content_info;
    }


    void
    BlkFilesystem::Impl::set_content_info(const ContentInfo& tmp)
    {
	content_info.set_value(tmp);
    }


    bool
    BlkFilesystem::Impl::detect_is_windows(const string& mount_point)
    {
	// The file '$Boot' is special. It is a reserved name of NTFS and from
	// linux not visible with 'ls /mnt' but with 'ls /mnt/$Boot'.

	const char* files[] = { "boot.ini", "msdos.sys", "io.sys", "config.sys", "MSDOS.SYS",
				"IO.SYS", "bootmgr", "$Boot" };

	for (const char* file : files)
	{
	    string tmp = mount_point + "/" + file;
	    if (access(tmp.c_str(), R_OK) == 0)
	    {
		y2mil("found windows file " << quote(tmp));
		return true;
	    }
	}

	return false;
    }


    bool
    BlkFilesystem::Impl::detect_is_efi(const string& mount_point)
    {
	return checkDir(mount_point + "/efi");
    }


    unsigned
    BlkFilesystem::Impl::detect_num_homes(const string& mount_point)
    {
	const char* files[] = { ".profile", ".bashrc", ".ssh", ".kde", ".kde4", ".gnome",
				".gnome2" };

	unsigned num_homes = 0;

	const vector<string> dirs = glob(mount_point + "/*", GLOB_NOSORT | GLOB_ONLYDIR);
	for (const string& dir : dirs)
	{
	    if (!boost::ends_with(dir, "/root") && checkDir(dir))
	    {
		for (const char* file : files)
		{
		    string tmp = dir + "/" + file;
		    if (access(tmp.c_str(), R_OK) == 0)
		    {
			y2mil("found home file " << quote(tmp));
			++num_homes;
			break;
		    }
		}
	    }

	    if (num_homes >= 2)
		return num_homes;
	}

	return num_homes;
    }


    void
    BlkFilesystem::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<shared_ptr<Action::Base>> actions;

	actions.push_back(make_shared<Action::Create>(get_sid()));

	if (!get_label().empty() && supports_modify_label())
	    actions.push_back(make_shared<Action::SetLabel>(get_sid()));

	if (!get_uuid().empty() && supports_modify_uuid())
	    actions.push_back(make_shared<Action::SetUuid>(get_sid()));

	if (!tune_options.empty())
	    actions.push_back(make_shared<Action::SetTuneOptions>(get_sid()));

	actiongraph.add_chain(actions);
    }


    void
    BlkFilesystem::Impl::add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs_base) const
    {
	Filesystem::Impl::add_modify_actions(actiongraph, lhs_base);

	const Impl& lhs = dynamic_cast<const Impl&>(lhs_base->get_impl());

	vector<shared_ptr<Action::Base>> actions;

	if (get_type() != lhs.get_type())
	{
	    ST_THROW(Exception("cannot change filesystem type"));
	}

	if (get_label() != lhs.get_label())
	{
	    actions.push_back(make_shared<Action::SetLabel>(get_sid()));
	}

	if (get_uuid() != lhs.get_uuid())
	{
	    actions.push_back(make_shared<Action::SetUuid>(get_sid()));
	}

	if (get_tune_options() != lhs.get_tune_options())
	{
	    actions.push_back(make_shared<Action::SetTuneOptions>(get_sid()));
	}

	actiongraph.add_chain(actions);
    }


    vector<BlkDevice*>
    BlkFilesystem::Impl::get_blk_devices()
    {
	Devicegraph* devicegraph = get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	return devicegraph->get_impl().filter_devices_of_type<BlkDevice>(devicegraph->get_impl().parents(vertex));
    }


    vector<const BlkDevice*>
    BlkFilesystem::Impl::get_blk_devices() const
    {
	const Devicegraph* devicegraph = get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	return devicegraph->get_impl().filter_devices_of_type<BlkDevice>(devicegraph->get_impl().parents(vertex));
    }


    const BlkDevice*
    BlkFilesystem::Impl::get_blk_device() const
    {
	vector<const BlkDevice*> blk_devices = get_blk_devices();
	if (blk_devices.empty())
	    ST_THROW(WrongNumberOfParents(0, 1));

	// min_element is required for the testsuite
	return *min_element(blk_devices.begin(), blk_devices.end(), BlkDevice::compare_by_name);
    }


    void
    BlkFilesystem::Impl::wait_for_devices() const
    {
	storage::wait_for_devices(get_blk_devices());
    }


    bool
    BlkFilesystem::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Filesystem::Impl::equal(rhs))
	    return false;

	return label == rhs.label && uuid == rhs.uuid && mkfs_options == rhs.mkfs_options &&
	    tune_options == rhs.tune_options;
    }


    void
    BlkFilesystem::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Filesystem::Impl::log_diff(log, rhs);

	storage::log_diff(log, "label", label, rhs.label);
	storage::log_diff(log, "uuid", uuid, rhs.uuid);

	storage::log_diff(log, "mkfs-options", mkfs_options, rhs.mkfs_options);
	storage::log_diff(log, "tune-options", tune_options, rhs.tune_options);
    }


    void
    BlkFilesystem::Impl::print(std::ostream& out) const
    {
	Filesystem::Impl::print(out);

	if (!label.empty())
	    out << " label:" << label;

	if (!uuid.empty())
	    out << " uuid:" << uuid;

	if (!mkfs_options.empty())
	    out << " mkfs-options:" << mkfs_options;

	if (!tune_options.empty())
	    out << " tune-options:" << tune_options;
    }


    Text
    BlkFilesystem::Impl::get_message_name() const
    {
	return join(get_blk_devices(), JoinMode::COMMA, 10);
    }


    string
    BlkFilesystem::Impl::get_mount_name() const
    {
	const BlkDevice* blk_device = get_blk_device();

	return blk_device->get_name();
    }


    string
    BlkFilesystem::Impl::get_mount_by_name(const MountPoint* mount_point) const
    {
	string ret;

	switch (mount_point->get_mount_by())
	{
	    case MountByType::UUID:
		if (!is_permanent())
		    y2war("no filesystem uuid possible for non-permanent filesystem, using fallback mount-by");
		else if (uuid.empty())
		    y2war("no filesystem uuid defined, using fallback mount-by");
		else
		    ret = "UUID=" + uuid;
		break;

	    case MountByType::LABEL:
		if (!is_permanent())
		    y2war("no filesystem label possible for non-permanent filesystem, using fallback mount-by");
		else if (label.empty())
		    y2war("no filesystem label defined, using fallback mount-by");
		else
		    ret = "LABEL=" + label;
		break;

	    case MountByType::ID:
	    case MountByType::PATH:
	    case MountByType::DEVICE:
	    case MountByType::PARTUUID:
	    case MountByType::PARTLABEL:
		break;
	}

	if (ret.empty())
	{
	    const BlkDevice* blk_device = get_etc_fstab_blk_device(mount_point);

	    ret = blk_device->get_impl().get_fstab_spec(mount_point->get_mount_by());
	}

	return ret;
    }


    bool
    BlkFilesystem::Impl::spec_match(const string& spec) const
    {
	if (!uuid.empty())
	{
	    if ((spec == "UUID=" + uuid) ||
		(spec == DEV_DISK_BY_UUID_DIR "/" + uuid))
		return true;
	}

	if (!label.empty())
	{
	    if ((spec == "LABEL=" + label) ||
		(spec == DEV_DISK_BY_LABEL_DIR "/" + udev_encode(label)))
		return true;
	}

	return false;
    }


    const BlkDevice*
    BlkFilesystem::Impl::get_etc_fstab_blk_device(const MountPoint* mount_point) const
    {
	return get_blk_device();
    }


    vector<ExtendedFstabEntry>
    BlkFilesystem::Impl::find_etc_fstab_entries_unfiltered(SystemInfo::Impl& system_info) const
    {
	const EtcFstab& etc_fstab = system_info.getEtcFstab(get_storage()->prepend_rootprefix(ETC_FSTAB));

	vector<ExtendedFstabEntry> ret;

	for (int i = 0; i < etc_fstab.get_entry_count(); ++i)
	{
	    const FstabEntry* fstab_entry = etc_fstab.get_entry(i);
	    const string& spec = fstab_entry->get_spec();

	    if (spec_match(spec))
	    {
		MountPointPath mount_point_path(fstab_entry->get_mount_point(), true);
		ret.emplace_back(mount_point_path, fstab_entry);
	    }
	    else
	    {
		for (const BlkDevice* blk_device : get_blk_devices())
		{
		    if (blk_device->get_impl().spec_match(system_info, spec))
		    {
			// For tmpfs, proc the spec can be anything (including a valid block device).
			if (fstab_entry->get_fs_type() == FsType::TMPFS)
			    continue;

			const FilesystemUser* filesystem_user =
			    to_filesystem_user(get_devicegraph()->find_holder(blk_device->get_sid(), get_sid()));

			MountPointPath mount_point_path(fstab_entry->get_mount_point(), true);
			ret.emplace_back(mount_point_path, fstab_entry, filesystem_user->get_impl().get_id());
		    }
		}
	    }
	}

	return ret;
    }


    vector<ExtendedFstabEntry>
    BlkFilesystem::Impl::find_proc_mounts_entries_unfiltered(SystemInfo::Impl& system_info) const
    {
	const string& rootprefix = get_storage()->get_rootprefix();
	const ProcMounts& proc_mounts = system_info.getProcMounts();

	vector<ExtendedFstabEntry> ret;

	for (const BlkDevice* blk_device : get_blk_devices())
	{
	    for (const FstabEntry* fstab_entry : proc_mounts.get_by_name(blk_device->get_name(), system_info))
	    {
		// See find_etc_fstab_entries_unfiltered().
		if (fstab_entry->get_fs_type() == FsType::TMPFS)
		    continue;

		MountPointPath mount_point_path(fstab_entry->get_mount_point(), rootprefix);
		ret.emplace_back(mount_point_path, fstab_entry);
	    }
	}

	return ret;
    }


    Text
    BlkFilesystem::Impl::do_create_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by file system name (e.g. ext4),
			   // %2$s is replaced by one or more devices (e.g /dev/sda1 (1.00 GiB) and
			   // /dev/sdb2 (1.00 GiB))
			   _("Create %1$s on %2$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by file system name (e.g. ext4),
			   // %2$s is replaced by one or more devices (e.g /dev/sda1 (1.00 GiB) and
			   // /dev/sdb2 (1.00 GiB))
			   _("Creating %1$s on %2$s"));

	return sformat(text, get_displayname(), get_message_name());
    }


    Text
    BlkFilesystem::Impl::do_set_label_text(Tense tense) const
    {
	if (label.empty())
	{
	    Text text = tenser(tense,
			       // TRANSLATORS: displayed before action,
			       // %1$s is replaced by file system name (e.g. ext4),
			       // %2$s is replaced by one or more devices (e.g /dev/sda1 (1.00 GiB) and
			       // /dev/sdb2 (1.00 GiB)),
			       _("Clear label of %1$s on %2$s"),
			       // TRANSLATORS: displayed during action,
			       // %1$s is replaced by file system name (e.g. ext4),
			       // %2$s is replaced by one or more devices (e.g /dev/sda1 (1.00 GiB) and
			       // /dev/sdb2 (1.00 GiB)),
			       _("Clearing label of %1$s on %2$s"));

	    return sformat(text, get_displayname(), get_message_name());
	}
	else
	{
	    Text text = tenser(tense,
			       // TRANSLATORS: displayed before action,
			       // %1$s is replaced by file system name (e.g. ext4),
			       // %2$s is replaced by one or more devices (e.g /dev/sda1 (1.00 GiB) and
			       // /dev/sdb2 (1.00 GiB)),
			       // %3$s is replaced by label (e.g. ROOT)
			       _("Set label of %1$s on %2$s to %3$s"),
			       // TRANSLATORS: displayed during action,
			       // %1$s is replaced by file system name (e.g. ext4),
			       // %2$s is replaced by one or more devices (e.g /dev/sda1 (1.00 GiB) and
			       // /dev/sdb2 (1.00 GiB)),
			       // %3$s is replaced by label (e.g. ROOT)
			       _("Setting label of %1$s on %2$s to %3$s"));

	    return sformat(text, get_displayname(), get_message_name(), label);
	}
    }


    void
    BlkFilesystem::Impl::do_set_label() const
    {
	ST_THROW(LogicException("stub do_set_label called"));
    }


    Text
    BlkFilesystem::Impl::do_set_uuid_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by file system name (e.g. ext4),
			   // %2$s is replaced by one or more devices (e.g /dev/sda1 (1.00 GiB) and
			   // /dev/sdb2 (1.00 GiB)),
			   // %3$s is replaced by UUID (e.g. 3cfa63b5-4d29-43e6-8658-57b74f68fd7f)
			   _("Set UUID of %1$s on %2$s to %3$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by file system name (e.g. ext4),
			   // %2$s is replaced by one or more devices (e.g /dev/sda1 (1.00 GiB) and
			   // /dev/sdb2 (1.00 GiB)),
			   // %3$s is replaced by UUID (e.g. 3cfa63b5-4d29-43e6-8658-57b74f68fd7f)
			   _("Setting UUID of %1$s on %2$s to %3$s"));

	return sformat(text, get_displayname(), get_message_name(), uuid);
    }


    void
    BlkFilesystem::Impl::do_set_uuid() const
    {
	ST_THROW(LogicException("stub do_set_uuid called"));
    }


    Text
    BlkFilesystem::Impl::do_set_tune_options_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by file system name (e.g. ext4),
			   // %2$s is replaced by one or more devices (e.g /dev/sda1 (1.00 GiB) and
			   // /dev/sdb2 (1.00 GiB))
			   _("Set tune options of %1$s on %2$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by file system name (e.g. ext4),
			   // %2$s is replaced by one or more devices (e.g /dev/sda1 (1.00 GiB) and
			   // /dev/sdb2 (1.00 GiB))
			   _("Setting tune options of %1$s on %2$s"));

	return sformat(text, get_displayname(), get_message_name());
    }


    void
    BlkFilesystem::Impl::do_set_tune_options() const
    {
	ST_THROW(LogicException("stub do_set_tune_options called"));
    }


    Text
    BlkFilesystem::Impl::do_resize_text(const CommitData& commit_data, const Action::Resize* action) const
    {
	const BlkFilesystem* blk_filesystem_lhs = to_blk_filesystem(action->get_device(commit_data.actiongraph, LHS));

	const BlkDevice* blk_device_lhs = action->get_resized_blk_device(commit_data.actiongraph, LHS);
	const BlkDevice* blk_device_rhs = action->get_resized_blk_device(commit_data.actiongraph, RHS);

	if (get_blk_devices().size() == 1)
	{
	    Text text;

	    switch (action->resize_mode)
	    {
		case ResizeMode::SHRINK:
		    text = tenser(commit_data.tense,
				  // TRANSLATORS: displayed before action,
				  // %1$s is replaced by file system (e.g. ext4),
				  // %2$s is replaced by device name (e.g. /dev/sda1),
				  // %3$s is replaced by old size (e.g. 2.00 GiB),
				  // %4$s is replaced by new size (e.g. 1.00 GiB)
				  _("Shrink %1$s on %2$s from %3$s to %4$s"),
				  // TRANSLATORS: displayed during action,
				  // %1$s is replaced by file system (e.g. ext4),
				  // %2$s is replaced by device name (e.g. /dev/sda1),
				  // %3$s is replaced by old size (e.g. 2.00 GiB),
				  // %4$s is replaced by new size (e.g. 1.00 GiB)
				  _("Shrinking %1$s on %2$s from %3$s to %4$s"));
		    break;

		case ResizeMode::GROW:
		    text = tenser(commit_data.tense,
				  // TRANSLATORS: displayed before action,
				  // %1$s is replaced by file system (e.g. ext4),
				  // %2$s is replaced by device name (e.g. /dev/sda1),
				  // %3$s is replaced by old size (e.g. 1.00 GiB),
				  // %4$s is replaced by new size (e.g. 2.00 GiB)
				  _("Grow %1$s on %2$s from %3$s to %4$s"),
				  // TRANSLATORS: displayed during action,
				  // %1$s is replaced by file system (e.g. ext4),
				  // %2$s is replaced by device name (e.g. /dev/sda1),
				  // %3$s is replaced by old size (e.g. 1.00 GiB),
				  // %4$s is replaced by new size (e.g. 2.00 GiB)
				  _("Growing %1$s on %2$s from %3$s to %4$s"));
		    break;

		default:
		    ST_THROW(LogicException("invalid value for resize_mode"));
	    }

	    return sformat(text, get_displayname(), action->blk_device->get_name(),
			   blk_device_lhs->get_impl().get_size_text(),
			   blk_device_rhs->get_impl().get_size_text());
	}
	else
	{
	    Text text;

	    switch (action->resize_mode)
	    {
		case ResizeMode::SHRINK:
		    text = tenser(commit_data.tense,
				  // TRANSLATORS: displayed before action,
				  // %1$s is replaced by device name (e.g. /dev/sda1),
				  // %2$s is replaced by file system (e.g. ext4),
				  // %3$s is replaced by several devices (e.g /dev/sda1 (1.00 GiB) and
				  // /dev/sdb2 (1.00 GiB)),
				  // %4$s is replaced by old size (e.g. 2.00 GiB),
				  // %5$s is replaced by new size (e.g. 1.00 GiB)
				  _("Shrink %1$s of %2$s on %3$s from %4$s to %5$s"),
				  // TRANSLATORS: displayed during action,
				  // %1$s is replaced by device name (e.g. /dev/sda1),
				  // %2$s is replaced by file system (e.g. ext4),
				  // %3$s is replaced by several devices (e.g /dev/sda1 (1.00 GiB) and
				  // /dev/sdb2 (1.00 GiB)),
				  // %4$s is replaced by old size (e.g. 2.00 GiB),
				  // %5$s is replaced by new size (e.g. 1.00 GiB)
				  _("Shrinking %1$s of %2$s on %3$s from %4$s to %5$s"));
		    break;

		case ResizeMode::GROW:
		    text = tenser(commit_data.tense,
				  // TRANSLATORS: displayed before action,
				  // %1$s is replaced by device name (e.g. /dev/sda1),
				  // %2$s is replaced by file system (e.g. ext4),
				  // %3$s is replaced by several devices (e.g /dev/sda1 (1.00 GiB) and
				  // /dev/sdb2 (1.00 GiB)),
				  // %4$s is replaced by old size (e.g. 1.00 GiB),
				  // %5$s is replaced by new size (e.g. 2.00 GiB)
				  _("Grow %1$s of %2$s on %3$s from %4$s to %5$s"),
				  // TRANSLATORS: displayed during action,
				  // %1$s is replaced by device name (e.g. /dev/sda1),
				  // %2$s is replaced by file system (e.g. ext4),
				  // %3$s is replaced by several devices (e.g /dev/sda1 (1.00 GiB) and
				  // /dev/sdb2 (1.00 GiB)),
				  // %4$s is replaced by old size (e.g. 1.00 GiB),
				  // %5$s is replaced by new size (e.g. 2.00 GiB)
				  _("Growing %1$s of %2$s on %3$s from %4$s to %5$s"));
		    break;

		default:
		    ST_THROW(LogicException("invalid value for resize_mode"));
	    }

	    return sformat(text, action->blk_device->get_name(), get_displayname(),
			   blk_filesystem_lhs->get_impl().get_message_name(),
			   blk_device_lhs->get_impl().get_size_text(),
			   blk_device_rhs->get_impl().get_size_text());
	}
    }


    Text
    BlkFilesystem::Impl::do_delete_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by file system name (e.g. ext4),
			   // %2$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			   // and /dev/sdb2 (1.00 GiB))
			   _("Delete %1$s on %2$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by file system name (e.g. ext4),
			   // %2$s is replaced by one or more device names (e.g /dev/sda1 (1.00 GiB)
			   // and /dev/sdb2 (1.00 GiB))
			   _("Deleting %1$s on %2$s"));

	return sformat(text, get_displayname(), get_message_name());
    }


    void
    BlkFilesystem::Impl::do_delete() const
    {
	for (const BlkDevice* blk_device : get_blk_devices())
	{
	    blk_device->get_impl().wipe_device();
	}
    }

}
