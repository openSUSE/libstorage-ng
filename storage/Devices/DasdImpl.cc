/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2020] SUSE LLC
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


#include "storage/Devices/DasdImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Utils/Exception.h"
#include "storage/Utils/Enum.h"
#include "storage/Utils/StorageTypes.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/HumanString.h"
#include "storage/UsedFeatures.h"
#include "storage/Prober.h"
#include "storage/Utils/AppUtil.h"
#include "storage/Utils/CallbacksImpl.h"
#include "storage/Utils/Format.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Dasd>::classname = "Dasd";


    const vector<string> EnumTraits<DasdType>::names({
	"UNKNOWN", "ECKD", "FBA"
    });


    const vector<string> EnumTraits<DasdFormat>::names({
	"NONE", "LDL", "CDL"
    });


    Dasd::Impl::Impl(const string& name)
	: Partitionable::Impl(name, 4)
    {
    }


    Dasd::Impl::Impl(const string& name, const Region& region)
	: Partitionable::Impl(name, region, 4)
    {
    }


    Dasd::Impl::Impl(const xmlNode* node)
	: Partitionable::Impl(node)
    {
	string tmp;

	getChildValue(node, "bus-id", bus_id);

	getChildValue(node, "rotational", rotational);

	if (getChildValue(node, "type", tmp))
	    type = toValueWithFallback(tmp, DasdType::UNKNOWN);

	if (getChildValue(node, "format", tmp))
	    format = toValueWithFallback(tmp, DasdFormat::NONE);
    }


    string
    Dasd::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("DASD").translated;
    }


    string
    Dasd::Impl::get_name_sort_key() const
    {
	static const vector<NameSchema> name_schemata = {
	    NameSchema(regex(DEV_DIR "/dasd([a-z]+)", regex::extended), { { PadInfo::A1, 5, } }),
	    NameSchema(regex(DEV_DIR "/vd([a-z]+)", regex::extended), { { PadInfo::A1, 5 } })
	};

	return format_to_name_schemata(get_name(), name_schemata);
    }


    void
    Dasd::Impl::check(const CheckCallbacks* check_callbacks) const
    {
	if (check_callbacks)
	{
	    if (has_children() && !has_single_child_of_type<const PartitionTable>())
	    {
		check_callbacks->error(sformat("DASD %s used without a partition table.",
                                               get_displayname()));
	    }
	}
    }


    bool
    Dasd::Impl::is_usable_as_partitionable() const
    {
	if (type == DasdType::ECKD && (format == DasdFormat::CDL || format == DasdFormat::LDL))
	    return true;

	if (type == DasdType::FBA)
	    return true;

	return false;
    }


    vector<PtType>
    Dasd::Impl::get_possible_partition_table_types() const
    {
	switch (type)
	{
	    case DasdType::ECKD:
	    {
		switch (format)
		{
		    case DasdFormat::CDL:
			return { PtType::DASD };

		    case DasdFormat::LDL:
			return { PtType::IMPLICIT };

		    default:
			return { };
		}
	    }

	    case DasdType::FBA:
	    {
		vector<PtType> ret = Partitionable::Impl::get_possible_partition_table_types();

		ret.push_back(PtType::IMPLICIT);

		return ret;
	    }

	    case DasdType::UNKNOWN:
	    {
		return { };
	    }
	}

	return { };
    }


    void
    Dasd::Impl::probe_dasds(Prober& prober)
    {
	for (const string& short_name : prober.get_sys_block_entries().dasds)
	{
	    string name = DEV_DIR "/" + short_name;

	    try
            {
		Dasd* dasd = Dasd::create(prober.get_system(), name);
		dasd->get_impl().probe_pass_1a(prober);
            }
            catch (const Exception& exception)
            {
		// TRANSLATORS: error message
		prober.handle(exception, sformat(_("Probing DASD %s failed"), name), UF_DASD);
            }
	}
    }


    void
    Dasd::Impl::probe_pass_1a(Prober& prober)
    {
	Partitionable::Impl::probe_pass_1a(prober);

	SystemInfo& system_info = prober.get_system_info();

	const File& rotational_file = get_sysfs_file(system_info, "queue/rotational");
	rotational = rotational_file.get<bool>();

	// For DASDs using virtio-blk the dasdtool does not work. So
	// it is assumed that those DASDs are CDL formatted ECKDs. See
	// bsc #1112037. Might be fragile.

	if (boost::starts_with(get_name(), DEV_DIR "/vd"))
	{
	    type = DasdType::ECKD;

	    format = DasdFormat::CDL;

	    return;
	}

	const Dasdview& dasdview = system_info.getDasdview(get_name());

	bus_id = dasdview.get_bus_id();

	type = dasdview.get_type();

	if (type == DasdType::ECKD)
	    format = dasdview.get_format();
    }


    uint64_t
    Dasd::Impl::used_features() const
    {
	return UF_DASD | Partitionable::Impl::used_features();
    }


    ResizeInfo
    Dasd::Impl::detect_resize_info(const BlkDevice* blk_device) const
    {
	return ResizeInfo(false, RB_RESIZE_NOT_SUPPORTED_BY_DEVICE);
    }


    void
    Dasd::Impl::save(xmlNode* node) const
    {
	Partitionable::Impl::save(node);

	setChildValue(node, "bus-id", bus_id);

	setChildValueIf(node, "rotational", rotational, rotational);

	setChildValueIf(node, "type", toString(type), type != DasdType::UNKNOWN);
	setChildValueIf(node, "format", toString(format), format != DasdFormat::NONE);
    }


    bool
    Dasd::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Partitionable::Impl::equal(rhs))
	    return false;

	return bus_id == rhs.bus_id && rotational == rhs.rotational && type == rhs.type &&
	    format == rhs.format;
    }


    void
    Dasd::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Partitionable::Impl::log_diff(log, rhs);

	storage::log_diff(log, "bus-id", bus_id, rhs.bus_id);

	storage::log_diff(log, "rotational", rotational, rhs.rotational);

	storage::log_diff_enum(log, "type", type, rhs.type);
	storage::log_diff_enum(log, "format", format, rhs.format);
    }


    void
    Dasd::Impl::print(std::ostream& out) const
    {
	Partitionable::Impl::print(out);

	out << " bus-id:" << bus_id;

	if (rotational)
	    out << " rotational";

	out << " type:" << toString(type);
	out << " format:" << toString(format);
    }


    void
    Dasd::Impl::process_udev_paths(vector<string>& udev_paths) const
    {
	// See doc/udev.md.

	erase_if(udev_paths, [](const string& udev_path) {
	    return !boost::starts_with(udev_path, "ccw-");
	});
    }


    void
    Dasd::Impl::process_udev_ids(vector<string>& udev_ids) const
    {
	// See doc/udev.md.

	erase_if(udev_ids, [](const string& udev_id) {
	    return !boost::starts_with(udev_id, "ccw-");
	});
    }

}
