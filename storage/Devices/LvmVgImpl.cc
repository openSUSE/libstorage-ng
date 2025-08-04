/*
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


#include <algorithm>

#include "storage/Utils/XmlFile.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/Math.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/HumanString.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/SystemInfo/SystemInfoImpl.h"
#include "storage/Devices/LvmVgImpl.h"
#include "storage/Devices/LvmPvImpl.h"
#include "storage/Devices/LvmLvImpl.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Holders/User.h"
#include "storage/Storage.h"
#include "storage/FindBy.h"
#include "storage/Prober.h"
#include "storage/Utils/Format.h"
#include "storage/Actions/ReallotImpl.h"
#include "storage/Actions/ResizeImpl.h"
#include "storage/Actions/CreateImpl.h"
#include "storage/Actions/DeleteImpl.h"
#include "storage/Actions/ReduceMissingImpl.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<LvmVg>::classname = "LvmVg";


    LvmVg::Impl::Impl(const string& vg_name)
	: Device::Impl(), vg_name(vg_name), region(0, 0, default_extent_size)
    {
    }


    LvmVg::Impl::Impl(const xmlNode* node)
	: Device::Impl(node), region(0, 0, default_extent_size)
    {
	if (!getChildValue(node, "vg-name", vg_name))
	    ST_THROW(Exception("no vg-name"));

	getChildValue(node, "uuid", uuid);

	getChildValue(node, "region", region);
	getChildValue(node, "reserved-extents", reserved_extents);
    }


    string
    LvmVg::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("LVM Volume Group").translated;
    }


    void
    LvmVg::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValue(node, "vg-name", vg_name);
	setChildValue(node, "uuid", uuid);

	setChildValue(node, "region", region);
	setChildValue(node, "reserved-extents", reserved_extents);
    }


    void
    LvmVg::Impl::check(const CheckCallbacks* check_callbacks) const
    {
	Device::Impl::check(check_callbacks);

	if (get_vg_name().empty())
	    ST_THROW(Exception("LvmVg has no vg-name"));

	if (check_callbacks)
	{
	    if (is_overcommitted())
		check_callbacks->error(sformat("Volume group %s is overcommitted.",
					       vg_name));
	}
    }


    void
    LvmVg::Impl::probe_lvm_vgs(Prober& prober)
    {
	for (const CmdVgs::Vg& vg : prober.get_system_info().getCmdVgs().get_vgs())
	{
	    LvmVg* lvm_vg = LvmVg::create(prober.get_system(), vg.vg_name);
	    lvm_vg->get_impl().set_uuid(vg.vg_uuid);
	    lvm_vg->get_impl().probe_pass_1a(prober);
	}
    }


    void
    LvmVg::Impl::probe_pass_1a(Prober& prober)
    {
	Device::Impl::probe_pass_1a(prober);

	const CmdVgs& cmd_vgs = prober.get_system_info().getCmdVgs();
	const CmdVgs::Vg& vg = cmd_vgs.find_by_vg_uuid(uuid);

	region = Region(0, vg.extent_count, vg.extent_size, ULL_HACK);
    }


    void
    LvmVg::Impl::calculate_reserved_extents(Prober& prober)
    {
	const CmdVgs& cmd_vgs = prober.get_system_info().getCmdVgs();
	const CmdVgs::Vg& vg = cmd_vgs.find_by_vg_uuid(uuid);

	reserved_extents = number_of_free_extents() - vg.free_extent_count;
    }


    unsigned long long
    LvmVg::Impl::get_size() const
    {
	return region.to_bytes(region.get_length());
    }


    Text
    LvmVg::Impl::get_size_text() const
    {
	// TODO see BlkDevice::Impl::get_size_text()

	return Text(byte_to_humanstring(get_size(), true, 2, false),
		    byte_to_humanstring(get_size(), false, 2, false));
    }


    void
    LvmVg::Impl::set_extent_size(unsigned long long extent_size)
    {
	// see vgcreate(8) for valid values

	unsigned long long old_extent_size = region.get_block_size(ULL_HACK);

	if (!is_power_of_two(extent_size))
	    ST_THROW(InvalidExtentSize("extent size not a power of two"));

	if (!is_multiple_of(extent_size, 128 * KiB))
	    ST_THROW(InvalidExtentSize("extent size not multiple of 128 KiB"));

	region.set_block_size(extent_size, ULL_HACK);

	calculate_region();

	for (LvmLv* lvm_lv : get_lvm_lvs())
	{
	    Region region = lvm_lv->get_region();

	    region.set_block_size(extent_size, ULL_HACK);
	    region.set_length(region.get_length() * old_extent_size / extent_size);

	    lvm_lv->set_region(region);
	}
    }


    unsigned long long
    LvmVg::Impl::number_of_used_extents(const vector<sid_t>& ignore_sids) const
    {
	unsigned long long extent_size = get_extent_size();

	unsigned long long ret = 0;
	unsigned long long spare_metadata_extents = 0;

	for (const LvmLv* lvm_lv : get_lvm_lvs())
	{
	    if (contains(ignore_sids, lvm_lv->get_sid()))
		continue;

	    // Note: thins are not included in LvmVg::get_lvm_lvs().

	    ret += lvm_lv->get_impl().get_used_extents();

	    // For thin pools that do not exist in probed also add the
	    // metadata size. For thin pools that do exist in probed the
	    // metadata size is included in reserved_extents.

	    if (lvm_lv->get_lv_type() == LvType::THIN_POOL && !lvm_lv->exists_in_system())
	    {
		unsigned long long metadata_size = lvm_lv->get_impl().default_metadata_size();
		unsigned long long metadata_extents = metadata_size / extent_size;

		ret += metadata_extents;
		spare_metadata_extents = max(spare_metadata_extents, metadata_extents);
	    }
	}

	ret += spare_metadata_extents;

	return ret;
    }


    unsigned long long
    LvmVg::Impl::number_of_free_extents(const vector<sid_t>& ignore_sids) const
    {
	unsigned long long a = number_of_extents();
	unsigned long long b = number_of_used_extents(ignore_sids) + reserved_extents;

	return b >= a ? 0 : a - b;
    }


    bool
    LvmVg::Impl::is_overcommitted() const
    {
	unsigned long long a = number_of_extents();
	unsigned long long b = number_of_used_extents() + reserved_extents;

	return b > a;
    }


    unsigned long long
    LvmVg::Impl::max_size_for_lvm_lv(LvType lv_type, const vector<sid_t>& ignore_sids) const
    {
	unsigned long long extent_size = get_extent_size();

	switch (lv_type)
	{
	    case LvType::NORMAL:
	    {
		return number_of_free_extents(ignore_sids) * extent_size;
	    }

	    case LvType::THIN_POOL:
	    {
		unsigned long long data_size = number_of_free_extents(ignore_sids) * extent_size;

		unsigned long long chunk_size = LvmLv::Impl::default_chunk_size(data_size);
		unsigned long long metadata_size =
		    LvmLv::Impl::default_metadata_size(data_size, chunk_size, extent_size);

		// Subtract metadata size twice due to spare metadata. This is
		// a bit conservative since there might already be a spare
		// metadata.

		if (data_size <= 2 * metadata_size)
		    return 0;

		return data_size - 2 * metadata_size;
	    }

	    default:
	    {
		return 0;
	    }
	}
    }


    void
    LvmVg::Impl::parent_has_new_region(const Device* parent)
    {
	calculate_region();
    }


    void
    LvmVg::Impl::set_vg_name(const string& vg_name)
    {
	Impl::vg_name = vg_name;

	// TODO call set_name() for all lvm_lvs
    }


    bool
    LvmVg::Impl::is_valid_vg_name(const string& vg_name)
    {
	static const regex rx("[a-zA-Z0-9+_.][a-zA-Z0-9+_.-]*", regex::extended);

	if (!regex_match(vg_name, rx))
	    return false;

	if (vg_name == "." || vg_name == "..")
	    return false;

	if (vg_name.size() > 127)
	    return false;

	return true;
    }


    ResizeInfo
    LvmVg::Impl::detect_resize_info(const BlkDevice* blk_device) const
    {
	return ResizeInfo(false, RB_RESIZE_NOT_SUPPORTED_BY_DEVICE);
    }


    void
    LvmVg::Impl::calculate_region()
    {
	unsigned long long extent_size = region.get_block_size(ULL_HACK);

	unsigned long long extent_count = 0;

	for (const LvmPv* lvm_pv : get_lvm_pvs())
	{
	    unsigned long long usable_size = lvm_pv->get_usable_size();

	    if (usable_size > 0)
		extent_count += usable_size / extent_size;
	}

	region.set_length(extent_count);
    }


    LvmPv*
    LvmVg::Impl::add_lvm_pv(BlkDevice* blk_device)
    {
	ST_CHECK_PTR(blk_device);

	Devicegraph* devicegraph = get_devicegraph();

	LvmPv* lvm_pv = nullptr;

	switch (blk_device->num_children())
	{
	    case 0:
		lvm_pv = LvmPv::create(devicegraph);
		User::create(devicegraph, blk_device, lvm_pv);
		break;

	    case 1:
		lvm_pv = blk_device->get_impl().get_single_child_of_type<LvmPv>();
		if (lvm_pv->has_children())
		    ST_THROW(WrongNumberOfChildren(blk_device->num_children(), 0));
		break;

	    default:
		ST_THROW(Exception("illegal number of children"));
	}

	lvm_pv->get_impl().calculate_pe_start();

	Subdevice::create(devicegraph, lvm_pv, get_non_impl());

	calculate_region();

	return lvm_pv;
    }


    void
    LvmVg::Impl::remove_lvm_pv(BlkDevice* blk_device)
    {
	ST_CHECK_PTR(blk_device);

	LvmPv* lvm_pv = blk_device->get_impl().get_single_child_of_type<LvmPv>();

	LvmVg* lvm_vg = lvm_pv->get_impl().get_single_child_of_type<LvmVg>();

	if (lvm_vg != get_non_impl())
	    ST_THROW(Exception("not a blk device of volume group"));

	Devicegraph* devicegraph = get_devicegraph();

	devicegraph->get_impl().remove_vertex(lvm_pv->get_impl().get_vertex());

	calculate_region();
    }


    vector<LvmPv*>
    LvmVg::Impl::get_lvm_pvs()
    {
	Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	return devicegraph.filter_devices_of_type<LvmPv>(devicegraph.parents(vertex));
    }


    vector<const LvmPv*>
    LvmVg::Impl::get_lvm_pvs() const
    {
	const Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	return devicegraph.filter_devices_of_type<const LvmPv>(devicegraph.parents(vertex));
    }


    LvmLv*
    LvmVg::Impl::create_lvm_lv(const std::string& lv_name, LvType lv_type,
			       unsigned long long size)
    {
	Devicegraph* devicegraph = get_devicegraph();

	LvmLv* lvm_lv = LvmLv::create(devicegraph, vg_name, lv_name, lv_type);
	Subdevice::create(devicegraph, get_non_impl(), lvm_lv);

	unsigned long long extent_size = region.get_block_size(ULL_HACK);
	lvm_lv->set_region(Region(0, size / extent_size, extent_size, ULL_HACK));

	return lvm_lv;
    }


    void
    LvmVg::Impl::delete_lvm_lv(LvmLv* lvm_lv)
    {
	lvm_lv->remove_descendants(View::REMOVE);

	get_devicegraph()->remove_device(lvm_lv);
    }


    LvmLv*
    LvmVg::Impl::get_lvm_lv(const string& lv_name)
    {
	Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	for (LvmLv* lvm_lv : devicegraph.filter_devices_of_type<LvmLv>(devicegraph.children(vertex)))
	{
	    if (lvm_lv->get_lv_name() == lv_name)
		return lvm_lv;
	}

	ST_THROW(LvmLvNotFoundByLvName(lv_name));
    }


    vector<LvmLv*>
    LvmVg::Impl::get_lvm_lvs()
    {
	Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	return devicegraph.filter_devices_of_type<LvmLv>(devicegraph.children(vertex));
    }


    vector<const LvmLv*>
    LvmVg::Impl::get_lvm_lvs() const
    {
	const Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	return devicegraph.filter_devices_of_type<LvmLv>(devicegraph.children(vertex));
    }


    LvmVg*
    LvmVg::Impl::find_by_uuid(Devicegraph* devicegraph, const std::string& uuid)
    {
	return storage::find_by_uuid<LvmVg>(devicegraph, uuid);
    }


    const LvmVg*
    LvmVg::Impl::find_by_uuid(const Devicegraph* devicegraph, const std::string& uuid)
    {
	return storage::find_by_uuid<const LvmVg>(devicegraph, uuid);
    }


    bool
    LvmVg::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Device::Impl::equal(rhs))
	    return false;

	return vg_name == rhs.vg_name && uuid == rhs.uuid && region == rhs.region &&
	    reserved_extents == rhs.reserved_extents;
    }


    void
    LvmVg::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Device::Impl::log_diff(log, rhs);

	storage::log_diff(log, "vg-name", vg_name, rhs.vg_name);
	storage::log_diff(log, "uuid", uuid, rhs.uuid);

	storage::log_diff(log, "region", region, rhs.region);
	storage::log_diff(log, "reserved-extents", reserved_extents, rhs.reserved_extents);
    }


    void
    LvmVg::Impl::print(std::ostream& out) const
    {
	Device::Impl::print(out);

	out << " vg-name:" << vg_name << " uuid:" << uuid << " region:" << region
	    << " reserved-extents:" << reserved_extents;
    }


    bool
    LvmVg::Impl::is_partial() const
    {
	vector<const LvmPv*> lvm_pvs = get_lvm_pvs();

	return any_of(lvm_pvs.begin(), lvm_pvs.end(), [](const LvmPv* lvm_pv) {
	    return !lvm_pv->has_blk_device();
	});
    }


    void
    LvmVg::Impl::add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs_base) const
    {
	Device::Impl::add_modify_actions(actiongraph, lhs_base);

	const Impl& lhs = dynamic_cast<const Impl&>(lhs_base->get_impl());

	if (get_vg_name() != lhs.get_vg_name())
	{
	    shared_ptr<Action::Base> action = make_shared<Action::Rename>(get_sid());
	    actiongraph.add_vertex(action);
	    action->first = action->last = true;
	}
    }


    void
    LvmVg::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<shared_ptr<Action::Base>> actions;

	if (is_partial())
	    actions.push_back(make_shared<Action::ReduceMissing>(get_sid()));

	actions.push_back(make_shared<Action::Delete>(get_sid()));

	actiongraph.add_chain(actions);
    }


    Text
    LvmVg::Impl::do_create_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by volume group name (e.g. system),
			   // %2$s is replaced by size (e.g. 2.00 GiB),
			   // %3$s is replaced by one or more devices (e.g /dev/sda1 (1.00 GiB) and
			   // /dev/sdb2 (1.00 GiB))
			   _("Create volume group %1$s (%2$s) from %3$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by volume group name (e.g. system),
			   // %2$s is replaced by size (e.g. 2.00 GiB),
			   // %3$s is replaced by one or more devices (e.g /dev/sda1 (1.00 GiB) and
			   // /dev/sdb2 (1.00 GiB))
			   _("Creating volume group %1$s (%2$s) from %3$s"));

	vector<const BlkDevice*> blk_devices;
	for (const LvmPv* lvm_pv : get_lvm_pvs())
	    blk_devices.push_back(lvm_pv->get_blk_device());

	return sformat(text, vg_name, get_size_text(), join(blk_devices, JoinMode::COMMA, 20));
    }


    void
    LvmVg::Impl::do_create()
    {
	SystemCmd::Args cmd_args = { VGCREATE_BIN, "--verbose", "--physicalextentsize",
	    to_string(get_extent_size()) + "b", "--", vg_name };

	for (const LvmPv* lvm_pv : get_lvm_pvs())
	    cmd_args << lvm_pv->get_blk_device()->get_name();

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);
    }


    void
    LvmVg::Impl::do_create_post_verify() const
    {
	// log some data about the volume group that might be useful for debugging

	CmdVgs cmd_vgs(vg_name);
	const CmdVgs::Vg& vg = cmd_vgs.get_vgs()[0];
	log_unexpected("lmv-vg extent-count", number_of_extents(), vg.extent_count);
    }


    Text
    LvmVg::Impl::do_rename_text(const CommitData& commit_data, const Action::Rename* action) const
    {
	const LvmVg* lvm_vg_lhs = to_lvm_vg(action->get_device(commit_data.actiongraph, LHS));
	const LvmVg* lvm_vg_rhs = to_lvm_vg(action->get_device(commit_data.actiongraph, RHS));

	// TRANSLATORS:
	// %1$s is replaced with the old volume group name (e.g. foo),
	// %2$s is replaced with the new volume group name (e.g. bar)
	Text text = _("Rename volume group %1$s to %2$s");

	return sformat(text, lvm_vg_lhs->get_displayname(), lvm_vg_rhs->get_displayname());
    }


    void
    LvmVg::Impl::do_rename(const CommitData& commit_data, const Action::Rename* action) const
    {
	const LvmVg* lvm_vg_lhs = to_lvm_vg(action->get_device(commit_data.actiongraph, LHS));
	const LvmVg* lvm_vg_rhs = to_lvm_vg(action->get_device(commit_data.actiongraph, RHS));

	SystemCmd::Args cmd_args = { VGRENAME_BIN, "--verbose", lvm_vg_lhs->get_vg_name(),
	    lvm_vg_rhs->get_vg_name() };

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);
    }


    Text
    LvmVg::Impl::do_delete_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by volume group name (e.g. system),
			   // %2$s is replaced by size (e.g. 2.00 GiB)
			   _("Delete volume group %1$s (%2$s)"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by volume group name (e.g. system),
			   // %2$s is replaced by size (e.g. 2.00 GiB)
			   _("Deleting volume group %1$s (%2$s)"));

	return sformat(text, vg_name, get_size_text());
    }


    void
    LvmVg::Impl::do_delete() const
    {
	SystemCmd::Args cmd_args = { VGREMOVE_BIN, "--verbose", "--", vg_name };

	udev_settle();

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);
    }


    Text
    LvmVg::Impl::do_reallot_text(const CommitData& commit_data, const Action::Reallot* action) const
    {
	Text text;

	switch (action->reallot_mode)
	{
	    case ReallotMode::REDUCE:
		text = tenser(commit_data.tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by device name (e.g. /dev/sdd),
			      // %2$s is replaced by volume group name (e.g. system)
			      _("Remove %1$s from %2$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by device name (e.g. /dev/sdd),
			      // %2$s is replaced by volume group name (e.g. system)
			      _("Removing %1$s from %2$s"));
		break;

	    case ReallotMode::EXTEND:
		text = tenser(commit_data.tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by device name (e.g. /dev/sdd),
			      // %2$s is replaced by volume group name (e.g. system)
			      _("Add %1$s to %2$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by device name (e.g. /dev/sdd),
			      // %2$s is replaced by volume group name (e.g. system)
			      _("Adding %1$s to %2$s"));
		break;

	    default:
		ST_THROW(LogicException("invalid value for reallot_mode"));
	}

	const LvmPv* lvm_pv = to_lvm_pv(action->device);
	const BlkDevice* blk_device = lvm_pv->get_blk_device();

	return sformat(text, blk_device->get_name(), vg_name);
    }


    void
    LvmVg::Impl::do_reallot(const CommitData& commit_data, const Action::Reallot* action) const
    {
	const LvmPv* lvm_pv = to_lvm_pv(action->device);

	switch (action->reallot_mode)
	{
	    case ReallotMode::REDUCE:
		do_reduce(lvm_pv);
		return;

	    case ReallotMode::EXTEND:
		do_extend(lvm_pv);
		return;
	}

	ST_THROW(LogicException("invalid value for reallot_mode"));
    }


    void
    LvmVg::Impl::do_reduce(const LvmPv* lvm_pv) const
    {
	SystemCmd::Args cmd_args = { VGREDUCE_BIN, "--verbose", "--", vg_name, lvm_pv->get_blk_device()->get_name() };

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);
    }


    void
    LvmVg::Impl::do_extend(const LvmPv* lvm_pv) const
    {
	SystemCmd::Args cmd_args = { VGEXTEND_BIN, "--verbose", "--", vg_name, lvm_pv->get_blk_device()->get_name() };

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);
    }


    Text
    LvmVg::Impl::do_reduce_missing_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by volume group name (e.g. system)
			   _("Reduce volume group %1$s by missing physical volumes"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by volume group name (e.g. system)
			   _("Reducing volume group %1$s by missing physical volumes"));

	return sformat(text, vg_name);
    }


    void
    LvmVg::Impl::do_reduce_missing() const
    {
	SystemCmd::Args cmd_args = { VGREDUCE_BIN, "--verbose", "--removemissing", "--force", "--", vg_name };

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);
    }


    void
    LvmVg::Impl::run_dependency_manager(Actiongraph::Impl& actiongraph)
    {
	// Ensure that a volume group with the same name is deleted before the new is
	// created.

	struct AllActions
	{
	    optional<Actiongraph::Impl::vertex_descriptor> create_action;
	    optional<Actiongraph::Impl::vertex_descriptor> delete_action;
	};

	map<string, AllActions> all_actions_per_vg_name;

	for (Actiongraph::Impl::vertex_descriptor vertex : actiongraph.vertices())
	{
	    const Action::Base* action = actiongraph[vertex];
	    if (!action->affects_device())
		continue;

	    const Action::Create* create_action = dynamic_cast<const Action::Create*>(action);
	    if (create_action)
	    {
		const Device* device = create_action->get_device(actiongraph);
		if (is_lvm_vg(device))
		{
		    const LvmVg* lvm_vg = to_lvm_vg(device);
		    all_actions_per_vg_name[lvm_vg->get_vg_name()].create_action = vertex;
		}
	    }

	    const Action::Delete* delete_action = dynamic_cast<const Action::Delete*>(action);
	    if (delete_action)
	    {
		const Device* device = delete_action->get_device(actiongraph);
		if (is_lvm_vg(device))
		{
		    const LvmVg* lvm_vg = to_lvm_vg(device);
		    all_actions_per_vg_name[lvm_vg->get_vg_name()].delete_action = vertex;
		}
	    }
	}

	for (const map<string, AllActions>::value_type& tmp : all_actions_per_vg_name)
	{
	    const AllActions& all_actions = tmp.second;

	    if (all_actions.create_action && all_actions.delete_action)
		actiongraph.add_edge(all_actions.delete_action.value(), all_actions.create_action.value());
	}
    }


    void
    LvmVg::Impl::add_dependencies(Actiongraph::Impl& actiongraph) const
    {
	// First, all the operations removing or shrinking LVs.
	vector<Actiongraph::Impl::vertex_descriptor> decrease_lv_actions;

	// Afterward, all the operations adding, removing or resizing PVs.
	// Since only unused extents of physical volume can be removed by
	// reduce and shrink the order of reallot and resize actions is not
	// important here.
	vector<Actiongraph::Impl::vertex_descriptor> reallot_and_resize_actions;

	// Finally, operations adding or growing LVs.
	vector<Actiongraph::Impl::vertex_descriptor> increase_lv_actions;

	// Look for the relevant actions
	for (Actiongraph::Impl::vertex_descriptor vertex : actiongraph.vertices())
	{
	    const Action::Base* action = actiongraph[vertex];

	    if (action_frees_vg_space(action, actiongraph))
		decrease_lv_actions.push_back(vertex);
	    else if (action_is_my_reallot(action, actiongraph))
		reallot_and_resize_actions.push_back(vertex);
	    else if (action_is_my_pv_resize(action, actiongraph))
		reallot_and_resize_actions.push_back(vertex);
	    else if (action_uses_vg_space(action, actiongraph))
		increase_lv_actions.push_back(vertex);
	}

	// Add the dependencies to the action graph
	actiongraph.add_chain({ decrease_lv_actions, reallot_and_resize_actions,
		    increase_lv_actions });
    }


    bool
    LvmVg::Impl::action_is_my_reallot(const Action::Base* action, const Actiongraph::Impl& actiongraph) const
    {
	if (!action->affects_device())
	    return false;

	const Action::Reallot* reallot = dynamic_cast<const Action::Reallot*>(action);
	return reallot && reallot->sid == get_sid();
    }


    bool
    LvmVg::Impl::action_is_my_pv_resize(const Action::Base* action, const Actiongraph::Impl& actiongraph) const
    {
	if (!action->affects_device())
	    return false;

	const Action::Resize* resize = dynamic_cast<const Action::Resize*>(action);
	if (!resize)
	    return false;

	const Devicegraph* devicegraph = actiongraph.get_devicegraph(RHS);
	const Device* device = devicegraph->find_device(resize->sid);
	if (!is_lvm_pv(device))
	    return false;

	const LvmPv* lvm_pv = to_lvm_pv(device);
	if (!lvm_pv->has_lvm_vg())
	    return false;

	return lvm_pv->get_lvm_vg()->get_sid() == get_sid();
    }


    bool
    LvmVg::Impl::action_uses_vg_space(const Action::Base* action, const Actiongraph::Impl& actiongraph) const
    {
	if (!action->affects_device())
	    return false;

	const Action::Create* create_action = dynamic_cast<const Action::Create*>(action);
	if (create_action)
	    return is_my_lvm_lv(create_action->get_device(actiongraph));

	const Action::Resize* resize_action = dynamic_cast<const Action::Resize*>(action);
	if (resize_action && resize_action->resize_mode == ResizeMode::GROW)
	    return is_my_lvm_lv_using_extents(resize_action->get_device(actiongraph, RHS));

	return false;
    }


    bool
    LvmVg::Impl::action_frees_vg_space(const Action::Base* action, const Actiongraph::Impl& actiongraph) const
    {
	if (!action->affects_device())
	    return false;

	const Action::Delete* delete_action = dynamic_cast<const Action::Delete*>(action);
	if (delete_action)
	    return is_my_lvm_lv(delete_action->get_device(actiongraph));

	const Action::Resize* resize_action = dynamic_cast<const Action::Resize*>(action);
	if (resize_action && resize_action->resize_mode == ResizeMode::SHRINK)
	    return is_my_lvm_lv_using_extents(resize_action->get_device(actiongraph, LHS));

	return false;
    }


    bool
    LvmVg::Impl::is_my_lvm_lv(const Device* device) const
    {
	if (!is_lvm_lv(device))
	    return false;

	const LvmLv* lvm_lv = to_lvm_lv(device);

	return lvm_lv->get_lvm_vg()->get_sid() == get_sid();
    }


    bool
    LvmVg::Impl::is_my_lvm_lv_using_extents(const Device* device) const
    {
	if (!is_lvm_lv(device))
	    return false;

	const LvmLv* lvm_lv = to_lvm_lv(device);

	if (lvm_lv->get_lv_type() == LvType::THIN)
	    return false;

	return lvm_lv->get_lvm_vg()->get_sid() == get_sid();
    }

}
