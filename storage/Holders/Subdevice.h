/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#ifndef STORAGE_SUBDEVICE_H
#define STORAGE_SUBDEVICE_H


#include "storage/Holders/Holder.h"


namespace storage
{

    class Subdevice : public Holder
    {
    public:

	static Subdevice* create(Devicegraph* devicegraph, const Device* source, const Device* target);
	static Subdevice* load(Devicegraph* devicegraph, const xmlNode* node);

	virtual Subdevice* clone() const override;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

    protected:

	Subdevice(Impl* impl);

    };


    bool is_subdevice(const Holder* holder);

    Subdevice* to_subdevice(Holder* device);
    const Subdevice* to_subdevice(const Holder* device);

}

#endif
