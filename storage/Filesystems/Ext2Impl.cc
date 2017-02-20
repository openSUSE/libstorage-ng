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


#include "storage/Utils/HumanString.h"
#include "storage/Filesystems/Ext2Impl.h"
#include "storage/FreeInfo.h"
#include "storage/UsedFeatures.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Ext2>::classname = "Ext2";


    Ext2::Impl::Impl(const xmlNode* node)
	: Ext::Impl(node)
    {
    }


    ResizeInfo
    Ext2::Impl::detect_resize_info() const
    {
	ResizeInfo resize_info = Ext::Impl::detect_resize_info();

	resize_info.combine(ResizeInfo(true, 512 * KiB, 2 * TiB));

	return resize_info;
    }


    uint64_t
    Ext2::Impl::used_features() const
    {
	return UF_EXT2 | Ext::Impl::used_features();
    }

}
