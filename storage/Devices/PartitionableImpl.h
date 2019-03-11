/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2019] SUSE LLC
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


#ifndef STORAGE_PARTITIONABLE_IMPL_H
#define STORAGE_PARTITIONABLE_IMPL_H


#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Devices/Partitionable.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Partitionable> { static const char* classname; };


    // abstract class

    class Partitionable::Impl : public BlkDevice::Impl
    {
    public:

	void check(const CheckCallbacks* check_callbacks) const override;

	unsigned int get_range() const { return range; }
	void set_range(unsigned int range) { Impl::range = range; }

	virtual bool is_usable_as_partitionable() const { return true; }

	PtType get_default_partition_table_type() const;

	virtual vector<PtType> get_possible_partition_table_types() const;

	/**
	 * Check if some other storage stuff (e.g. filesystem, LVM
	 * logical volume) is found on the partitionable (next to a
	 * partitiontable).
	 */
	void check_unambiguity(Prober& prober) const;

	virtual void probe_pass_1a(Prober& prober) override;
	virtual void probe_pass_1c(Prober& prober) override;

	PartitionTable* create_partition_table(PtType pt_type);

	bool has_partition_table() const;

	PartitionTable* get_partition_table();
	const PartitionTable* get_partition_table() const;

	virtual string partition_name(int number) const;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	static const unsigned int default_range = 256;

    protected:

	Impl(const string& name, unsigned int range = default_range)
	    : BlkDevice::Impl(name), range(range) {}

	Impl(const string& name, const Region& region, unsigned int range = default_range)
	    : BlkDevice::Impl(name, region), range(range) {}

	Impl(const xmlNode* node);

	void save(xmlNode* node) const override;

    private:

	unsigned int range;

    };

}

#endif
