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
	: Partitionable::Impl(name), bus_id(), rotational(false), type(DasdType::UNKNOWN),
	  format(DasdFormat::NONE)
    {
    }


    Dasd::Impl::Impl(const string& name, const Region& region)
	: Partitionable::Impl(name, region, 4), bus_id(), rotational(false),
	  type(DasdType::UNKNOWN), format(DasdFormat::NONE)
    {
    }


    Dasd::Impl::Impl(const xmlNode* node)
	: Partitionable::Impl(node), bus_id(), rotational(false), type(DasdType::UNKNOWN),
	  format(DasdFormat::NONE)
    {
	string tmp;

	getChildValue(node, "bus-id", bus_id);

	getChildValue(node, "rotational", rotational);

	if (getChildValue(node, "type", tmp))
	    type = toValueWithFallback(tmp, DasdType::UNKNOWN);

	if (getChildValue(node, "format", tmp))
	    format = toValueWithFallback(tmp, DasdFormat::NONE);
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
	for (const string& short_name : prober.get_system_info().getDir(SYSFSDIR "/block"))
	{
	    string name = DEVDIR "/" + short_name;

	    if (!boost::starts_with(name, DEVDIR "/dasd"))
		continue;

	    const CmdUdevadmInfo udevadminfo = prober.get_system_info().getCmdUdevadmInfo(name);

	    const File range_file = prober.get_system_info().getFile(SYSFSDIR + udevadminfo.get_path() +
								     "/ext_range");

	    if (range_file.get<int>() <= 1)
		continue;

	    Dasd* dasd = Dasd::create(prober.get_probed(), name);
	    dasd->get_impl().probe_pass_1a(prober);
	}
    }


    void
    Dasd::Impl::probe_pass_1a(Prober& prober)
    {
	Partitionable::Impl::probe_pass_1a(prober);

	const File rotational_file = prober.get_system_info().getFile(SYSFSDIR + get_sysfs_path() +
								      "/queue/rotational");
	rotational = rotational_file.get<bool>();

	const Dasdview dasdview = prober.get_system_info().getDasdview(get_name());

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


    bool
    compare_by_name(const Dasd* lhs, const Dasd* rhs)
    {
	const string& string_lhs = lhs->get_name();
	const string& string_rhs = rhs->get_name();

	string::size_type size_lhs = string_lhs.size();
	string::size_type size_rhs = string_rhs.size();

	for (const string& tmp : { DEVDIR "/dasd", DEVDIR "/vd" })
	{
	    if (boost::starts_with(string_lhs, tmp) && boost::starts_with(string_rhs, tmp))
	    {
		if (size_lhs != size_rhs)
		    return size_lhs < size_rhs;
		else
		    return string_lhs < string_rhs;
	    }
	}

	return string_lhs < string_rhs;
    }

}
