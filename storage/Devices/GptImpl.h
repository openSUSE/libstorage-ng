/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2023] SUSE LLC
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
	    : PartitionTable::Impl(), partition_slots(default_partition_slots), undersized(false),
	      backup_broken(false), pmbr_boot(false) {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<Gpt>::classname; }

	virtual string get_pretty_classname() const override;

	virtual string get_displayname() const override { return "gpt"; }

	virtual PtType get_type() const override { return PtType::GPT; }

	virtual unique_ptr<Device::Impl> clone() const override { return make_unique<Impl>(*this); }

	virtual void save(xmlNode* node) const override;

	virtual void probe_pass_1c(Prober& prober) override;

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs_base) const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;
	virtual void print(std::ostream& out) const override;

	virtual bool is_partition_type_supported(PartitionType type) const override;
	virtual bool is_partition_id_supported(unsigned int id) const override;
	virtual bool is_partition_legacy_boot_flag_supported() const override { return true; }
	virtual bool is_partition_no_automount_flag_supported() const override;

	virtual unsigned int max_primary() const override;

	bool is_undersized() const { return undersized; }
	void set_undersized(bool undersized) { Impl::undersized = undersized; }

	bool is_backup_broken() const { return backup_broken; }

	bool is_pmbr_boot() const { return pmbr_boot; }
	void set_pmbr_boot(bool pmbr_boot) { Impl::pmbr_boot = pmbr_boot; }

	virtual pair<unsigned long long, unsigned long long> unusable_sectors() const override;

	virtual Text do_create_text(Tense tense) const override;
	virtual void do_create() override;

	virtual Text do_repair_text(Tense tense) const;
	virtual void do_repair() const;

	virtual Text do_set_pmbr_boot_text(Tense tense) const;
	virtual void do_set_pmbr_boot() const;

	virtual Text do_delete_text(Tense tense) const override;

    private:

	/**
	 * Number of partitions slots if parted does not report anything else and
	 * for new GPTs.
	 */
	static const unsigned int default_partition_slots = 128;

	/**
	 * Number of partitions slots.
	 */
	unsigned int partition_slots;

	/**
	 * The GPT does not use the complete device.
	 */
	bool undersized;

	/**
	 * The backup GPT is broken.
	 */
	bool backup_broken;

	bool pmbr_boot;

    };

}

#endif
