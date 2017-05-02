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

	virtual EncryptionType get_type() const { return EncryptionType::UNKNOWN; }

	const string& get_password() const { return password; }

	void set_password(const string& password) { Impl::password = password; }

	bool is_in_etc_crypttab() const { return in_etc_crypttab; }
	void set_in_etc_crypttab(bool in_etc_crypttab) { Impl::in_etc_crypttab = in_etc_crypttab; }

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

	virtual Text do_resize_text(ResizeMode resize_mode, const Device* lhs, const Device* rhs,
				    Tense tense) const override;

	virtual Text do_activate_text(Tense tense) const override;

	virtual Text do_deactivate_text(Tense tense) const override;

	virtual Text do_add_to_etc_crypttab_text(Tense tense) const;
	virtual void do_add_to_etc_crypttab(CommitData& commit_data) const;

	virtual Text do_rename_in_etc_crypttab_text(const Device* lhs, Tense tense) const;
	virtual void do_rename_in_etc_crypttab(CommitData& commit_data, const Device* lhs) const;

	virtual Text do_remove_from_etc_crypttab_text(Tense tense) const;
	virtual void do_remove_from_etc_crypttab(CommitData& commit_data) const;

    private:

	string password;

	bool in_etc_crypttab;

	// mount-by for crypttab

    };


    namespace Action
    {

	class AddToEtcCrypttab : public Modify
	{
	public:

	    AddToEtcCrypttab(sid_t sid) : Modify(sid) {}

	    virtual Text text(const CommitData& commit_data) const override;
	    virtual void commit(CommitData& commit_data) const override;

	    virtual void add_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
					  Actiongraph::Impl& actiongraph) const override;

	};


	class RenameInEtcCrypttab : public RenameIn
	{
	public:

	    RenameInEtcCrypttab(sid_t sid) : RenameIn(sid) {}

	    virtual Text text(const CommitData& commit_data) const override;
	    virtual void commit(CommitData& commit_data) const override;

	    virtual const BlkDevice* get_renamed_blk_device(const Actiongraph::Impl& actiongraph,
							    Side side) const override;

	};


	class RemoveFromEtcCrypttab : public Modify
	{
	public:

	    RemoveFromEtcCrypttab(sid_t sid) : Modify(sid) {}

	    virtual Text text(const CommitData& commit_data) const override;
	    virtual void commit(CommitData& commit_data) const override;

	};

    }

}

#endif
