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


#ifndef STORAGE_EXT_IMPL_H
#define STORAGE_EXT_IMPL_H


#include "storage/Filesystems/Ext.h"
#include "storage/Filesystems/FilesystemImpl.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Ext> { static const char* classname; };


    class Ext::Impl : public Filesystem::Impl
    {
    public:

	Impl()
	    : Filesystem::Impl() {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<Ext>::classname; }

	virtual void do_create() const override;

	virtual void do_set_label() const override;

	virtual void do_resize(ResizeMode resize_mode, const Device* rhs) const override;

    };

}

#endif
