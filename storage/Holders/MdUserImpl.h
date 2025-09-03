/*
 * Copyright (c) [2016-2020] SUSE LLC
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


#ifndef STORAGE_MD_USER_IMPL_H
#define STORAGE_MD_USER_IMPL_H


#include "storage/Holders/MdUser.h"
#include "storage/Holders/UserImpl.h"


namespace storage
{

    template <> struct HolderTraits<MdUser> { static const char* classname; };


    class MdUser::Impl : public User::Impl
    {
    public:

	Impl() = default;

	Impl(const xmlNode* node);

	virtual unique_ptr<Holder::Impl> clone() const override { return make_unique<Impl>(*this); }

	virtual void save(xmlNode* node) const override;

	virtual const char* get_classname() const override { return HolderTraits<MdUser>::classname; }

	virtual bool equal(const Holder::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Holder::Impl& rhs_base) const override;
	virtual void print(std::ostream& out) const override;

	bool is_spare() const { return spare; }
	void set_spare(bool spare);

	bool is_faulty() const { return faulty; }
	void set_faulty(bool faulty);

	bool is_journal() const { return journal; }
	void set_journal(bool journal);

	unsigned int get_sort_key() const { return sort_key; }
	void set_sort_key(unsigned int sort_key) { Impl::sort_key = sort_key; }

    private:

	// TODO is an enum instead of three booleans more appropriate?
	bool spare = false;
	bool faulty = false;
	bool journal = false;

	unsigned int sort_key = 0;

	void recalculate();

    };


    static_assert(!std::is_abstract<MdUser>(), "MdUser ought not to be abstract.");
    static_assert(!std::is_abstract<MdUser::Impl>(), "MdUser::Impl ought not to be abstract.");

}

#endif
