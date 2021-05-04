/*
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
#include "storage/Storage.h"
#include "storage/Utils/Format.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<LvmPv>::classname = "LvmPv";

    const unsigned long long LvmPv::Impl::default_pe_start;


    LvmPv::Impl::Impl(const xmlNode* node)
	: Device::Impl(node)
    {
	getChildValue(node, "uuid", uuid);

	getChildValue(node, "pe-start", pe_start);
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

	setChildValue(node, "pe-start", pe_start);
    }


    void
    LvmPv::Impl::check(const CheckCallbacks* check_callbacks) const
    {
	Device::Impl::check(check_callbacks);

	if (check_callbacks)
	{
	    if (!has_single_parent_of_type<const BlkDevice>())
		check_callbacks->error(sformat("Physical volume %s is broken.", uuid));
	}
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


    void
    LvmPv::Impl::calculate_pe_start()
    {
	const BlkDevice* blk_device = get_blk_device();
	const Topology& topology = blk_device->get_topology();

	// The pe_start is simply optimal_io_size, even when the blk
	// device, e.g. partition, is not properly aligned.

	pe_start = max(default_pe_start, (unsigned long long) topology.get_optimal_io_size());
    }


    unsigned long long
    LvmPv::Impl::get_usable_size() const
    {
	unsigned long long size = get_blk_device()->get_size();

	if (pe_start >= size)
	    return 0;

	return size - pe_start;
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
	const CmdPvs& cmd_pvs = prober.get_system_info().getCmdPvs();

	for (const CmdPvs::Pv& pv : cmd_pvs.get_pvs())
	{
	    // Duplicate PVs are not inserted. Only the one not reported as
	    // duplicate by pvs is.

	    if (pv.duplicate)
		continue;

	    LvmPv* lvm_pv = LvmPv::create(prober.get_system());
	    lvm_pv->get_impl().set_uuid(pv.pv_uuid);
	    lvm_pv->get_impl().set_pe_start(pv.pe_start);

	    if (!pv.vg_uuid.empty())
	    {
		LvmVg* lvm_vg = LvmVg::Impl::find_by_uuid(prober.get_system(), pv.vg_uuid);
		Subdevice::create(prober.get_system(), lvm_pv, lvm_vg);
	    }
	}
    }


    void
    LvmPv::Impl::probe_pass_1b(Prober& prober)
    {
	const CmdPvs& cmd_pvs = prober.get_system_info().getCmdPvs();

	for (const CmdPvs::Pv& pv : cmd_pvs.get_pvs())
	{
	    // In the case of duplicates there are several PVs with the same
	    // UUID. All underlying devices must be marked as parents,
	    // otherwise the underlying devices would look unused.

	    if (pv.pv_uuid == uuid && !pv.missing)
	    {
		prober.add_holder(pv.pv_name, get_non_impl(), [](Devicegraph* system, Device* a, Device* b) {
		    User::create(system, a, b);
		});
	    }
	}
    }


    ResizeInfo
    LvmPv::Impl::detect_resize_info(const BlkDevice* blk_device) const
    {
	ResizeInfo resize_info(true, 0);

	// A physical volume must have at least one extent and space for
	// metadata.

	resize_info.min_size = get_pe_start();

	if (has_lvm_vg())
	{
	    const LvmVg* lvm_vg = get_lvm_vg();

	    resize_info.min_size += lvm_vg->get_extent_size();
	}

	// Currently we only support growing if the physical volume is already
	// on disk - shrinking is more complicated.

	if (exists_in_system())
	{
	    resize_info.min_size = get_blk_device()->get_size();
	}

	return resize_info;
    }


    uf_t
    LvmPv::Impl::used_features(UsedFeaturesDependencyType used_features_dependency_type) const
    {
	return UF_LVM | Device::Impl::used_features(used_features_dependency_type);
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

	return uuid == rhs.uuid && pe_start == rhs.pe_start;
    }


    void
    LvmPv::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Device::Impl::log_diff(log, rhs);

	storage::log_diff(log, "uuid", uuid, rhs.uuid);

	storage::log_diff(log, "pe-start", pe_start, rhs.pe_start);
    }


    void
    LvmPv::Impl::print(std::ostream& out) const
    {
	Device::Impl::print(out);

	out << " uuid:" << uuid << " pe-start:" << pe_start;
    }


    void
    LvmPv::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	if (!has_blk_device())
	    return;

	Device::Impl::add_delete_actions(actiongraph);
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

	return sformat(text, blk_device->get_name());
    }


    void
    LvmPv::Impl::do_create()
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = PVCREATE_BIN " --force " + quote(blk_device->get_name());

	wait_for_devices({ blk_device });

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    void
    LvmPv::Impl::do_create_post_verify() const
    {
	// log some data about the physical volume that might be useful for debugging

	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = PVS_BIN " --options pv_name,pv_uuid,pv_size,pe_start --units b " +
	    quote(blk_device->get_name());

	SystemCmd cmd(cmd_line, SystemCmd::NoThrow);
    }


    Text
    LvmPv::Impl::do_resize_text(const CommitData& commit_data, const Action::Resize* action) const
    {
	const LvmPv* lvm_pv_lhs = to_lvm_pv(action->get_device(commit_data.actiongraph, LHS));
	const LvmPv* lvm_pv_rhs = to_lvm_pv(action->get_device(commit_data.actiongraph, RHS));

	const BlkDevice* blk_device_lhs = lvm_pv_lhs->get_impl().get_blk_device();
	const BlkDevice* blk_device_rhs = lvm_pv_rhs->get_impl().get_blk_device();

	Text text;

	switch (action->resize_mode)
	{
	    case ResizeMode::SHRINK:
		text = tenser(commit_data.tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by device name (e.g. /dev/sdb1),
			      // %2$s is replaced by old size (e.g. 2.00 GiB),
			      // %3$s is replaced by new size (e.g. 1.00 GiB)
			      _("Shrink physical volume on %1$s from %2$s to %3$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by device name (e.g. /dev/sdb1),
			      // %2$s is replaced by old size (e.g. 2.00 GiB),
			      // %3$s is replaced by new size (e.g. 1.00 GiB)
			      _("Shrinking physical volume on %1$s from %2$s to %3$s"));
		break;

	    case ResizeMode::GROW:
		text = tenser(commit_data.tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by device name (e.g. /dev/sdb1),
			      // %2$s is replaced by old size (e.g. 1.00 GiB),
			      // %3$s is replaced by new size (e.g. 2.00 GiB)
			      _("Grow physical volume on %1$s from %2$s to %3$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by device name (e.g. /dev/sdb1),
			      // %2$s is replaced by old size (e.g. 1.00 GiB),
			      // %3$s is replaced by new size (e.g. 2.00 GiB)
			      _("Growing physical volume on %1$s from %2$s to %3$s"));
		break;

	    default:
		ST_THROW(LogicException("invalid value for resize_mode"));
	}

	return sformat(text, action->blk_device->get_name(), blk_device_lhs->get_impl().get_size_text(),
		       blk_device_rhs->get_impl().get_size_text());
    }


    void
    LvmPv::Impl::do_resize(const CommitData& commit_data, const Action::Resize* action) const
    {
	const LvmPv* lvm_pv_rhs = to_lvm_pv(action->get_device(commit_data.actiongraph, RHS));

	const BlkDevice* blk_device_rhs = lvm_pv_rhs->get_impl().get_blk_device();

	string cmd_line = PVRESIZE_BIN " " + quote(action->blk_device->get_name());
	if (action->resize_mode == ResizeMode::SHRINK)
	    cmd_line += " --yes --setphysicalvolumesize " + to_string(blk_device_rhs->get_size()) + "b";

	wait_for_devices({ action->blk_device });

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
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

	return sformat(text, blk_device->get_name());
    }


    void
    LvmPv::Impl::do_delete() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = PVREMOVE_BIN " " + quote(blk_device->get_name());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }

}
