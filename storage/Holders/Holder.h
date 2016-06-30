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


#ifndef STORAGE_HOLDER_H
#define STORAGE_HOLDER_H


#include <libxml/tree.h>
#include <memory>
#include <boost/noncopyable.hpp>

#include "storage/Devices/Device.h"


namespace storage
{
    class Devicegraph;
    class Device;


    struct HolderHasWrongType : public Exception
    {
	HolderHasWrongType(const char* seen, const char* expected);
    };


    // abstract class

    class Holder : private boost::noncopyable
    {
    public:

	virtual ~Holder();

	sid_t get_source_sid() const;
	sid_t get_target_sid() const;

	bool operator==(const Holder& rhs) const;
	bool operator!=(const Holder& rhs) const;

    public:

	class Impl;

	Impl& get_impl() { return *impl; }
	const Impl& get_impl() const { return *impl; }

	virtual Holder* clone() const = 0;

	void save(xmlNode* node) const;

	friend std::ostream& operator<<(std::ostream& out, const Holder& holder);

    protected:

	Holder(Impl* impl);

	void create(Devicegraph* devicegraph, const Device* source, const Device* target);
	void load(Devicegraph* devicegraph, const xmlNode* node);

    private:

	void add_to_devicegraph(Devicegraph* devicegraph, const Device* source,
				const Device* target);

	std::shared_ptr<Impl> impl;

    };

}

#endif
