/*
 * Copyright (c) [2004-2010] Novell, Inc.
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


#include <limits>

#include "storage/Utils/HumanString.h"
#include "storage/Utils/XmlFile.h"
#include "storage/FreeInfo.h"


namespace storage
{

    using namespace std;


    ResizeInfo::ResizeInfo(bool resize_ok, unsigned long long min_size,
			   unsigned long long max_size)
	: resize_ok(resize_ok), min_size(min_size), max_size(max_size)
    {
	check();
    }


    ResizeInfo::ResizeInfo(bool resize_ok)
	: ResizeInfo(resize_ok, 0 * B, std::numeric_limits<unsigned long long>::max() * B)
    {
    }


    ResizeInfo::ResizeInfo(const xmlNode* node)
	: ResizeInfo(false)
    {
	getChildValue(node, "resize-ok", resize_ok);
	getChildValue(node, "min-size", min_size);
	getChildValue(node, "max-size", max_size);
    }


    void
    ResizeInfo::save(xmlNode* node) const
    {
	setChildValue(node, "resize-ok", resize_ok);
	setChildValue(node, "min-size", min_size);
	setChildValue(node, "max-size", max_size);
    }


    void
    ResizeInfo::combine(ResizeInfo extra_resize_info)
    {
	resize_ok = resize_ok && extra_resize_info.resize_ok;

	combine_min(extra_resize_info.min_size);
	combine_max(extra_resize_info.max_size);

	check();
    }


    void
    ResizeInfo::combine_min(unsigned long long extra_min_size)
    {
	min_size = max(min_size, extra_min_size);

	check();
    }


    void
    ResizeInfo::combine_max(unsigned long long extra_max_size)
    {
	max_size = min(max_size, extra_max_size);

	check();
    }


    void
    ResizeInfo::shift(unsigned long long offset)
    {
	// handling saturation

	min_size += offset;
	if (min_size < offset)
	    min_size = std::numeric_limits<unsigned long long>::max();

	max_size += offset;
	if (max_size < offset)
	    max_size = std::numeric_limits<unsigned long long>::max();

	check();
    }


    void
    ResizeInfo::check()
    {
	resize_ok = resize_ok && max_size > min_size;
    }


    std::ostream&
    operator<<(std::ostream& out, const ResizeInfo& resize_info)
    {
	return out << "resize-ok:" << resize_info.resize_ok << " min-size:" <<
	    resize_info.min_size << " max-size:" << resize_info.max_size;
    }


    ContentInfo::ContentInfo(bool is_windows, bool is_efi, unsigned num_homes)
	: is_windows(is_windows), is_efi(is_efi), num_homes(num_homes)
    {
    }


    ContentInfo::ContentInfo()
	: is_windows(false), is_efi(false), num_homes(0)
    {
    }


    ContentInfo::ContentInfo(const xmlNode* node)
	: is_windows(false), is_efi(false), num_homes(0)
    {
	getChildValue(node, "is-windows", is_windows);
	getChildValue(node, "is-efi", is_efi);
	getChildValue(node, "num-homes", num_homes);
    }


    void
    ContentInfo::save(xmlNode* node) const
    {
	setChildValue(node, "is-windows", is_windows);
	setChildValue(node, "is-efi", is_efi);
	setChildValue(node, "num-homes", num_homes);
    }


    std::ostream&
    operator<<(std::ostream& out, const ContentInfo& content_info)
    {
	return out << "is-windows:" << content_info.is_windows << " is-efi:" << content_info.is_efi
		   << " num-homes:" << content_info.num_homes;
    }


    SpaceInfo::SpaceInfo(unsigned long long size, unsigned long long used)
	: size(size), used(used)
    {
    }


    SpaceInfo::SpaceInfo(const xmlNode* node)
	: size(0), used(0)
    {
	getChildValue(node, "size", size);
	getChildValue(node, "used", used);
    }


    string
    SpaceInfo::get_size_string() const
    {
	return byte_to_humanstring(size, false, 2, false);
    }


    void
    SpaceInfo::save(xmlNode* node) const
    {
	setChildValue(node, "size", size);
	setChildValue(node, "used", used);
    }


    std::ostream&
    operator<<(std::ostream& out, const SpaceInfo& space_info)
    {
	return out << "size:" << space_info.size << " used:" << space_info.used;
    }

}
