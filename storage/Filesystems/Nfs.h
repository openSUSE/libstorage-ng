/*
 * Copyright (c) [2017-2020] SUSE LLC
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


#ifndef STORAGE_NFS_H
#define STORAGE_NFS_H


#include "storage/Filesystems/Filesystem.h"
#include "storage/Devicegraph.h"


namespace storage
{

    class NfsNotFoundByServerAndPath : public DeviceNotFound
    {
    public:

	NfsNotFoundByServerAndPath(const std::string& server, const std::string& path);
    };


    /**
     * Class to represent a NFS mount.
     *
     * The class does not provide functions to change the server or path since
     * that would create a completely different filesystem. (Internally that
     * would invalidate the SpaceInfo object which is shared across
     * devicegraphs.)
     *
     * Different NFS versions are handled entirely via mount options.
     *
     * The Nfs object should always have MountPoint as child. So when deleting the
     * MountPoint also delete the Nfs.
     */
    class Nfs : public Filesystem
    {
    public:

	/**
	 * Create a device of type Nfs.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static Nfs* create(Devicegraph* devicegraph, const std::string& server, const std::string& path);

	static Nfs* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Get the server of the Nfs.
	 */
	const std::string& get_server() const;

	/**
	 * Get the path of the Nfs.
	 */
	const std::string& get_path() const;

	/**
	 * Get all Nfses.
	 */
	static std::vector<Nfs*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const Nfs*> get_all(const Devicegraph* devicegraph);

	/**
	 * Find a Nfs by its server and path. If there are several Nfses with the server
	 * and path it is undefined which is found.
	 *
	 * @throw DeviceNotFound, DeviceHasWrongType
	 */
	static Nfs* find_by_server_and_path(Devicegraph* devicegraph, const std::string& server,
					    const std::string& path);

	/**
	 * @copydoc find_by_server_and_path
	 */
	static const Nfs* find_by_server_and_path(const Devicegraph* devicegraph,
						  const std::string& server, const std::string& path);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Nfs* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	Nfs(Impl* impl);
	ST_NO_SWIG Nfs(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a Nfs.
     *
     * @throw NullPointerException
     */
    bool is_nfs(const Device* device);

    /**
     * Converts pointer to Device to pointer to Nfs.
     *
     * @return Pointer to Nfs.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Nfs* to_nfs(Device* device);

    /**
     * @copydoc to_nfs(Device*)
     */
    const Nfs* to_nfs(const Device* device);

}

#endif
