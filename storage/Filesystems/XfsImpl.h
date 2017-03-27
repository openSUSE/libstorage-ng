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


#ifndef STORAGE_XFS_IMPL_H
#define STORAGE_XFS_IMPL_H


#include "storage/Filesystems/Xfs.h"
#include "storage/Filesystems/BlkFilesystemImpl.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Xfs> { static const char* classname; };


    class Xfs::Impl : public BlkFilesystem::Impl
    {

    public:

	virtual bool supports_label() const override { return true; }
	virtual unsigned int max_labelsize() const override { return 12; }

	virtual bool supports_uuid() const override { return true; }

    public:

	Impl()
	    : BlkFilesystem::Impl() {}

	Impl(const xmlNode* node);

	virtual FsType get_type() const override { return FsType::XFS; }

	virtual const char* get_classname() const override { return DeviceTraits<Xfs>::classname; }

	virtual string get_displayname() const override { return "xfs"; }

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual ResizeInfo detect_resize_info() const override;

	virtual uint64_t used_features() const override;

	virtual void do_create() override;

	virtual void do_set_label() const override;

    };

}

#endif
