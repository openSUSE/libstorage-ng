/*
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


#ifndef STORAGE_BCACHE_CSET_IMPL_H
#define STORAGE_BCACHE_CSET_IMPL_H


#include "storage/Devices/BcacheCset.h"
#include "storage/Devices/DeviceImpl.h"


namespace storage
{

    using namespace std;


    class Bcache;
    class BlkDevice;


    template <> struct DeviceTraits<BcacheCset> { static const char* classname; };


    class BcacheCset::Impl : public Device::Impl
    {
    public:

	Impl() : Device::Impl() {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<BcacheCset>::classname; }

	virtual string get_displayname() const override { return "bcache cache"; }

	virtual string get_pretty_classname() const override;

	static bool is_valid_uuid(const string& uuid);

	vector<const BlkDevice*> get_blk_devices() const;

	vector<const Bcache*> get_bcaches() const;

	static void probe_bcache_csets(Prober& prober);
	virtual void probe_pass_1b(Prober& prober) override;

	virtual unique_ptr<Device::Impl> clone() const override { return make_unique<Impl>(*this); }

	virtual ResizeInfo detect_resize_info(const BlkDevice* blk_device = nullptr) const override;

	virtual void save(xmlNode* node) const override;

	virtual void check(const CheckCallbacks* check_callbacks) const override;

	virtual uf_t used_features(UsedFeaturesDependencyType used_features_dependency_type) const override;

	const string& get_uuid() const { return uuid; }
	void set_uuid(const string& uuid) { Impl::uuid = uuid; }

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;
	virtual void print(std::ostream& out) const override;

	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual Text do_create_text(Tense tense) const override;
	virtual void do_create() override;
	virtual uf_t do_create_used_features() const override { return UF_BCACHE; }

	virtual Text do_delete_text(Tense tense) const override;
	virtual void do_delete() const override;
	virtual uf_t do_delete_used_features() const override { return UF_BCACHE; }

	virtual Text do_deactivate_text(Tense tense) const override;
	virtual void do_deactivate() override;
	virtual uf_t do_deactivate_used_features() const override { return UF_BCACHE; }

    private:

	string uuid;		// TODO called "set uuid" in make-bcache output

    };

}

#endif
