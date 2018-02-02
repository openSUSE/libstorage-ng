/*
 * Copyright (c) [2016-2018] SUSE LLC
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


#ifndef STORAGE_BCACHE_CSET_IMPL_H
#define STORAGE_BCACHE_CSET_IMPL_H


#include "storage/Utils/Region.h"
#include "storage/Devices/BcacheCset.h"
#include "storage/Devices/DeviceImpl.h"


namespace storage
{

    using namespace std;


    class Bcache;
    class BlkDevice;


    template <> struct DeviceTraits<BcacheCset> { static const char* classname; };


    class BcacheCset::Impl : public Device::Impl
    {
    public:

	Impl()
	    : Device::Impl(), uuid() {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<BcacheCset>::classname; }

	virtual string get_displayname() const override { return get_uuid(); }

	virtual string get_pretty_classname() const override;

	static bool is_valid_uuid(const string& uuid);

	vector<const BlkDevice*> get_blk_devices() const;

	vector<const Bcache*> get_bcaches() const;

	static void probe_bcache_csets(Prober& prober);
	virtual void probe_pass_1b(Prober& prober) override;

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	virtual void check(const CheckCallbacks* check_callbacks) const override;

	virtual uint64_t used_features() const override;

	const string& get_uuid() const { return uuid; }
	void set_uuid(const string& uuid) { Impl::uuid = uuid; }

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	static BcacheCset* find_by_uuid(Devicegraph* devicegraph, const string& uuid);
	static const BcacheCset* find_by_uuid(const Devicegraph* devicegraph, const string& uuid);

    private:

	string uuid;

    };

}

#endif
