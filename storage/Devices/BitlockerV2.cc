/*
 * Copyright (c) [2022-2023] SUSE LLC
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


#include "storage/Devices/BitlockerV2Impl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    BitlockerV2*
    BitlockerV2::create(Devicegraph* devicegraph, const string& dm_table_name)
    {
	shared_ptr<BitlockerV2> bitlocker_v2 = make_shared<BitlockerV2>(make_unique<BitlockerV2::Impl>(dm_table_name));
	Device::Impl::create(devicegraph, bitlocker_v2);
	return bitlocker_v2.get();
    }


    BitlockerV2*
    BitlockerV2::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<BitlockerV2> bitlocker_v2 = make_shared<BitlockerV2>(make_unique<BitlockerV2::Impl>(node));
	Device::Impl::load(devicegraph, bitlocker_v2);
	return bitlocker_v2.get();
    }


    BitlockerV2::BitlockerV2(Impl* impl)
	: Encryption(impl)
    {
    }


    BitlockerV2::BitlockerV2(unique_ptr<Device::Impl>&& impl)
	: Encryption(std::move(impl))
    {
    }


    BitlockerV2*
    BitlockerV2::clone() const
    {
	return new BitlockerV2(get_impl().clone());
    }


    std::unique_ptr<Device>
    BitlockerV2::clone_v2() const
    {
	return make_unique<BitlockerV2>(get_impl().clone());
    }


    BitlockerV2::Impl&
    BitlockerV2::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const BitlockerV2::Impl&
    BitlockerV2::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    vector<BitlockerV2*>
    BitlockerV2::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<BitlockerV2>();
    }


    vector<const BitlockerV2*>
    BitlockerV2::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const BitlockerV2>();
    }


    const string&
    BitlockerV2::get_uuid() const
    {
	return get_impl().get_uuid();
    }


    void
    BitlockerV2::reset_activation_infos()
    {
	BitlockerV2::Impl::reset_activation_infos();
    }


    bool
    is_bitlocker_v2(const Device* device)
    {
	return is_device_of_type<const BitlockerV2>(device);
    }


    BitlockerV2*
    to_bitlocker_v2(Device* device)
    {
	return to_device_of_type<BitlockerV2>(device);
    }


    const BitlockerV2*
    to_bitlocker_v2(const Device* device)
    {
	return to_device_of_type<const BitlockerV2>(device);
    }

}
