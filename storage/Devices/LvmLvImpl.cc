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


#include <boost/algorithm/string.hpp>

#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/Math.h"
#include "storage/Utils/CallbacksImpl.h"
#include "storage/SystemInfo/SystemInfoImpl.h"
#include "storage/Devices/LvmLvImpl.h"
#include "storage/Devices/LvmVgImpl.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Holders/Snapshot.h"
#include "storage/Storage.h"
#include "storage/FreeInfo.h"
#include "storage/Holders/User.h"
#include "storage/Devicegraph.h"
#include "storage/FindBy.h"
#include "storage/Prober.h"
#include "storage/Redirect.h"
#include "storage/Utils/Format.h"
#include "storage/Actions/ResizeImpl.h"
#include "storage/Actions/RenameImpl.h"


using namespace std;


namespace storage
{

    const char* DeviceTraits<LvmLv>::classname = "LvmLv";


    const vector<string> EnumTraits<LvType>::names({
	"unknown", "normal", "thin-pool", "thin", "raid", "cache-pool", "cache", "writecache",
	"snapshot", "mirror"
    });


    LvmLv::Impl::Impl(const string& vg_name, const string& lv_name, LvType lv_type)
	: BlkDevice::Impl(make_name(vg_name, lv_name)), lv_name(lv_name), lv_type(lv_type),
	  stripes(lv_type == LvType::THIN ? 0 : 1)
    {
	set_active(lv_type != LvType::THIN_POOL && lv_type != LvType::CACHE_POOL);

	set_dm_table_name(make_dm_table_name(vg_name, lv_name));
    }


    LvmLv::Impl::Impl(const xmlNode* node)
	: BlkDevice::Impl(node), lv_type(LvType::NORMAL)
    {
	string tmp;

	if (get_dm_table_name().empty())
	    ST_THROW(Exception("no dm-table-name"));

	if (!getChildValue(node, "lv-name", lv_name))
	    ST_THROW(Exception("no lv-name"));

	if (getChildValue(node, "lv-type", tmp))
	    lv_type = toValueWithFallback(tmp, LvType::NORMAL);

	getChildValue(node, "uuid", uuid);

	getChildValue(node, "used-extents", used_extents);

	getChildValue(node, "stripes", stripes);
	getChildValue(node, "stripe-size", stripe_size);

	getChildValue(node, "chunk-size", chunk_size);
    }


    string
    LvmLv::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("LVM Logical Volume").translated;
    }


    void
    LvmLv::Impl::save(xmlNode* node) const
    {
	BlkDevice::Impl::save(node);

	setChildValue(node, "lv-name", lv_name);

	setChildValue(node, "lv-type", toString(lv_type));

	setChildValue(node, "uuid", uuid);

	setChildValueIf(node, "used-extents", used_extents, used_extents != 0);

	setChildValueIf(node, "stripes", stripes, stripes != 0);
	setChildValueIf(node, "stripe-size", stripe_size, stripe_size != 0);

	setChildValueIf(node, "chunk-size", chunk_size, chunk_size != 0);
    }


    bool
    LvmLv::Impl::is_usable_as_blk_device() const
    {
	switch (lv_type)
	{
	    case LvType::NORMAL:
	    case LvType::THIN:
	    case LvType::RAID:
	    case LvType::CACHE:
	    case LvType::WRITECACHE:
	    case LvType::SNAPSHOT:
	    case LvType::MIRROR:
		return true;

	    case LvType::UNKNOWN:
	    case LvType::THIN_POOL:
	    case LvType::CACHE_POOL:
		return false;
	}

	return false;
    }


    void
    LvmLv::Impl::check(const CheckCallbacks* check_callbacks) const
    {
	BlkDevice::Impl::check(check_callbacks);

	if (get_lv_name().empty())
	    ST_THROW(Exception("LvmLv has no lv-name"));

	if (get_region().get_start() != 0)
	    ST_THROW(Exception("LvmLv region start not zero"));

	if (check_callbacks)
	{
	    const LvmVg* lvm_vg = get_lvm_vg();

	    if (stripes > 1)
	    {
		if (!is_multiple_of(get_used_extents(), stripes))
		    check_callbacks->error(sformat("Number of extents not a multiple of stripes "
						   "of logical volume %s in volume group %s.",
						   lv_name, lvm_vg->get_vg_name()));
	    }

	    if (stripe_size > 0)
	    {
		if (stripe_size > lvm_vg->get_extent_size())
		    check_callbacks->error(sformat("Stripe size is greater then the extent size "
						   "of logical volume %s in volume group %s.",
						   lv_name, lvm_vg->get_vg_name()));
	    }

	    // the constant 265289728 is calculated from the LVM sources
	    if (lv_type == LvType::THIN_POOL && chunk_size > 0 && get_size() > chunk_size * 265289728)
		check_callbacks->error(sformat("Chunk size is too small for thin pool logical "
					       "volume %s in volume group %s.", lv_name,
					       lvm_vg->get_vg_name()));
	}
    }


    bool
    LvmLv::Impl::activate_lvm_lvs(const ActivateCallbacks* activate_callbacks)
    {
	y2mil("activate_lvm_lvs");

	try
	{
	    size_t number_of_inactive = CmdLvs().number_of_inactive();

	    if (number_of_inactive == 0)
		return false;

	    // TRANSLATORS: progress message
	    message_callback(activate_callbacks, _("Activating LVM"));

	    try
	    {
		SystemCmd cmd({ VGCHANGE_BIN, "--verbose", "--activate", "y" }, SystemCmd::DoThrow);
	    }
	    catch (const Exception& exception)
	    {
		ST_CAUGHT(exception);

		// TRANSLATORS: error message
		error_callback(activate_callbacks, _("Activating LVM failed"), exception);
	    }

	    bool ret = number_of_inactive != CmdLvs().number_of_inactive();

	    if (ret)
		udev_settle();

	    return ret;
	}
	catch (const Exception& exception)
	{
	    ST_CAUGHT(exception);

	    if (typeid(exception) == typeid(Aborted))
		ST_RETHROW(exception);

	    // Ignore failure to detect whether LVM needs to be activated.

	    return false;
	}
    }


    bool
    LvmLv::Impl::deactivate_lvm_lvs()
    {
	y2mil("deactivate_lvm_lvs");

	SystemCmd::Args cmd_args = { VGCHANGE_BIN, "--verbose", "--activate", "n" };

	SystemCmd cmd(cmd_args);

	return cmd.retcode() == 0;
    }


    void
    LvmLv::Impl::probe_lvm_lvs(Prober& prober)
    {
	SystemInfo::Impl& system_info = prober.get_system_info();

	vector<CmdLvs::Lv> lvs = system_info.getCmdLvs().get_lvs();

	// ensure thin-pools are probed before thins

	stable_partition(lvs.begin(), lvs.end(), [](const CmdLvs::Lv& lv) {
	    return lv.lv_type != LvType::THIN;
	});

	Devicegraph* system = prober.get_system();

	vector<string> unsupported_lvs;

	for (const CmdLvs::Lv& lv : lvs)
	{
	    LvmLv* lvm_lv = nullptr;

	    switch (lv.lv_type)
	    {
		case LvType::NORMAL:
		case LvType::RAID:
		case LvType::CACHE:
		case LvType::WRITECACHE:
		case LvType::SNAPSHOT:
		case LvType::MIRROR:
		{
		    // A cache is private if uses as a thin-pool.

		    if (lv.role == CmdLvs::Role::PUBLIC)
		    {
			// The size of the block device for the logical volume.
			unsigned long long size = lv.origin_size == 0 ? lv.size : lv.origin_size;

			LvmVg* lvm_vg = LvmVg::Impl::find_by_uuid(system, lv.vg_uuid);
			lvm_lv = lvm_vg->create_lvm_lv(lv.lv_name, lv.lv_type, size);

			lvm_lv->get_impl().set_used_extents(lv.size / lvm_vg->get_extent_size());
		    }
		}
		break;

		case LvType::THIN_POOL:
		{
		    // A thin-pool is always private but needed in the devicegraph anyway.

		    LvmVg* lvm_vg = LvmVg::Impl::find_by_uuid(system, lv.vg_uuid);
		    lvm_lv = lvm_vg->create_lvm_lv(lv.lv_name, LvType::THIN_POOL, lv.size);
		}
		break;

		case LvType::THIN:
		{
		    // A thin has the thin-pool as parent.

		    LvmLv* thin_pool = LvmLv::Impl::find_by_uuid(system, lv.pool_uuid);
		    lvm_lv = thin_pool->create_lvm_lv(lv.lv_name, LvType::THIN, lv.size);
		}
		break;

		case LvType::CACHE_POOL:
		{
		    // A cache-pool is only included in the devicegraph if unused.

		    if (!contains_if(lvs, [&lv](const CmdLvs::Lv& tmp) { return tmp.pool_uuid == lv.lv_uuid; }))
		    {
			LvmVg* lvm_vg = LvmVg::Impl::find_by_uuid(system, lv.vg_uuid);
			lvm_lv = lvm_vg->create_lvm_lv(lv.lv_name, LvType::CACHE_POOL, lv.size);
		    }
		}
		break;

		case LvType::UNKNOWN:
		{
		}
		break;
	    }

	    if (!lvm_lv)
	    {
		// private lvm lvs (e.g. metadata) are ignored, for
		// public ones the user is informed

		y2war("ignoring lvm_lv " << lv.vg_name << " " << lv.lv_name);

		if (lv.role == CmdLvs::Role::PUBLIC)
		    unsupported_lvs.push_back(DEV_DIR "/" + lv.vg_name + "/" + lv.lv_name);

		continue;
	    }

	    ST_CHECK_PTR(lvm_lv);

	    lvm_lv->get_impl().set_uuid(lv.lv_uuid);
	    lvm_lv->get_impl().set_active(lv.active && lv.lv_type != LvType::THIN_POOL);
	    lvm_lv->get_impl().probe_pass_1a(prober);
	}

	for (const CmdLvs::Lv& lv : lvs)
	{
	    if (lv.origin_uuid.empty())
		continue;

	    if (lv.lv_type == LvType::SNAPSHOT || lv.lv_type == LvType::THIN)
	    {
		LvmLv* a = LvmLv::Impl::find_by_uuid(system, lv.origin_uuid);
		LvmLv* b = LvmLv::Impl::find_by_uuid(system, lv.lv_uuid);

		Snapshot::create(system, a, b);
	    }
	}

	if (!unsupported_lvs.empty())
	{
	    sort(unsupported_lvs.begin(), unsupported_lvs.end());

	    // TRANSLATORS: Error message displayed during probing,
	    // %1$s is replaced by a list of device names joined by newlines (e.g.
	    // /dev/test/cached\n/dev/test/snapshot)
	    Text text = sformat(_("Detected LVM logical volumes of unsupported types:\n\n%1$s\n\n"
				  "These logical volumes are ignored. Operations on the\n"
				  "corresponding volume groups may fail."),
				join(unsupported_lvs, JoinMode::NEWLINE, 10));

	    error_callback(prober.get_probe_callbacks(), text);
	}
    }


    void
    LvmLv::Impl::probe_pass_1a(Prober& prober)
    {
	BlkDevice::Impl::probe_pass_1a(prober);

	const LvmVg* lvm_vg = get_lvm_vg();

	set_dm_table_name(make_dm_table_name(lvm_vg->get_vg_name(), lv_name));

	// Use the stripes, stripe_size and chunk_size from the first segment.

	SystemInfo::Impl& system_info = prober.get_system_info();

	if (lv_type == LvType::THIN_POOL)
	{
	    const CmdLvs::Lv& lv = system_info.getCmdLvs().find_by_lv_uuid(uuid);
	    chunk_size = lv.segments[0].chunk_size;

	    const CmdLvs::Lv& data_lv = system_info.getCmdLvs().find_by_lv_uuid(lv.data_uuid);
	    stripes = data_lv.segments[0].stripes;
	    stripe_size = data_lv.segments[0].stripe_size;
	}
	else if (lv_type == LvType::CACHE_POOL || lv_type == LvType::CACHE)
	{
	    const CmdLvs::Lv& lv = system_info.getCmdLvs().find_by_lv_uuid(uuid);
	    chunk_size = lv.segments[0].chunk_size;
	}
	else
	{
	    const CmdLvs::Lv& lv = system_info.getCmdLvs().find_by_lv_uuid(uuid);
	    stripes = lv.segments[0].stripes;
	    stripe_size = lv.segments[0].stripe_size;
	}
    }


    void
    LvmLv::Impl::set_region(const Region& region)
    {
	BlkDevice::Impl::set_region(region);

	if (lv_type != LvType::THIN)
	    used_extents = region.get_length();

	// resizing thick snapshots is not supported
    }


    bool
    LvmLv::Impl::supports_stripes() const
    {
	// AFAIS cache-pool and cache always have stripes = 1. The
	// lvconvert command used to create them does not accept the
	// --stripes options. The underlying LVs can have stripes.

	return lv_type == LvType::NORMAL || lv_type == LvType::THIN_POOL;
    }


    void
    LvmLv::Impl::set_stripes(unsigned int stripes)
    {
	if (stripes > 128)
	    ST_THROW(Exception("stripes above 128"));

	Impl::stripes = stripes;
    }


    void
    LvmLv::Impl::set_stripe_size(unsigned long long stripe_size)
    {
	if (stripe_size > 0)
	{
	    if (stripe_size < 4 * KiB)
		ST_THROW(InvalidStripeSize("stripe size below 4 KiB"));

	    if (!is_power_of_two(stripe_size))
		ST_THROW(InvalidStripeSize("stripe size not a power of two"));
	}

	Impl::stripe_size = stripe_size;
    }


    bool
    LvmLv::Impl::supports_chunk_size() const
    {
	return lv_type == LvType::THIN_POOL || lv_type == LvType::CACHE_POOL ||
	    lv_type == LvType::CACHE;
    }


    void
    LvmLv::Impl::set_chunk_size(unsigned long long chunk_size)
    {
	if (chunk_size > 0)
	{
	    if (chunk_size < 64 * KiB)
		ST_THROW(InvalidChunkSize("chunk size below 64 KiB"));

	    if (chunk_size > 1 * GiB)
		ST_THROW(InvalidChunkSize("chunk size above 1 GiB"));

	    if (!is_multiple_of(chunk_size, 64 * KiB))
		ST_THROW(InvalidChunkSize("chunk size not multiple of 64 KiB"));
	}

	Impl::chunk_size = chunk_size;
    }


    unsigned long long
    LvmLv::Impl::default_chunk_size(unsigned long long size)
    {
	// Calculation researched, limits can be found in the LVM documentation.

	unsigned long long tmp = next_power_of_two(size >> 21);

	return clamp(tmp, 64 * KiB, 1 * GiB);
    }


    unsigned long long
    LvmLv::Impl::default_chunk_size() const
    {
	return default_chunk_size(get_size());
    }


    unsigned long long
    LvmLv::Impl::default_metadata_size(unsigned long long size, unsigned long long chunk_size,
				       unsigned long long extent_size)
    {
	// Calculation and limits can be found in the LVM documentation.

	unsigned long long tmp = round_up(size / chunk_size * 64 * B, extent_size);

	return clamp(tmp, 2 * MiB, 16 * GiB);
    }


    unsigned long long
    LvmLv::Impl::default_metadata_size() const
    {
	unsigned long long tmp = chunk_size > 0 ? chunk_size : default_chunk_size();

	return default_metadata_size(get_size(), tmp, get_region().get_block_size(ULL_HACK));
    }


    void
    LvmLv::Impl::set_lv_name(const string& lv_name)
    {
	Impl::lv_name = lv_name;

	const LvmVg* lvm_vg = get_lvm_vg();
	set_name(make_name(lvm_vg->get_vg_name(), lv_name));
	set_dm_table_name(make_dm_table_name(lvm_vg->get_vg_name(), lv_name));

	// TODO clear or update udev-ids; update looks difficult/impossible.
    }


    bool
    LvmLv::Impl::is_valid_lv_name(const string& lv_name)
    {
	static const regex rx("[a-zA-Z0-9+_.][a-zA-Z0-9+_.-]*", regex::extended);

	if (!regex_match(lv_name, rx))
	    return false;

	if (lv_name == "." || lv_name == ".." || lv_name == "snapshot" || lv_name == "pvmove")
	    return false;

	if (lv_name.size() > 127)
	    return false;

	static const vector<string> illegal_subnames = { "_cdata", "_cmeta", "_corig", "_mlog",
	    "_mimage", "_pmspare", "_rimage", "_rmeta", "_tdata", "_tmeta", "_vorigin", "_vdata" };

	for (const string& illegal_subname : illegal_subnames)
	    if (boost::contains(lv_name, illegal_subname))
		return false;

	return true;
    }


    const LvmVg*
    LvmLv::Impl::get_lvm_vg() const
    {
	if (lv_type == LvType::THIN)
	    return get_thin_pool()->get_lvm_vg();

	return get_single_parent_of_type<const LvmVg>();
    }


    const LvmLv*
    LvmLv::Impl::get_thin_pool() const
    {
	if (lv_type != LvType::THIN)
	    ST_THROW(Exception("not a thin logical volume"));

	return get_single_parent_of_type<const LvmLv>();
    }


    bool
    LvmLv::Impl::has_snapshots() const
    {
	return !get_out_holders_of_type<const Snapshot>(View::ALL).empty();
    }


    vector<LvmLv*>
    LvmLv::Impl::get_snapshots()
    {
	vector<LvmLv*> ret;

	for (Snapshot* snapshot : get_out_holders_of_type<Snapshot>(View::ALL))
	    ret.push_back(to_lvm_lv(snapshot->get_target()));

	return ret;
    }


    vector<const LvmLv*>
    LvmLv::Impl::get_snapshots() const
    {
	vector<const LvmLv*> ret;

	for (const Snapshot* snapshot : get_out_holders_of_type<const Snapshot>(View::ALL))
	    ret.push_back(to_lvm_lv(snapshot->get_target()));

	return ret;
    }


    bool
    LvmLv::Impl::has_origin() const
    {
	return !get_in_holders_of_type<const Snapshot>(View::ALL).empty();
    }


    LvmLv*
    LvmLv::Impl::get_origin()
    {
	vector<Snapshot*> snapshots = get_in_holders_of_type<Snapshot>(View::ALL);

	if (snapshots.size() != 1)
	    ST_THROW(WrongNumberOfParents(snapshots.size(), 1));

	return to_lvm_lv(snapshots.front()->get_source());
    }


    const LvmLv*
    LvmLv::Impl::get_origin() const
    {
	vector<const Snapshot*> snapshots = get_in_holders_of_type<const Snapshot>(View::ALL);

	if (snapshots.size() != 1)
	    ST_THROW(WrongNumberOfParents(snapshots.size(), 1));

	return to_lvm_lv(snapshots.front()->get_source());
    }


    unsigned long long
    LvmLv::Impl::max_size_for_lvm_lv(LvType lv_type, const vector<sid_t>& ignore_sids) const
    {
	switch (lv_type)
	{
	    case LvType::THIN:
	    {
		return LvmVg::Impl::max_extent_number * get_lvm_vg()->get_extent_size();
	    }

	    default:
	    {
		return 0;
	    }
	}
    }


    LvmLv*
    LvmLv::Impl::create_lvm_lv(const string& lv_name, LvType lv_type, unsigned long long size)
    {
	Devicegraph* devicegraph = get_devicegraph();

	const LvmVg* lvm_vg = get_lvm_vg();

	LvmLv* lvm_lv = LvmLv::create(devicegraph, lvm_vg->get_vg_name(), lv_name, lv_type);
	Subdevice::create(devicegraph, get_non_impl(), lvm_lv);

	unsigned long long extent_size = lvm_vg->get_extent_size();
	lvm_lv->set_region(Region(0, size / extent_size, extent_size, ULL_HACK));

	return lvm_lv;
    }


    LvmLv*
    LvmLv::Impl::get_lvm_lv(const string& lv_name)
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
    LvmLv::Impl::get_lvm_lvs()
    {
	Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	return devicegraph.filter_devices_of_type<LvmLv>(devicegraph.children(vertex));
    }


    vector<const LvmLv*>
    LvmLv::Impl::get_lvm_lvs() const
    {
	const Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	return devicegraph.filter_devices_of_type<LvmLv>(devicegraph.children(vertex));
    }


    LvmLv*
    LvmLv::Impl::find_by_uuid(Devicegraph* devicegraph, const string& uuid)
    {
	return storage::find_by_uuid<LvmLv>(devicegraph, uuid);
    }


    const LvmLv*
    LvmLv::Impl::find_by_uuid(const Devicegraph* devicegraph, const string& uuid)
    {
	return storage::find_by_uuid<const LvmLv>(devicegraph, uuid);
    }


    bool
    LvmLv::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!BlkDevice::Impl::equal(rhs))
	    return false;

	return lv_name == rhs.lv_name && lv_type == rhs.lv_type && uuid == rhs.uuid &&
	    stripes == rhs.stripes && stripe_size == rhs.stripe_size &&
	    chunk_size == rhs.chunk_size && used_extents == rhs.used_extents;
    }


    void
    LvmLv::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	BlkDevice::Impl::log_diff(log, rhs);

	storage::log_diff(log, "lv-name", lv_name, rhs.lv_name);

	storage::log_diff_enum(log, "lv-type", lv_type, rhs.lv_type);

	storage::log_diff(log, "uuid", uuid, rhs.uuid);

	storage::log_diff(log, "used-extents", used_extents, rhs.used_extents);

	storage::log_diff(log, "stripes", stripes, rhs.stripes);
	storage::log_diff(log, "stripe-size", stripe_size, rhs.stripe_size);

	storage::log_diff(log, "chunk-size", chunk_size, rhs.chunk_size);
    }


    void
    LvmLv::Impl::print(std::ostream& out) const
    {
	BlkDevice::Impl::print(out);

	out << " lv-name:" << lv_name << " lv-type:" << toString(lv_type) << " uuid:" << uuid;

	if (used_extents != 0)
	    out << " used-extents:" << used_extents;

	if (stripes != 0)
	    out << " stripes:" << stripes;
	if (stripe_size != 0)
	    out << " stripe-size:" << stripe_size;

	if (chunk_size != 0)
	    out << " chunk-size:" << chunk_size;
    }


    ResizeInfo
    LvmLv::Impl::detect_resize_info(const BlkDevice* blk_device) const
    {
	const LvmVg* lvm_vg = get_lvm_vg();

	unsigned long long extent_size = lvm_vg->get_extent_size();

	// A logical volume must have at least one extent.

	switch (lv_type)
	{
	    case LvType::NORMAL:
	    {
		if (has_snapshots())
		    return ResizeInfo(false, RB_RESIZE_NOT_SUPPORTED_DUE_TO_SNAPSHOTS);

		ResizeInfo resize_info = BlkDevice::Impl::detect_resize_info(get_non_impl());

		unsigned long long free_extents = lvm_vg->get_impl().number_of_free_extents({ get_sid() });

		unsigned long long data_size = free_extents * extent_size;

		resize_info.combine(ResizeInfo(true, 0, extent_size, data_size));

		if (get_region().get_length() <= 1)
		    resize_info.reasons |= RB_MIN_SIZE_FOR_LVM_LV;

		if (get_region().get_length() >= free_extents)
		    resize_info.reasons |= RB_NO_SPACE_IN_LVM_VG;

		resize_info.combine_block_size(max(1U, stripes) * extent_size);

		return resize_info;
	    }

	    case LvType::THIN_POOL:
	    {
		if (exists_in_system())
		{
		    // Shrinking thin pools is not supported by LVM. Since the
		    // metadata is already on disk and does not get resized no
		    // need to handle them here.

		    // TODO The underlying LV can be of type RAID or mirror. In that case
		    // resizing can need more space.

		    const LvmLv* tmp_lvm_lv = to_lvm_lv(redirect_to_system(get_non_impl()));

		    unsigned long long data_size = (lvm_vg->get_impl().number_of_free_extents() +
						    get_used_extents()) * extent_size;

		    ResizeInfo resize_info(true, RB_SHRINK_NOT_SUPPORTED_FOR_LVM_LV_TYPE,
					   tmp_lvm_lv->get_size(), data_size);

		    if (get_region().get_length() * extent_size >= data_size)
			resize_info.reasons |= RB_NO_SPACE_IN_LVM_VG;

		    resize_info.combine_block_size(max(1U, stripes) * extent_size);

		    return resize_info;
		}
		else
		{
		    unsigned long long data_size = lvm_vg->get_impl().max_size_for_lvm_lv(lv_type, { get_sid() });

		    ResizeInfo resize_info(true, 0, extent_size, data_size);

		    if (get_region().get_length() <= 1)
			resize_info.reasons |= RB_MIN_SIZE_FOR_LVM_LV;

		    if (get_region().get_length() * extent_size >= data_size)
			resize_info.reasons |= RB_NO_SPACE_IN_LVM_VG;

		    resize_info.combine_block_size(max(1U, stripes) * extent_size);

		    return resize_info;
		}
	    }

	    case LvType::THIN:
	    {
		ResizeInfo resize_info = BlkDevice::Impl::detect_resize_info(get_non_impl());

		const LvmLv* thin_pool = get_thin_pool();

		unsigned long long data_size = thin_pool->max_size_for_lvm_lv(LvType::THIN);

		resize_info.combine(ResizeInfo(true, 0, extent_size, data_size));

		if (get_region().get_length() <= 1)
		    resize_info.reasons |= RB_MIN_SIZE_FOR_LVM_LV;

		if (get_region().get_length() * extent_size >= data_size)
		    resize_info.reasons |= RB_MAX_SIZE_FOR_LVM_LV_THIN;

		resize_info.combine_block_size(extent_size);

		return resize_info;
	    }

	    default:
	    {
		return ResizeInfo(false, RB_RESIZE_NOT_SUPPORTED_FOR_LVM_LV_TYPE);
	    }
	}
    }


    void
    LvmLv::Impl::add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs_base) const
    {
	BlkDevice::Impl::add_modify_actions(actiongraph, lhs_base);

	const Impl& lhs = dynamic_cast<const Impl&>(lhs_base->get_impl());

	if (get_lv_name() != lhs.get_lv_name())
	{
	    shared_ptr<Action::Base> action = make_shared<Action::Rename>(get_sid());
	    actiongraph.add_vertex(action);
	    action->first = action->last = true;
	}

	if (get_lv_type() != lhs.get_lv_type())
	{
	    ST_THROW(Exception("changing lv type not supported"));
	}

	if (get_stripes() != lhs.get_stripes())
	{
	    ST_THROW(Exception("changing stripes not supported"));
	}

	if (get_stripe_size() != lhs.get_stripe_size())
	{
	    ST_THROW(Exception("changing stripe size not supported"));
	}

	if (get_chunk_size() != lhs.get_chunk_size())
	{
	    ST_THROW(Exception("changing chunk size not supported"));
	}
    }


    Text
    LvmLv::Impl::do_create_text(Tense tense) const
    {
	const LvmVg* lvm_vg = get_lvm_vg();

	Text text;

	switch (lv_type)
	{
	    case LvType::THIN_POOL:
		text = tenser(tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by logical volume name (e.g. root),
			      // %2$s is replaced by size (e.g. 2.00 GiB),
			      // %3$s is replaced by volume group name (e.g. system)
			      _("Create thin pool logical volume %1$s (%2$s) on volume group %3$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by logical volume name (e.g. root),
			      // %2$s is replaced by size (e.g. 2.00 GiB),
			      // %3$s is replaced by volume group name (e.g. system)
			      _("Creating thin pool logical volume %1$s (%2$s) on volume group %3$s"));
		break;

	    case LvType::THIN:
		text = tenser(tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by logical volume name (e.g. root),
			      // %2$s is replaced by size (e.g. 2.00 GiB),
			      // %3$s is replaced by volume group name (e.g. system)
			      _("Create thin logical volume %1$s (%2$s) on volume group %3$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by logical volume name (e.g. root),
			      // %2$s is replaced by size (e.g. 2.00 GiB),
			      // %3$s is replaced by volume group name (e.g. system)
			      _("Creating thin logical volume %1$s (%2$s) on volume group %3$s"));
		break;

	    default:
		text = tenser(tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by logical volume name (e.g. root),
			      // %2$s is replaced by size (e.g. 2.00 GiB),
			      // %3$s is replaced by volume group name (e.g. system)
			      _("Create logical volume %1$s (%2$s) on volume group %3$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by logical volume name (e.g. root),
			      // %2$s is replaced by size (e.g. 2.00 GiB),
			      // %3$s is replaced by volume group name (e.g. system)
			      _("Creating logical volume %1$s (%2$s) on volume group %3$s"));
		break;
	}

	return sformat(text, lv_name, get_size_text(), lvm_vg->get_vg_name());
    }


    void
    LvmLv::Impl::do_create()
    {
	const LvmVg* lvm_vg = get_lvm_vg();
	const Region& region = get_region();

	SystemCmd::Args cmd_args = { LVCREATE_BIN, "--verbose" };

	switch (lv_type)
	{
	    case LvType::NORMAL:
	    {
		cmd_args << "--zero=y" << "--wipesignatures=y" << "--yes" << "--extents" << to_string(region.get_length());
	    }
	    break;

	    case LvType::THIN_POOL:
	    {
		cmd_args << "--type" << "thin-pool" << "--zero=y" << "--yes" << "--extents" << to_string(region.get_length());
	    }
	    break;

	    case LvType::THIN:
	    {
		const LvmLv* thin_pool = get_thin_pool();

		cmd_args << "--type" << "thin" << "--wipesignatures=y" << "--yes" << "--virtualsize" <<
		    to_string(region.to_bytes(region.get_length())) + "B" << "--thin-pool" << thin_pool->get_lv_name();
	    }
	    break;

	    case LvType::UNKNOWN:
	    case LvType::RAID:
	    case LvType::CACHE_POOL:
	    case LvType::CACHE:
	    case LvType::WRITECACHE:
	    case LvType::SNAPSHOT:
	    case LvType::MIRROR:
	    {
		ST_THROW(UnsupportedException(sformat("creating LvmLv with type %s is unsupported",
						      toString(lv_type))));
	    }
	    break;
	}

	if (supports_stripes() && stripes > 1)
	{
	    cmd_args << "--stripes" << to_string(stripes);
	    if (stripe_size > 0)
		cmd_args << "--stripesize" << to_string(stripe_size / KiB);
	}

	if (supports_chunk_size() && chunk_size > 0)
	{
	    cmd_args << "--chunksize" << to_string(chunk_size / KiB);
	}

	cmd_args << "--name" << lv_name << "--" << lvm_vg->get_vg_name();

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);
    }


    void
    LvmLv::Impl::do_create_post_verify() const
    {
	// log some data about the logical volume that might be useful for debugging

	const LvmVg* lvm_vg = get_lvm_vg();

	CmdLvs cmd_lvs(get_lvm_vg()->get_vg_name(), lv_name);
	const CmdLvs::Lv& lv = cmd_lvs.get_lvs()[0];
	log_unexpected("lvm-lv extents", lvm_vg->get_extent_size(), lv.size);
    }


    Text
    LvmLv::Impl::do_rename_text(const CommitData& commit_data, const Action::Rename* action) const
    {
	const LvmVg* lvm_vg = get_lvm_vg();

	const LvmLv* lvm_lv_lhs = to_lvm_lv(action->get_device(commit_data.actiongraph, LHS));
	const LvmLv* lvm_lv_rhs = to_lvm_lv(action->get_device(commit_data.actiongraph, RHS));

	// TRANSLATORS:
	// %1$s is replaced with the old logical volume name (e.g. foo),
	// %2$s is replaced with the new logical volume name (e.g. bar),
	// %3$s is replaced with the volume group name (e.g. system)
	Text text = _("Rename logical volume %1$s to %2$s on volume group %3$s");

	return sformat(text, lvm_lv_lhs->get_displayname(), lvm_lv_rhs->get_displayname(), lvm_vg->get_vg_name());
    }


    void
    LvmLv::Impl::do_rename(const CommitData& commit_data, const Action::Rename* action) const
    {
	const LvmVg* lvm_vg = get_lvm_vg();

	const LvmLv* lvm_lv_lhs = to_lvm_lv(action->get_device(commit_data.actiongraph, LHS));
	const LvmLv* lvm_lv_rhs = to_lvm_lv(action->get_device(commit_data.actiongraph, RHS));

	SystemCmd::Args cmd_args = { LVRENAME_BIN, "--verbose", lvm_vg->get_vg_name(), lvm_lv_lhs->get_lv_name(),
	    lvm_lv_rhs->get_lv_name() };

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);
    }


    Text
    LvmLv::Impl::do_resize_text(const CommitData& commit_data, const Action::Resize* action) const
    {
	const LvmVg* lvm_vg = get_lvm_vg();

	const LvmLv* lvm_lv_lhs = to_lvm_lv(action->get_device(commit_data.actiongraph, LHS));
	const LvmLv* lvm_lv_rhs = to_lvm_lv(action->get_device(commit_data.actiongraph, RHS));

	Text text;

	switch (action->resize_mode)
	{
	    case ResizeMode::SHRINK:

		switch (lv_type)
		{
		    case LvType::THIN_POOL:
			text = tenser(commit_data.tense,
				      // TRANSLATORS: displayed before action,
				      // %1$s is replaced by logical volume name (e.g. root),
				      // %2$s is replaced by volume group name (e.g. system),
				      // %3$s is replaced by old size (e.g. 2.00 GiB),
				      // %4$s is replaced by new size (e.g. 1.00 GiB)
				      _("Shrink thin pool logical volume %1$s on volume group %2$s from %3$s to %4$s"),
				      // TRANSLATORS: displayed during action,
				      // %1$s is replaced by logical volume name (e.g. root),
				      // %2$s is replaced by volume group name (e.g. system),
				      // %3$s is replaced by old size (e.g. 2.00 GiB),
				      // %4$s is replaced by new size (e.g. 1.00 GiB)
				      _("Shrinking thin pool logical volume %1$s on volume group %2$s from %3$s to %4$s"));
			break;

		    case LvType::THIN:
			text = tenser(commit_data.tense,
				      // TRANSLATORS: displayed before action,
				      // %1$s is replaced by logical volume name (e.g. root),
				      // %2$s is replaced by volume group name (e.g. system),
				      // %3$s is replaced by old size (e.g. 2.00 GiB),
				      // %4$s is replaced by new size (e.g. 1.00 GiB)
				      _("Shrink thin logical volume %1$s on volume group %2$s from %3$s to %4$s"),
				      // TRANSLATORS: displayed during action,
				      // %1$s is replaced by logical volume name (e.g. root),
				      // %2$s is replaced by volume group name (e.g. system),
				      // %3$s is replaced by old size (e.g. 2.00 GiB),
				      // %4$s is replaced by new size (e.g. 1.00 GiB)
				      _("Shrinking thin logical volume %1$s on volume group %2$s from %3$s to %4$s"));
			break;

		    default:
			text = tenser(commit_data.tense,
				      // TRANSLATORS: displayed before action,
				      // %1$s is replaced by logical volume name (e.g. root),
				      // %2$s is replaced by volume group name (e.g. system),
				      // %3$s is replaced by old size (e.g. 2.00 GiB),
				      // %4$s is replaced by new size (e.g. 1.00 GiB)
				      _("Shrink logical volume %1$s on volume group %2$s from %3$s to %4$s"),
				      // TRANSLATORS: displayed during action,
				      // %1$s is replaced by logical volume name (e.g. root),
				      // %2$s is replaced by volume group name (e.g. system),
				      // %3$s is replaced by old size (e.g. 2.00 GiB),
				      // %4$s is replaced by new size (e.g. 1.00 GiB)
				      _("Shrinking logical volume %1$s on volume group %2$s from %3$s to %4$s"));
			break;
		}
		break;

	    case ResizeMode::GROW:

		switch (lv_type)
		{
		    case LvType::THIN_POOL:
			text = tenser(commit_data.tense,
				      // TRANSLATORS: displayed before action,
				      // %1$s is replaced by logical volume name (e.g. root),
				      // %2$s is replaced by volume group name (e.g. system),
				      // %3$s is replaced by old size (e.g. 1.00 GiB),
				      // %4$s is replaced by new size (e.g. 2.00 GiB)
				      _("Grow thin pool logical volume %1$s on volume group %2$s from %3$s to %4$s"),
				      // TRANSLATORS: displayed during action,
				      // %1$s is replaced by logical volume name (e.g. root),
				      // %2$s is replaced by volume group name (e.g. system),
				      // %3$s is replaced by old size (e.g. 1.00 GiB),
				      // %4$s is replaced by new size (e.g. 2.00 GiB)
				      _("Growing thin pool logical volume %1$s on volume group %2$s from %3$s to %4$s"));
			break;

		    case LvType::THIN:
			text = tenser(commit_data.tense,
				      // TRANSLATORS: displayed before action,
				      // %1$s is replaced by logical volume name (e.g. root),
				      // %2$s is replaced by volume group name (e.g. system),
				      // %3$s is replaced by old size (e.g. 1.00 GiB),
				      // %4$s is replaced by new size (e.g. 2.00 GiB)
				      _("Grow thin logical volume %1$s on volume group %2$s from %3$s to %4$s"),
				      // TRANSLATORS: displayed during action,
				      // %1$s is replaced by logical volume name (e.g. root),
				      // %2$s is replaced by volume group name (e.g. system),
				      // %3$s is replaced by old size (e.g. 1.00 GiB),
				      // %4$s is replaced by new size (e.g. 2.00 GiB)
				      _("Growing thin logical volume %1$s on volume group %2$s from %3$s to %4$s"));
			break;

		    default:
			text = tenser(commit_data.tense,
				      // TRANSLATORS: displayed before action,
				      // %1$s is replaced by logical volume name (e.g. root),
				      // %2$s is replaced by volume group name (e.g. system),
				      // %3$s is replaced by old size (e.g. 1.00 GiB),
				      // %4$s is replaced by new size (e.g. 2.00 GiB)
				      _("Grow logical volume %1$s on volume group %2$s from %3$s to %4$s"),
				      // TRANSLATORS: displayed during action,
				      // %1$s is replaced by logical volume name (e.g. root),
				      // %2$s is replaced by volume group name (e.g. system),
				      // %3$s is replaced by old size (e.g. 1.00 GiB),
				      // %4$s is replaced by new size (e.g. 2.00 GiB)
				      _("Growing logical volume %1$s on volume group %2$s from %3$s to %4$s"));
			break;
		}
		break;

	    default:
		ST_THROW(LogicException("invalid value for resize_mode"));
	}

	return sformat(text, lv_name, lvm_vg->get_vg_name(),
		       lvm_lv_lhs->get_impl().get_size_text(),
		       lvm_lv_rhs->get_impl().get_size_text());
    }


    void
    LvmLv::Impl::do_resize(const CommitData& commit_data, const Action::Resize* action) const
    {
	const LvmVg* lvm_vg = get_lvm_vg();

	const LvmLv* lvm_lv_rhs = to_lvm_lv(action->get_device(commit_data.actiongraph, RHS));

	SystemCmd::Args cmd_args = { LVRESIZE_BIN, "--verbose" };

	if (action->resize_mode == ResizeMode::SHRINK)
	    cmd_args << "--force";

	cmd_args << "--extents" << to_string(lvm_lv_rhs->get_region().get_length()) << "--"
		 << lvm_vg->get_vg_name() + "/" + lv_name;

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);
    }


    Text
    LvmLv::Impl::do_delete_text(Tense tense) const
    {
	const LvmVg* lvm_vg = get_lvm_vg();

	Text text;

	switch (lv_type)
	{
	    case LvType::THIN_POOL:
		text = tenser(tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by logical volume name (e.g. root),
			      // %2$s is replaced by size (e.g. 2.00 GiB),
			      // %3$s is replaced by volume group name (e.g. system)
			      _("Delete thin pool logical volume %1$s (%2$s) on volume group %3$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by logical volume name (e.g. root),
			      // %2$s is replaced by size (e.g. 2.00 GiB),
			      // %3$s is replaced by volume group name (e.g. system)
			      _("Deleting thin pool logical volume %1$s (%2$s) on volume group %3$s"));
		break;

	    case LvType::THIN:
		text = tenser(tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by logical volume name (e.g. root),
			      // %2$s is replaced by size (e.g. 2.00 GiB),
			      // %3$s is replaced by volume group name (e.g. system)
			      _("Delete thin logical volume %1$s (%2$s) on volume group %3$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by logical volume name (e.g. root),
			      // %2$s is replaced by size (e.g. 2.00 GiB),
			      // %3$s is replaced by volume group name (e.g. system)
			      _("Deleting thin logical volume %1$s (%2$s) on volume group %3$s"));
		break;

	    default:
		text = tenser(tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by logical volume name (e.g. root),
			      // %2$s is replaced by size (e.g. 2.00 GiB),
			      // %3$s is replaced by volume group name (e.g. system)
			      _("Delete logical volume %1$s (%2$s) on volume group %3$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by logical volume name (e.g. root),
			      // %2$s is replaced by size (e.g. 2.00 GiB),
			      // %3$s is replaced by volume group name (e.g. system)
			      _("Deleting logical volume %1$s (%2$s) on volume group %3$s"));
		break;
	}

	return sformat(text, lv_name, get_size_text(), lvm_vg->get_vg_name());
    }


    void
    LvmLv::Impl::do_delete() const
    {
	const LvmVg* lvm_vg = get_lvm_vg();

	SystemCmd::Args cmd_args = { LVREMOVE_BIN, "--verbose", "--force", "--", lvm_vg->get_vg_name() +
	    "/" + lv_name };

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);
    }


    Text
    LvmLv::Impl::do_activate_text(Tense tense) const
    {
	const LvmVg* lvm_vg = get_lvm_vg();

	Text text;

	switch (lv_type)
	{
	    case LvType::THIN_POOL:
		text = tenser(tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by logical volume name (e.g. root),
			      // %2$s is replaced by size (e.g. 2.00 GiB),
			      // %3$s is replaced by volume group name (e.g. system)
			      _("Activate thin pool logical volume %1$s (%2$s) on volume group %3$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by logical volume name (e.g. root),
			      // %2$s is replaced by size (e.g. 2.00 GiB),
			      // %3$s is replaced by volume group name (e.g. system)
			      _("Activating thin pool logical volume %1$s (%2$s) on volume group %3$s"));
		break;

	    case LvType::THIN:
		text = tenser(tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by logical volume name (e.g. root),
			      // %2$s is replaced by size (e.g. 2.00 GiB),
			      // %3$s is replaced by volume group name (e.g. system)
			      _("Activate thin logical volume %1$s (%2$s) on volume group %3$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by logical volume name (e.g. root),
			      // %2$s is replaced by size (e.g. 2.00 GiB),
			      // %3$s is replaced by volume group name (e.g. system)
			      _("Activating thin logical volume %1$s (%2$s) on volume group %3$s"));
		break;

	    default:
		text = tenser(tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by logical volume name (e.g. root),
			      // %2$s is replaced by size (e.g. 2.00 GiB),
			      // %3$s is replaced by volume group name (e.g. system)
			      _("Activate logical volume %1$s (%2$s) on volume group %3$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by logical volume name (e.g. root),
			      // %2$s is replaced by size (e.g. 2.00 GiB),
			      // %3$s is replaced by volume group name (e.g. system)
			      _("Activating logical volume %1$s (%2$s) on volume group %3$s"));
		break;
	}

	return sformat(text, lv_name, get_size_text(), lvm_vg->get_vg_name());
    }


    void
    LvmLv::Impl::do_activate()
    {
	const LvmVg* lvm_vg = get_lvm_vg();

	SystemCmd::Args cmd_args = { LVCHANGE_BIN, "--verbose", "--activate", "y", "--", lvm_vg->get_vg_name() +
	    "/" + lv_name };

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);
    }


    Text
    LvmLv::Impl::do_deactivate_text(Tense tense) const
    {
	const LvmVg* lvm_vg = get_lvm_vg();

	Text text;

	switch (lv_type)
	{
	    case LvType::THIN_POOL:
		text = tenser(tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by logical volume name (e.g. root),
			      // %2$s is replaced by size (e.g. 2.00 GiB),
			      // %3$s is replaced by volume group name (e.g. system)
			      _("Deactivate thin pool logical volume %1$s (%2$s) on volume group %3$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by logical volume name (e.g. root),
			      // %2$s is replaced by size (e.g. 2.00 GiB),
			      // %3$s is replaced by volume group name (e.g. system)
			      _("Deactivating thin pool logical volume %1$s (%2$s) on volume group %3$s"));
		break;

	    case LvType::THIN:
		text = tenser(tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by logical volume name (e.g. root),
			      // %2$s is replaced by size (e.g. 2.00 GiB),
			      // %3$s is replaced by volume group name (e.g. system)
			      _("Deactivate thin logical volume %1$s (%2$s) on volume group %3$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by logical volume name (e.g. root),
			      // %2$s is replaced by size (e.g. 2.00 GiB),
			      // %3$s is replaced by volume group name (e.g. system)
			      _("Deactivating thin logical volume %1$s (%2$s) on volume group %3$s"));
		break;

	    default:
		text = tenser(tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by logical volume name (e.g. root),
			      // %2$s is replaced by size (e.g. 2.00 GiB),
			      // %3$s is replaced by volume group name (e.g. system)
			      _("Deactivate logical volume %1$s (%2$s) on volume group %3$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by logical volume name (e.g. root),
			      // %2$s is replaced by size (e.g. 2.00 GiB),
			      // %3$s is replaced by volume group name (e.g. system)
			      _("Deactivating logical volume %1$s (%2$s) on volume group %3$s"));
		break;
	}

	return sformat(text, lv_name, get_size_text(), lvm_vg->get_vg_name());
    }


    void
    LvmLv::Impl::do_deactivate()
    {
	const LvmVg* lvm_vg = get_lvm_vg();

	SystemCmd::Args cmd_args = { LVCHANGE_BIN, "--verbose", "--activate", "n", "--", lvm_vg->get_vg_name() +
	    "/" + lv_name };

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);
    }


    string
    LvmLv::Impl::make_name(const string& vg_name, const string& lv_name)
    {
	return DEV_DIR "/" + vg_name + "/" + lv_name;
    }


    string
    LvmLv::Impl::make_dm_table_name(const string& vg_name, const string& lv_name)
    {
	return boost::replace_all_copy(vg_name, "-", "--") + "-" +
	    boost::replace_all_copy(lv_name, "-", "--");
    }

}
