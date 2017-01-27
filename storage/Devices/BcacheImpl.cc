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


#include <boost/regex.hpp>

#include "storage/Utils/AppUtil.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Holders/User.h"
#include "storage/Devices/BcacheImpl.h"
#include "storage/Devices/BcacheCset.h"
#include "storage/Devicegraph.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/UsedFeatures.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Bcache>::classname = "Bcache";


    Bcache::Impl::Impl(const xmlNode* node)
	: BlkDevice::Impl(node)
    {
    }


    void
    Bcache::Impl::save(xmlNode* node) const
    {
	BlkDevice::Impl::save(node);
    }


    bool
    Bcache::Impl::is_valid_name(const string& name)
    {
	static boost::regex name_regex(DEVDIR "/bcache[0-9]+", boost::regex_constants::extended);

	return boost::regex_match(name, name_regex);
    }


    void
    Bcache::Impl::probe_bcaches(Devicegraph* probed, SystemInfo& systeminfo)
    {
	for (const string& short_name : systeminfo.getDir(SYSFSDIR "/block"))
	{
	    string name = DEVDIR "/" + short_name;
	    if (!is_valid_name(name))
		continue;

	    Bcache* bcache = Bcache::create(probed, name);
	    bcache->get_impl().probe_pass_1(probed, systeminfo);
	}
    }


    void
    Bcache::Impl::probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo)
    {
	BlkDevice::Impl::probe_pass_1(probed, systeminfo);

	const File size_file = systeminfo.getFile(SYSFSDIR + get_sysfs_path() + "/size");

	set_region(Region(0, size_file.get_int(), 512));
    }


    void
    Bcache::Impl::probe_pass_2(Devicegraph* probed, SystemInfo& systeminfo)
    {
	BlkDevice::Impl::probe_pass_2(probed, systeminfo);

	const File dev_file = systeminfo.getFile(SYSFSDIR + get_sysfs_path() + "/bcache/../dev");
	string dev = "/dev/block/" + dev_file.get_string();

	const BlkDevice* blk_device = BlkDevice::Impl::find_by_name(probed, dev, systeminfo);
	User::create(probed, blk_device, get_device());
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


    bool
    compare_by_number(const Bcache* lhs, const Bcache* rhs)
    {
	return lhs->get_number() < rhs->get_number();
    }

}
