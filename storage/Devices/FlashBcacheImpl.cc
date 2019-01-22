/*
 * Copyright (c) [2019] SUSE LLC
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


#include "storage/Devices/FlashBcacheImpl.h"
#include "storage/Utils/HumanString.h"
#include "storage/Utils/Format.h"


namespace storage
{

    using namespace std;


    // TODO actions for creating and deleting

    const char* DeviceTraits<FlashBcache>::classname = "FlashBcache";

    FlashBcache::Impl::Impl(const string& name)
	: Bcache::Impl(name)
    {
    }


    FlashBcache::Impl::Impl(const xmlNode* node)
	: Bcache::Impl(node)
    {
    }


    string
    FlashBcache::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("FlashBcache").translated;
    }


    Text
    FlashBcache::Impl::do_create_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/bcache0),
			   // %2$s is replaced by size (e.g. 2 GiB)
			   _("Create Flash-only Bcache %1$s (%2$s)"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/bcache0),
			   // %2$s is replaced by size (e.g. 2 GiB)
			   _("Creating Flash-only Bcache %1$s (%2$s)"));

	return sformat(text, get_name(), get_size_text());
    }


    void
    FlashBcache::Impl::do_create()
    {
	// TODO 
    }


    Text
    FlashBcache::Impl::do_delete_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/bcache0),
			   // %2$s is replaced by size (e.g. 2 GiB)
			   _("Delete Flash-only Bcache %1$s (%2$s)"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/bcache0),
			   // %2$s is replaced by size (e.g. 2 GiB)
			   _("Deleting Flash-only Bcache %1$s (%2$s)"));

	return sformat(text, get_name(), get_size_text());
    }


    void
    FlashBcache::Impl::do_delete() const
    {
	// TODO
    }

}
