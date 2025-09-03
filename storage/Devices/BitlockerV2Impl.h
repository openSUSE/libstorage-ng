/*
 * Copyright (c) 2022 SUSE LLC
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


#ifndef STORAGE_BITLOCKER_V2_IMPL_H
#define STORAGE_BITLOCKER_V2_IMPL_H


#include "storage/Devices/BitlockerV2.h"
#include "storage/Devices/EncryptionImpl.h"


namespace storage
{

    using namespace std;

    class ActivateCallbacksV3;


    template <> struct DeviceTraits<BitlockerV2> { static const char* classname; };


    class BitlockerV2::Impl : public Encryption::Impl
    {
    public:

	Impl(const string& dm_table_name);

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return "BitlockerV2"; }

	virtual string get_pretty_classname() const override;

	static bool activate_bitlocker(const ActivateCallbacksV3* activate_callbacks, const Storage& storage,
				       SystemInfo::Impl& system_info, const string& name, const string& uuid);

	static bool activate_bitlockers(const ActivateCallbacksV3* activate_callbacks, const Storage& storage);

	static bool deactivate_bitlockers();

	static void probe_bitlockers(Prober& prober);

	virtual unique_ptr<Device::Impl> clone() const override { return make_unique<Impl>(*this); }

	virtual void save(xmlNode* node) const override;

	virtual void check(const CheckCallbacks* check_callbacks) const override;

	virtual string get_crypttab_spec(MountByType mount_by_type) const override;

	const string& get_uuid() const { return uuid; }

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;
	virtual void print(std::ostream& out) const override;

	virtual ResizeInfo detect_resize_info(const BlkDevice* blk_device = nullptr) const override;

	virtual uf_t used_features(UsedFeaturesDependencyType used_features_dependency_type) const override;

	virtual void do_delete() const override;
	virtual uf_t do_delete_used_features() const override { return UF_BITLOCKER; }

	virtual void do_activate() override;
	virtual uf_t do_activate_used_features() const override { return UF_BITLOCKER; }

	virtual void do_deactivate() override;
	virtual uf_t do_deactivate_used_features() const override { return UF_BITLOCKER; }

	static void reset_activation_infos();

    private:

	string uuid;

    };

}

#endif
