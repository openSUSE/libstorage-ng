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


#include <boost/graph/graphviz.hpp>

#include "storage/GraphvizImpl.h"
#include "storage/FreeInfo.h"
#include "storage/Holders/User.h"
#include "storage/Holders/MdUser.h"
#include "storage/Holders/FilesystemUser.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Devices/Disk.h"
#include "storage/Devices/Dasd.h"
#include "storage/Devices/Multipath.h"
#include "storage/Devices/DmRaid.h"
#include "storage/Devices/Md.h"
#include "storage/Devices/StrayBlkDevice.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmPv.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/Bcache.h"
#include "storage/Devices/BcacheCset.h"
#include "storage/Devices/Encryption.h"
#include "storage/Filesystems/MountPoint.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/ActiongraphImpl.h"


namespace storage
{

    namespace
    {

	void
	set_colors(map<string, string>& attributes, const char* color, const char* fillcolor)
	{
	    attributes["color"] = color;
	    attributes["fillcolor"] = fillcolor;
	}

    }


    string
    GraphStyleCallbacks::escape(const string& s)
    {
	string r;

	for (const char c : s)
	{
	    if (c == '\\')
		r += "\\";
	    r += c;
	}

	return r;
    }


    AdvancedDevicegraphStyleCallbacks::AdvancedDevicegraphStyleCallbacks(GraphvizFlags flags,
									 GraphvizFlags tooltip_flags)
	: flags(flags), tooltip_flags(tooltip_flags)
    {
    }


    map<string, string>
    AdvancedDevicegraphStyleCallbacks::graph()
    {
	return { };
    }


    map<string, string>
    AdvancedDevicegraphStyleCallbacks::nodes()
    {
	return { { "shape", "rectangle" }, { "style", "filled"}, { "fontname", "Arial" } };
    }


    map<string, string>
    AdvancedDevicegraphStyleCallbacks::node(const Device* device)
    {
	map<string, string> ret;

	if (flags != GraphvizFlags::NONE)
	    ret["label"] = vertex_text(device, flags);

	if (tooltip_flags != GraphvizFlags::NONE)
	    ret["tooltip"] = vertex_text(device, tooltip_flags);

	if (is_disk(device) || is_dasd(device) || is_multipath(device) || is_dm_raid(device))
	    set_colors(ret, "#ff0000", "#ffaaaa");
	else if (is_md(device))
	    set_colors(ret, "#aaaa00", "#ffffaa");
	else if (is_partition_table(device))
	    set_colors(ret, "#ff0000", "#ffaaaa");
	else if (is_partition(device))
	    set_colors(ret, "#cc33cc", "#eeaaee");
	else if (is_stray_blk_device(device))
	    set_colors(ret, "#cc33cc", "#eeaaee");
	else if (is_lvm_pv(device))
	    set_colors(ret, "#66dd22", "#bbff99");
	else if (is_lvm_vg(device))
	    set_colors(ret, "#0000ff", "#aaaaff");
	else if (is_lvm_lv(device))
	    set_colors(ret, "#6622dd", "#bb99ff");
	else if (is_encryption(device))
	    set_colors(ret, "#6622dd", "#bb99ff");
	else if (is_bcache(device))
	    set_colors(ret, "#6622dd", "#bb99ff");
	else if (is_bcache_cset(device))
	    set_colors(ret, "#6622dd", "#bb99ff");
	else if (is_mountable(device))
	    set_colors(ret, "#008800", "#99ee99");
	else if (is_mount_point(device))
	    set_colors(ret, "#ff0000", "#ffaaaa");
	else
	    ST_THROW(LogicException("unknown Device subclass"));

	return ret;
    }


    map<string, string>
    AdvancedDevicegraphStyleCallbacks::edges()
    {
	return { { "color", "#444444" } };
    }


    map<string, string>
    AdvancedDevicegraphStyleCallbacks::edge(const Holder* holder)
    {
	map<string, string> ret;

	if (is_subdevice(holder))
	{
	    ret["style"] = "solid";
	}
	else if (is_md_user(holder))
	{
	    const MdUser* md_user = to_md_user(holder);
	    if (md_user->is_spare() || md_user->is_faulty())
		ret["style"] = "dotted";
	    else
		ret["style"] = "dashed";
	}
	else if (is_filesystem_user(holder))
	{
	    const FilesystemUser* filesystem_user = to_filesystem_user(holder);
	    if (filesystem_user->is_journal())
		ret["style"] = "dotted";
	    else
		ret["style"] = "dashed";
	}
	else if (is_user(holder))
	{
	    ret["style"] = "dashed";
	}
	else
	{
	    ST_THROW(LogicException("unknown Holder subclass"));
	}

	return ret;
    }


    string
    AdvancedDevicegraphStyleCallbacks::vertex_text(const Device* device, GraphvizFlags flags) const
    {
	string ret;

	if (flags && GraphvizFlags::CLASSNAME)
	{
	    ret += string(device->get_impl().get_classname()) + "\\n";
	}

	if (flags && GraphvizFlags::PRETTY_CLASSNAME)
	{
	    ret += string(device->get_impl().get_pretty_classname()) + "\\n";
	}

	if (flags && GraphvizFlags::NAME)
	{
	    if (is_blk_device(device))
	    {
		const BlkDevice* blk_device = to_blk_device(device);
		ret += escape(blk_device->get_name()) + "\\n";
	    }
	    else if (is_lvm_vg(device))
	    {
		const LvmVg* lvm_vg = to_lvm_vg(device);
		ret += escape(DEV_DIR "/" + lvm_vg->get_vg_name()) + "\\n";
	    }
	}

	if (flags && GraphvizFlags::DISPLAYNAME)
	{
	    ret += escape(device->get_displayname()) + "\\n";
	}

	if (flags && GraphvizFlags::SID)
	{
	    ret += "sid:" + to_string(device->get_sid()) + "\\n";
	}

	if (flags && GraphvizFlags::SIZE)
	{
	    if (is_blk_device(device))
	    {
		const BlkDevice* blk_device = to_blk_device(device);
		ret += blk_device->get_size_string() + "\\n";
	    }
	    else if (is_lvm_vg(device))
	    {
		const LvmVg* lvm_vg = to_lvm_vg(device);
		ret += lvm_vg->get_size_string() + "\\n";
	    }
	    else if (is_filesystem(device))
	    {
		const Filesystem* filesystem = to_filesystem(device);
		if (filesystem->has_space_info())
		    ret += filesystem->detect_space_info().get_size_string() + "\\n";
	    }
	}

	if (flags && GraphvizFlags::ACTIVE)
	{
	    if (is_blk_device(device))
	    {
		const BlkDevice* blk_device = to_blk_device(device);
		if (blk_device->get_impl().is_active())
		    ret += "active" "\\n";
	    }

	    if (is_mount_point(device))
	    {
		const MountPoint* mount_point = to_mount_point(device);
		if (mount_point->is_active())
		    ret += "active" "\\n";
	    }
	}

	if (flags && GraphvizFlags::IN_ETC)
	{
	    if (is_mount_point(device))
	    {
		const MountPoint* mount_point = to_mount_point(device);
		if (mount_point->is_in_etc_fstab())
		    ret += "in fstab" "\\n";
	    }

	    if (is_encryption(device))
	    {
		const Encryption* encryption = to_encryption(device);
		if (encryption->is_in_etc_crypttab())
		    ret += "in crypttab" "\\n";
	    }
	}

	if (!ret.empty())
	    ret.erase(ret.size() - 2); // erase trailing "\\n"

	return ret;
    }


    AdvancedActiongraphStyleCallbacks::AdvancedActiongraphStyleCallbacks(GraphvizFlags flags,
									 GraphvizFlags tooltip_flags)
	: flags(flags), tooltip_flags(tooltip_flags)
    {
    }


    map<string, string>
    AdvancedActiongraphStyleCallbacks::graph()
    {
	return { };
    }


    map<string, string>
    AdvancedActiongraphStyleCallbacks::nodes()
    {
	return { { "shape", "rectangle" }, { "style", "filled" }, { "fontname", "Arial" } };
    }


    map<string, string>
    AdvancedActiongraphStyleCallbacks::node(const CommitData& commit_data,
					    const Action::Base* action)
    {
	map<string, string> ret;

	if (flags != GraphvizFlags::NONE)
	    ret["label"] = vertex_text(action, commit_data, flags);

	if (tooltip_flags != GraphvizFlags::NONE)
	    ret["tooltip"] = vertex_text(action, commit_data, tooltip_flags);

	if (is_create(action))
	    set_colors(ret, "#00ff00", "#ccffcc");
	else if (is_modify(action))
	    set_colors(ret, "#0000ff", "#ccccff");
	else if (is_delete(action))
	    set_colors(ret, "#ff0000", "#ffcccc");
	else
	    ST_THROW(LogicException("unknown Action::Base subclass"));

	return ret;
    }


    map<string, string>
    AdvancedActiongraphStyleCallbacks::edges()
    {
	return { };
    }


    string
    AdvancedActiongraphStyleCallbacks::vertex_text(const Action::Base* action, const CommitData& commit_data,
						   GraphvizFlags flags)
    {
	string ret;

	if (flags && GraphvizFlags::NAME)
	{
	    ret += escape(action->text(commit_data).translated) + "\\n";
	}

	if (flags && GraphvizFlags::SID)
	{
	    ret += "sid:" + to_string(action->sid);

	    ret += " [";
	    if (action->first)
		ret += "f";
	    if (action->last)
		ret += "l";
	    if (action->only_sync)
		ret += "s";
	    ret += "]" "\\n";
	}

	if (!ret.empty())
	    ret.erase(ret.size() - 2); // erase trailing "\\n"

	return ret;
    }


    void
    GraphWriter::write_attributes(ostream& out, const map<string, string>& attributes) const
    {
	out << " [";

	for (map<string, string>::const_iterator it = attributes.begin();
	     it != attributes.end(); ++it)
	{
	    out << (it == attributes.begin() ? " " : ", ")
		<< it->first << "=" << boost::escape_dot_string(it->second);
	}

	out << " ]";
    }


    void
    GraphWriter::write_attributes(ostream& out, const char* before, const map<string, string>& attributes) const
    {
	out << before;

	write_attributes(out, attributes);

	out << ";\n";
    }

}
