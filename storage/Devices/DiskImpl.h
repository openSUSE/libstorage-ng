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


#ifndef STORAGE_DISK_IMPL_H
#define STORAGE_DISK_IMPL_H


#include "storage/Utils/Enum.h"
#include "storage/Devices/Disk.h"
#include "storage/Devices/PartitionableImpl.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Disk> { static const char* classname; };

    template <> struct EnumTraits<Transport> { static const vector<string> names; };


    class Disk::Impl : public Partitionable::Impl
    {
    public:

	Impl(const string& name)
	    : Partitionable::Impl(name), rotational(false), transport(Transport::UNKNOWN) {}

	Impl(const string& name, const Region& region)
	    : Partitionable::Impl(name, region), rotational(false), transport(Transport::UNKNOWN) {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<Disk>::classname; }

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	bool is_rotational() const { return rotational; }
	void set_rotational(bool rotational) { Impl::rotational = rotational; }

	Transport get_transport() const { return transport; }
	void set_transport(Transport transport) { Impl::transport = transport; }

	static void probe_disks(Prober& prober);
	virtual void probe_pass_1a(Prober& prober) override;

	virtual uint64_t used_features() const override;

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	virtual void process_udev_paths(vector<string>& udev_paths) const override;
	virtual void process_udev_ids(vector<string>& udev_ids) const override;

	virtual Text do_create_text(Tense tense) const override;

    private:

	bool rotational;

	Transport transport;

    };


    static_assert(!std::is_abstract<Disk>(), "Disk ought not to be abstract.");
    static_assert(!std::is_abstract<Disk::Impl>(), "Disk::Impl ought not to be abstract.");

}

#endif
