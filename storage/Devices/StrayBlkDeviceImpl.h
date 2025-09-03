/*
 * Copyright (c) [2018-2021] SUSE LLC
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


#ifndef STORAGE_STRAY_BLK_DEVICE_IMPL_H
#define STORAGE_STRAY_BLK_DEVICE_IMPL_H


#include "storage/Devices/StrayBlkDevice.h"
#include "storage/Devices/BlkDeviceImpl.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<StrayBlkDevice> { static const char* classname; };


    class StrayBlkDevice::Impl : public BlkDevice::Impl
    {
    public:

	Impl(const string& name)
	    : BlkDevice::Impl(name) {}

	Impl(const string& name, const Region& region)
	    : BlkDevice::Impl(name, region) {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<StrayBlkDevice>::classname; }

	virtual string get_pretty_classname() const override;

	virtual string get_name_sort_key() const override;

	virtual unique_ptr<Device::Impl> clone() const override { return make_unique<Impl>(*this); }

	virtual void save(xmlNode* node) const override;

	virtual ResizeInfo detect_resize_info(const BlkDevice* blk_device = nullptr) const override;

	virtual RemoveInfo detect_remove_info() const override { return RemoveInfo(false, RMB_HARDWARE); }

	static void probe_stray_blk_devices(Prober& prober);
	virtual void probe_pass_1a(Prober& prober) override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;
	virtual void print(std::ostream& out) const override;

    };


    static_assert(!std::is_abstract<StrayBlkDevice>(), "StrayBlkDevice ought not to be abstract.");
    static_assert(!std::is_abstract<StrayBlkDevice::Impl>(), "StrayBlkDevice::Impl ought not to be abstract.");

}

#endif
