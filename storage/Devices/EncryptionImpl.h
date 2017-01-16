/*
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


#ifndef STORAGE_ENCRYPTION_IMPL_H
#define STORAGE_ENCRYPTION_IMPL_H


#include "storage/Utils/Enum.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Devices/Encryption.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Encryption> { static const char* classname; };

    template <> struct EnumTraits<EncryptionType> { static const vector<string> names; };


    class Encryption::Impl : public BlkDevice::Impl
    {
    public:

	Impl(const string& dm_table_name);
	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return "Encryption"; }

	virtual string get_displayname() const override { return get_dm_table_name(); }

	const string& get_password() const { return password; }

	void set_password(const string& password) { Impl::password = password; }

	const BlkDevice* get_blk_device() const;

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	void probe_pass_2(Devicegraph* probed, SystemInfo& systeminfo) override;

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs) const override;
	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	virtual Text do_create_text(Tense tense) const override;

	virtual Text do_delete_text(Tense tense) const override;

	virtual Text do_resize_text(ResizeMode resize_mode, const Device* lhs, Tense tense) const override;

	virtual Text do_activate_text(Tense tense) const override;

	virtual Text do_deactivate_text(Tense tense) const override;

	virtual Text do_add_etc_crypttab_text(Tense tense) const;
	virtual void do_add_etc_crypttab(const Actiongraph::Impl& actiongraph) const;

	virtual Text do_rename_etc_crypttab_text(const Device* lhs, Tense tense) const;
	virtual void do_rename_etc_crypttab(const Actiongraph::Impl& actiongraph, const Device* lhs) const;

	virtual Text do_remove_etc_crypttab_text(Tense tense) const;
	virtual void do_remove_etc_crypttab(const Actiongraph::Impl& actiongraph) const;

    private:

	string password;

	// mount-by for crypttab

    };


    namespace Action
    {

	class AddEtcCrypttab : public Modify
	{
	public:

	    AddEtcCrypttab(sid_t sid) : Modify(sid) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, Tense tense) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	    virtual void add_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
					  Actiongraph::Impl& actiongraph) const override;

	};


	class RenameEtcCrypttab : public Modify
	{
	public:

	    RenameEtcCrypttab(sid_t sid) : Modify(sid) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, Tense tense) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	};


	class RemoveEtcCrypttab : public Modify
	{
	public:

	    RemoveEtcCrypttab(sid_t sid) : Modify(sid) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, Tense tense) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	};

    }

}

#endif
