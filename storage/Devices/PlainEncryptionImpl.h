/*
 * Copyright (c) 2019 SUSE LLC
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


#ifndef STORAGE_PLAIN_ENCRYPTION_IMPL_H
#define STORAGE_PLAIN_ENCRYPTION_IMPL_H


#include "storage/Utils/Enum.h"
#include "storage/Devices/PlainEncryption.h"
#include "storage/Devices/EncryptionImpl.h"


namespace storage
{

    using namespace std;

    class ActivateCallbacks;


    template <> struct DeviceTraits<PlainEncryption> { static const char* classname; };


    class PlainEncryption::Impl : public Encryption::Impl
    {
    public:

	Impl(const string& dm_name)
	    : Encryption::Impl(dm_name) { set_type(EncryptionType::PLAIN); }

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return "PlainEncryption"; }

	virtual string get_pretty_classname() const override;

	static void probe_plain_encryptions(Prober& prober);

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void check(const CheckCallbacks* check_callbacks) const override;

	virtual void parent_has_new_region(const Device* parent) override;

	virtual uint64_t used_features() const override;

	virtual PlainEncryption* get_non_impl() override { return to_plain_encryption(Device::Impl::get_non_impl()); }
	virtual const PlainEncryption* get_non_impl() const override { return to_plain_encryption(Device::Impl::get_non_impl()); }

	virtual void do_create() override;

	virtual void do_delete() const override;

	virtual void do_activate() const override;

	virtual void do_deactivate() const override;

    private:

	void calculate_region_and_topology();

    };

}

#endif
