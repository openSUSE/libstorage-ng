/*
 * Copyright (c) 2017 SUSE LLC
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


#include <iostream>

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


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Multipath>::classname = "Multipath";


    Multipath::Impl::Impl(const string& name)
	: Partitionable::Impl(name), vendor(), model(), rotational(false)
    {
	if (!is_valid_name(name))
	    ST_THROW(Exception("invalid Multipath name"));

	set_range(Partitionable::Impl::default_range);

	set_dm_table_name(name.substr(strlen(DEVMAPPERDIR "/")));
    }


    Multipath::Impl::Impl(const string& name, const Region& region)
	: Partitionable::Impl(name, region, Partitionable::Impl::default_range),
	  vendor(), model(), rotational(false)
    {
	if (!is_valid_name(name))
	    ST_THROW(Exception("invalid Multipath name"));

	set_dm_table_name(name.substr(strlen(DEVMAPPERDIR "/")));
    }


    Multipath::Impl::Impl(const xmlNode* node)
	: Partitionable::Impl(node), vendor(), model(), rotational(false)
    {
	getChildValue(node, "vendor", vendor);
	getChildValue(node, "model", model);

	getChildValue(node, "rotational", rotational);
    }


    void
    Multipath::Impl::check() const
    {
	Partitionable::Impl::check();

	if (get_region().get_start() != 0)
	    cerr << "multiapth region start not zero" << endl;

	if (!is_valid_name(get_name()))
	    ST_THROW(Exception("invalid name"));
    }


    bool
    Multipath::Impl::is_valid_name(const string& name)
    {
	return boost::starts_with(name, DEVMAPPERDIR "/");
    }


    bool
    Multipath::Impl::activate_multipaths(const ActivateCallbacks* activate_callbacks)
    {
	y2mil("activate_multipaths");

	if (!CmdMultipath(true).looks_like_real_multipath())
	{
	    y2mil("does not look like real multipath");
	    return false;
	}

	if (!activate_callbacks->multipath())
	{
	    y2mil("user canceled activation of multipath");
	    return false;
	}
	else
	{
	    y2mil("user allowed activation of multipath");
	}

	string cmd_line1 = MULTIPATHBIN;
	cout << cmd_line1 << endl;

	SystemCmd cmd1(cmd_line1);
	if (cmd1.retcode() != 0)
	    ST_THROW(Exception("activate multipath failed"));

	SystemCmd(UDEVADMBIN_SETTLE);

	string cmd_line2 = MULTIPATHDBIN;
	cout << cmd_line2 << endl;

	SystemCmd cmd2(cmd_line2);
	if (cmd2.retcode() != 0)
	    ST_THROW(Exception("activate multipath failed"));

	SystemCmd(UDEVADMBIN_SETTLE);

	return true;
    }


    void
    Multipath::Impl::probe_multipaths(Prober& prober)
    {
	const CmdMultipath& cmd_multipath = prober.get_system_info().getCmdMultipath();

	for (const string& dm_name : cmd_multipath.get_entries())
	{
	    Multipath* multipath = Multipath::create(prober.get_probed(), DEVMAPPERDIR "/" + dm_name);
	    multipath->get_impl().probe_pass_1a(prober);
	}
    }


    void
    Multipath::Impl::probe_pass_1a(Prober& prober)
    {
	Partitionable::Impl::probe_pass_1a(prober);

	const File rotational_file = prober.get_system_info().getFile(SYSFSDIR + get_sysfs_path() +
								      "/queue/rotational");
	rotational = rotational_file.get_int() != 0;

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
	    BlkDevice* blk_device = BlkDevice::Impl::find_by_name(prober.get_probed(), device,
								  prober.get_system_info());
	    User::create(prober.get_probed(), blk_device, get_non_impl());
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


    void
    Multipath::Impl::process_udev_ids(vector<string>& udev_ids) const
    {
	stable_partition(udev_ids.begin(), udev_ids.end(), string_starts_with("wwn-"));
	stable_partition(udev_ids.begin(), udev_ids.end(), string_starts_with("scsi-"));
    }


    bool
    compare_by_name(const Multipath* lhs, const Multipath* rhs)
    {
	return lhs->get_name() < rhs->get_name();
    }

}
