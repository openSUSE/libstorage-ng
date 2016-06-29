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


#include "storage/Utils/XmlFile.h"
#include "storage/Devices/LuksImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/SystemInfo/SystemInfo.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Luks>::classname = "Luks";


    Luks::Impl::Impl(const xmlNode* node)
	: Encryption::Impl(node), uuid()
    {
	getChildValue(node, "uuid", uuid);
    }


    void
    Luks::Impl::save(xmlNode* node) const
    {
	Encryption::Impl::save(node);

	setChildValue(node, "uuid", uuid);
    }


    void
    Luks::Impl::probe_lukses(Devicegraph* probed, SystemInfo& systeminfo)
    {
	for (const CmdDmsetupInfo::value_type& value : systeminfo.getCmdDmsetupInfo())
	{
	    if (value.second.subsystem != "CRYPT")
		continue;

	    const CmdCryptsetup& cmd_cryptsetup = systeminfo.getCmdCryptsetup(value.first);
	    if (cmd_cryptsetup.encryption_type != EncryptionType::LUKS)
		continue;

	    Luks* luks = Luks::create(probed, value.first);
	    luks->get_impl().probe_pass_1(probed, systeminfo);
	}
    }


    void
    Luks::Impl::probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo)
    {
	Encryption::Impl::probe_pass_1(probed, systeminfo);

	const File size_file = systeminfo.getFile(SYSFSDIR + get_sysfs_path() + "/size");

	set_region(Region(0, size_file.get_int(), 512));
    }


    void
    Luks::Impl::probe_pass_2(Devicegraph* probed, SystemInfo& systeminfo)
    {
	Encryption::Impl::probe_pass_2(probed, systeminfo);

	const BlkDevice* blk_device = get_blk_device();

	const Blkid& blkid = systeminfo.getBlkid();
	Blkid::Entry entry2;
	if (!blkid.find_by_name(blk_device->get_name(), entry2, systeminfo))
	    ST_THROW(Exception("failed to probe luks uuid"));

	uuid = entry2.luks_uuid;
    }


    void
    Luks::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Create(get_sid()));
	actions.push_back(new Action::OpenLuks(get_sid()));

	actiongraph.add_chain(actions);
    }


    bool
    Luks::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Encryption::Impl::equal(rhs))
	    return false;

	return uuid == rhs.uuid;
    }


    void
    Luks::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Encryption::Impl::log_diff(log, rhs);

	storage::log_diff(log, "uuid", uuid, rhs.uuid);
    }


    void
    Luks::Impl::print(std::ostream& out) const
    {
	Encryption::Impl::print(out);

	out << " uuid:" << uuid;
    }


    Text
    Luks::Impl::do_create_text(Tense tense) const
    {
	return sformat(_("Create LUKS on %1$s"), get_displayname().c_str());
    }


    Text
    Luks::Impl::do_open_text(Tense tense) const
    {
	return sformat(_("Open LUKS on %1$s"), get_displayname().c_str());
    }


    namespace Action
    {

	Text
	OpenLuks::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const Luks* luks = to_luks(get_device_rhs(actiongraph));
	    return luks->get_impl().do_open_text(tense);
	}

	void
	OpenLuks::commit(const Actiongraph::Impl& actiongraph) const
	{
	    // TODO
	}

    }

}
