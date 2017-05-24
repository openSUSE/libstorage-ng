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
 * with this program; if not, contact SUSE LLC.
 *
 * To contact SUSE LLC about this file by physical or electronic mail, you may
 * find current contact information at www.suse.com.
 */


#include <vector>

#include <boost/algorithm/string/join.hpp>

#include "storage/CompoundAction/Formatter/LvmVg.h"
#include "storage/Devices/LvmPv.h"


namespace storage
{

    using std::vector;
    using std::string;


    CompoundAction::Formatter::LvmVg::LvmVg(const CompoundAction::Impl* compound_action) :
	CompoundAction::Formatter(compound_action),
	vg(to_lvm_vg(compound_action->get_target_device()))
    {}


    string
    CompoundAction::Formatter::LvmVg::name_of_devices() const
    {
	auto pvs = vg->get_lvm_pvs();

	vector<string> names;
	for (auto pv : pvs)
	    names.push_back(pv->get_blk_device()->get_displayname());

	return boost::algorithm::join(names, ", ");
    }


    Text
    CompoundAction::Formatter::LvmVg::text() const
    {
	if (has_create<storage::LvmVg>())
	{
	    if (vg->get_lvm_pvs().size() > 0)
		return create_with_pvs_text();
	    
	    else
		return create_text();
	}

	else
	    return default_text();
    }


    Text
    CompoundAction::Formatter::LvmVg::create_with_pvs_text() const
    {
	// TRANSLATORS: displayed before action,
	// %1$s is replaced by volume group name (e.g. system),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by name of devices (e.g. sda1, sda2)
	Text text = _("Create volume group %1$s (%2$s) with %3$s");

	return sformat(text,
		       vg->get_vg_name().c_str(),
		       vg->get_size_string().c_str(),
		       name_of_devices().c_str());
    }


    Text
    CompoundAction::Formatter::LvmVg::create_text() const
    {
	// TRANSLATORS: displayed before action,
	// %1$s is replaced by volume group name (e.g. system),
	// %2$s is replaced by size (e.g. 2GiB),
	Text text = _("Create volume group %1$s (%2$s)");

	return sformat(text,
		       vg->get_vg_name().c_str(),
		       vg->get_size_string().c_str());
    }

}

