/*
 * Copyright (c) [2019-2023] SUSE LLC
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


#include "storage/Devices/PlainEncryptionImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    PlainEncryption*
    PlainEncryption::create(Devicegraph* devicegraph, const string& dm_table_name)
    {
	shared_ptr<PlainEncryption> plain_encryption = make_shared<PlainEncryption>(make_unique<PlainEncryption::Impl>(dm_table_name));
	Device::Impl::create(devicegraph, plain_encryption);
	return plain_encryption.get();
    }


    PlainEncryption*
    PlainEncryption::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<PlainEncryption> plain_encryption = make_shared<PlainEncryption>(make_unique<PlainEncryption::Impl>(node));
	Device::Impl::load(devicegraph, plain_encryption);
	return plain_encryption.get();
    }


    PlainEncryption::PlainEncryption(Impl* impl)
	: Encryption(impl)
    {
    }


    PlainEncryption::PlainEncryption(unique_ptr<Device::Impl>&& impl)
	: Encryption(std::move(impl))
    {
    }


    PlainEncryption*
    PlainEncryption::clone() const
    {
	return new PlainEncryption(get_impl().clone());
    }


    std::unique_ptr<Device>
    PlainEncryption::clone_v2() const
    {
	return make_unique<PlainEncryption>(get_impl().clone());
    }


    PlainEncryption::Impl&
    PlainEncryption::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const PlainEncryption::Impl&
    PlainEncryption::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    vector<PlainEncryption*>
    PlainEncryption::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<PlainEncryption>();
    }


    vector<const PlainEncryption*>
    PlainEncryption::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const PlainEncryption>();
    }


    bool
    is_plain_encryption(const Device* device)
    {
	return is_device_of_type<const PlainEncryption>(device);
    }


    PlainEncryption*
    to_plain_encryption(Device* device)
    {
	return to_device_of_type<PlainEncryption>(device);
    }


    const PlainEncryption*
    to_plain_encryption(const Device* device)
    {
	return to_device_of_type<const PlainEncryption>(device);
    }

}
