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


#ifndef STORAGE_LVM_PV_IMPL_H
#define STORAGE_LVM_PV_IMPL_H


#include "storage/Devices/LvmPv.h"
#include "storage/Devices/DeviceImpl.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<LvmPv> { static const char* classname; };


    class LvmPv::Impl : public Device::Impl
    {
    public:

	Impl()
	    : Device::Impl(), uuid() {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<LvmPv>::classname; }

	virtual string get_displayname() const override { return "lvm pv"; }

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	virtual void check() const override;

	const string& get_uuid() const { return uuid; }
	void set_uuid(const string& uuid) { Impl::uuid = uuid; }

	bool has_blk_device() const;

	BlkDevice* get_blk_device();
	const BlkDevice* get_blk_device() const;

	bool has_lvm_vg() const;

	LvmVg* get_lvm_vg();
	const LvmVg* get_lvm_vg() const;

	static void probe_lvm_pvs(Prober& prober);
	virtual void probe_pass_1b(Prober& prober) override;

	virtual ResizeInfo detect_resize_info() const override;

	virtual uint64_t used_features() const override;

	virtual void parent_has_new_region(const Device* parent) override;

	static LvmPv* find_by_uuid(Devicegraph* devicegraph, const string& uuid);
	static const LvmPv* find_by_uuid(const Devicegraph* devicegraph, const string& uuid);

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	virtual Text do_create_text(Tense tense) const override;
	virtual void do_create() override;

	virtual Text do_resize_text(ResizeMode resize_mode, const Device* lhs, const Device* rhs,
				    Tense tense) const override;
	virtual void do_resize(ResizeMode resize_mode, const Device* rhs) const override;

	virtual Text do_delete_text(Tense tense) const override;
	virtual void do_delete() const override;

    private:

	string uuid;

    };

}

#endif
