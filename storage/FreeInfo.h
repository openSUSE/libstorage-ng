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


#ifndef FREE_INFO_H
#define FREE_INFO_H

#include <ostream>

#include "storage/StorageInterface.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{
    class Volume;


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

    protected:

	static bool isWindows(const string& mp);
	static unsigned numHomes(const string& mp);

    };

    std::ostream& operator<<(std::ostream& s, const ResizeInfo& resize_info);
    std::ostream& operator<<(std::ostream& s, const ContentInfo& content_info);
    std::ostream& operator<<(std::ostream& s, const FreeInfo& free_info);

}


#endif
