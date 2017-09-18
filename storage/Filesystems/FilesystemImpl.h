/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#ifndef STORAGE_FILESYSTEM_IMPL_H
#define STORAGE_FILESYSTEM_IMPL_H


#include "storage/Utils/Enum.h"
#include "storage/Utils/CDgD.h"
#include "storage/Utils/FileUtils.h"
#include "storage/Filesystems/Filesystem.h"
#include "storage/Filesystems/MountableImpl.h"
#include "storage/FreeInfo.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Filesystem> { static const char* classname; };


    // abstract class

    class Filesystem::Impl : public Mountable::Impl
    {
    public:

	virtual FsType get_type() const = 0;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	virtual FsType get_mount_type() const override { return get_type(); }

	virtual SpaceInfo detect_space_info() const;
	virtual SpaceInfo detect_space_info_on_disk() const;
	void set_space_info(const SpaceInfo& space_info);
	bool has_space_info() const { return space_info.has_value(); }

	virtual Filesystem* get_non_impl() override { return to_filesystem(Device::Impl::get_non_impl()); }
	virtual const Filesystem* get_non_impl() const override { return to_filesystem(Device::Impl::get_non_impl()); }

    protected:

	Impl() : Mountable::Impl() {}

	Impl(const xmlNode* node);

	void save(xmlNode* node) const override;

    private:

	/**
	 * mutable to allow updating cache from const functions. Otherwise
	 * caching would not be possible when working with the probed
	 * devicegraph.
	 */
	mutable CDgD<SpaceInfo> space_info;

    };

}

#endif
