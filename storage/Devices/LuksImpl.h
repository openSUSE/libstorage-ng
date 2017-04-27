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


#ifndef STORAGE_LUKS_IMPL_H
#define STORAGE_LUKS_IMPL_H


#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/Enum.h"
#include "storage/Utils/HumanString.h"
#include "storage/Devices/Luks.h"
#include "storage/Devices/EncryptionImpl.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;

    class ActivationCallbacks;


    template <> struct DeviceTraits<Luks> { static const char* classname; };


    class Luks::Impl : public Encryption::Impl
    {
    public:

	Impl(const string& dm_name)
	    : Encryption::Impl(dm_name), uuid() {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return "Luks"; }

	/**
	 * Returns the next free name for automatic naming of lukses. It is
	 * guaranteed that the name does not exist in the system and that the
	 * same name is never returned twice.
	 */
	static string next_free_cr_auto_name(SystemInfo& systeminfo);

	static bool activate_luks(const ActivationCallbacks* activation_callbacks,
				  SystemInfo& systeminfo, const string& name, const string& uuid);

	static bool activate_lukses(const ActivationCallbacks* activation_callbacks);

	static void probe_lukses(Devicegraph* probed, SystemInfo& systeminfo);
	virtual void probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo) override;
	virtual void probe_pass_2(Devicegraph* probed, SystemInfo& systeminfo) override;

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual EncryptionType get_type() const override { return EncryptionType::LUKS; }

	virtual void save(xmlNode* node) const override;

	virtual void check() const override;

	virtual void parent_has_new_region(const Device* parent) override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	virtual ResizeInfo detect_resize_info() const override;

	virtual uint64_t used_features() const override;

	virtual void do_create() override;

	virtual void do_delete() const override;

	virtual void do_resize(ResizeMode resize_mode, const Device* rhs) const override;

	virtual void do_activate() const override;

	virtual void do_deactivate() const override;

	virtual void do_add_to_etc_crypttab(CommitData& commit_data) const override;

	virtual void do_rename_in_etc_crypttab(CommitData& commit_data, const Device* lhs)
	    const override;

	virtual void do_remove_from_etc_crypttab(CommitData& commit_data) const override;

    protected:

	void probe_uuid();

    private:

	static const unsigned long long metadata_size = 2 * MiB;

	void calculate_region();

	string uuid;

    };

}

#endif
