/*
 * Copyright (c) [2017-2026] SUSE LLC
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


#include "storage/Filesystems/ErofsImpl.h"
#include "storage/FreeInfo.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Erofs>::classname = "Erofs";


    Erofs::Impl::Impl(const xmlNode* node)
	: BlkFilesystem::Impl(node)
    {
    }


    string
    Erofs::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("Erofs").translated;
    }

}
