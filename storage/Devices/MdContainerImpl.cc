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


#include "storage/Devices/MdContainerImpl.h"
#include "storage/Devices/MdMember.h"
#include "storage/EtcMdadm.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<MdContainer>::classname = "MdContainer";


    MdContainer::Impl::Impl(const string& name)
	: Md::Impl(name)
    {
	set_md_level(MdLevel::CONTAINER);
    }


    MdContainer::Impl::Impl(const xmlNode* node)
	: Md::Impl(node)
    {
    }


    void
    MdContainer::Impl::check() const
    {
	Md::Impl::check();

	if (get_md_level() != MdLevel::CONTAINER)
	    ST_THROW(Exception("invalid md-level for MdContainer"));

	if (get_metadata() != "" && get_metadata() != "imsm" && get_metadata() != "ddf")
	    ST_THROW(Exception("invalid metadata for MdContainer"));
    }


    vector<MdMember*>
    MdContainer::Impl::get_md_members()
    {
	Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	return devicegraph.filter_devices_of_type<MdMember>(devicegraph.children(vertex),
							    compare_by_name_and_number);
    }


    vector<const MdMember*>
    MdContainer::Impl::get_md_members() const
    {
	const Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	return devicegraph.filter_devices_of_type<MdMember>(devicegraph.children(vertex),
							    compare_by_name_and_number);
    }


    void
    MdContainer::Impl::calculate_region_and_topology()
    {
	// Not implemented since MdContainer can only be probed. Also the size
	// is zero in /sys.
    }


    void
    MdContainer::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	ST_THROW(Exception("cannot create MdContainer"));
    }


    void
    MdContainer::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	ST_THROW(Exception("cannot delete MdContainer"));
    }


    void
    MdContainer::Impl::do_add_to_etc_mdadm(CommitData& commit_data) const
    {
	EtcMdadm& etc_mdadm = commit_data.get_etc_mdadm();

	etc_mdadm.init(get_storage());

	EtcMdadm::Entry entry;

	entry.uuid = get_uuid();
	entry.metadata = get_metadata();

	etc_mdadm.update_entry(entry);
    }


    Text
    MdContainer::Impl::do_deactivate_text(Tense tense) const
    {
	return Device::Impl::do_deactivate_text(tense);
    }


    void
    MdContainer::Impl::do_deactivate() const
    {
	Device::Impl::do_deactivate();
    }

}
