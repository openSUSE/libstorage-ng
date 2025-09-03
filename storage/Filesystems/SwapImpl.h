/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2021] SUSE LLC
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


#ifndef STORAGE_SWAP_IMPL_H
#define STORAGE_SWAP_IMPL_H


#include "storage/Filesystems/Swap.h"
#include "storage/Filesystems/BlkFilesystemImpl.h"
#include "storage/Utils/HumanString.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Swap> { static const char* classname; };


    class Swap::Impl : public BlkFilesystem::Impl
    {

    public:

	virtual unsigned long long min_size() const override { return 40 * KiB; }
	virtual unsigned long long max_size() const override { return 1 * TiB; }

	virtual bool supports_mounted_shrink() const override { return false; }
	virtual bool supports_mounted_grow() const override { return false; }

	virtual bool supports_unmounted_shrink() const override { return true; }
	virtual bool supports_unmounted_grow() const override { return true; }

	virtual bool supports_label() const override { return true; }
	virtual bool supports_modify_label() const override { return true; }
	virtual unsigned int max_labelsize() const override { return 15; }

	virtual bool supports_uuid() const override { return true; }
	virtual bool supports_modify_uuid() const override { return true; }

    public:

	Impl()
	    : BlkFilesystem::Impl() {}

	Impl(const xmlNode* node);

	virtual FsType get_type() const override { return FsType::SWAP; }

	virtual const char* get_classname() const override { return DeviceTraits<Swap>::classname; }

	virtual string get_pretty_classname() const override;

	virtual string get_displayname() const override { return "swap"; }

	virtual unique_ptr<Device::Impl> clone() const override { return make_unique<Impl>(*this); }

	virtual ResizeInfo detect_resize_info(const BlkDevice* blk_device = nullptr) const override;

	virtual ContentInfo detect_content_info() const override;

	virtual SpaceInfo detect_space_info() const override;

	virtual uf_t used_features_pure() const override { return UF_SWAP; }

	virtual void do_create() override;

	virtual void do_resize(const CommitData& commit_data, const Action::Resize* action) const override;

	virtual void do_set_label() const override;

	virtual void do_set_uuid() const override;

	virtual void immediate_activate(MountPoint* mount_point, bool force_rw = false) const override;
	virtual void immediate_deactivate(MountPoint* mount_point) const override;

    protected:

	virtual bool is_permanent() const override;

    };

}

#endif
