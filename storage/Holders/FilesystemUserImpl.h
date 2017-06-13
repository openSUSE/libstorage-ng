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


#ifndef STORAGE_FILESYSTEM_USER_IMPL_H
#define STORAGE_FILESYSTEM_USER_IMPL_H


#include "storage/Holders/FilesystemUser.h"
#include "storage/Holders/UserImpl.h"


namespace storage
{

    template <> struct HolderTraits<FilesystemUser> { static const char* classname; };


    class FilesystemUser::Impl : public User::Impl
    {
    public:

	Impl()
	    : User::Impl(), journal(false) {}

	Impl(const xmlNode* node);

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	virtual const char* get_classname() const override { return HolderTraits<FilesystemUser>::classname; }

	virtual bool equal(const Holder::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Holder::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	bool is_journal() const { return journal; }
	void set_journal(bool journal) { Impl::journal = journal; }

    private:

	bool journal;

    };

}

#endif
