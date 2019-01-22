/*
 * Copyright (c) [2016-2019] SUSE LLC
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

#include "storage/Devices/BcacheImpl.h"
#include "storage/Devices/BackedBcache.h"
#include "storage/Devices/FlashBcache.h"
#include "storage/Utils/Algorithm.h"
#include "storage/Utils/AppUtil.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/Format.h"
#include "storage/Utils/CallbacksImpl.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/UsedFeatures.h"
#include "storage/Storage.h"


namespace storage
{

    using namespace std;


    // TODO handle attach and detach

    const char* DeviceTraits<Bcache>::classname = "Bcache";


    Bcache::Impl::Impl(const string& name)
	: Partitionable::Impl(name)
    {
	update_sysfs_name_and_path();
    }


    Bcache::Impl::Impl(const xmlNode* node)
	: Partitionable::Impl(node)
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
	    NameSchema(regex(DEV_DIR "/bcache([0-9]+)", regex::extended), { { 3, '0' } }),
	};

	return format_to_name_schemata(get_name(), name_schemata);
    }


    void
    Bcache::Impl::reassign_numbers(Devicegraph* devicegraph)
    {
	// not many bcache devices expected (<10), so lets just do tail-recursive
	// algorithm. It finds free name and ensure that all devices
	// with higher number are not in memory. And if there is a memory one,
	// then rename it to free name and run again. It always terminate as each step remove one
	// in-memory bcache that has hole before till all in-memory caches occupy the lowest
	// available numbers.
	const string free_name = find_free_name(devicegraph);
	unsigned free_number = device_to_name_and_number(free_name).second;

	vector<Bcache*> bcaches = get_all(devicegraph);

	sort(bcaches.begin(), bcaches.end(), compare_by_number);

	for(Bcache* bcache: bcaches)
	{
	    if (bcache->get_number() > free_number && !bcache->exists_in_probed())
	    {
		bcache->get_impl().set_number(free_number);
		reassign_numbers(devicegraph);
		return;
	    }
	}
    }


    string
    Bcache::Impl::find_free_name(const Devicegraph* devicegraph)
    {
	vector<const Bcache*> bcaches = get_all(devicegraph);

	sort(bcaches.begin(), bcaches.end(), compare_by_number);

	unsigned int free_number = first_missing_number(bcaches, 0);

	return DEV_DIR "/bcache" + to_string(free_number);
    }


    ResizeInfo
    Bcache::Impl::detect_resize_info() const
    {
	return ResizeInfo(false, RB_RESIZE_NOT_SUPPORTED_BY_DEVICE);
    }


    bool
    Bcache::Impl::is_valid_name(const string& name)
    {
	static regex name_regex(DEV_DIR "/bcache[0-9]+", regex::extended);

	return regex_match(name, name_regex);
    }


    static bool
    is_backed(Prober& prober, const string& short_name)
    {
	string dev_path = SYSFS_DIR "/devices/virtual/block/" + short_name + "/dev";
	File dev_file = prober.get_system_info().getFile(dev_path);
	string majorminor = dev_file.get<string>();

	string backing_dev_path = SYSFS_DIR "/devices/virtual/block/" + short_name + "/bcache/../dev";
	File backing_dev_file = prober.get_system_info().getFile(backing_dev_path);
	string backing_majorminor = backing_dev_file.get<string>();

	return majorminor != backing_majorminor;
    }


    void
    Bcache::Impl::probe_bcaches(Prober& prober)
    {
	for (const string& short_name : prober.get_sys_block_entries().bcaches)
	{
	    string name = DEV_DIR "/" + short_name;

	    try
	    {
		Bcache* bcache;

		if(is_backed(prober, short_name))
		    bcache = BackedBcache::create(prober.get_system(), name);
		else
		    bcache = FlashBcache::create(prober.get_system(), name);

		bcache->get_impl().probe_pass_1a(prober);
	    }
	    catch (const Exception& exception)
	    {
		// TRANSLATORS: error message
		error_callback(prober.get_probe_callbacks(), sformat(_("Probing bcache %s failed"),
								     name), exception);
	    }
	}
    }


    void
    Bcache::Impl::probe_pass_1a(Prober& prober)
    {
	Partitionable::Impl::probe_pass_1a(prober);

	SystemInfo& system_info = prober.get_system_info();

	const File size_file = system_info.getFile(SYSFS_DIR + get_sysfs_path() + "/size");

	set_region(Region(0, size_file.get<unsigned long long>(), 512));
    }


    uint64_t
    Bcache::Impl::used_features() const
    {
	return UF_BCACHE | Partitionable::Impl::used_features();
    }


    unsigned int
    Bcache::Impl::get_number() const
    {
	return device_to_name_and_number(get_name()).second;
    }


    void
    Bcache::Impl::set_number(unsigned int number)
    {
	std::pair<string, unsigned int> pair = device_to_name_and_number(get_name());

	set_name(name_and_number_to_device(pair.first, number));

	update_sysfs_name_and_path();
    }


    const BcacheCset*
    Bcache::Impl::get_bcache_cset() const
    {
	const Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	vector<const BcacheCset*> ret = devicegraph.filter_devices_of_type<const BcacheCset>(devicegraph.parents(vertex));

	return ret.front();
    }


    void
    Bcache::Impl::update_sysfs_name_and_path()
    {
	set_sysfs_name(get_name().substr(strlen(DEV_DIR "/")));
	set_sysfs_path("/devices/virtual/block/" + get_sysfs_name());
    }


    void
    Bcache::Impl::run_dependency_manager(Actiongraph::Impl& actiongraph)
    {
	struct AllActions
	{
	    vector<Actiongraph::Impl::vertex_descriptor> delete_actions;
	    vector<Actiongraph::Impl::vertex_descriptor> create_actions;
	};

	// Build vectors with all actions.

	AllActions all_actions;

	for (Actiongraph::Impl::vertex_descriptor vertex : actiongraph.vertices())
	{
	    const Action::Base* action = actiongraph[vertex];

	    const Action::Create* create_action = dynamic_cast<const Action::Create*>(action);
	    if (create_action)
	    {
		const Device* device = create_action->get_device(actiongraph);
		if (is_bcache(device))
		    all_actions.create_actions.push_back(vertex);
	    }

	    const Action::Delete* delete_action = dynamic_cast<const Action::Delete*>(action);
	    if (delete_action)
	    {
		const Device* device = delete_action->get_device(actiongraph);
		if (is_bcache(device))
		    all_actions.delete_actions.push_back(vertex);
	    }
	}

	// Some functions used for sorting actions by bcache number.

	const Devicegraph* devicegraph_rhs = actiongraph.get_devicegraph(RHS);
	const Devicegraph* devicegraph_lhs = actiongraph.get_devicegraph(LHS);

	std::function<unsigned int(Actiongraph::Impl::vertex_descriptor)> key_fnc1 =
	    [&actiongraph, &devicegraph_lhs](Actiongraph::Impl::vertex_descriptor vertex) {
	    const Action::Base* action = actiongraph[vertex];
	    const Bcache* bcache = to_bcache(devicegraph_lhs->find_device(action->sid));
	    return bcache->get_number();
	};

	std::function<unsigned int(Actiongraph::Impl::vertex_descriptor)> key_fnc2 =
	    [&actiongraph, &devicegraph_rhs](Actiongraph::Impl::vertex_descriptor vertex) {
	    const Action::Base* action = actiongraph[vertex];
	    const Bcache* bcache = to_bcache(devicegraph_rhs->find_device(action->sid));
	    return bcache->get_number();
	};

	// Iterate over all bcaches with actions and build chain of
	// dependencies.

	vector<Actiongraph::Impl::vertex_descriptor> actions;

	all_actions.delete_actions = sort_by_key(all_actions.delete_actions, key_fnc1);
	actions.insert(actions.end(), all_actions.delete_actions.rbegin(),
		       all_actions.delete_actions.rend());

	all_actions.create_actions = sort_by_key(all_actions.create_actions, key_fnc2);
	actions.insert(actions.end(), all_actions.create_actions.begin(),
		       all_actions.create_actions.end());

	actiongraph.add_chain(actions);
    }

}
