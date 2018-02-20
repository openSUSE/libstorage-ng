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

#include "storage/Utils/AppUtil.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Holders/User.h"
#include "storage/Devices/BcacheImpl.h"
#include "storage/Devices/BcacheCset.h"
#include "storage/Devicegraph.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/UsedFeatures.h"
#include "storage/Prober.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Bcache>::classname = "Bcache";


    Bcache::Impl::Impl(const xmlNode* node)
	: BlkDevice::Impl(node)
    {
    }


    string
    Bcache::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("Bcache").translated;
    }


    string
    Bcache::Impl::get_sort_key() const
    {
	static const vector<NameSchema> name_schemata = {
	    NameSchema(regex(DEVDIR "/bcache([0-9]+)", regex::extended), 3, '0'),
	};

	return format_to_name_schemata(get_name(), name_schemata);
    }


    void
    Bcache::Impl::save(xmlNode* node) const
    {
	BlkDevice::Impl::save(node);
    }


    bool
    Bcache::Impl::is_valid_name(const string& name)
    {
	static regex name_regex(DEVDIR "/bcache[0-9]+", regex::extended);

	return regex_match(name, name_regex);
    }


    void
    Bcache::Impl::probe_bcaches(Prober& prober)
    {
	for (const string& short_name : prober.get_system_info().getDir(SYSFSDIR "/block"))
	{
	    string name = DEVDIR "/" + short_name;
	    if (!is_valid_name(name))
		continue;

	    Bcache* bcache = Bcache::create(prober.get_system(), name);
	    bcache->get_impl().probe_pass_1a(prober);
	}
    }


    void
    Bcache::Impl::probe_pass_1a(Prober& prober)
    {
	BlkDevice::Impl::probe_pass_1a(prober);

	const File size_file = prober.get_system_info().getFile(SYSFSDIR + get_sysfs_path() + "/size");

	set_region(Region(0, size_file.get<unsigned long long>(), 512));
    }


    void
    Bcache::Impl::probe_pass_1b(Prober& prober)
    {
	BlkDevice::Impl::probe_pass_1b(prober);

	const File dev_file = prober.get_system_info().getFile(SYSFSDIR + get_sysfs_path() + "/bcache/../dev");
	string dev = DEVDIR "/block/" + dev_file.get<string>();

	prober.add_holder(dev, get_non_impl(), [](Devicegraph* system, Device* a, Device* b) {
	    User::create(system, a, b);
	});
    }


    uint64_t
    Bcache::Impl::used_features() const
    {
	return UF_BCACHE | BlkDevice::Impl::used_features();
    }


    unsigned int
    Bcache::Impl::get_number() const
    {
	return device_to_name_and_number(get_name()).second;
    }


    const BlkDevice*
    Bcache::Impl::get_blk_device() const
    {
	// TODO, write some generic helper

	const Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	vector<const BlkDevice*> ret = devicegraph.filter_devices_of_type<const BlkDevice>(devicegraph.parents(vertex));

	return ret.front();
    }


    bool
    Bcache::Impl::has_bcache_cset() const
    {
	// TODO, write some generic helper

	const Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	vector<const BcacheCset*> ret = devicegraph.filter_devices_of_type<const BcacheCset>(devicegraph.parents(vertex));

	return !ret.empty();
    }


    const BcacheCset*
    Bcache::Impl::get_bcache_cset() const
    {
	// TODO, write some generic helper

	const Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	vector<const BcacheCset*> ret = devicegraph.filter_devices_of_type<const BcacheCset>(devicegraph.parents(vertex));

	return ret.front();
    }


    bool
    Bcache::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	return BlkDevice::Impl::equal(rhs);
    }


    void
    Bcache::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	BlkDevice::Impl::log_diff(log, rhs);
    }


    void
    Bcache::Impl::print(std::ostream& out) const
    {
	BlkDevice::Impl::print(out);
    }

}
