/*
 * Copyright (c) [2004-2010] Novell, Inc.
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


#include "storage/Utils/AppUtil.h"
#include "storage/FreeInfo.h"


namespace storage
{

    using namespace std;


    ResizeInfo::ResizeInfo(unsigned long long min_size_k, unsigned long long max_size_k)
	: resize_ok(true), min_size_k(min_size_k), max_size_k(max_size_k)
    {
    }


    ResizeInfo::ResizeInfo()
	: resize_ok(false), min_size_k(0), max_size_k(1 * EiB)
    {
    }


    void
    ResizeInfo::combine(ResizeInfo resize_info)
    {
	resize_ok = resize_ok && resize_info.resize_ok;

	min_size_k = max(min_size_k, resize_info.min_size_k);
	max_size_k = min(max_size_k, resize_info.max_size_k);
    }


    std::ostream&
    operator<<(std::ostream& out, const ResizeInfo& resize_info)
    {
	return out << "resize-ok:" << resize_info.resize_ok << " min-size-k:" <<
	    resize_info.min_size_k << " max-size-k:" << resize_info.max_size_k;
    }


    ContentInfo::ContentInfo()
	: is_windows(false), is_efi(false), num_homes(0)
    {
    }


    std::ostream&
    operator<<(std::ostream& out, const ContentInfo& content_info)
    {
	return out << "is-windows:" << content_info.is_windows << " is-efi:" << content_info.is_efi
		   << " num-homes:" << content_info.num_homes;
    }


    /*
    FreeInfo::FreeInfo(const xmlNode* node)
	: resize_cached(false), content_cached(false)
    {
	if (getChildValue(node, "resize_cached", resize_cached) && resize_cached)
	{
	    getChildValue(node, "df_free_k", resize_info.df_freeK);
	    getChildValue(node, "resize_free_k", resize_info.resize_freeK);
	    getChildValue(node, "used_k", resize_info.usedK);
	    getChildValue(node, "resize_ok", resize_info.resize_ok);
	}

	if (getChildValue(node, "content_cached", content_cached) && content_cached)
	{
	    getChildValue(node, "windows", content_info.windows);
	    getChildValue(node, "efi", content_info.efi);
	    getChildValue(node, "homes", content_info.homes);
	}
    }


    void
    FreeInfo::saveData(xmlNode* node) const
    {
	if (resize_cached)
	{
	    setChildValue(node, "resize_cached", resize_cached);

	    setChildValue(node, "df_free_k", resize_info.df_freeK);
	    setChildValue(node, "resize_free_k", resize_info.resize_freeK);
	    setChildValue(node, "used_k", resize_info.usedK);
	    setChildValue(node, "resize_ok", resize_info.resize_ok);
	}

	if (content_cached)
	{
	    setChildValue(node, "content_cached", content_cached);

	    setChildValue(node, "windows", content_info.windows);
	    setChildValue(node, "efi", content_info.efi);
	    setChildValue(node, "homes", content_info.homes);
	}
    }


    void
    FreeInfo::update(bool new_resize_cached, const ResizeInfo& new_resize_info,
		     bool new_content_cached, const ContentInfo& new_content_info)
    {
	if (new_resize_cached)
	{
	    resize_cached = true;
	    resize_info = new_resize_info;
	}

	if (new_content_cached)
	{
	    content_cached = true;
	    content_info = new_content_info;
	}
    }


    std::ostream& operator<<(std::ostream& s, const FreeInfo& free_info)
    {
	s << "resize_cached:" << free_info.resize_cached;
	if (free_info.resize_cached)
	    s << " resize_info " << free_info.resize_info;

	s << "content_cached:" << free_info.content_cached;
	if (free_info.content_cached)
	    s << " content_info " << free_info.content_info;

	return s;
    }
    */

}
