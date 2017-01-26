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


#ifndef STORAGE_MD_IMPL_H
#define STORAGE_MD_IMPL_H


#include "storage/Devices/Md.h"
#include "storage/Devices/PartitionableImpl.h"
#include "storage/Utils/Enum.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Md> { static const char* classname; };

    template <> struct EnumTraits<MdLevel> { static const vector<string> names; };
    template <> struct EnumTraits<MdParity> { static const vector<string> names; };


    class Md::Impl : public Partitionable::Impl
    {
    public:

	Impl(const string& name);
	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<Md>::classname; }

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	virtual void check() const override;

	MdUser* add_device(BlkDevice* blk_device);
	void remove_device(BlkDevice* blk_device);

	vector<BlkDevice*> get_devices();
	vector<const BlkDevice*> get_devices() const;

	virtual void parent_has_new_region(const Device* parent) override;

	unsigned int get_number() const;

	MdLevel get_md_level() const { return md_level; }
	void set_md_level(MdLevel md_level);

	MdParity get_md_parity() const { return md_parity; }
	void set_md_parity(MdParity md_parity) { Impl::md_parity = md_parity; }

	unsigned long get_chunk_size() const { return chunk_size; }
	void set_chunk_size(unsigned long chunk_size);

	unsigned long get_default_chunk_size() const;

	static bool is_valid_name(const string& name);

	static void probe_mds(Devicegraph* probed, SystemInfo& systeminfo);
	virtual void probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo) override;
	virtual void probe_pass_2(Devicegraph* probed, SystemInfo& systeminfo) override;

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	virtual void process_udev_ids(vector<string>& udev_ids) const override;

	virtual uint64_t used_features() const override;

	virtual Text do_create_text(Tense tense) const override;
	virtual void do_create() const override;

	virtual Text do_delete_text(Tense tense) const override;
	virtual void do_delete() const override;

	virtual Text do_add_to_etc_mdadm_text(Tense tense) const;
	virtual void do_add_to_etc_mdadm(const Actiongraph::Impl& actiongraph) const;

	virtual Text do_remove_from_etc_mdadm_text(Tense tense) const;
	virtual void do_remove_from_etc_mdadm(const Actiongraph::Impl& actiongraph) const;

	virtual Text do_reallot_text(ReallotMode reallot_mode, const Device* device,
				     Tense tense) const override;
	virtual void do_reallot(ReallotMode reallot_mode, const Device* device)
	    const override;
	virtual void do_reduce(const BlkDevice* blk_device) const;
	virtual void do_extend(const BlkDevice* blk_device) const;

    private:

	void calculate_region_and_topology();

	MdLevel md_level;

	MdParity md_parity;

	unsigned long chunk_size;

    };


    namespace Action
    {

	class AddToEtcMdadm : public Modify
	{
	public:

	    AddToEtcMdadm(sid_t sid)
		: Modify(sid) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, Tense tense) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	    virtual void add_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
					  Actiongraph::Impl& actiongraph) const override;

	};


	class RemoveFromEtcMdadm : public Modify
	{
	public:

	    RemoveFromEtcMdadm(sid_t sid)
		: Modify(sid) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, Tense tense) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	};

    }


    bool compare_by_number(const Md* lhs, const Md* rhs);

}

#endif
