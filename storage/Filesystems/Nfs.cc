/*
 * Copyright (c) [2017-2023] SUSE LLC
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


#include "storage/Filesystems/NfsImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/Format.h"


namespace storage
{

    using namespace std;


    NfsNotFoundByServerAndPath::NfsNotFoundByServerAndPath(const string& server, const string& path)
	: DeviceNotFound(sformat("Nfs not found, server:%s path:", server, path))
    {
    }


    Nfs*
    Nfs::create(Devicegraph* devicegraph, const string& server, const string& path)
    {
	shared_ptr<Nfs> nfs = make_shared<Nfs>(make_unique<Nfs::Impl>(server, path));
	Device::Impl::create(devicegraph, nfs);
	return nfs.get();
    }


    Nfs*
    Nfs::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<Nfs> nfs = make_shared<Nfs>(make_unique<Nfs::Impl>(node));
	Device::Impl::load(devicegraph, nfs);
	return nfs.get();
    }


    Nfs::Nfs(Impl* impl)
	: Filesystem(impl)
    {
    }


    Nfs::Nfs(unique_ptr<Device::Impl>&& impl)
	: Filesystem(std::move(impl))
    {
    }


    const string&
    Nfs::get_server() const
    {
	return get_impl().get_server();
    }


    const string&
    Nfs::get_path() const
    {
	return get_impl().get_path();
    }


    Nfs*
    Nfs::clone() const
    {
	return new Nfs(get_impl().clone());
    }


    std::unique_ptr<Device>
    Nfs::clone_v2() const
    {
	return make_unique<Nfs>(get_impl().clone());
    }


    Nfs::Impl&
    Nfs::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Nfs::Impl&
    Nfs::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    vector<Nfs*>
    Nfs::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<Nfs>();
    }


    vector<const Nfs*>
    Nfs::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const Nfs>();
    }


    Nfs*
    Nfs::find_by_server_and_path(Devicegraph* devicegraph, const string& server, const string& path)
    {
	for (Devicegraph::Impl::vertex_descriptor vertex : devicegraph->get_impl().vertices())
        {
            Nfs* nfs = dynamic_cast<Nfs*>(devicegraph->get_impl()[vertex]);
            if (nfs && nfs->get_server() == server && nfs->get_path() == path)
                return nfs;
        }

	ST_THROW(NfsNotFoundByServerAndPath(server, path));
    }


    const Nfs*
    Nfs::find_by_server_and_path(const Devicegraph* devicegraph, const string& server, const string& path)
    {
	for (Devicegraph::Impl::vertex_descriptor vertex : devicegraph->get_impl().vertices())
        {
            const Nfs* nfs = dynamic_cast<const Nfs*>(devicegraph->get_impl()[vertex]);
            if (nfs && nfs->get_server() == server && nfs->get_path() == path)
                return nfs;
        }

	ST_THROW(NfsNotFoundByServerAndPath(server, path));
    }


    bool
    is_nfs(const Device* device)
    {
	return is_device_of_type<const Nfs>(device);
    }


    Nfs*
    to_nfs(Device* device)
    {
	return to_device_of_type<Nfs>(device);
    }


    const Nfs*
    to_nfs(const Device* device)
    {
	return to_device_of_type<const Nfs>(device);
    }

}
