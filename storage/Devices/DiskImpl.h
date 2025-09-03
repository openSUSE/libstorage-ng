/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2022] SUSE LLC
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


#ifndef STORAGE_DISK_IMPL_H
#define STORAGE_DISK_IMPL_H


#include "storage/Utils/Enum.h"
#include "storage/Devices/Disk.h"
#include "storage/Devices/PartitionableImpl.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Disk> { static const char* classname; };

    template <> struct EnumTraits<Transport> { static const vector<string> names; };

    template <> struct EnumTraits<ZoneModel> { static const vector<string> names; };


    class Disk::Impl : public Partitionable::Impl
    {
    public:

	Impl(const string& name)
	    : Partitionable::Impl(name) {}

	Impl(const string& name, const Region& region)
	    : Partitionable::Impl(name, region) {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<Disk>::classname; }

	virtual string get_pretty_classname() const override;

	virtual string get_name_sort_key() const override;

	virtual bool is_usable_as_blk_device() const override;
	virtual bool is_usable_as_partitionable() const override;

	virtual void check(const CheckCallbacks* check_callbacks) const override;

	virtual string pool_name() const override;

	virtual unique_ptr<Device::Impl> clone() const override { return make_unique<Impl>(*this); }

	virtual void save(xmlNode* node) const override;

	virtual ResizeInfo detect_resize_info(const BlkDevice* blk_device = nullptr) const override;

	virtual RemoveInfo detect_remove_info() const override { return RemoveInfo(false, RMB_HARDWARE); }

	bool is_rotational() const { return rotational; }
	void set_rotational(bool rotational) { Impl::rotational = rotational; }

	bool is_dax() const { return dax; }
	void set_dax(bool dax) { Impl::dax = dax; }

	Transport get_transport() const { return transport; }
	void set_transport(Transport transport) { Impl::transport = transport; }

	ZoneModel get_zone_model() const { return zone_model; }
	void set_zone_model(ZoneModel zone_model) { Impl::zone_model = zone_model; }

	bool is_pmem() const;
	bool is_nvme() const;
	bool is_brd() const;
	bool is_nbd() const;

	const string& get_image_filename() const { return image_filename; }
	void set_image_filename(const string& image_filename) { Impl::image_filename = image_filename; }

	static void probe_disks(Prober& prober);
	virtual void probe_pass_1a(Prober& prober) override;

	virtual uf_t used_features(UsedFeaturesDependencyType used_features_dependency_type) const override;

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;
	virtual void print(std::ostream& out) const override;

	virtual void process_udev_paths(vector<string>& udev_paths, const UdevFilters& udev_filters) const override;
	virtual void process_udev_ids(vector<string>& udev_ids, const UdevFilters& udev_filters) const override;

	virtual Text do_create_text(Tense tense) const override;
	virtual void do_create() override;

	virtual Text do_activate_text(Tense tense) const override;
	virtual void do_activate() override;

    private:

	bool rotational = false;
	bool dax = false;

	Transport transport = Transport::UNKNOWN;

	ZoneModel zone_model = ZoneModel::NONE;

	string image_filename;

    };


    static_assert(!std::is_abstract<Disk>(), "Disk ought not to be abstract.");
    static_assert(!std::is_abstract<Disk::Impl>(), "Disk::Impl ought not to be abstract.");

}

#endif
