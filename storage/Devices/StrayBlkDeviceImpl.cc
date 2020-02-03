/*
 * Copyright (c) [2018-2020] SUSE LLC
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


#include "storage/Devices/StrayBlkDeviceImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Utils/Exception.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/CallbacksImpl.h"
#include "storage/Prober.h"
#include "storage/Utils/Format.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<StrayBlkDevice>::classname = "StrayBlkDevice";


    StrayBlkDevice::Impl::Impl(const xmlNode* node)
	: BlkDevice::Impl(node)
    {
    }


    string
    StrayBlkDevice::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("Stray Block Device").translated;
    }


    string
    StrayBlkDevice::Impl::get_name_sort_key() const
    {
	static const vector<NameSchema> name_schemata = {
	    NameSchema(regex(DEV_DIR "/xvd([a-z]+)([0-9]+)", regex::extended), { { PadInfo::A1, 5 }, { PadInfo::N1, 3 } })
	};

	return format_to_name_schemata(get_name(), name_schemata);
    }


    void
    StrayBlkDevice::Impl::probe_stray_blk_devices(Prober& prober)
    {
	for (const string& short_name : prober.get_sys_block_entries().stray_blk_devices)
	{
	    string name = DEV_DIR "/" + short_name;

	    try
	    {
		StrayBlkDevice* stray_blk_device = StrayBlkDevice::create(prober.get_system(), name);
		stray_blk_device->get_impl().probe_pass_1a(prober);
	    }
	    catch (const Exception& exception)
	    {
		// TRANSLATORS: error message
		prober.handle(exception, sformat(_("Probing stray block device %s failed"), name), 0);
	    }
	}
    }


    void
    StrayBlkDevice::Impl::probe_pass_1a(Prober& prober)
    {
	BlkDevice::Impl::probe_pass_1a(prober);

	BlkDevice::Impl::probe_size(prober);
    }


    ResizeInfo
    StrayBlkDevice::Impl::detect_resize_info(const BlkDevice* blk_device) const
    {
	return ResizeInfo(false, RB_RESIZE_NOT_SUPPORTED_BY_DEVICE);
    }


    void
    StrayBlkDevice::Impl::save(xmlNode* node) const
    {
	BlkDevice::Impl::save(node);
    }


    bool
    StrayBlkDevice::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	return BlkDevice::Impl::equal(rhs);
    }


    void
    StrayBlkDevice::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	BlkDevice::Impl::log_diff(log, rhs);
    }


    void
    StrayBlkDevice::Impl::print(std::ostream& out) const
    {
	BlkDevice::Impl::print(out);
    }

}
