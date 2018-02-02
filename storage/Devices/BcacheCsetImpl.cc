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
#include "storage/FindBy.h"
#include "storage/Prober.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<BcacheCset>::classname = "BcacheCset";


    BcacheCset::Impl::Impl(const xmlNode* node)
	: Device::Impl(node), uuid()
    {
	getChildValue(node, "uuid", uuid);
    }


    string
    BcacheCset::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("Bcache Cache Set").translated;
    }


    void
    BcacheCset::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValue(node, "uuid", uuid);
    }


    void
    BcacheCset::Impl::check(const CheckCallbacks* check_callbacks) const
    {
	Device::Impl::check(check_callbacks);

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
				regex::extended);

	return regex_match(uuid, uuid_regex);
    }


    void
    BcacheCset::Impl::probe_bcache_csets(Prober& prober)
    {
	for (const string& uuid : prober.get_system_info().getDir(SYSFSDIR "/fs/bcache"))
	{
	    if (!is_valid_uuid(uuid))
		continue;

	    BcacheCset* bcache_cset = BcacheCset::create(prober.get_probed());
	    bcache_cset->get_impl().set_uuid(uuid);
	}
    }


    void
    BcacheCset::Impl::probe_pass_1b(Prober& prober)
    {
	static regex bdev_regex("bdev[0-9]+", regex::extended);
	static regex cache_regex("cache[0-9]+", regex::extended);

	Device::Impl::probe_pass_1b(prober);

	string path = SYSFSDIR "/fs/bcache/" + uuid;

	const Dir& dir = prober.get_system_info().getDir(path);
	for (const string& name : dir)
	{
	    if (regex_match(name, bdev_regex))
	    {
		const File dev_file = prober.get_system_info().getFile(path + "/" + name + "/dev/dev");
		string dev = DEVDIR "/block/" + dev_file.get<string>();

		prober.add_holder(dev, get_non_impl(), [](Devicegraph* probed, Device* a, Device* b) {
		    User::create(probed, b, a);
		});
	    }

	    if (regex_match(name, cache_regex))
	    {
		const File dev_file = prober.get_system_info().getFile(path + "/" + name + "/../dev");
		string dev = DEVDIR "/block/" + dev_file.get<string>();

		prober.add_holder(dev, get_non_impl(), [](Devicegraph* probed, Device* a, Device* b) {
		    User::create(probed, a, b);
		});
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


    BcacheCset*
    BcacheCset::Impl::find_by_uuid(Devicegraph* devicegraph, const string& uuid)
    {
	return storage::find_by_uuid<BcacheCset>(devicegraph, uuid);
    }


    const BcacheCset*
    BcacheCset::Impl::find_by_uuid(const Devicegraph* devicegraph, const string& uuid)
    {
	return storage::find_by_uuid<const BcacheCset>(devicegraph, uuid);
    }

}
