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


#ifndef STORAGE_LUKS_IMPL_H
#define STORAGE_LUKS_IMPL_H


#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/Enum.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Devices/Luks.h"
#include "storage/Devices/EncryptionImpl.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Luks> { static const char* classname; };


    class Luks::Impl : public Encryption::Impl
    {
    public:

	Impl(const string& dm_name)
	    : Encryption::Impl(dm_name), uuid() {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return "Luks"; }

	static vector<string> probe_luks(SystemInfo& systeminfo);

	virtual void probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo) override;
	virtual void probe_pass_2(Devicegraph* probed, SystemInfo& systeminfo) override;

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	virtual Text do_create_text(Tense tense) const override;

	virtual Text do_open_text(Tense tense) const;

    private:

	string uuid;

    };


    namespace Action
    {

	class OpenLuks : public Modify
	{
	public:

	    OpenLuks(sid_t sid) : Modify(sid) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, Tense tense) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	};

    }


}

#endif
