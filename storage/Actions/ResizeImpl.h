/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2021] SUSE LLC
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


#ifndef STORAGE_ACTION_RESIZE_IMPL_H
#define STORAGE_ACTION_RESIZE_IMPL_H


#include "storage/Actions/ModifyImpl.h"
#include "storage/Devices/BlkDevice.h"


namespace storage
{

    /**
     * We use the term "resize" for chaning the size of a non-container block
     * device, e.g. change the size of a partition or LVM logical volume.
     */
    enum class ResizeMode
    {
	SHRINK, GROW
    };


    namespace Action
    {

	class Resize : public Modify
	{
	public:

	    Resize(sid_t sid, ResizeMode resize_mode, const BlkDevice* blk_device)
		: Modify(sid), resize_mode(resize_mode), blk_device(blk_device) {}

	    virtual Text text(const CommitData& commit_data) const override;
	    virtual void commit(CommitData& commit_data, const CommitOptions& commit_options) const override;
	    virtual uf_t used_features(const Actiongraph::Impl& actiongraph) const override;

	    virtual void add_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
					  Actiongraph::Impl& actiongraph) const override;

	    Side get_side() const { return resize_mode == ResizeMode::GROW ? RHS : LHS; }

	    const ResizeMode resize_mode;

	    /**
	     * The underlying blk device being resized. Only allowed
	     * if blk_device is not nullptr, see below.
	     */
	    const BlkDevice* get_resized_blk_device(const Actiongraph::Impl& actiongraph,
						    Side side) const;

	    /**
	     * The underlying blk device being resized. nullptr for
	     * Partitions, LvmLvs and Nfs. Esp. important for Btrfs
	     * which can have multiple underlying blk devices.
	     *
	     * TODO on what side?
	     */
	    const BlkDevice* blk_device;

	private:

	    bool is_child(const Device* device, sid_t sid) const;

	};

    }

}


#endif
