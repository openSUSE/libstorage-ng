/*
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


#ifndef STORAGE_LUKS_IMPL_H
#define STORAGE_LUKS_IMPL_H


#include "storage/Utils/HumanString.h"
#include "storage/Devices/Luks.h"
#include "storage/Devices/EncryptionImpl.h"


namespace storage
{

    using namespace std;

    class ActivateCallbacks;


    template <> struct DeviceTraits<Luks> { static const char* classname; };


    class Luks::Impl : public Encryption::Impl
    {
    public:

	Impl(const string& dm_table_name)
	    : Encryption::Impl(dm_table_name) {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return "Luks"; }

	virtual string get_pretty_classname() const override;

	static bool activate_luks(const ActivateCallbacks* activate_callbacks, const Storage& storage,
				  SystemInfo::Impl& system_info, const string& name, const string& uuid,
				  const string& label);

	static bool activate_lukses(const ActivateCallbacks* activate_callbacks, const Storage& storage);

	static bool deactivate_lukses();

	static void probe_lukses(Prober& prober);

	virtual unique_ptr<Device::Impl> clone() const override { return make_unique<Impl>(*this); }

	virtual void save(xmlNode* node) const override;

	virtual void check(const CheckCallbacks* check_callbacks) const override;

	virtual void set_type(EncryptionType type) override;

	virtual void set_integrity(const string& integrity) override;

	virtual string get_crypttab_spec(MountByType mount_by_type) const override;

	const string& get_uuid() const { return uuid; }
	void set_uuid(const string& uuid) { Impl::uuid = uuid; }

	const string& get_label() const { return label; }
	void set_label(const string& label) { Impl::label = label; }

	const string& get_format_options() const { return format_options; }
	void set_format_options(const string& format_options) { Impl::format_options = format_options; }

	virtual void parent_has_new_region(const Device* parent) override;

	virtual bool do_resize_needs_password() const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;
	virtual void print(std::ostream& out) const override;

	virtual ResizeInfo detect_resize_info(const BlkDevice* blk_device = nullptr) const override;

	virtual uf_t used_features(UsedFeaturesDependencyType used_features_dependency_type) const override;

	virtual Luks* get_non_impl() override { return to_luks(Device::Impl::get_non_impl()); }
	virtual const Luks* get_non_impl() const override { return to_luks(Device::Impl::get_non_impl()); }

	virtual void do_create() override;
	virtual uf_t do_create_used_features() const override { return UF_LUKS; }

	virtual void do_delete() const override;
	virtual uf_t do_delete_used_features() const override { return UF_LUKS; }

	virtual void do_activate() override;
	virtual void do_activate_post_verify() const override;
	virtual uf_t do_activate_used_features() const override { return UF_LUKS; }

	virtual void do_deactivate() override;
	virtual uf_t do_deactivate_used_features() const override { return UF_LUKS; }

	static void reset_activation_infos();

    protected:

	void probe_uuid();

    private:

	// The metadata_size here includes both the metadata and the
	// keyslots area.

	static const unsigned long long v1_metadata_size = 2 * MiB;
	static const unsigned long long v2_metadata_size = 16 * MiB;

	unsigned long long metadata_size() const;

	void calculate_region_and_topology();

	string uuid;
	string label;

	string format_options;

    };

}

#endif
