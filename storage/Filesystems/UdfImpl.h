/*
 * Copyright (c) [2017-2018] SUSE LLC
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
#include "storage/Filesystems/BlkFilesystemImpl.h"
#include "storage/Utils/HumanString.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Udf> { static const char* classname; };


    class Udf::Impl : public BlkFilesystem::Impl
    {

    public:

	virtual unsigned long long min_size() const override { return 256 * KiB; }
	virtual unsigned long long max_size() const override;

	virtual bool supports_mounted_shrink() const override { return false; }
	virtual bool supports_mounted_grow() const override { return false; }

	virtual bool supports_unmounted_shrink() const override { return false; }
	virtual bool supports_unmounted_grow() const override { return false; }

	virtual bool supports_label() const override { return true; }
	virtual unsigned int max_labelsize() const override { return 30; }

	virtual bool supports_uuid() const override { return true; }

    public:

	Impl()
	    : BlkFilesystem::Impl() {}

	Impl(const xmlNode* node);

	virtual FsType get_type() const override { return FsType::UDF; }

	virtual const char* get_classname() const override { return DeviceTraits<Udf>::classname; }

	virtual string get_pretty_classname() const override;

	virtual string get_displayname() const override { return "udf"; }

	virtual unique_ptr<Device::Impl> clone() const override { return make_unique<Impl>(*this); }

	virtual uf_t used_features_pure() const override { return UF_UDF; }

	virtual void do_create() override;

    };

}

#endif
