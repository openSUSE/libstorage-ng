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


#ifndef STORAGE_FLASH_BCACHE_IMPL_H
#define STORAGE_FLASH_BCACHE_IMPL_H


#include "storage/Devices/FlashBcache.h"
#include "storage/Devices/BcacheImpl.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/Text.h"

namespace storage
{

    using namespace std;

    template <> struct DeviceTraits<FlashBcache> { static const char* classname; };

    class FlashBcache::Impl : public Bcache::Impl
    {
    public:

	Impl(const string& name);

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return "FlashBcache"; }

	virtual string get_pretty_classname() const override;

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual Text do_create_text(Tense tense) const override;
	virtual void do_create() override;

	virtual Text do_delete_text(Tense tense) const override;
	virtual void do_delete() const override;

    };

}

#endif
