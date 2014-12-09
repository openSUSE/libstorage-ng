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


#include "storage/Utils/Region.h"


namespace storage_bgl
{

    std::ostream& operator<<(std::ostream& s, const Region& p)
    {
	return s << "[" << p.s << "," << p.l << "]";
    }


    bool
    getChildValue(const xmlNode* node, const char* name, Region& value)
    {
	const xmlNode* tmp = getChildNode(node, name);
	if (!tmp)
	    return false;

	getChildValue(tmp, "start", value.s);
	getChildValue(tmp, "length", value.l);
	return true;
    }


    void
    setChildValue(xmlNode* node, const char* name, const Region& value)
    {
	xmlNode* tmp = xmlNewChild(node, name);

	setChildValue(tmp, "start", value.s);
	setChildValue(tmp, "length", value.l);
    }

}
