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


#ifndef STORAGE_FREE_INFO_H
#define STORAGE_FREE_INFO_H

#include <ostream>


namespace storage
{
    class Volume;


    class ResizeInfo
    {
    public:

	ResizeInfo(unsigned long long min_size_k, unsigned long long max_size_k);
	ResizeInfo();

	void combine(ResizeInfo resize_info);

	bool resize_ok;

	unsigned long long min_size_k;
	unsigned long long max_size_k;

	friend std::ostream& operator<<(std::ostream& out, const ResizeInfo& resize_info);

    };


    class ContentInfo
    {
    public:

	ContentInfo();

	bool is_windows;
	bool is_efi;
	unsigned num_homes;

	friend std::ostream& operator<<(std::ostream& out, const ContentInfo& content_info);

    };


    /*
    class FreeInfo
    {

    public:

	FreeInfo(bool resize_cached, const ResizeInfo& resize_info,
		 bool content_cached, const ContentInfo& content_info)
	    : resize_cached(resize_cached), resize_info(resize_info),
	      content_cached(content_cached), content_info(content_info) {}

	FreeInfo(const xmlNode* node);

	void saveData(xmlNode* node) const;

	bool resize_cached;
	ResizeInfo resize_info;

	bool content_cached;
	ContentInfo content_info;

	void update(bool resize_cached, const ResizeInfo& resize_info,
		    bool content_cached, const ContentInfo& content_info);

	static ResizeInfo detectResizeInfo(const string& mp, const Volume& vol);
	static ContentInfo detectContentInfo(const string& mp, const Volume& vol);

    };

    std::ostream& operator<<(std::ostream& s, const FreeInfo& free_info);
    */

}


#endif
