/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#ifndef STORAGE_GPT_IMPL_H
#define STORAGE_GPT_IMPL_H


#include "storage/Devices/Gpt.h"
#include "storage/Devices/PartitionTableImpl.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Gpt> { static const char* classname; };


    class Gpt::Impl : public PartitionTable::Impl
    {
    public:

	Impl()
	    : PartitionTable::Impl(), enlarge(false), pmbr_boot(false) {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<Gpt>::classname; }

	virtual string get_displayname() const override { return "gpt"; }

	virtual PtType get_type() const override { return PtType::GPT; }

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	virtual void probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo) override;

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs_base) const override;
	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	virtual bool is_partition_type_supported(PartitionType type) const override;
	virtual bool is_partition_id_supported(unsigned int id) const override;
	virtual bool is_partition_legacy_boot_flag_supported() const override { return true; }

	virtual unsigned int max_primary() const override;

	bool is_enlarge() const { return enlarge; }
	void set_enlarge(bool enlarge) { Impl::enlarge = enlarge; }

	bool is_pmbr_boot() const { return pmbr_boot; }
	void set_pmbr_boot(bool pmbr_boot) { Impl::pmbr_boot = pmbr_boot; }

	virtual Region get_usable_region() const override;

	virtual Text do_create_text(Tense tense) const override;
	virtual void do_create() const override;

	virtual Text do_set_pmbr_boot_text(Tense tense) const;
	virtual void do_set_pmbr_boot() const;

    private:

	bool enlarge;
	bool pmbr_boot;

    };


    namespace Action
    {

	class SetPmbrBoot : public Modify
	{
	public:

	    SetPmbrBoot(sid_t sid) : Modify(sid) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, Tense tense) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	};

    }
}

#endif
