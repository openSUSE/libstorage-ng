/*
 * Copyright (c) [2017-2022] SUSE LLC
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


#ifndef STORAGE_MULTIPATH_IMPL_H
#define STORAGE_MULTIPATH_IMPL_H


#include "storage/Utils/Enum.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Devices/Multipath.h"
#include "storage/Devices/PartitionableImpl.h"


namespace storage
{

    using namespace std;

    class ActivateCallbacks;


    template <> struct DeviceTraits<Multipath> { static const char* classname; };


    class Multipath::Impl : public Partitionable::Impl
    {
    public:

	Impl(const string& name);
	Impl(const string& name, const Region& region);
	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<Multipath>::classname; }

	virtual string get_pretty_classname() const override;

	static bool activate_multipaths(const ActivateCallbacks* activate_callbacks);

	static bool deactivate_multipaths();

	virtual unique_ptr<Device::Impl> clone() const override { return make_unique<Impl>(*this); }

	virtual ResizeInfo detect_resize_info(const BlkDevice* blk_device = nullptr) const override;

	virtual void save(xmlNode* node) const override;

	virtual void check(const CheckCallbacks* check_callbacks) const override;

	virtual RemoveInfo detect_remove_info() const override { return RemoveInfo(false, RMB_HARDWARE); }

	static bool is_valid_name(const string& name);

	virtual string pool_name() const override { return "MPs"; }

	const string& get_vendor() const { return vendor; }

	const string& get_model() const { return model; }

	vector<BlkDevice*> get_blk_devices();
	vector<const BlkDevice*> get_blk_devices() const;

	bool is_rotational() const { return rotational; }
	void set_rotational(bool rotational) { Impl::rotational = rotational; }

	static void probe_multipaths(Prober& prober);
	virtual void probe_pass_1a(Prober& prober) override;
	virtual void probe_pass_1b(Prober& prober) override;

	virtual uf_t used_features(UsedFeaturesDependencyType used_features_dependency_type) const override;

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;
	virtual void print(std::ostream& out) const override;

	virtual void process_udev_paths(vector<string>& udev_paths, const UdevFilters& udev_filters) const override;
	virtual void process_udev_ids(vector<string>& udev_ids, const UdevFilters& udev_filters) const override;

	virtual Text do_deactivate_text(Tense tense) const override;
	virtual void do_deactivate() override;
	virtual uf_t do_deactivate_used_features() const override { return UF_MULTIPATH; }

    private:

	string vendor;
	string model;

	bool rotational = false;

    };


    static_assert(!std::is_abstract<Multipath>(), "Multipath ought not to be abstract.");
    static_assert(!std::is_abstract<Multipath::Impl>(), "Multipath::Impl ought not to be abstract.");

}

#endif
