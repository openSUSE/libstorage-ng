/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#ifndef STORAGE_IMPLICIT_PT_IMPL_H
#define STORAGE_IMPLICIT_PT_IMPL_H


#include "storage/Devices/ImplicitPt.h"
#include "storage/Devices/PartitionTableImpl.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<ImplicitPt> { static const char* classname; };


    class ImplicitPt::Impl : public PartitionTable::Impl
    {
    public:

	Impl()
	    : PartitionTable::Impl() {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<ImplicitPt>::classname; }

	virtual string get_pretty_classname() const override;

	virtual string get_displayname() const override { return "ImplicitPt"; }

	virtual PtType get_type() const override { return PtType::IMPLICIT; }

	virtual unique_ptr<Device::Impl> clone() const override { return make_unique<Impl>(*this); }

	virtual void probe_pass_1c(Prober& prober) override;

	virtual void check(const CheckCallbacks* check_callbacks) const override;

	virtual void save(xmlNode* node) const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;
	virtual void print(std::ostream& out) const override;

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual bool is_partition_type_supported(PartitionType type) const override;
	virtual bool is_partition_id_supported(unsigned int id) const override;

	virtual unsigned int max_primary() const override;

	virtual pair<unsigned long long, unsigned long long> unusable_sectors() const override;

	virtual vector<PartitionSlot> get_unused_partition_slots(AlignPolicy align_policy,
								 AlignType align_type) const override;

	void create_implicit_partition();

	virtual Text do_create_text(Tense tense) const override;

	virtual Text do_delete_text(Tense tense) const override;

    };

}

#endif
