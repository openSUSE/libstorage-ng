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


#ifndef STORAGE_DASD_IMPL_H
#define STORAGE_DASD_IMPL_H


#include "storage/Utils/Enum.h"
#include "storage/Devices/Dasd.h"
#include "storage/Devices/PartitionableImpl.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Dasd> { static const char* classname; };

    template <> struct EnumTraits<DasdType> { static const vector<string> names; };
    template <> struct EnumTraits<DasdFormat> { static const vector<string> names; };


    class Dasd::Impl : public Partitionable::Impl
    {
    public:

	Impl(const string& name);

	Impl(const string& name, const Region& region);

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<Dasd>::classname; }

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	string get_bus_id() const { return bus_id; }
	void set_bus_id(string bus_id) { Impl::bus_id = bus_id; }

	bool is_rotational() const { return rotational; }
	void set_rotational(bool rotational) { Impl::rotational = rotational; }

	DasdType get_type() const { return type; }
	void set_type(DasdType type) { Impl::type = type; }

	DasdFormat get_format() const { return format; }
	void set_format(DasdFormat format) { Impl::format = format; }

	virtual vector<PtType> get_possible_partition_table_types() const override;

	static void probe_dasds(Prober& prober);
	virtual void probe_pass_1a(Prober& prober) override;

	virtual uint64_t used_features() const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	virtual void process_udev_paths(vector<string>& udev_paths) const override;
	virtual void process_udev_ids(vector<string>& udev_ids) const override;

    private:

	string bus_id;

	bool rotational;

	DasdType type;
	DasdFormat format;

    };


    static_assert(!std::is_abstract<Dasd>(), "Dasd ought not to be abstract.");
    static_assert(!std::is_abstract<Dasd::Impl>(), "Dasd::Impl ought not to be abstract.");


    bool compare_by_name(const Dasd* lhs, const Dasd* rhs);

}

#endif
