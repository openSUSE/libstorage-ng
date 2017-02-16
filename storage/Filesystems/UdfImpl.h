/*
 * Copyright (c) 2017 SUSE LLC
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


#ifndef STORAGE_UDF_IMPL_H
#define STORAGE_UDF_IMPL_H


#include "storage/Filesystems/Udf.h"
#include "storage/Filesystems/FilesystemImpl.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Udf> { static const char* classname; };


    class Udf::Impl : public Filesystem::Impl
    {
    public:

	Impl()
	    : Filesystem::Impl() {}

	Impl(const xmlNode* node);

	virtual FsType get_type() const override { return FsType::UDF; }

	virtual const char* get_classname() const override { return DeviceTraits<Udf>::classname; }

	virtual string get_displayname() const override { return "udf"; }

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual ResizeInfo detect_resize_info() const override;

    };

}

#endif
