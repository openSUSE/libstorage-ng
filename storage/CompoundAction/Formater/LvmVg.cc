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
 * with this program; if not, contact SUSE LLC.
 *
 * To contact SUSE LLC about this file by physical or electronic mail, you may
 * find current contact information at www.suse.com.
 */


#include "storage/CompoundAction/Formater/LvmVg.h"


namespace storage
{

    CompoundAction::Formater::LvmVg::LvmVg(const CompoundAction::Impl* compound_action) :
	CompoundAction::Formater(compound_action),
	vg(to_lvm_vg(compound_action->get_target_device()))
    {}


    Text
    CompoundAction::Formater::LvmVg::text() const
    {
        Text text = tenser(tense,
                           _("String representation for a LvmVg target (pending)"),
                           _("String representation for a LvmVg target (pending)"));

        return sformat(text);
    }

}

