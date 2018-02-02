/*
 * Copyright (c) [2016-2018] SUSE LLC
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


#include "storage/Utils/XmlFile.h"
#include "storage/Prober.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Holders/User.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devices/LvmPvImpl.h"
#include "storage/Devices/LvmVgImpl.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/FindBy.h"
#include "storage/FreeInfo.h"
#include "storage/UsedFeatures.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<LvmPv>::classname = "LvmPv";


    LvmPv::Impl::Impl(const xmlNode* node)
	: Device::Impl(node), uuid()
    {
	getChildValue(node, "uuid", uuid);
    }


    string
    LvmPv::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("LVM Physical Volume").translated;
    }


    void
    LvmPv::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValue(node, "uuid", uuid);
    }


    void
    LvmPv::Impl::check(const CheckCallbacks* check_callbacks) const
    {
	Device::Impl::check(check_callbacks);

	if (!has_single_parent_of_type<const BlkDevice>())
	    ST_THROW(Exception("LvmPv has no BlkDevice parent"));
    }


    bool
    LvmPv::Impl::has_blk_device() const
    {
	return has_single_parent_of_type<const BlkDevice>();
    }


    BlkDevice*
    LvmPv::Impl::get_blk_device()
    {
	return get_single_parent_of_type<BlkDevice>();
    }


    const BlkDevice*
    LvmPv::Impl::get_blk_device() const
    {
	return get_single_parent_of_type<const BlkDevice>();
    }


    bool
    LvmPv::Impl::has_lvm_vg() const
    {
	return has_single_child_of_type<const LvmVg>();
    }


    LvmVg*
    LvmPv::Impl::get_lvm_vg()
    {
	return get_single_child_of_type<LvmVg>();
    }


    const LvmVg*
    LvmPv::Impl::get_lvm_vg() const
    {
	return get_single_child_of_type<const LvmVg>();
    }


    void
    LvmPv::Impl::probe_lvm_pvs(Prober& prober)
    {
	for (const CmdPvs::Pv& pv : prober.get_system_info().getCmdPvs().get_pvs())
	{
	    LvmPv* lvm_pv = LvmPv::create(prober.get_probed());
	    lvm_pv->get_impl().set_uuid(pv.pv_uuid);

	    if (!pv.vg_uuid.empty())
	    {
		LvmVg* lvm_vg = LvmVg::Impl::find_by_uuid(prober.get_probed(), pv.vg_uuid);
		Subdevice::create(prober.get_probed(), lvm_pv, lvm_vg);
	    }
	}
    }


    void
    LvmPv::Impl::probe_pass_1b(Prober& prober)
    {
	const CmdPvs& cmd_pvs = prober.get_system_info().getCmdPvs();
	const CmdPvs::Pv& pv = cmd_pvs.find_by_pv_uuid(uuid);

	prober.add_holder(pv.pv_name, get_non_impl(), [](Devicegraph* probed, Device* a, Device* b) {
	    User::create(probed, a, b);
	});
    }


    ResizeInfo
    LvmPv::Impl::detect_resize_info() const
    {
	ResizeInfo resize_info(true);

	// A physical volume must have at least one extent and space for
	// metadata.

	// TODO 1 MiB due to metadata and physical extent alignment, needs
	// more research.

	resize_info.min_size = 1 * MiB;

	if (has_lvm_vg())
	{
	    const LvmVg* lvm_vg = get_lvm_vg();

	    resize_info.min_size += lvm_vg->get_extent_size();
	}

	// Currently we only support growing if the physical volume is already
	// on disk - shrinking is more complicated.

	if (exists_in_probed())
	{
	    resize_info.min_size = get_blk_device()->get_size();
	}

	return resize_info;
    }


    uint64_t
    LvmPv::Impl::used_features() const
    {
	return UF_LVM | Device::Impl::used_features();
    }


    void
    LvmPv::Impl::parent_has_new_region(const Device* parent)
    {
	if (has_lvm_vg())
	{
	    LvmVg* lvm_vg = get_lvm_vg();
	    lvm_vg->get_impl().parent_has_new_region(get_non_impl());
	}
    }


    LvmPv*
    LvmPv::Impl::find_by_uuid(Devicegraph* devicegraph, const string& uuid)
    {
	return storage::find_by_uuid<LvmPv>(devicegraph, uuid);
    }


    const LvmPv*
    LvmPv::Impl::find_by_uuid(const Devicegraph* devicegraph, const string& uuid)
    {
	return storage::find_by_uuid<const LvmPv>(devicegraph, uuid);
    }


    bool
    LvmPv::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Device::Impl::equal(rhs))
	    return false;

	return uuid == rhs.uuid;
    }


    void
    LvmPv::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Device::Impl::log_diff(log, rhs);

	storage::log_diff(log, "uuid", uuid, rhs.uuid);
    }


    void
    LvmPv::Impl::print(std::ostream& out) const
    {
	Device::Impl::print(out);

	out << " uuid:" << uuid;
    }


    Text
    LvmPv::Impl::do_create_text(Tense tense) const
    {
	const BlkDevice* blk_device = get_blk_device();

	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda1)
			   _("Create physical volume on %1$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda1)
			   _("Creating physical volume on %1$s"));

	return sformat(text, blk_device->get_name().c_str());
    }


    void
    LvmPv::Impl::do_create()
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = PVCREATEBIN " --force " + quote(blk_device->get_name());

	wait_for_devices({ blk_device });

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create LvmPv failed"));
    }


    Text
    LvmPv::Impl::do_resize_text(ResizeMode resize_mode, const Device* lhs, const Device* rhs,
				Tense tense) const
    {
	const BlkDevice* blk_device = get_blk_device();

	const BlkDevice* blk_device_lhs = to_lvm_pv(lhs)->get_impl().get_blk_device();
	const BlkDevice* blk_device_rhs = to_lvm_pv(rhs)->get_impl().get_blk_device();

	Text text;

	switch (resize_mode)
	{
	    case ResizeMode::SHRINK:
		text = tenser(tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by device name (e.g. /dev/sdb1),
			      // %2$s is replaced by old size (e.g. 2GiB),
			      // %3$s is replaced by new size (e.g. 1GiB)
			      _("Shrink physical volume on %1$s from %2$s to %3$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by device name (e.g. /dev/sdb1),
			      // %2$s is replaced by old size (e.g. 2GiB),
			      // %3$s is replaced by new size (e.g. 1GiB)
			      _("Shrinking physical volume on %1$s from %2$s to %3$s"));
		break;

	    case ResizeMode::GROW:
		text = tenser(tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by device name (e.g. /dev/sdb1),
			      // %2$s is replaced by old size (e.g. 1GiB),
			      // %3$s is replaced by new size (e.g. 2GiB)
			      _("Grow physical volume on %1$s from %2$s to %3$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by device name (e.g. /dev/sdb1),
			      // %2$s is replaced by old size (e.g. 1GiB),
			      // %3$s is replaced by new size (e.g. 2GiB)
			      _("Growing physical volume on %1$s from %2$s to %3$s"));
		break;

	    default:
		ST_THROW(LogicException("invalid value for resize_mode"));
	}

	return sformat(text, blk_device->get_name().c_str(),
		       blk_device_lhs->get_size_string().c_str(),
		       blk_device_rhs->get_size_string().c_str());
    }


    void
    LvmPv::Impl::do_resize(ResizeMode resize_mode, const Device* rhs) const
    {
	const BlkDevice* blk_device = get_blk_device();
	const BlkDevice* blk_device_rhs = to_lvm_pv(rhs)->get_impl().get_blk_device();

	string cmd_line = PVRESIZEBIN " " + quote(blk_device->get_name());
	if (resize_mode == ResizeMode::SHRINK)
	    cmd_line += " --yes --setphysicalvolumesize " + to_string(blk_device_rhs->get_size()) + "b";

	wait_for_devices({ blk_device });

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("resize LvmPv failed"));
    }


    Text
    LvmPv::Impl::do_delete_text(Tense tense) const
    {
	const BlkDevice* blk_device = get_blk_device();

	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda1)
			   _("Delete physical volume on %1$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda1)
			   _("Deleting physical volume on %1$s"));

	return sformat(text, blk_device->get_name().c_str());
    }


    void
    LvmPv::Impl::do_delete() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = PVREMOVEBIN " " + quote(blk_device->get_name());

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("delete LvmPv failed"));
    }

}
