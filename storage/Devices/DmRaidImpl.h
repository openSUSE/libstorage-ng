/*
 * Copyright (c) 2017 SUSE LLC
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


#ifndef STORAGE_DM_RAID_IMPL_H
#define STORAGE_DM_RAID_IMPL_H


#include "storage/Utils/Enum.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Devices/DmRaid.h"
#include "storage/Devices/PartitionableImpl.h"


namespace storage
{

    using namespace std;

    class ActivateCallbacks;


    template <> struct DeviceTraits<DmRaid> { static const char* classname; };


    class DmRaid::Impl : public Partitionable::Impl
    {
    public:

	Impl(const string& name);
	Impl(const string& name, const Region& region);
	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<DmRaid>::classname; }

	static bool activate_dm_raids(const ActivateCallbacks* activate_callbacks);

	static bool deactivate_dm_raids();

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	virtual void check(const CheckCallbacks* check_callbacks) const override;

	static bool is_valid_name(const string& name);

	bool is_rotational() const { return rotational; }
	void set_rotational(bool rotational) { Impl::rotational = rotational; }

	static void probe_dm_raids(Prober& prober);
	virtual void probe_pass_1a(Prober& prober) override;
	virtual void probe_pass_1b(Prober& prober) override;

	virtual uint64_t used_features() const override;

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	virtual void process_udev_ids(vector<string>& udev_ids) const override;

    private:

	bool rotational;

    };


    static_assert(!std::is_abstract<DmRaid>(), "Dmraid ought not to be abstract.");
    static_assert(!std::is_abstract<DmRaid::Impl>(), "Dmraid::Impl ought not to be abstract.");


    bool compare_by_name(const DmRaid* lhs, const DmRaid* rhs);

}

#endif
