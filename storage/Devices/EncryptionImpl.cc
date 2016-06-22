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
#include "storage/Utils/StorageTmpl.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Devices/EncryptionImpl.h"
#include "storage/Holders/User.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Encryption>::classname = "Encryption";


    const vector<string> EnumTraits<EncryptionType>::names({
	"none", "twofish256", "twofish", "twofishSL92", "luks", "unknown"
    });


    Encryption::Impl::Impl(const xmlNode* node)
	: BlkDevice::Impl(node), dm_name()
    {
	if (!getChildValue(node, "dm-name", dm_name))
	    ST_THROW(Exception("no dm-name"));
    }


    void
    Encryption::Impl::save(xmlNode* node) const
    {
	BlkDevice::Impl::save(node);

	setChildValue(node, "dm-name", dm_name);
    }


    const BlkDevice*
    Encryption::Impl::get_blk_device() const
    {
	Devicegraph::Impl::vertex_descriptor vertex = get_devicegraph()->get_impl().parent(get_vertex());

	return to_blk_device(get_devicegraph()->get_impl()[vertex]);
    }


    void
    Encryption::Impl::probe_pass_2(Devicegraph* probed, SystemInfo& systeminfo)
    {
	BlkDevice::Impl::probe_pass_2(probed, systeminfo);

	const CmdDmsetupTable& cmd_dmsetup_table = systeminfo.getCmdDmsetupTable();

	vector<CmdDmsetupTable::Table> tables;
	if (!cmd_dmsetup_table.get_tables(dm_name, tables))
	    ST_THROW(Exception("dmsetup table not found"));

	if (tables.size() != 1)
	    ST_THROW(Exception("crypt with several dm-tables"));

	const vector<dev_t>& majorminors = tables.front().majorminors;

	if (majorminors.size() != 1)
	    ST_THROW(Exception("crypt target with several devices"));

	BlkDevice* blk_device = BlkDevice::Impl::find_by_name(probed,
	    make_dev_block_name(majorminors.front()), systeminfo);

	User::create(probed, blk_device, get_device());
    }


    void
    Encryption::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Create(get_sid()));
	actions.push_back(new Action::OpenEncryption(get_sid()));

	actiongraph.add_chain(actions);
    }


    bool
    Encryption::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!BlkDevice::Impl::equal(rhs))
	    return false;

	return dm_name == rhs.dm_name;
    }


    void
    Encryption::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	BlkDevice::Impl::log_diff(log, rhs);

	storage::log_diff(log, "dm-name", dm_name, rhs.dm_name);
    }


    void
    Encryption::Impl::print(std::ostream& out) const
    {
	BlkDevice::Impl::print(out);

	out << " dm-name:" << dm_name;
    }


    Text
    Encryption::Impl::do_create_text(Tense tense) const
    {
	return sformat(_("Create encryption on %1$s"), get_displayname().c_str());
    }


    Text
    Encryption::Impl::do_open_text(Tense tense) const
    {
	return sformat(_("Open encryption on %1$s"), get_displayname().c_str());
    }


    namespace Action
    {

	Text
	OpenEncryption::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const Encryption* encryption = to_encryption(get_device_rhs(actiongraph));
	    return encryption->get_impl().do_open_text(tense);
	}

	void
	OpenEncryption::commit(const Actiongraph::Impl& actiongraph) const
	{
	    // TODO
	}

    }


    bool
    compare_by_dm_name(const Encryption* lhs, const Encryption* rhs)
    {
	return lhs->get_dm_name() < rhs->get_dm_name();
    }

}
