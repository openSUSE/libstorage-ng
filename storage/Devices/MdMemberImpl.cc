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


#include "storage/Devices/MdMemberImpl.h"
#include "storage/Devices/MdContainerImpl.h"
#include "storage/Devices/MdImpl.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Holders/MdSubdeviceImpl.h"
#include "storage/EtcMdadm.h"
#include "storage/Prober.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/StorageDefines.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<MdMember>::classname = "MdMember";


    MdMember::Impl::Impl(const string& name)
	: Md::Impl(name)
    {
    }


    MdMember::Impl::Impl(const xmlNode* node)
	: Md::Impl(node)
    {
    }


    void
    MdMember::Impl::check() const
    {
	Md::Impl::check();

	if (get_metadata() != "")
	    ST_THROW(Exception("invalid metadata for MdMember"));

	// TODO check that container exists
    }


    MdContainer*
    MdMember::Impl::get_md_container()
    {
	vector<MdContainer*> tmp = get_parents_of_type<MdContainer>();
	if (tmp.size() != 1)
	    ST_THROW(Exception("MdMember has no MdContainer"));

	return tmp[0];
    }


    const MdContainer*
    MdMember::Impl::get_md_container() const
    {
	vector<const MdContainer*> tmp = get_parents_of_type<const MdContainer>();
	if (tmp.size() != 1)
	    ST_THROW(Exception("MdMember has no MdContainer"));

	return tmp[0];
    }


    vector<BlkDevice*>
    MdMember::Impl::get_devices()
    {
	vector<BlkDevice*> tmp = Md::Impl::get_devices();

	erase_if(tmp, [](BlkDevice* blk_device) { return is_md_container(blk_device); });

	return tmp;
    }


    vector<const BlkDevice*>
    MdMember::Impl::get_devices() const
    {
	vector<const BlkDevice*> tmp = Md::Impl::get_devices();

	erase_if(tmp, [](const BlkDevice* blk_device) { return is_md_container(blk_device); });

	return tmp;
    }


    void
    MdMember::Impl::probe_pass_1b(Prober& prober)
    {
	Md::Impl::probe_pass_1b(prober);

	const ProcMdstat::Entry& entry = prober.get_system_info().getProcMdstat().get_entry(get_sysfs_name());

	prober.add_holder(DEVDIR "/" + entry.container_name, get_non_impl(),
			  [&entry](Devicegraph* probed, Device* a, Device* b) {
	    MdSubdevice* md_subdevice = MdSubdevice::create(probed, a, b);
	    md_subdevice->set_member(entry.container_member);
	});
    }


    void
    MdMember::Impl::calculate_region_and_topology()
    {
	// Not implemented since MdMember can only be probed.
    }


    void
    MdMember::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	ST_THROW(Exception("cannot create MdMember"));
    }


    void
    MdMember::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	ST_THROW(Exception("cannot delete MdMember"));
    }


    void
    MdMember::Impl::do_add_to_etc_mdadm(CommitData& commit_data) const
    {
	const MdSubdevice* md_subdevice = get_single_in_holder_of_type<const MdSubdevice>();
	const MdContainer* md_container = to_md_container(md_subdevice->get_source());

	EtcMdadm& etc_mdadm = commit_data.get_etc_mdadm();

	etc_mdadm.init(get_storage());

	EtcMdadm::Entry entry;

	entry.device = get_name();
	entry.uuid = get_uuid();

	entry.container_uuid = md_container->get_uuid();
	entry.container_member = md_subdevice->get_member();

	etc_mdadm.update_entry(entry);
    }

}
