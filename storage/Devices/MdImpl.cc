/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#include <cctype>
#include <boost/integer/common_factor_rt.hpp>
#include <boost/algorithm/string.hpp>

#include "storage/Devices/MdImpl.h"
#include "storage/Devices/MdContainerImpl.h"
#include "storage/Devices/MdMemberImpl.h"
#include "storage/Holders/MdUserImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Prober.h"
#include "storage/EnvironmentImpl.h"
#include "storage/SystemInfo/SystemInfoImpl.h"
#include "storage/Utils/AppUtil.h"
#include "storage/Utils/Exception.h"
#include "storage/Utils/Enum.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/HumanString.h"
#include "storage/Utils/Algorithm.h"
#include "storage/Utils/Math.h"
#include "storage/UsedFeatures.h"
#include "storage/EtcMdadm.h"
#include "storage/Utils/CallbacksImpl.h"
#include "storage/Utils/Format.h"
#include "storage/Actions/DeactivateImpl.h"
#include "storage/Actions/ReallotImpl.h"
#include "storage/Actions/CreateImpl.h"
#include "storage/Actions/DeleteImpl.h"
#include "storage/Actions/AddToEtcMdadmImpl.h"
#include "storage/Actions/RemoveFromEtcMdadmImpl.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Md>::classname = "Md";


    // strings must match /proc/mdstat
    const vector<string> EnumTraits<MdLevel>::names({
	"unknown", "RAID0", "RAID1", "RAID4", "RAID5", "RAID6", "RAID10", "CONTAINER", "LINEAR"
    });


    // strings must match "mdadm --parity" option
    const vector<string> EnumTraits<MdParity>::names({
	"default", "left-asymmetric", "left-symmetric", "right-asymmetric",
	"right-symmetric", "parity-first", "parity-last", "left-asymmetric-6",
	"left-symmetric-6", "right-asymmetric-6", "right-symmetric-6",
	"parity-first-6", "n2", "o2", "f2", "n3", "o3", "f3"
    });


    // Matches names of the form /dev/md<number> and /dev/md/<number>. The latter looks
    // like a named MD but since mdadm creates /dev/md<number> (without leading zeros) in
    // that case and not /dev/md<some big dynamic number> the number must be considered in
    // find_free_numeric_name().

    const regex Md::Impl::numeric_name_regex(DEV_DIR "/md/?([0-9]+)", regex::extended);


    // mdadm(8) states that any string for the names is allowed. That is
    // not correct: A '/' is reported as invalid by mdadm itself. A ' '
    // does not work, e.g. the links in /dev/md/ are broken.

    const regex Md::Impl::format1_name_regex(DEV_MD_DIR "/([^/ ]+)", regex::extended);


    // Adding "CREATE names=yes" to mdadm.conf gets named RAIDs using /dev/md_<name>.

    const regex Md::Impl::format2_name_regex(DEV_DIR "/md_([^/ ]+)", regex::extended);


    Md::Impl::Impl(const string& name)
	: Partitionable::Impl(name)
    {
	if (!is_valid_name(name))
	    ST_THROW(Exception("invalid Md name"));

	update_sysfs_name_and_path();
    }


    Md::Impl::Impl(const xmlNode* node)
	: Partitionable::Impl(node)
    {
	string tmp;

	if (getChildValue(node, "md-level", tmp))
	    md_level = toValueWithFallback(tmp, MdLevel::RAID0);

	if (getChildValue(node, "md-parity", tmp))
	    md_parity = toValueWithFallback(tmp, MdParity::DEFAULT);

	getChildValue(node, "chunk-size", chunk_size);

	getChildValue(node, "uuid", uuid);

	getChildValue(node, "metadata", metadata);

	getChildValue(node, "in-etc-mdadm", in_etc_mdadm);
    }


    string
    Md::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("MD RAID").translated;
    }


    string
    Md::Impl::get_name_sort_key() const
    {
	static const vector<NameSchema> name_schemata = {
	    NameSchema(regex(DEV_DIR "/md([0-9]+)", regex::extended), { { PadInfo::N1, 4 } })
	};

	return format_to_name_schemata(get_name(), name_schemata);
    }


    void
    Md::Impl::set_name(const string& name)
    {
	if (!is_valid_name(name))
	    ST_THROW(Exception("invalid Md name"));

	Partitionable::Impl::set_name(name);

	update_sysfs_name_and_path();
    }


    void
    Md::Impl::update_sysfs_name_and_path()
    {
	smatch match;

	if (regex_match(get_name(), match, numeric_name_regex) && match.size() == 2)
	{
	    string tmp = match[1];
	    tmp.erase(0, min(tmp.find_first_not_of('0'), tmp.size() - 1));
	    set_sysfs_name("md" + tmp);
	    set_sysfs_path("/devices/virtual/block/md" + tmp);
	}
	else
	{
	    set_sysfs_name("");
	    set_sysfs_path("");
	}
    }


    string
    Md::Impl::find_free_numeric_name(const Devicegraph* devicegraph)
    {
	vector<const Md*> mds = get_all_if(devicegraph, [](const Md* md) { return md->is_numeric(); });

	sort(mds.begin(), mds.end(), compare_by_number);

	// The non-numeric MDs also need numbers but those start at 127
	// counting backwards.

	unsigned int free_number = first_missing_number(mds, 0);

	return DEV_DIR "/md" + to_string(free_number);
    }


    unsigned long long
    Md::Impl::calculate_underlying_size(MdLevel md_level, unsigned int number_of_devices, unsigned long long size)
    {
	if (number_of_devices < minimal_number_of_devices(md_level))
	    ST_THROW(Exception("too few devices"));

	unsigned long long underlying_size = 0;

	switch (md_level)
	{
	    case MdLevel::LINEAR:
	    case MdLevel::RAID0:
		underlying_size = size / number_of_devices;
		break;

	    case MdLevel::RAID1:
		underlying_size = size;
		break;

	    case MdLevel::RAID4:
	    case MdLevel::RAID5:
		underlying_size = size / (number_of_devices - 1);
		break;

	    case MdLevel::RAID6:
		underlying_size = size / (number_of_devices - 2);
		break;

	    case MdLevel::RAID10:
		underlying_size = size * 2 / number_of_devices;
		break;

	    default:
		ST_THROW(Exception("illegal raid level"));
	}

	// see calculate_region_and_topology
	underlying_size += max(128 * KiB, underlying_size / 1024);
	underlying_size += get_default_chunk_size();
	underlying_size += 1 * MiB;
	underlying_size += 8 * KiB;
	underlying_size += min(128 * MiB, underlying_size / 64);
	underlying_size += 16 * KiB;

	return underlying_size;
    }


    void
    Md::Impl::check(const CheckCallbacks* check_callbacks) const
    {
	Partitionable::Impl::check(check_callbacks);

	if (!is_valid_name(get_name()))
	    ST_THROW(Exception("invalid name"));

	if (check_callbacks && chunk_size > 0)
	{
	    // See man page of mdadm and http://bugzilla.suse.com/show_bug.cgi?id=1065381
	    // for the constraints.

	    switch (md_level)
	    {
		case MdLevel::RAID0:
		{
		    if (chunk_size < 4 * KiB)
		    {
			check_callbacks->error(sformat("Chunk size of MD %s is smaller than 4 KiB.",
						       get_name()));
		    }

		    unsigned long long tmp = 1 * KiB;
		    for (const BlkDevice* blk_device : get_blk_devices())
		    {
			tmp = boost::integer::lcm(tmp, (unsigned long long)
						  blk_device->get_region().get_block_size());
		    }

		    if (!is_multiple_of(chunk_size, tmp))
		    {
			check_callbacks->error(sformat("Chunk size of MD %s is not a multiple of "
						       "the sector size of the devices.",
						       get_name()));
		    }
		}
		break;

		case MdLevel::RAID4:
		case MdLevel::RAID5:
		case MdLevel::RAID6:
		case MdLevel::RAID10:
		{
		    if (chunk_size < get_devicegraph()->get_storage()->get_arch().get_page_size())
		    {
			check_callbacks->error(sformat("Chunk size of MD %s is smaller than the "
						       "page size.", get_name()));
		    }

		    if (!is_power_of_two(chunk_size))
		    {
			check_callbacks->error(sformat("Chunk size of MD %s is not a power of two.",
						       get_name()));
		    }
		}
		break;

		default:
		    break;
	    }
	}
    }


    ResizeInfo
    Md::Impl::detect_resize_info(const BlkDevice* blk_device) const
    {
	return ResizeInfo(false, RB_RESIZE_NOT_SUPPORTED_BY_DEVICE);
    }


    void
    Md::Impl::set_md_level(MdLevel md_level)
    {
	if (Impl::md_level == md_level)
	    return;

	Impl::md_level = md_level;

	calculate_region_and_topology();
    }


    vector<MdParity>
    Md::Impl::get_allowed_md_parities() const
    {
	switch (md_level)
	{
	    case MdLevel::UNKNOWN:
		return { };

	    case MdLevel::LINEAR:
	    case MdLevel::RAID0:
	    case MdLevel::RAID1:
	    case MdLevel::RAID4:
		return { };

	    case MdLevel::RAID5:
		return { MdParity::DEFAULT, MdParity::LEFT_ASYMMETRIC, MdParity::LEFT_SYMMETRIC,
			 MdParity::RIGHT_ASYMMETRIC, MdParity::RIGHT_SYMMETRIC, MdParity::FIRST,
			 MdParity::LAST };

	    case MdLevel::RAID6:
		return { MdParity::DEFAULT, MdParity::LEFT_ASYMMETRIC, MdParity::LEFT_SYMMETRIC,
			 MdParity::RIGHT_ASYMMETRIC, MdParity::RIGHT_SYMMETRIC, MdParity::FIRST,
			 MdParity::LAST, MdParity::LEFT_ASYMMETRIC_6, MdParity::LEFT_SYMMETRIC_6,
			 MdParity::RIGHT_ASYMMETRIC_6, MdParity::RIGHT_SYMMETRIC_6, MdParity::FIRST_6 };

	    case MdLevel::RAID10:
		if (number_of_devices() <= 2)
		    return { MdParity::DEFAULT, MdParity::NEAR_2, MdParity::OFFSET_2, MdParity::FAR_2 };
		else
		    return { MdParity::DEFAULT, MdParity::NEAR_2, MdParity::OFFSET_2, MdParity::FAR_2,
			     MdParity::NEAR_3, MdParity::OFFSET_3, MdParity::FAR_3 };

	    case MdLevel::CONTAINER:
		return { };
	}

	return { };
    }


    bool
    Md::Impl::is_chunk_size_meaningful() const
    {
	return md_level == MdLevel::RAID0 || md_level == MdLevel::RAID4 || md_level == MdLevel::RAID5 ||
	    md_level == MdLevel::RAID6 || md_level == MdLevel::RAID10;
    }


    void
    Md::Impl::set_chunk_size(unsigned long chunk_size)
    {
	if (chunk_size > 0)
	{
	    // See check().

	    if (chunk_size < 4 * KiB)
		ST_THROW(InvalidChunkSize("chunk size below 4 KiB"));

	    if (!is_multiple_of(chunk_size, 512 * B))
		ST_THROW(InvalidChunkSize("chunk size not multiple of 512 B"));
	}

	if (Impl::chunk_size == chunk_size)
	    return;

	Impl::chunk_size = chunk_size;

	calculate_region_and_topology();
    }


    unsigned long
    Md::Impl::get_default_chunk_size()
    {
	return 512 * KiB;
    }


    bool
    Md::Impl::is_valid_name(const string& name)
    {
	return regex_match(name, numeric_name_regex) || regex_match(name, format1_name_regex);
    }


    bool
    Md::Impl::is_valid_sysfs_name(const string& name)
    {
	return regex_match(name, numeric_name_regex) || regex_match(name, format2_name_regex);
    }


    vector<MountByType>
    Md::Impl::possible_mount_bys() const
    {
	return { MountByType::DEVICE, MountByType::ID };
    }


    bool
    Md::Impl::activate_mds(const ActivateCallbacks* activate_callbacks, const TmpDir& tmp_dir)
    {
	y2mil("activate_mds");

	// When using 'mdadm --assemble --scan' without the previously
	// generated config file some devices, e.g. members of IMSM
	// containers, get non 'local' names (ending in '_' followed by a
	// digit string). Using 'mdadm --assemble --scan --config=partitions'
	// the members of containers are not started at all.

	// But using the additional step with --examine causes problems with duplicate MD
	// device names (see bsc #1199307).

	switch (mdadm_activate_method())
	{
	    case 0:
	    {
		SystemCmd cmd({ MDADM_BIN, "--assemble", "--scan" });

		if (cmd.retcode() == 0)
		    udev_settle();

		return cmd.retcode() == 0;
	    }

	    case 1:
	    {
		string filename = tmp_dir.get_fullname() + "/mdadm.conf";

		string cmd_line1 = MDADM_BIN " --examine --scan > " + quote(filename);
		SystemCmd cmd1(cmd_line1);

		string cmd_line2 = MDADM_BIN " --assemble --scan --config=" + quote(filename);
		SystemCmd cmd2(cmd_line2);

		if (cmd2.retcode() == 0)
		    udev_settle();

		unlink(filename.c_str());

		return cmd2.retcode() == 0;
	    }
	}

	return false;
    }


    bool
    Md::Impl::deactivate_mds()
    {
	y2mil("deactivate_mds");

	SystemCmd::Args cmd_args = { MDADM_BIN, "--stop", "--scan" };

	SystemCmd cmd(cmd_args);

	return cmd.retcode() == 0;
    }


    void
    Md::Impl::probe_mds(Prober& prober)
    {
	SystemInfo::Impl& system_info = prober.get_system_info();
	const MdLinks& md_links = system_info.getMdLinks();

	for (const string& short_name : prober.get_sys_block_entries().mds)
	{
	    string name = DEV_DIR "/" + short_name;

	    try
	    {
		const CmdMdadmDetail& cmd_mdadm_detail = system_info.getCmdMdadmDetail(name);

		if (!cmd_mdadm_detail.devname.empty())
		{
		    MdLinks::const_iterator it = md_links.find(short_name);
		    if (it != md_links.end())
		    {
			// the mapping is backwards so we must iterate the result
			const vector<string>& links = it->second;
			if (std::find(links.begin(), links.end(), cmd_mdadm_detail.devname) != links.end())
			{
			    name = DEV_MD_DIR "/" + cmd_mdadm_detail.devname;
			}
		    }
		}

		const ProcMdstat::Entry& entry = system_info.getProcMdstat().get_entry(short_name);

		if (entry.is_container)
		{
		    MdContainer* md_container = MdContainer::create(prober.get_system(), name);
		    md_container->get_impl().probe_pass_1a(prober);
		}
		else if (entry.has_container)
		{
		    MdMember* md_member = MdMember::create(prober.get_system(), name);
		    md_member->get_impl().probe_pass_1a(prober);
		}
		else
		{
		    Md* md = Md::create(prober.get_system(), name);
		    md->get_impl().set_active(!entry.inactive);
		    md->get_impl().probe_pass_1a(prober);
		}
	    }
	    catch (const Exception& exception)
	    {
		// TRANSLATORS: error message
		prober.handle(exception, sformat(_("Probing MD RAID %s failed"), name), UF_MDRAID);
	    }
	}
    }


    void
    Md::Impl::probe_pass_1a(Prober& prober)
    {
	Partitionable::Impl::probe_pass_1a(prober);

	SystemInfo::Impl& system_info = prober.get_system_info();

	const ProcMdstat::Entry& entry = system_info.getProcMdstat().get_entry(get_sysfs_name());
	md_parity = entry.md_parity;
	chunk_size = entry.chunk_size;

	const CmdMdadmDetail& cmd_mdadm_detail = system_info.getCmdMdadmDetail(get_name());
	uuid = cmd_mdadm_detail.uuid;
	metadata = cmd_mdadm_detail.metadata;
	md_level = cmd_mdadm_detail.level;

	const Storage& storage = prober.get_storage();
	const EtcMdadm& etc_mdadm = system_info.getEtcMdadm(storage.prepend_rootprefix(ETC_MDADM));
	in_etc_mdadm = etc_mdadm.has_entry(uuid);
    }


    void
    Md::Impl::probe_pass_1b(Prober& prober)
    {
	const ProcMdstat::Entry& entry = prober.get_system_info().getProcMdstat().get_entry(get_sysfs_name());

	for (const ProcMdstat::Device& device : entry.devices)
	{
	    prober.add_holder(device.name, get_non_impl(), [&device](Devicegraph* system, Device* a, Device* b) {
		MdUser* md_user = MdUser::create(system, a, b);
		md_user->set_spare(device.spare);
		md_user->set_faulty(device.faulty);
		md_user->set_journal(device.journal);
	    });
	}
    }


    void
    Md::Impl::probe_pass_1f(Prober& prober)
    {
	Devicegraph* system = prober.get_system();
	SystemInfo::Impl& system_info = prober.get_system_info();

	// The order/sort-key/role cannot be probed by looking at
	// /proc/mdstat. As an example consider a RAID10 where the
	// devices must be evenly split between two disk subsystems
	// (https://fate.suse.com/313521). Let us simply call the
	// devices sdc1, sdd1, sdc2, sdd2. If sdd1 fails and gets
	// replaced by sdd3 using the role in /proc/mdstat would be
	// wrong (sdd3[4] sdd2[3] sdc2[2] sdc1[0]). The role reported
	// by 'mdadm --detail' seems to be fine.

	// AFAIS probing the order for spare devices is not possible
	// (and likely also not useful).

	const CmdMdadmDetail& cmd_mdadm_detail = system_info.getCmdMdadmDetail(get_name());

	// Convert roles from map<name, role> to map<sid, role>.

	map<sid_t, string> roles;

	for (const map<string, string>::value_type& role : cmd_mdadm_detail.roles)
	{
	    BlkDevice* blk_device = BlkDevice::Impl::find_by_any_name(system, role.first, system_info);
	    roles[blk_device->get_sid()] = role.second;
	}

	// Set sort-key for each (non spare or faulty) device based on
	// the role. Since for libstorage-ng a sort-key of 0 means
	// unknown (or should mean unknown) an offset of 1 is
	// added.

	for (MdUser* md_user : get_in_holders_of_type<MdUser>())
	{
	    sid_t sid = md_user->get_source()->get_sid();

	    map<sid_t, string>::const_iterator it = roles.find(sid);
	    if (it == roles.end() || it->second == "spare")
		continue;

	    unsigned int role = 0;
	    it->second >> role;

	    md_user->set_sort_key(role + 1);
	}
    }


    void
    Md::Impl::probe_uuid()
    {
	udev_settle();

	// Note: The UUID reported by mdadm has a different format than reported by
	// e.g. blkid.

	CmdMdadmDetail cmd_mdadm_detail(get_name());
	uuid = cmd_mdadm_detail.uuid;
    }


    void
    Md::Impl::parent_has_new_region(const Device* parent)
    {
	calculate_region_and_topology();
    }


    void
    Md::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<shared_ptr<Action::Base>> actions;

	actions.push_back(make_shared<Action::Create>(get_sid()));

	if (in_etc_mdadm)
	    actions.push_back(make_shared<Action::AddToEtcMdadm>(get_sid()));

	actiongraph.add_chain(actions);

	// see Encryption::Impl::add_create_actions()

	if (in_etc_mdadm)
	{
	    actions[0]->last = true;
	    actions[1]->last = false;
	}
    }


    void
    Md::Impl::add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs_base) const
    {
	BlkDevice::Impl::add_modify_actions(actiongraph, lhs_base);

	const Impl& lhs = dynamic_cast<const Impl&>(lhs_base->get_impl());

	if (lhs.get_name() != get_name())
	    ST_THROW(Exception("cannot rename raid"));

	if (lhs.md_level != md_level)
	    ST_THROW(Exception("cannot change raid level"));

	if (lhs.metadata != metadata)
	    ST_THROW(Exception("cannot change raid metadata"));

	if (lhs.chunk_size != chunk_size)
	    ST_THROW(Exception("cannot change chunk size"));

	if (lhs.get_region() != get_region())
	    ST_THROW(Exception("cannot change size"));

	if (!lhs.in_etc_mdadm && in_etc_mdadm)
	{
	    shared_ptr<Action::Base> action = make_shared<Action::AddToEtcMdadm>(get_sid());
	    actiongraph.add_vertex(action);
	}
	else if (lhs.in_etc_mdadm && !in_etc_mdadm)
	{
	    shared_ptr<Action::Base> action = make_shared<Action::RemoveFromEtcMdadm>(get_sid());
	    actiongraph.add_vertex(action);
	}
    }


    void
    Md::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<shared_ptr<Action::Base>> actions;

	if (in_etc_mdadm)
	    actions.push_back(make_shared<Action::RemoveFromEtcMdadm>(get_sid()));

	if (is_active())
	    actions.push_back(make_shared<Action::Deactivate>(get_sid()));

	actions.push_back(make_shared<Action::Delete>(get_sid()));

	actiongraph.add_chain(actions);
    }


    void
    Md::Impl::save(xmlNode* node) const
    {
	Partitionable::Impl::save(node);

	setChildValue(node, "md-level", toString(md_level));
	setChildValueIf(node, "md-parity", toString(md_parity), md_parity != MdParity::DEFAULT);

	setChildValueIf(node, "chunk-size", chunk_size, chunk_size != 0);

	setChildValueIf(node, "uuid", uuid, !uuid.empty());

	setChildValueIf(node, "metadata", metadata, !metadata.empty());

	setChildValueIf(node, "in-etc-mdadm", in_etc_mdadm, !in_etc_mdadm);
    }


    MdUser*
    Md::Impl::add_device(BlkDevice* blk_device)
    {
	ST_CHECK_PTR(blk_device);

	if (blk_device->has_children())
	    ST_THROW(WrongNumberOfChildren(blk_device->num_children(), 0));

	MdUser* md_user = MdUser::create(get_devicegraph(), blk_device, get_non_impl());

	calculate_region_and_topology();

	return md_user;
    }


    void
    Md::Impl::remove_device(BlkDevice* blk_device)
    {
	ST_CHECK_PTR(blk_device);

	MdUser* md_user = to_md_user(get_devicegraph()->find_holder(blk_device->get_sid(), get_sid()));

	get_devicegraph()->remove_holder(md_user);

	calculate_region_and_topology();
    }


    vector<BlkDevice*>
    Md::Impl::get_blk_devices()
    {
	Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	return devicegraph.filter_devices_of_type<BlkDevice>(devicegraph.parents(vertex));
    }


    vector<const BlkDevice*>
    Md::Impl::get_blk_devices() const
    {
	const Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	return devicegraph.filter_devices_of_type<const BlkDevice>(devicegraph.parents(vertex));
    }


    bool
    Md::Impl::is_numeric() const
    {
	return regex_match(get_name(), numeric_name_regex);
    }


    unsigned int
    Md::Impl::get_number() const
    {
	smatch match;

	if (!regex_match(get_name(), match, numeric_name_regex) || match.size() != 2)
	    ST_THROW(Exception("not a numeric Md"));

	try
	{
	    return stoi(match[1]);
	}
	catch (const std::out_of_range& e)
	{
	    ST_THROW(Exception("Md number out of range"));
	}
    }


    string
    Md::Impl::get_short_name() const
    {
	smatch match;

	if (!regex_match(get_name(), match, format1_name_regex) || match.size() != 2)
	    ST_THROW(Exception("not a named Md"));

	return match[1];
    }


    uf_t
    Md::Impl::used_features(UsedFeaturesDependencyType used_features_dependency_type) const
    {
	return UF_MDRAID | Partitionable::Impl::used_features(used_features_dependency_type);
    }


    bool
    Md::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Partitionable::Impl::equal(rhs))
	    return false;

	return md_level == rhs.md_level && md_parity == rhs.md_parity &&
	    chunk_size == rhs.chunk_size && metadata == rhs.metadata &&
	    uuid == rhs.uuid && in_etc_mdadm == rhs.in_etc_mdadm;
    }


    void
    Md::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Partitionable::Impl::log_diff(log, rhs);

	storage::log_diff_enum(log, "md-level", md_level, rhs.md_level);
	storage::log_diff_enum(log, "md-parity", md_parity, rhs.md_parity);

	storage::log_diff(log, "chunk-size", chunk_size, rhs.chunk_size);

	storage::log_diff(log, "metadata", metadata, rhs.metadata);

	storage::log_diff(log, "uuid", uuid, rhs.uuid);

	storage::log_diff(log, "in-etc-mdadm", in_etc_mdadm, rhs.in_etc_mdadm);
    }


    void
    Md::Impl::print(std::ostream& out) const
    {
	Partitionable::Impl::print(out);

	out << " md-level:" << toString(get_md_level());
	out << " md-parity:" << toString(get_md_parity());

	out << " chunk-size:" << get_chunk_size();

	out << " metadata:" << metadata;

	out << " uuid:" << uuid;

	out << " in-etc-mdadm:" << in_etc_mdadm;
    }


    void
    Md::Impl::process_udev_paths(vector<string>& udev_paths, const UdevFilters& udev_filters) const
    {
	udev_filter(udev_paths, udev_filters.md.allowed_path_patterns);
    }


    void
    Md::Impl::process_udev_ids(vector<string>& udev_ids, const UdevFilters& udev_filters) const
    {
	udev_filter(udev_ids, udev_filters.md.allowed_id_patterns);
    }


    unsigned int
    Md::Impl::minimal_number_of_devices(MdLevel md_level)
    {
	switch (md_level)
	{
	    case MdLevel::LINEAR:
		return 2;

	    case MdLevel::RAID0:
		return 2;

	    case MdLevel::RAID1:
		return 2;

	    case MdLevel::RAID4:
	    case MdLevel::RAID5:
		return 3;

	    case MdLevel::RAID6:
		return 4;

	    case MdLevel::RAID10:
		return 2;

	    default:
		return 0;
	}
    }


    unsigned int
    Md::Impl::minimal_number_of_devices() const
    {
	return minimal_number_of_devices(md_level);
    }


    bool
    Md::Impl::supports_spare_devices() const
    {
	switch (md_level)
	{
	    case MdLevel::LINEAR:
	    case MdLevel::RAID0:
		return false;

	    case MdLevel::RAID1:
	    case MdLevel::RAID4:
	    case MdLevel::RAID5:
	    case MdLevel::RAID6:
	    case MdLevel::RAID10:
		return true;

	    default:
		return false;
	}
    }


    bool
    Md::Impl::supports_journal_device() const
    {
	switch (md_level)
	{
	    case MdLevel::LINEAR:
	    case MdLevel::RAID0:
	    case MdLevel::RAID1:
		return false;

	    case MdLevel::RAID4:
	    case MdLevel::RAID5:
	    case MdLevel::RAID6:
		return true;

	    case MdLevel::RAID10:
		return false;

	    default:
		return false;
	}
    }


    unsigned int
    Md::Impl::number_of_devices() const
    {
	vector<const BlkDevice*> blk_devices = get_blk_devices();

	return std::count_if(blk_devices.begin(), blk_devices.end(), [](const BlkDevice* blk_device) {
	    const MdUser* md_user = blk_device->get_impl().get_single_out_holder_of_type<const MdUser>();
	    return !md_user->is_spare() && !md_user->is_journal();
	});
    }


    void
    Md::Impl::calculate_region_and_topology()
    {
	// Calculating the exact size of a MD is difficult. Since a size too
	// big can lead to severe problems later on, e.g. a partition not
	// fitting anymore, we make a conservative calculation.

	// The size depends slightly on the metadata version but this effect is not
	// considered here.

	// Location of superblock, data offset and more can be seen with "mdadm --examine
	// /dev/sdxn" for each device.

	// For combining disks with different block sizes, see doc/md-raid.md.

	// Since our size calculation is not accurate we must not recalculate
	// the size of an RAID existing on disk. That would cause a resize
	// action to be generated. Operations changing the RAID size are not
	// supported.

	if (exists_in_system())
	    return;

	vector<const BlkDevice*> blk_devices = as_const(*this).get_blk_devices();

	long real_chunk_size = chunk_size;

	if (real_chunk_size == 0)
	    real_chunk_size = get_default_chunk_size();

	// mdadm uses a chunk size of 64 KiB just in case the RAID1 is ever reshaped to RAID5.
	if (md_level == MdLevel::RAID1)
	    real_chunk_size = 64 * KiB;

	/* values of the underlying devices */
	unsigned int underlying_number = 0;
	unsigned long long underlying_sum = 0;
	unsigned long long underlying_smallest = std::numeric_limits<unsigned long long>::max();
	unsigned int underlying_block_size = 0;
	unsigned long underlying_optimal_io_size = 0;

	for (const BlkDevice* blk_device : blk_devices)
	{
	    const Region& region = blk_device->get_region();
	    const Topology& topology = blk_device->get_topology();

	    unsigned long long underlying_size = blk_device->get_size();

	    underlying_block_size = std::max(underlying_block_size, region.get_block_size());
	    underlying_optimal_io_size = std::max(underlying_optimal_io_size, topology.get_optimal_io_size());

	    const MdUser* md_user = blk_device->get_impl().get_single_out_holder_of_type<const MdUser>();
	    bool spare = md_user->is_spare();
	    bool journal = md_user->is_journal();

	    // see super1.c in mdadm sources for hints

	    // includes some alignment
	    unsigned long long superblock_size = 16 * KiB;

	    // roughly 1% with 128 MiB limit
	    unsigned long long bitmap_size = min(128 * MiB, underlying_size / 64);

	    // man page says 4 KiB but from the code it looks like 8 KiB
	    unsigned long long badblocklog_size = 8 * KiB;

	    underlying_size = subtract_saturated(underlying_size, superblock_size + bitmap_size +
						 badblocklog_size + real_chunk_size);

	    // for metadata version 1.1 and 1.2 the data-offset is rounded by 1 MiB
	    underlying_size = round_down(underlying_size, 1 * MiB);

	    long rest = underlying_size % real_chunk_size;
	    if (rest > 0)
		underlying_size = subtract_saturated(underlying_size, rest);

	    // safety net
	    underlying_size = subtract_saturated(underlying_size, max(128 * KiB, underlying_size / 1024));

	    if (!spare && !journal)
	    {
		underlying_number++;
		underlying_sum += underlying_size;
	    }

	    if (!journal)
	    {
		underlying_smallest = min(underlying_smallest, underlying_size);
	    }
	}

	unsigned long long size = 0;
	unsigned long optimal_io_size = 0;

	switch (md_level)
	{
	    case MdLevel::LINEAR:
		if (underlying_number >= 2)
		{
		    size = underlying_sum;

		    if (underlying_optimal_io_size > 0)
			optimal_io_size = underlying_optimal_io_size;
		}
		break;

	    case MdLevel::RAID0:
		if (underlying_number >= 2)
		{
		    size = underlying_sum;
		    optimal_io_size = real_chunk_size * underlying_number;

		    if (underlying_optimal_io_size > 0)
			optimal_io_size = boost::integer::lcm(optimal_io_size, underlying_optimal_io_size);
		}
		break;

	    case MdLevel::RAID1:
		if (underlying_number >= 2)
		{
		    size = underlying_smallest;

		    if (underlying_optimal_io_size > 0)
			optimal_io_size = underlying_optimal_io_size;
		}
		break;

	    case MdLevel::RAID4:
	    case MdLevel::RAID5:
		if (underlying_number >= 3)
		{
		    size = underlying_smallest * (underlying_number - 1);
		    optimal_io_size = real_chunk_size * (underlying_number - 1);

		    if (underlying_optimal_io_size > 0)
			optimal_io_size = boost::integer::lcm(optimal_io_size, underlying_optimal_io_size);
		}
		break;

	    case MdLevel::RAID6:
		if (underlying_number >= 4)
		{
		    size = underlying_smallest * (underlying_number - 2);
		    optimal_io_size = real_chunk_size * (underlying_number - 2);

		    if (underlying_optimal_io_size > 0)
			optimal_io_size = boost::integer::lcm(optimal_io_size, underlying_optimal_io_size);
		}
		break;

	    case MdLevel::RAID10:
		if (underlying_number >= 2)
		{
		    size = ((underlying_smallest / real_chunk_size) * underlying_number / 2) * real_chunk_size;
		    optimal_io_size = real_chunk_size * underlying_number / 2;
		    if (underlying_number % 2 == 1)
			optimal_io_size *= 2;

		    if (underlying_optimal_io_size > 0)
			optimal_io_size = boost::integer::lcm(optimal_io_size, underlying_optimal_io_size);
		}
		break;

	    case MdLevel::CONTAINER:
	    case MdLevel::UNKNOWN:
		break;
	}

	if (underlying_block_size && underlying_block_size != get_region().get_block_size())
	    set_region(Region(0, 0, underlying_block_size));

	set_size(size);
	set_topology(Topology(0, optimal_io_size));
    }


    Text
    Md::Impl::do_create_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by RAID level (e.g. RAID0),
			   // %2$s is replaced by RAID name (e.g. /dev/md0),
			   // %3$s is replaced by size (e.g. 2.00 GiB),
			   // %4$s is replaced by one or more devices (e.g /dev/sda1 (1.00 GiB) and
			   // /dev/sdb2 (1.00 GiB))
			   _("Create MD %1$s %2$s (%3$s) from %4$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by RAID level (e.g. RAID0),
			   // %2$s is replaced by RAID name (e.g. /dev/md0),
			   // %3$s is replaced by size (e.g. 2.00 GiB),
			   // %4$s is replaced by one or more devices (e.g /dev/sda1 (1.00 GiB) and
			   // /dev/sdb2 (1.00 GiB))
			   _("Creating MD %1$s %2$s (%3$s) from %4$s"));

	return sformat(text, get_md_level_name(md_level), get_displayname(),
		       get_size_text(), join(get_blk_devices(), JoinMode::COMMA, 20));
    }


    void
    Md::Impl::do_create()
    {
	// Note: Changing any parameter to "mdadm --create' requires the
	// function calculate_region_and_topology() to be checked!

	// place devices in multimaps to sort them according to the sort-key

	multimap<unsigned int, string> devices;
	multimap<unsigned int, string> spares;
	vector<string> journals;

	for (const BlkDevice* blk_device : get_blk_devices())
	{
	    const MdUser* md_user = blk_device->get_impl().get_single_out_holder_of_type<const MdUser>();

	    if (md_user->is_spare())
		spares.insert(make_pair(md_user->get_sort_key(), blk_device->get_name()));
	    else if (md_user->is_journal())
		journals.push_back(blk_device->get_name());
	    else
		devices.insert(make_pair(md_user->get_sort_key(), blk_device->get_name()));
	}

	if (devices.empty())
	    ST_THROW(Exception("no devices"));

	if (journals.size() > 1)
	    ST_THROW(Exception("only one journal device allowed"));

	SystemCmd::Args cmd_args = { MDADM_BIN, "--create", get_name(), "--run", "--level=" +
	    boost::to_lower_copy(toString(md_level), locale::classic()), "--metadata=" +
	    (metadata.empty() ? "1.0" : metadata), "--homehost=any" };

	if ((md_level == MdLevel::RAID1 || md_level == MdLevel::RAID4 ||
	     md_level == MdLevel::RAID5 || md_level == MdLevel::RAID6 ||
	     md_level == MdLevel::RAID10) && journals.empty())
	    cmd_args << "--bitmap=internal";

	// mdadm 4.2 bails out if a chunk size is provided for RAID1 (bsc#1205172)
	if (is_chunk_size_meaningful() && chunk_size > 0)
	    cmd_args << "--chunk=" + to_string(chunk_size / KiB);

	if (md_parity != MdParity::DEFAULT)
	    cmd_args << "--parity=" + toString(md_parity);

	if (!uuid.empty())
	    cmd_args << "--uuid=" + uuid;

	cmd_args << "--raid-devices=" + to_string(devices.size());

	if (!spares.empty())
	    cmd_args << "--spare-devices=" + to_string(spares.size());

	for (const pair<const unsigned int, string>& value : devices)
	    cmd_args << value.second;

	for (const pair<const unsigned int, string>& value : spares)
	    cmd_args << value.second;

	if (!journals.empty())
	    cmd_args << "--write-journal=" + journals.front();

	wait_for_devices(std::as_const(*this).get_blk_devices());

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);

	if (!is_numeric())
	{
	    SystemInfo::Impl system_info;
	    const MdLinks& md_links = system_info.getMdLinks();

	    MdLinks::const_iterator it = md_links.find_reverse(get_short_name());
	    if (it == md_links.end())
		ST_THROW(Exception("named md link not found"));

	    set_sysfs_name(it->first);
	    set_sysfs_path("/devices/virtual/block/" + it->first);
	}

	if (uuid.empty())
	{
	    probe_uuid();
	}
    }


    void
    Md::Impl::do_create_post_verify() const
    {
	// log some data about the MD RAID that might be useful for debugging

	SystemCmd::Args cmd_args = { CAT_BIN, PROC_DIR "/mdstat" };

	SystemCmd cmd(cmd_args, SystemCmd::NoThrow);

	SystemInfo::Impl system_info;

	const File& size_file = get_sysfs_file(system_info, "size");
	const File& alignment_offset_file = get_sysfs_file(system_info, "alignment_offset");
	const File& optimal_io_size_file = get_sysfs_file(system_info, "queue/optimal_io_size");

	unsigned long long size = 512 * size_file.get<unsigned long long>();
	long alignment_offset = alignment_offset_file.get<long>();
	unsigned long optimal_io_size = optimal_io_size_file.get<unsigned long>();

	log_unexpected("md size", get_size(), size);
	log_unexpected("md alignment_offset", get_topology().get_alignment_offset(), alignment_offset);
	log_unexpected("md optimal_io_size", get_topology().get_optimal_io_size(), optimal_io_size);
    }


    Text
    Md::Impl::do_delete_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by RAID level (e.g. RAID0),
			   // %2$s is replaced by RAID name (e.g. /dev/md0),
			   // %3$s is replaced by size (e.g. 2.00 GiB)
			   _("Delete MD %1$s %2$s (%3$s)"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by RAID level (e.g. RAID0),
			   // %2$s is replaced by RAID name (e.g. /dev/md0),
			   // %3$s is replaced by size (e.g. 2.00 GiB)
			   _("Deleting MD %1$s %2$s (%3$s)"));

	return sformat(text, get_md_level_name(md_level), get_displayname(), get_size_text());
    }


    void
    Md::Impl::do_delete() const
    {
	SystemCmd::Args cmd_args = { MDADM_BIN, "--zero-superblock" };

	for (const BlkDevice* blk_device : get_blk_devices())
	    cmd_args << blk_device->get_name();

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);
    }


    Text
    Md::Impl::do_add_to_etc_mdadm_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by md name (e.g. /dev/md0)
			   _("Add %1$s to /etc/mdadm.conf"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by md name (e.g. /dev/md0)
			   _("Adding %1$s to /etc/mdadm.conf"));

	return sformat(text, get_name());
    }


    void
    Md::Impl::do_add_to_etc_mdadm(CommitData& commit_data) const
    {
	EtcMdadm& etc_mdadm = commit_data.get_etc_mdadm();

	etc_mdadm.init(get_storage());

	EtcMdadm::Entry entry;

	entry.device = get_name();
	entry.uuid = uuid;

	etc_mdadm.update_entry(entry);
    }


    Text
    Md::Impl::do_remove_from_etc_mdadm_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by md name (e.g. /dev/md0)
			   _("Remove %1$s from /etc/mdadm.conf"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by md name (e.g. /dev/md0)
			   _("Removing %1$s from /etc/mdadm.conf"));

	return sformat(text, get_name());
    }


    void
    Md::Impl::do_remove_from_etc_mdadm(CommitData& commit_data) const
    {
	EtcMdadm& etc_mdadm = commit_data.get_etc_mdadm();

	// TODO containers?

	etc_mdadm.remove_entry(uuid);
    }


    Text
    Md::Impl::do_reallot_text(const CommitData& commit_data, const Action::Reallot* action) const
    {
	Text text;

	switch (action->reallot_mode)
	{
	    case ReallotMode::REDUCE:
		text = tenser(commit_data.tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by device name (e.g. /dev/sdd),
			      // %2$s is replaced by device name (e.g. /dev/md0)
			      _("Remove %1$s from %2$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by device name (e.g. /dev/sdd),
			      // %2$s is replaced by device name (e.g. /dev/md0)
			      _("Removing %1$s from %2$s"));
		break;

	    case ReallotMode::EXTEND:
		text = tenser(commit_data.tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by device name (e.g. /dev/sdd),
			      // %2$s is replaced by device name (e.g. /dev/md0)
			      _("Add %1$s to %2$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by device name (e.g. /dev/sdd),
			      // %2$s is replaced by device name (e.g. /dev/md0)
			      _("Adding %1$s to %2$s"));
		break;

	    default:
		ST_THROW(LogicException("invalid value for reallot_mode"));
	}

	return sformat(text, to_blk_device(action->device)->get_name(), get_displayname());
    }


    void
    Md::Impl::do_reallot(const CommitData& commit_data, const Action::Reallot* action) const
    {
	const BlkDevice* blk_device = to_blk_device(action->device);

	switch (action->reallot_mode)
	{
	    case ReallotMode::REDUCE:
		do_reduce(blk_device);
		return;

	    case ReallotMode::EXTEND:
		do_extend(blk_device);
		return;
	}

	ST_THROW(LogicException("invalid value for reallot_mode"));
    }


    void
    Md::Impl::do_reduce(const BlkDevice* blk_device) const
    {
	SystemCmd::Args cmd_args = { MDADM_BIN, "--remove", get_name(), blk_device->get_name() };

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);

	// Thanks to udev "md-raid-assembly.rules" running "parted <disk>
	// print" re-adds the device to the md if the signature is still
	// valid. Thus remove the signature.
	blk_device->get_impl().wipe_device();
    }


    void
    Md::Impl::do_extend(const BlkDevice* blk_device) const
    {
	const MdUser* md_user = blk_device->get_impl().get_single_out_holder_of_type<const MdUser>();

	SystemCmd::Args cmd_args = { MDADM_BIN, !md_user->is_spare() ? "--add" : "--add-spare", get_name(),
	    blk_device->get_name() };

	storage::wait_for_devices({ blk_device });

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);
    }


    Text
    Md::Impl::do_deactivate_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by RAID level (e.g. RAID0),
			   // %2$s is replaced by RAID name (e.g. /dev/md0),
			   // %3$s is replaced by size (e.g. 2.00 GiB)
			   _("Deactivate MD %1$s %2$s (%3$s)"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by RAID level (e.g. RAID0),
			   // %2$s is replaced by RAID name (e.g. /dev/md0),
			   // %3$s is replaced by size (e.g. 2.00 GiB)
			   _("Deactivating MD %1$s %2$s (%3$s)"));

	return sformat(text, get_md_level_name(md_level), get_displayname(), get_size_text());
    }


    void
    Md::Impl::do_deactivate()
    {
	SystemCmd::Args cmd_args = { MDADM_BIN, "--stop", get_name() };

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);
    }

}
