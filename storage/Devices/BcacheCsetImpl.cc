/*
 * Copyright (c) 2016 SUSE LLC
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


#include <regex>

#include "storage/Utils/XmlFile.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Devices/BcacheCsetImpl.h"
#include "storage/Devices/Bcache.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Holders/User.h"
#include "storage/UsedFeatures.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<BcacheCset>::classname = "BcacheCset";


    BcacheCset::Impl::Impl(const xmlNode* node)
	: Device::Impl(node), uuid()
    {
	getChildValue(node, "uuid", uuid);
    }


    void
    BcacheCset::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValue(node, "uuid", uuid);
    }


    void
    BcacheCset::Impl::check() const
    {
	Device::Impl::check();

	if (!get_uuid().empty() && !is_valid_uuid(get_uuid()))
	    ST_THROW(Exception("invalid uuid"));
    }


    uint64_t
    BcacheCset::Impl::used_features() const
    {
	return UF_BCACHE | Device::Impl::used_features();
    }


    bool
    BcacheCset::Impl::is_valid_uuid(const string& uuid)
    {
	static regex uuid_regex("[a-f0-9]{8}-[a-f0-9]{4}-[a-f0-9]{4}-[a-f0-9]{4}-[a-f0-9]{12}",
				regex_constants::extended);

	return regex_match(uuid, uuid_regex);
    }


    void
    BcacheCset::Impl::probe_bcache_csets(Devicegraph* probed, SystemInfo& systeminfo)
    {
	for (const string& uuid : systeminfo.getDir(SYSFSDIR "/fs/bcache"))
	{
	    if (!is_valid_uuid(uuid))
		continue;

	    BcacheCset* bcache_cset = BcacheCset::create(probed);
	    bcache_cset->get_impl().set_uuid(uuid);
	}
    }


    void
    BcacheCset::Impl::probe_pass_2(Devicegraph* probed, SystemInfo& systeminfo)
    {
	static regex bdev_regex("bdev[0-9]+", regex_constants::extended);
	static regex cache_regex("cache[0-9]+", regex_constants::extended);

	Device::Impl::probe_pass_2(probed, systeminfo);

	string path = SYSFSDIR "/fs/bcache/" + uuid;

	const Dir& dir = systeminfo.getDir(path);
	for (const string& name : dir)
	{
	    if (regex_match(name, bdev_regex))
	    {
		const File dev_file = systeminfo.getFile(path + "/" + name + "/dev/dev");
		string dev = "/dev/block/" + dev_file.get_string();

		const BlkDevice* blk_device = BlkDevice::Impl::find_by_name(probed, dev, systeminfo);
		User::create(probed, get_device(), blk_device);
	    }

	    if (regex_match(name, cache_regex))
	    {
		const File dev_file = systeminfo.getFile(path + "/" + name + "/../dev");
		string dev = "/dev/block/" + dev_file.get_string();

		const BlkDevice* blk_device = BlkDevice::Impl::find_by_name(probed, dev, systeminfo);
		User::create(probed, blk_device, get_device());
	    }
	}
    }


    vector<const BlkDevice*>
    BcacheCset::Impl::get_blk_devices() const
    {
	const Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	return devicegraph.filter_devices_of_type<const BlkDevice>(devicegraph.parents(vertex));
    }


    vector<const Bcache*>
    BcacheCset::Impl::get_bcaches() const
    {
	const Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	return devicegraph.filter_devices_of_type<const Bcache>(devicegraph.children(vertex));
    }


    bool
    BcacheCset::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Device::Impl::equal(rhs))
	    return false;

	return uuid == rhs.uuid;
    }


    void
    BcacheCset::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Device::Impl::log_diff(log, rhs);

	storage::log_diff(log, "uuid", uuid, rhs.uuid);
    }


    void
    BcacheCset::Impl::print(std::ostream& out) const
    {
	Device::Impl::print(out);

	out << " uuid:" << uuid;
    }


    bool
    compare_by_uuid(const BcacheCset* lhs, const BcacheCset* rhs)
    {
	return lhs->get_uuid() < rhs->get_uuid();
    }

}
