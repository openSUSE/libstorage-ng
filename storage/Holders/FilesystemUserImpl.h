/*
 * Copyright (c) [2016-2019] SUSE LLC
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

	Impl() = default;

	Impl(const xmlNode* node);

	virtual unique_ptr<Holder::Impl> clone() const override { return make_unique<Impl>(*this); }

	virtual void save(xmlNode* node) const override;

	virtual const char* get_classname() const override { return HolderTraits<FilesystemUser>::classname; }

	virtual bool equal(const Holder::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Holder::Impl& rhs_base) const override;
	virtual void print(std::ostream& out) const override;

	bool is_journal() const { return journal; }
	void set_journal(bool journal) { Impl::journal = journal; }

	unsigned int get_id() const { return id; }
	void set_id(unsigned int id) { Impl::id = id; }

    private:

	bool journal = false;

	/**
	 * An id for the block device as seen from the filesystem. So
	 * far only used for btrfs where it is the btrfs devid.
	 */
	unsigned int id = 0;

    };

}

#endif
