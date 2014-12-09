/*
 * Copyright (c) [2004-2014] Novell, Inc.
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


#include "storage/Storage.h"
#include "storage/StorageTypes.h"
#include "storage/Volume.h"


namespace storage
{

bool commitAction::operator<( const commitAction& rhs ) const
    {
    contOrder l(type);
    contOrder r(rhs.type);

    if( stage==rhs.stage && stage==MOUNT )
	{
	if( vol()==NULL || rhs.vol()==NULL )
	    return( false );
	else
	    {
	    if( rhs.vol()->getMount()=="swap" )
		return( false );
	    else if( vol()->getMount()=="swap" )
		return( true );
	    else if( vol()->hasOrigMount() != rhs.vol()->hasOrigMount() )
		return( rhs.vol()->hasOrigMount() );
	    else
		return( vol()->getMount()<rhs.vol()->getMount() );
	    }
	}
    else if( unsigned(r)==unsigned(l) )
	{
	if( stage==rhs.stage )
	    {
	    if( stage==DECREASE )
		{
		if( type!=rhs.type )
		    return( type>rhs.type );
		else
		    return( container<rhs.container );
		}
	    else
		{
		if( type!=rhs.type )
		    return( type<rhs.type );
		else
		    return( container>rhs.container );
		}
	    }
	else
	    return( stage<rhs.stage );
	}
    else
	return( unsigned(l)<unsigned(r) );
    }


    std::ostream& operator<<(std::ostream& s, const commitAction& a)
    {
	s << "stage:" << a.stage
	  << " type:" << toString(a.type)
	  << " cont:" << a.container
	  << " dest:" << a.destructive;
	if (a.co())
	    s << " name:" << a.co()->name();
	if (a.vol())
	    s << " name:" << a.vol()->name();
	if (!a.description.native.empty())
	    s << " desc:" << a.description.native;
	return s;
    };


    std::ostream& operator<<(std::ostream& s, const UsedBy& usedby)
    {
	switch (usedby.type())
	{
	    case UB_LVM:
		s << "lvm[" << usedby.device() << "]";
		break;
	    case UB_MD:
		s << "md[" << usedby.device() << "]";
		break;
	    case UB_MDPART:
		s << "mdpart[" << usedby.device() << "]";
		break;
	    case UB_DM:
		s << "dm[" << usedby.device() << "]";
		break;
	    case UB_DMRAID:
		s << "dmraid[" << usedby.device() << "]";
		break;
	    case UB_DMMULTIPATH:
		s << "dmmultipath[" << usedby.device() << "]";
		break;
	    case UB_BTRFS:
		s << "btrfs[" << usedby.device() << "]";
		break;
	    case UB_NONE:
		break;
	}

	return s;
    }

    void
    setChildValue(xmlNode* node, const char* name, const UsedBy& value)
    {
	xmlNode* tmp = xmlNewChild(node, name);

	switch (value.ub_type)
	{
	    case UB_LVM: setChildValue(tmp, "type", "lvm"); break;
	    case UB_MD: setChildValue(tmp, "type", "md"); break;
	    case UB_MDPART: setChildValue(tmp, "type", "mdpart"); break;
	    case UB_DM: setChildValue(tmp, "type", "dm"); break;
	    case UB_DMRAID: setChildValue(tmp, "type", "dmraid"); break;
	    case UB_DMMULTIPATH: setChildValue(tmp, "type", "dmmultipath"); break;
	    case UB_BTRFS: setChildValue(tmp, "type", "btrfs"); break;
	    case UB_NONE: break;
	}

	setChildValue(tmp, "device", value.ub_device);
    }

    std::ostream& operator<<(std::ostream& s, const Subvolume& sv)
    {
	s << sv.path();
	return s;
    }

    void
    setChildValue(xmlNode* node, const char* name, const Subvolume& v)
    {
	xmlNode* tmp = xmlNewChild(node, name);
	setChildValue(tmp, "path", v.path());
    }



    std::ostream&
    operator<<(std::ostream& s, const PartitionSlotInfo& a)
    {
	s << "region:" << Region(a.cylRegion) << " nr:" << a.nr << " device:" << a.device
	  << " primary_slot:" << a.primarySlot << " primary_possible:" << a.primaryPossible
	  << " extended_slot:" << a.extendedSlot << " extended_possible:" << a.extendedPossible
	  << " logical_slot:" << a.logicalSlot << " logical_possible:" << a.logicalPossible;
	return s;
    }


    std::ostream&
    operator<<(std::ostream& s, const FsCapabilities& a)
    {
	s << "ext:" << a.isExtendable << " ext_mounted:" << a.isExtendableWhileMounted
	  << " red:" << a.isReduceable << " red_mounted:" << a.isReduceableWhileMounted
	  << " uuid:" << a.supportsUuid << " label:" << a.supportsLabel
	  << " label_mounted:" << a.labelWhileMounted << " label_length:" << a.labelLength
	  << " min_fs_size_k:" << a.minimalFsSizeK;
	return s;
    }

}
