/*
 * Copyright (c) [2017-2019] SUSE LLC
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
 * with this program; if not, contact SUSE LLC.
 *
 * To contact SUSE LLC about this file by physical or electronic mail, you may
 * find current contact information at www.suse.com.
 */


#ifndef STORAGE_FORMATTER_LVM_VG_H
#define STORAGE_FORMATTER_LVM_VG_H


#include <string>

#include "storage/CompoundAction/Formatter.h"
#include "storage/Devices/LvmVg.h"


namespace storage
{

    class CompoundAction::Formatter::LvmVg : public CompoundAction::Formatter
    {

    public:

	LvmVg(const CompoundAction::Impl* compound_action);

    private:

	Text blk_devices_text() const;

	virtual Text text() const override;

	Text create_with_pvs_text() const;
	Text create_text() const;

    private:

	const storage::LvmVg* vg = nullptr;

    };

}

#endif
