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


#ifndef STORAGE_BLK_DEVICE_IMPL_H
#define STORAGE_BLK_DEVICE_IMPL_H


#include "storage/Utils/Region.h"
#include "storage/Utils/Topology.h"
#include "storage/Devices/BlkDevice.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/FreeInfo.h"
#include "storage/Utils/UdevFilters.h"


namespace storage
{

    using namespace std;


    class File;


    template <> struct DeviceTraits<BlkDevice> { static const char* classname; };


    /**
     * abstract class
     *
     * Class has no major and minor number since those from the Block Extended
     * Major range are not stable. E.g. when you have sda20 and sda21 and
     * delete sda20 the minor number of sda21 can change.
     */
    class BlkDevice::Impl : public Device::Impl
    {
    public:

	virtual string get_displayname() const override { return get_name(); }

	virtual string get_name_sort_key() const override { return get_name(); }

	virtual void check(const CheckCallbacks* check_callbacks) const override;

	virtual bool is_usable_as_blk_device() const { return active; }

	virtual RemoveInfo detect_remove_info() const { return RemoveInfo(true, 0); }

	const string& get_name() const { return name; }
	virtual void set_name(const string& name);

	const string& get_sysfs_name() const { return sysfs_name; }
	void set_sysfs_name(const string& sysfs_name) { Impl::sysfs_name = sysfs_name; }

	const string& get_sysfs_path() const { return sysfs_path; }
	void set_sysfs_path(const string& sysfs_path) { Impl::sysfs_path = sysfs_path; }

	const File& get_sysfs_file(SystemInfo::Impl& system_info, const char* filename) const;

	/**
	 * The implementation in BlkDevice looks at udev paths and ids thus
	 * should be fine for existing devices. But for devices that can be
	 * created likely special handling is needed. E.g. Md does not have a
	 * udev id until created.
	 */
	virtual vector<MountByType> possible_mount_bys() const;

	bool is_active() const { return active; }
	void set_active(bool active) { Impl::active = active; }

	bool is_read_only() const { return read_only; }

	const Region& get_region() const { return region; }
	virtual void set_region(const Region& region);

	unsigned long long get_size() const;
	void set_size(unsigned long long size);

	Text get_size_text() const;

	const Topology& get_topology() const { return topology; }
	void set_topology(const Topology& topology) { Impl::topology = topology; }

	const vector<string>& get_udev_paths() const { return udev_paths; }
	void set_udev_paths(const vector<string>& udev_paths) { Impl::udev_paths = udev_paths; }

	const vector<string>& get_udev_ids() const { return udev_ids; }
	void set_udev_ids(const vector<string>& udev_ids) { Impl::udev_ids = udev_ids; }

	virtual string get_fstab_spec(MountByType mount_by_type) const;

	/**
	 * Checks whether spec (from fstab) matches the blk device. PARTUUID and PARTLABEL
	 * are supported. UUID and LABEL are not supported since those are a property of
	 * the filesystem.
	 */
	virtual bool spec_match(SystemInfo::Impl& system_info, const string& spec) const;

	const string& get_dm_table_name() const { return dm_table_name; }
	virtual void set_dm_table_name(const string& dm_table_name) { Impl::dm_table_name = dm_table_name; }

	static bool is_valid_dm_table_name(const string& dm_table_name);

	BlkFilesystem* create_blk_filesystem(FsType fs_type);

	bool has_blk_filesystem() const;

	BlkFilesystem* get_blk_filesystem();
	const BlkFilesystem* get_blk_filesystem() const;

	Encryption* create_encryption(const string& dm_table_name, EncryptionType type);

	void remove_encryption();

	bool has_encryption() const;

	Encryption* get_encryption();
	const Encryption* get_encryption() const;

	Bcache* create_bcache(const std::string& name);

	bool has_bcache() const;

	Bcache* get_bcache();
	const Bcache* get_bcache() const;

	BcacheCset* create_bcache_cset();

	bool has_bcache_cset() const;

	BcacheCset* get_bcache_cset();
	const BcacheCset* get_bcache_cset() const;

	virtual BlkDevice* get_non_impl() override { return to_blk_device(Device::Impl::get_non_impl()); }
	virtual const BlkDevice* get_non_impl() const override { return to_blk_device(Device::Impl::get_non_impl()); }

	/**
	 * Check if name is an alias for the blk device by looking at the udev links. By
	 * far not all aliases are known.
	 */
	bool is_alias_of(const string& name) const;

	/**
	 * This is a invasive version of BlkDevice::find_by_name(). If no
	 * block device is found via the name a second search via the sysfs
	 * path is done. This requires system lookups and is also the reason
	 * the function needs an SystemInfo object and must be used on the
	 * probed devicegraph.
	 *
	 * Using the sysfs path is simpler compared to using the major:minor
	 * numbers since the latter are not included in the devicegraph. This
	 * is esp. relevant when not used during probing (where the system
	 * lookup is not cached),
	 */
	static bool exists_by_any_name(const Devicegraph* devicegraph, const string& name,
				       SystemInfo::Impl& system_info);

	static BlkDevice* find_by_any_name(Devicegraph* devicegraph, const string& name,
					   SystemInfo::Impl& system_info);
	static const BlkDevice* find_by_any_name(const Devicegraph* devicegraph, const string& name,
						 SystemInfo::Impl& system_info);

	virtual void probe_pass_1a(Prober& prober) override;

	virtual void probe_size(Prober& prober);

	virtual void probe_topology(Prober& prober);

	virtual ResizeInfo detect_resize_info(const BlkDevice* blk_device = nullptr) const override;

	virtual void add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs) const override;

	virtual bool equal(const Device::Impl& rhs) const override = 0;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override = 0;
	virtual void print(std::ostream& out) const override = 0;

	virtual void process_udev_paths(vector<string>& udev_paths, const UdevFilters& udev_filters) const { udev_paths.clear(); }
	virtual void process_udev_ids(vector<string>& udev_ids, const UdevFilters& udev_filters) const { udev_ids.clear(); }

	void wipe_device() const;
	void discard_device() const;

	static bool is_valid_name(const string& name);

    protected:

	// TODO remove this constructor so always require a region?
	Impl(const string& name);

	Impl(const string& name, const Region& region);

	Impl(const xmlNode* node);

	virtual void save(xmlNode* node) const override;

    private:

	string name;

	string sysfs_name;
	string sysfs_path;

	/**
	 * Some blk devices can be inactive, e.g. MDs, LVM LVs or LUKSes.
	 *
	 * TODO But active has different meanings for those subsystems.
	 *
	 * This active here means that the device node exists.
	 */
	bool active = true;

	bool read_only = false;

	/**
	 * For most devices region.start is zero. Always used to keep track of
	 * size and sector size.
	 */
	Region region;

	/**
	 * The topology is used for 1. partition alignment and 2. LVM
	 * alignment (in LvmPv::Impl::calculate_pe_start()).
	 */
	Topology topology;

	vector<string> udev_paths;
	vector<string> udev_ids;

	string dm_table_name;

    };


    static_assert(std::is_abstract<BlkDevice>(), "BlkDevice ought to be abstract.");
    static_assert(std::is_abstract<BlkDevice::Impl>(), "BlkDevice::Impl ought to be abstract.");


    /**
     * Run "udevadm settle" and check existence of all blk devices.
     */
    void wait_for_devices(const vector<const BlkDevice*>& blk_devices);


    /**
     * Run "udevadm settle" and check non existence of all blk devices.
     */
    void wait_for_detach_devices(const vector<const BlkDevice*>& blk_devices);
    void wait_for_detach_devices(const vector<string>& dev_names);


    /**
     * Joins the names of the block devices including the size using
     * the join(const vector<Text>&, JoinMode, size_t) function. The
     * block devices are sorted by name prior to joining.
     */
    Text
    join(const vector<const BlkDevice*>& blk_devices, JoinMode join_mode, size_t limit);


}

#endif
