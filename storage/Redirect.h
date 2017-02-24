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


#ifndef STORAGE_REDIRECT_H
#define STORAGE_REDIRECT_H


#include "storage/Storage.h"
#include "storage/Devicegraph.h"


namespace storage
{

    /**
     * Finds the device corresponding to original in devicegraph.
     */
    template<class Type>
    const Type*
    redirect_to(const Devicegraph* devicegraph, const Type* original)
    {
	sid_t sid = original->get_sid();
	const Device* device = devicegraph->find_device(sid);
	return dynamic_cast<const Type*>(device);
    }


    /**
     * Finds the device corresponding to original in the probed devicegraph.
     */
    template<class Type>
    const Type*
    redirect_to_probed(const Type* original)
    {
	const Devicegraph* probed = original->get_impl().get_storage()->get_probed();
	return redirect_to<Type>(probed, original);
    }

}


#endif
