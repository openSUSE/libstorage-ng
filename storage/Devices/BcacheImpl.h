/*
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


#ifndef STORAGE_BCACHE_IMPL_H
#define STORAGE_BCACHE_IMPL_H


#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/Enum.h"
#include "storage/Devices/Bcache.h"
#include "storage/Devices/BlkDeviceImpl.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Bcache> { static const char* classname; };


    class Bcache::Impl : public BlkDevice::Impl
    {
    public:

	Impl(const string& name)
	    : BlkDevice::Impl(name) {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return "Bcache"; }

	static bool is_valid_name(const string& name);

	static void probe_bcaches(Prober& prober);
	virtual void probe_pass_1a(Prober& prober) override;
	virtual void probe_pass_1b(Prober& prober) override;

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	virtual uint64_t used_features() const override;

	unsigned int get_number() const;

	const BlkDevice* get_blk_device() const;

	bool has_bcache_cset() const;

	const BcacheCset* get_bcache_cset() const;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

    };


    bool compare_by_number(const Bcache* lhs, const Bcache* rhs);

}

#endif
