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


#ifndef STORAGE_MD_MEMBER_IMPL_H
#define STORAGE_MD_MEMBER_IMPL_H


#include "storage/Devices/MdMember.h"
#include "storage/Devices/MdImpl.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<MdMember> { static const char* classname; };


    class MdMember::Impl : public Md::Impl
    {
    public:

	Impl(const string& name);
	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<MdMember>::classname; }

	virtual unique_ptr<Device::Impl> clone() const override { return make_unique<Impl>(*this); }

	virtual void check(const CheckCallbacks* check_callbacks) const override;

	virtual RemoveInfo detect_remove_info() const override { return RemoveInfo(false, RMB_HARDWARE); }

	MdContainer* get_md_container();
	const MdContainer* get_md_container() const;

	virtual vector<BlkDevice*> get_blk_devices() override;
	virtual vector<const BlkDevice*> get_blk_devices() const override;

	virtual void probe_pass_1b(Prober& prober) override;

	virtual void calculate_region_and_topology() override;

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual void do_add_to_etc_mdadm(CommitData& commit_data) const override;

	virtual Text do_deactivate_text(Tense tense) const override;
	virtual void do_deactivate() override;

    };

}

#endif
