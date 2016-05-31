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


#include "storage/Devices/LvmVgImpl.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<LvmVg>::classname = "LvmVg";


    LvmVg::Impl::Impl(const xmlNode* node)
	: Device::Impl(node), uuid()
    {
	if (!getChildValue(node, "name", name))
	    throw runtime_error("no name");

	getChildValue(node, "uuid", uuid);
    }


    void
    LvmVg::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValue(node, "name", name);
	setChildValue(node, "uuid", uuid);
    }


    bool
    LvmVg::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Device::Impl::equal(rhs))
	    return false;

	return name == rhs.name && uuid == rhs.uuid;
    }


    void
    LvmVg::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Device::Impl::log_diff(log, rhs);

	storage::log_diff(log, "name", name, rhs.name);
	storage::log_diff(log, "uuid", uuid, rhs.uuid);
    }


    void
    LvmVg::Impl::print(std::ostream& out) const
    {
	Device::Impl::print(out);
    }


    Text
    LvmVg::Impl::do_create_text(Tense tense) const
    {
	return sformat(_("Create volume group %1$s"), get_displayname().c_str());
    }

}
