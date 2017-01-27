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


#ifndef STORAGE_PARTITION_IMPL_H
#define STORAGE_PARTITION_IMPL_H


#include "storage/Utils/Region.h"
#include "storage/Utils/Enum.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    class Partitionable;


    template <> struct DeviceTraits<Partition> { static const char* classname; };

    template <> struct EnumTraits<PartitionType> { static const vector<string> names; };


    class Partition::Impl : public BlkDevice::Impl
    {
    public:

	Impl(const string& name, const Region& region, PartitionType type);

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<Partition>::classname; }

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo) override;

	virtual void save(xmlNode* node) const override;

	virtual void check() const override;

	unsigned int get_number() const;
	void set_number(unsigned int number);

	virtual void set_region(const Region& region) override;

	PartitionType get_type() const { return type; }
	void set_type(PartitionType type);

	unsigned int get_id() const { return id; }
	void set_id(unsigned int id);

	bool is_boot() const { return boot; }
	void set_boot(bool boot);

	bool is_legacy_boot() const { return legacy_boot; }
	void set_legacy_boot(bool legacy_boot);

	void update_sysfs_name_and_path();
	void update_udev_paths_and_ids();

	virtual ResizeInfo detect_resize_info() const override;

	PartitionTable* get_partition_table();
	const PartitionTable* get_partition_table() const;

	const Partitionable* get_partitionable() const;

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs) const override;
	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	virtual void process_udev_paths(vector<string>& udev_paths) const override;
	virtual void process_udev_ids(vector<string>& udev_ids) const override;

	virtual Text do_create_text(Tense tense) const override;
	virtual void do_create() const override;

	virtual Text do_set_id_text(Tense tense) const;
	virtual void do_set_id() const;

	virtual Text do_set_boot_text(Tense tense) const;
	virtual void do_set_boot() const;

	virtual Text do_set_legacy_boot_text(Tense tense) const;
	virtual void do_set_legacy_boot() const;

	virtual Text do_delete_text(Tense tense) const override;
	virtual void do_delete() const override;

	virtual Text do_resize_text(ResizeMode resize_mode, const Device* lhs, Tense tense) const override;
	virtual void do_resize(ResizeMode resize_mode) const override;

	static unsigned int default_id_for_type(PartitionType type);

    private:

	PartitionType type;
	unsigned int id;
	bool boot;
	bool legacy_boot;

    };


    namespace Action
    {

	class SetPartitionId : public Modify
	{
	public:

	    SetPartitionId(sid_t sid) : Modify(sid) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, Tense tense) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	};


	class SetBoot : public Modify
	{
	public:

	    SetBoot(sid_t sid) : Modify(sid) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, Tense tense) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	};


	class SetLegacyBoot : public Modify
	{
	public:

	    SetLegacyBoot(sid_t sid) : Modify(sid) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, Tense tense) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	};

    }


    string id_to_string(unsigned int id);

    bool compare_by_number(const Partition* lhs, const Partition* rhs);

}

#endif
