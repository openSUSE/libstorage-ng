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


#ifndef STORAGE_FREE_INFO_H
#define STORAGE_FREE_INFO_H


#include <libxml/tree.h>
#include <ostream>


// TODO find better name for file and class


namespace storage
{

    class ResizeInfo
    {
    public:

	ResizeInfo(bool resize_ok, unsigned long long min_size, unsigned long long max_size);
	ResizeInfo(bool resize_ok);

	/**
	 * min_size = max(min_size, extra_resize_info.min_size)
	 * max_size = min(max_size, extra_resize_info.max_size)
	 *
	 * Use e.g. to combine limits from filesystem with limits from partition.
	 */
	void combine(ResizeInfo extra_resize_info);

	void combine_min(unsigned long long extra_min_size);

	void combine_max(unsigned long long extra_max_size);

	/**
	 * min_size += offset
	 * max_size += offset
	 *
	 * Use e.g. to shift limits from filesystem by metadata of LUKS.
	 */
	void shift(unsigned long long offset);

	void check();

	bool resize_ok;

	unsigned long long min_size;
	unsigned long long max_size;

	friend std::ostream& operator<<(std::ostream& out, const ResizeInfo& resize_info);

    public:

	ResizeInfo(const xmlNode* node);

	void save(xmlNode* node) const;

    };


    class ContentInfo
    {
    public:

	ContentInfo(bool is_windows, bool is_efi, unsigned num_homes);
	ContentInfo();

	bool is_windows;
	bool is_efi;
	unsigned num_homes;

	friend std::ostream& operator<<(std::ostream& out, const ContentInfo& content_info);

    public:

	ContentInfo(const xmlNode* node);

	void save(xmlNode* node) const;

    };


    class SpaceInfo
    {
    public:

	SpaceInfo(unsigned long long size, unsigned long long used);

	unsigned long long size;
	unsigned long long used;

	std::string get_size_string() const;

	friend std::ostream& operator<<(std::ostream& out, const SpaceInfo& space_info);

    public:

	SpaceInfo(const xmlNode* node);

	void save(xmlNode* node) const;

    };

}


#endif
