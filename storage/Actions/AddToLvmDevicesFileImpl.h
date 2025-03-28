/*
 * Copyright (c) [2017-2025] SUSE LLC
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


#ifndef STORAGE_ACTION_ADD_TO_LVM_DEVICES_FILE_IMPL_H
#define STORAGE_ACTION_ADD_TO_LVM_DEVICES_FILE_IMPL_H


#include "storage/Actions/ModifyImpl.h"


namespace storage
{

    namespace Action
    {

	class AddToLvmDevicesFile : public Modify
	{
	public:

	    AddToLvmDevicesFile(sid_t sid) : Modify(sid) {}

	    virtual Text text(const CommitData& commit_data) const override;
	    virtual Color color() const override { return Color::GREEN; }
	    virtual void commit(CommitData& commit_data, const CommitOptions& commit_options) const override;

	};

    }

}

#endif
