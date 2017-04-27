/*
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


#ifndef STORAGE_LVM_LV_IMPL_H
#define STORAGE_LVM_LV_IMPL_H


#include "storage/Devices/LvmLv.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;

    class ActivateCallbacks;


    template <> struct DeviceTraits<LvmLv> { static const char* classname; };


    class LvmLv::Impl : public BlkDevice::Impl
    {
    public:

	Impl(const string& vg_name, const string& lv_name);
	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<LvmLv>::classname; }

	virtual string get_displayname() const override { return get_lv_name(); }

	static bool activate_lvm_lvs(const ActivateCallbacks* activate_callbacks);

	static void probe_lvm_lvs(Devicegraph* probed, SystemInfo& systeminfo);
	virtual void probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo) override;

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	virtual void check() const override;

	const LvmVg* get_lvm_vg() const;

	const string& get_lv_name() const { return lv_name; }
	void set_lv_name(const string& lv_name);

	const string& get_uuid() const { return uuid; }
	void set_uuid(const string& uuid) { Impl::uuid = uuid; }

	unsigned long long number_of_extents() const { return get_region().get_length(); }

	unsigned int get_stripes() const { return stripes; }
	void set_stripes(unsigned int stripes) { Impl::stripes = stripes; }

	unsigned long get_stripe_size() const { return stripe_size; }
	void set_stripe_size(unsigned long stripe_size) { Impl::stripe_size = stripe_size; }

	virtual ResizeInfo detect_resize_info() const override;

	static LvmLv* find_by_uuid(Devicegraph* devicegraph, const std::string& uuid);
	static const LvmLv* find_by_uuid(const Devicegraph* devicegraph, const std::string& uuid);

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	virtual void add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs) const override;

	virtual Text do_create_text(Tense tense) const override;
	virtual void do_create() override;

	virtual Text do_rename_text(const Impl& lhs, Tense tense) const;
	virtual void do_rename(const Impl& lhs) const;

	virtual Text do_resize_text(ResizeMode resize_mode, const Device* lhs, const Device* rhs,
				    Tense tense) const override;
	virtual void do_resize(ResizeMode resize_mode, const Device* rhs) const override;

	virtual Text do_delete_text(Tense tense) const override;
	virtual void do_delete() const override;

    private:

	static string make_name(const string& vg_name, const string& lv_name);

	static string make_dm_table_name(const string& vg_name, const string& lv_name);

	string lv_name;
	string uuid;

	unsigned int stripes;
	unsigned long stripe_size;

    };


    namespace Action
    {

	class Rename : public Modify
	{
	public:

	    Rename(sid_t sid) : Modify(sid) {}

	    virtual Text text(const CommitData& commit_data) const override;
	    virtual void commit(CommitData& commit_data) const override;

	};

    }


    bool compare_by_lv_name(const LvmLv* lhs, const LvmLv* rhs);

}

#endif
