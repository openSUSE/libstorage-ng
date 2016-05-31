/*
 * Copyright (c) 2016 SUSE LLC
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


#ifndef STORAGE_LVM_LV_IMPL_H
#define STORAGE_LVM_LV_IMPL_H


#include "storage/Devices/LvmLv.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<LvmLv> { static const char* classname; };


    class LvmLv::Impl : public BlkDevice::Impl
    {
    public:

	Impl(const string& name)
	    : BlkDevice::Impl(name), uuid() {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<LvmLv>::classname; }

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	const string& get_uuid() const { return uuid; }
	void set_uuid(const string& uuid) { Impl::uuid = uuid; }

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	virtual void add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs) const override;

	virtual Text do_create_text(Tense tense) const override;

	virtual Text do_rename_text(const Impl& lhs, Tense tense) const;
	virtual void do_rename(const Impl& lhs) const;

    private:

	string uuid;

    };


    namespace Action
    {

	class Rename : public Modify
	{
	public:

	    Rename(sid_t sid) : Modify(sid) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, Tense tense) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	};

    }

}

#endif
