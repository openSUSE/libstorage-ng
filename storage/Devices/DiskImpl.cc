/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#include <ctype.h>

#include "storage/Devices/DiskImpl.h"
#include "storage/Devices/MdImpl.h"
#include "storage/Devices/BcacheImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Utils/Exception.h"
#include "storage/Utils/Enum.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/StorageTypes.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/AppUtil.h"
#include "storage/Utils/CallbacksImpl.h"
#include "storage/UsedFeatures.h"
#include "storage/Prober.h"
#include "storage/Utils/Format.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Disk>::classname = "Disk";


    const vector<string> EnumTraits<Transport>::names({
	"UNKNOWN", "SBP", "ATA", "FC", "iSCSI", "SAS", "SATA", "SPI", "USB", "FCoE"
    });


    Disk::Impl::Impl(const xmlNode* node)
	: Partitionable::Impl(node), rotational(false), transport(Transport::UNKNOWN)
    {
	string tmp;

	getChildValue(node, "rotational", rotational);

	if (getChildValue(node, "transport", tmp))
	    transport = toValueWithFallback(tmp, Transport::UNKNOWN);
    }


    string
    Disk::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("Disk").translated;
    }


    string
    Disk::Impl::get_sort_key() const
    {
	static const vector<NameSchema> name_schemata = {
	    NameSchema(regex(DEV_DIR "/sd([a-z]+)", regex::extended), { { 4, ' ' } }),
	    NameSchema(regex(DEV_DIR "/vd([a-z]+)", regex::extended), { { 4, ' ' } }),
	    NameSchema(regex(DEV_DIR "/mmcblk([0-9]+)", regex::extended), { { 3, '0' } }),
	    NameSchema(regex(DEV_DIR "/rsxx([0-9]+)", regex::extended), { { 3, '0' } }),
	    NameSchema(regex(DEV_DIR "/pmem([0-9]+)", regex::extended), { { 3, '0' } }),
	    NameSchema(regex(DEV_DIR "/nvme([0-9]+)n([0-9]+)", regex::extended), { { 3, '0' }, { 3, '0' } }),
	    NameSchema(regex(DEV_DIR "/xvd([a-z]+)", regex::extended), { { 4, ' ' } }),
	};

	return format_to_name_schemata(get_name(), name_schemata);
    }


    void
    Disk::Impl::probe_disks(Prober& prober)
    {
	for (const string& short_name : prober.get_sys_block_entries().disks)
	{
	    string name = DEV_DIR "/" + short_name;

	    try
	    {
		Disk* disk = Disk::create(prober.get_system(), name);
		disk->get_impl().probe_pass_1a(prober);
	    }
	    catch (const Exception& exception)
	    {
		// TRANSLATORS: error message
		error_callback(prober.get_probe_callbacks(), sformat(_("Probing disk %s failed"),
								     name.c_str()), exception);
	    }
	}
    }


    void
    Disk::Impl::probe_pass_1a(Prober& prober)
    {
	Partitionable::Impl::probe_pass_1a(prober);

	SystemInfo& system_info = prober.get_system_info();

	const File rotational_file = system_info.getFile(SYSFS_DIR + get_sysfs_path() +
							 "/queue/rotational");
	rotational = rotational_file.get<bool>();

	Lsscsi::Entry entry;
	if (system_info.getLsscsi().getEntry(get_name(), entry))
	    transport = entry.transport;
    }


    uint64_t
    Disk::Impl::used_features() const
    {
	uint64_t ret = 0;

	switch (transport)
	{
	    case Transport::FC: ret = UF_FC; break;
	    case Transport::FCOE: ret = UF_FCOE; break;
	    case Transport::ISCSI: ret = UF_ISCSI; break;
	    default: break;
	}

	return ret | Partitionable::Impl::used_features();
    }


    ResizeInfo
    Disk::Impl::detect_resize_info() const
    {
	return ResizeInfo(false, RB_RESIZE_NOT_SUPPORTED_BY_DEVICE);
    }


    void
    Disk::Impl::save(xmlNode* node) const
    {
	Partitionable::Impl::save(node);

	setChildValueIf(node, "rotational", rotational, rotational);

	setChildValueIf(node, "transport", toString(transport), transport != Transport::UNKNOWN);
    }


    void
    Disk::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	const Environment& environment = actiongraph.get_storage().get_environment();
	if (environment.get_target_mode() == TargetMode::IMAGE)
	{
	    vector<Action::Base*> actions;
	    actions.push_back(new Action::Create(get_sid()));
	    actiongraph.add_chain(actions);
	}
	else
	{
	    ST_THROW(Exception("cannot create disk"));
	}
    }


    void
    Disk::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	ST_THROW(Exception("cannot delete disk"));
    }


    bool
    Disk::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Partitionable::Impl::equal(rhs))
	    return false;

	return rotational == rhs.rotational && transport == rhs.transport;
    }


    void
    Disk::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Partitionable::Impl::log_diff(log, rhs);

	storage::log_diff(log, "rotational", rotational, rhs.rotational);

	storage::log_diff_enum(log, "transport", transport, rhs.transport);
    }


    void
    Disk::Impl::print(std::ostream& out) const
    {
	Partitionable::Impl::print(out);

	if (rotational)
	    out << " rotational";

	out << " transport:" << toString(get_transport());
    }


    void
    Disk::Impl::process_udev_paths(vector<string>& udev_paths) const
    {
	// See doc/udev.md.

	static const vector<string> allowed_prefixes = { "scsi-", "pci-", "ccw-" };

	erase_if(udev_paths, [](const string& udev_path) {
	    return none_of(allowed_prefixes.begin(), allowed_prefixes.end(), [&udev_path](const string& prefix)
			   { return boost::starts_with(udev_path, prefix); });
	});
    }


    void
    Disk::Impl::process_udev_ids(vector<string>& udev_ids) const
    {
	// See doc/udev.md.

	static const vector<string> allowed_prefixes = { "ata-", "scsi-", "usb-", "wwn-", "nvme-" };

	erase_if(udev_ids, [](const string& udev_id) {
	    return none_of(allowed_prefixes.begin(), allowed_prefixes.end(), [&udev_id](const string& prefix)
			   { return boost::starts_with(udev_id, prefix); });
	});

	stable_partition(udev_ids.begin(), udev_ids.end(), string_starts_with("ata-"));
    }


    Text
    Disk::Impl::do_create_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by the device name (e.g. /dev/vda),
			   // %2$s is replaced by the size (e.g. 20 GiB)
			   _("Create hard disk %1$s (%2$s)"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by the device name (e.g. /dev/vda),
			   // %2$s is replaced by the size (e.g. 20 GiB)
			   _("Creating hard disk %1$s (%2$s)"));

	return sformat(text, get_displayname().c_str(), get_size_string().c_str());
    }

}
