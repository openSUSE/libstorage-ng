/*
 * Copyright (c) [2017-2019] SUSE LLC
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


#include "storage/Devices/MultipathImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Prober.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Utils/Exception.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/StorageTypes.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/UsedFeatures.h"
#include "storage/Holders/User.h"
#include "storage/Utils/StorageTypes.h"
#include "storage/Utils/CallbacksImpl.h"
#include "storage/Utils/Format.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Multipath>::classname = "Multipath";


    Multipath::Impl::Impl(const string& name)
	: Partitionable::Impl(name), vendor(), model(), rotational(false)
    {
	if (!is_valid_name(name))
	    ST_THROW(Exception("invalid Multipath name"));

	set_dm_table_name(name.substr(strlen(DEV_MAPPER_DIR "/")));
    }


    Multipath::Impl::Impl(const string& name, const Region& region)
	: Partitionable::Impl(name, region), vendor(), model(), rotational(false)
    {
	if (!is_valid_name(name))
	    ST_THROW(Exception("invalid Multipath name"));

	set_dm_table_name(name.substr(strlen(DEV_MAPPER_DIR "/")));
    }


    Multipath::Impl::Impl(const xmlNode* node)
	: Partitionable::Impl(node), vendor(), model(), rotational(false)
    {
	getChildValue(node, "vendor", vendor);
	getChildValue(node, "model", model);

	getChildValue(node, "rotational", rotational);
    }


    string
    Multipath::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("Multipath").translated;
    }


    void
    Multipath::Impl::check(const CheckCallbacks* check_callbacks) const
    {
	Partitionable::Impl::check(check_callbacks);

	if (!is_valid_name(get_name()))
	    ST_THROW(Exception("invalid name"));
    }


    bool
    Multipath::Impl::is_valid_name(const string& name)
    {
	return boost::starts_with(name, DEV_MAPPER_DIR "/");
    }


    bool
    Multipath::Impl::activate_multipaths(const ActivateCallbacks* activate_callbacks)
    {
	y2mil("activate_multipaths");

	try
	{
	    bool looks_like_real_multipath = CmdMultipath(true).looks_like_real_multipath();

	    y2mil("looks-like-real-multipath:" << looks_like_real_multipath);

	    if (!activate_callbacks->multipath(looks_like_real_multipath))
	    {
		y2mil("user does not want multipath");
		return false;
	    }
	    else
	    {
		y2mil("user does want multipath");
	    }

	    // TRANSLATORS: progress message
	    message_callback(activate_callbacks, _("Activating multipath"));

	    try
	    {
		SystemCmd cmd1(MULTIPATHBIN, SystemCmd::DoThrow);

		SystemCmd(UDEVADM_BIN_SETTLE);

		SystemCmd cmd2(MULTIPATHDBIN, SystemCmd::DoThrow);

		SystemCmd(UDEVADM_BIN_SETTLE);

		return true;
	    }
	    catch (const Exception& exception)
	    {
		ST_CAUGHT(exception);

		// TRANSLATORS: error message
		error_callback(activate_callbacks, _("Activating multipath failed"), exception);

		return false;
	    }
	}
	catch (const Exception& exception)
	{
	    ST_CAUGHT(exception);

	    if (typeid(exception) == typeid(Aborted))
		ST_RETHROW(exception);

	    // Ignore failure to detect whether multipath needs to be activated.

	    return false;
	}
    }


    bool
    Multipath::Impl::deactivate_multipaths()
    {
	y2mil("deactivate_multipaths");

	string cmd_line1 = MULTIPATHDBIN " -k'shutdown'";

	SystemCmd cmd1(cmd_line1);

	string cmd_line2 = MULTIPATHBIN " -F";

	SystemCmd cmd2(cmd_line2);

	return cmd2.retcode() == 0;
    }


    void
    Multipath::Impl::probe_multipaths(Prober& prober)
    {
	const CmdMultipath& cmd_multipath = prober.get_system_info().getCmdMultipath();

	for (const string& dm_name : cmd_multipath.get_entries())
	{
	    Multipath* multipath = Multipath::create(prober.get_system(), DEV_MAPPER_DIR "/" + dm_name);
	    multipath->get_impl().probe_pass_1a(prober);
	}
    }


    void
    Multipath::Impl::probe_pass_1a(Prober& prober)
    {
	Partitionable::Impl::probe_pass_1a(prober);

	SystemInfo& system_info = prober.get_system_info();

	const File& rotational_file = get_sysfs_file(system_info, "queue/rotational");
	rotational = rotational_file.get<bool>();

	const CmdMultipath& cmd_multipath = prober.get_system_info().getCmdMultipath();

	const CmdMultipath::Entry& entry = cmd_multipath.get_entry(get_dm_table_name());

	vendor = entry.vendor;
	model = entry.model;
    }


    void
    Multipath::Impl::probe_pass_1b(Prober& prober)
    {
	const CmdMultipath& cmd_multipath = prober.get_system_info().getCmdMultipath();

	const CmdMultipath::Entry& entry = cmd_multipath.get_entry(get_dm_table_name());

	for (const string& device : entry.devices)
	{
	    BlkDevice* blk_device = BlkDevice::Impl::find_by_any_name(prober.get_system(), device,
								      prober.get_system_info());
	    User::create(prober.get_system(), blk_device, get_non_impl());
	}
    }


    uint64_t
    Multipath::Impl::used_features() const
    {
	return UF_MULTIPATH | Partitionable::Impl::used_features();
    }


    void
    Multipath::Impl::save(xmlNode* node) const
    {
	Partitionable::Impl::save(node);

	setChildValue(node, "vendor", vendor);
	setChildValue(node, "model", model);

	setChildValueIf(node, "rotational", rotational, rotational);
    }


    void
    Multipath::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	ST_THROW(Exception("cannot create multipath"));
    }


    void
    Multipath::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	ST_THROW(Exception("cannot delete multipath"));
    }


    bool
    Multipath::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Partitionable::Impl::equal(rhs))
	    return false;

	return vendor == rhs.vendor && model == rhs.model && rotational == rhs.rotational;
    }


    void
    Multipath::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Partitionable::Impl::log_diff(log, rhs);

	storage::log_diff(log, "vendor", vendor, rhs.vendor);
	storage::log_diff(log, "model", model, rhs.model);

	storage::log_diff(log, "rotational", rotational, rhs.rotational);
    }


    void
    Multipath::Impl::print(std::ostream& out) const
    {
	Partitionable::Impl::print(out);

	out << " vendor:" << vendor << " model:" << model;

	if (rotational)
	    out << " rotational";
    }


    ResizeInfo
    Multipath::Impl::detect_resize_info(const BlkDevice* blk_device) const
    {
	return ResizeInfo(false, RB_RESIZE_NOT_SUPPORTED_BY_DEVICE);
    }


    void
    Multipath::Impl::process_udev_ids(vector<string>& udev_ids) const
    {
	// See doc/udev.md.

	static const vector<string> allowed_prefixes = { "scsi-", "wwn-", "dm-name-", "dm-uuid-" };

	erase_if(udev_ids, [](const string& udev_id) {
	    return none_of(allowed_prefixes.begin(), allowed_prefixes.end(), [&udev_id](const string& prefix)
			   { return boost::starts_with(udev_id, prefix); });
	});

	stable_partition(udev_ids.begin(), udev_ids.end(), string_starts_with("wwn-"));
	stable_partition(udev_ids.begin(), udev_ids.end(), string_starts_with("scsi-"));
    }


    Text
    Multipath::Impl::do_deactivate_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by multipath name (e.g. /dev/mapper/36005076305ffc73a00000000000013b4),
			   // %2$s is replaced by size (e.g. 1 TiB)
			   _("Deactivate multipath %1$s (%2$s)"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by multipath name (e.g. /dev/mapper/36005076305ffc73a00000000000013b4),
			   // %2$s is replaced by size (e.g. 1 TiB)
			   _("Deactivating multipath %1$s (%2$s)"));

	return sformat(text, get_displayname(), get_size_text());
    }


    void
    Multipath::Impl::do_deactivate() const
    {
	string cmd_line = MULTIPATHBIN " -f " + quote(get_name());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }

}
