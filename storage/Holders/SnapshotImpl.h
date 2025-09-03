/*
 * Copyright (c) 2020 SUSE LLC
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


#ifndef STORAGE_SNAPSHOT_IMPL_H
#define STORAGE_SNAPSHOT_IMPL_H


#include "storage/Holders/Snapshot.h"
#include "storage/Holders/HolderImpl.h"


namespace storage
{

    template <> struct HolderTraits<Snapshot> { static const char* classname; };


    class Snapshot::Impl : public Holder::Impl
    {
    public:

	Impl() = default;

	Impl(const xmlNode* node);

	virtual unique_ptr<Holder::Impl> clone() const override { return make_unique<Impl>(*this); }

	virtual void save(xmlNode* node) const override;

	virtual const char* get_classname() const override { return HolderTraits<Snapshot>::classname; }

	virtual bool is_in_view(View view) const override;

	virtual bool equal(const Holder::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Holder::Impl& rhs_base) const override;
	virtual void print(std::ostream& out) const override;

    };


    static_assert(!std::is_abstract<Snapshot>(), "Snapshot ought not to be abstract.");
    static_assert(!std::is_abstract<Snapshot::Impl>(), "Snapshot::Impl ought not to be abstract.");

}

#endif
