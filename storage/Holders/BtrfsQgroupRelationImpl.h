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


#ifndef STORAGE_BTRFS_QGROUP_RELATION_IMPL_H
#define STORAGE_BTRFS_QGROUP_RELATION_IMPL_H


#include "storage/Holders/BtrfsQgroupRelation.h"
#include "storage/Holders/HolderImpl.h"


namespace storage
{

    class Btrfs;


    template <> struct HolderTraits<BtrfsQgroupRelation> { static const char* classname; };


    class BtrfsQgroupRelation::Impl : public Holder::Impl
    {
    public:

	Impl() = default;

	Impl(const xmlNode* node);

	virtual unique_ptr<Holder::Impl> clone() const override { return make_unique<Impl>(*this); }

	virtual void save(xmlNode* node) const override;

	virtual const char* get_classname() const override { return HolderTraits<BtrfsQgroupRelation>::classname; }

	virtual bool is_in_view(View view) const override;

	const Btrfs* get_btrfs() const;

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual void add_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
				      Actiongraph::Impl& actiongraph) const override;

	virtual bool equal(const Holder::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Holder::Impl& rhs_base) const override;
	virtual void print(std::ostream& out) const override;

	virtual Text do_create_text(Tense tense) const override;
	virtual void do_create() override;
	virtual uf_t do_create_used_features() const override { return UF_BTRFS; }

	virtual Text do_delete_text(Tense tense) const override;
	virtual void do_delete() const override;
	virtual uf_t do_delete_used_features() const override { return UF_BTRFS; }

    };


    static_assert(!std::is_abstract<BtrfsQgroupRelation>(), "BtrfsQgroupRelation ought not to be abstract.");
    static_assert(!std::is_abstract<BtrfsQgroupRelation::Impl>(), "BtrfsQgroupRelation::Impl ought not to be abstract.");

}

#endif
