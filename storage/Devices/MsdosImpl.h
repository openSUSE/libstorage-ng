/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2017] SUSE LLC
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


#ifndef STORAGE_MSDOS_IMPL_H
#define STORAGE_MSDOS_IMPL_H


#include "storage/Utils/HumanString.h"
#include "storage/Devices/Msdos.h"
#include "storage/Devices/PartitionTableImpl.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Msdos> { static const char* classname; };


    class Msdos::Impl : public PartitionTable::Impl
    {
    public:

	Impl()
	    : PartitionTable::Impl(), minimal_mbr_gap(default_minimal_mbr_gap) {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<Msdos>::classname; }

	virtual string get_displayname() const override { return "msdos"; }

	virtual void delete_partition(Partition* partition) override;

	virtual PtType get_type() const override { return PtType::MSDOS; }

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	virtual bool is_partition_type_supported(PartitionType type) const override { return true; }
	virtual bool is_partition_id_supported(unsigned int id) const override;
	virtual bool are_partition_id_values_standardized() const override { return true; }
	virtual bool is_partition_boot_flag_supported() const override { return true; }

	virtual unsigned int max_primary() const override;
	virtual bool extended_possible() const override { return true; }
	virtual unsigned int max_logical() const override;

	virtual bool has_extended() const override;
	virtual unsigned int num_logical() const override;

	virtual const Partition* get_extended() const override;

	virtual vector<Partition*> get_partitions() override;
	virtual vector<const Partition*> get_partitions() const override;

	/**
	 * For an extended partition return logical partitions.
	 */
	vector<Partition*> get_logical_partitions(Partition* partition);

	/**
	 * @copydoc get_logical_partitions()
	 */
	vector<const Partition*> get_logical_partitions(const Partition* partition) const;

	unsigned long get_minimal_mbr_gap() const { return minimal_mbr_gap; }
	void set_minimal_mbr_gap(unsigned long minimal_mbr_gap);

	virtual Region get_usable_region() const override;

	virtual Text do_create_text(Tense tense) const override;
	virtual void do_create() const override;

    private:

	static const unsigned long default_minimal_mbr_gap = 1 * MiB;

	unsigned long minimal_mbr_gap;

    };

}

#endif
