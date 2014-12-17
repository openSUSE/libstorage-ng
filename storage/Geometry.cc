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


#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <string.h>
#include <unistd.h>
#include <linux/hdreg.h>

#include "storage/Utils/AppUtil.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/StorageDefines.h"
#include "storage/Geometry.h"
#include "storage/Utils/Enum.h"


namespace storage
{
    using namespace std;


    Geometry::Geometry()
	: cylinders(0), heads(32), sectors(16), sector_size(512)
    {
    }


    unsigned long long
    Geometry::sizeK() const
    {
	return (unsigned long long)(cylinders) * cylinderSize() / 1024;
    }


    unsigned long
    Geometry::cylinderSize() const
    {
	return heads * sectors * sector_size;
    }

    unsigned long
    Geometry::headSize() const
    {
	return sectors * sector_size;
    }


    unsigned long long
    Geometry::cylinderToKb(unsigned long cylinder) const
    {
	const unsigned long long cyl_size = cylinderSize();

	return cyl_size * cylinder / 1024;
    }


    unsigned long
    Geometry::kbToCylinder(unsigned long long kb) const
    {
	const unsigned long long cyl_size = cylinderSize();

	unsigned long long bytes = kb * 1024;
	bytes += cyl_size - 1;
	return bytes / cyl_size;
    }


    unsigned long long
    Geometry::sectorToKb(unsigned long long sector) const
    {
	// TODO use 128 arithmetic
	return sector * sector_size / 1024;
    }


    unsigned long long
    Geometry::kbToSector(unsigned long long kb) const
    {
	// TODO use 128 arithmetic
	return kb * 1024 / sector_size;
    }


    bool
    Geometry::operator==(const Geometry& rhs) const
    {
	return cylinders == rhs.cylinders && heads == rhs.heads && sectors == rhs.sectors &&
	    sector_size == rhs.sector_size;
    }


    std::ostream& operator<<(std::ostream& s, const Geometry& geo)
    {
	return s << "[" << geo.cylinders << "," << geo.heads << "," << geo.sectors << ","
		 << geo.sector_size << "]";
    }


    bool
    getChildValue(const xmlNode* node, const char* name, Geometry& value)
    {
	const xmlNode* tmp = getChildNode(node, name);
	if (!tmp)
	    return false;

	getChildValue(tmp, "cylinders", value.cylinders);
	getChildValue(tmp, "heads", value.heads);
	getChildValue(tmp, "sectors", value.sectors);

	if (!getChildValue(tmp, "sector_size", value.sector_size))
	    value.sector_size = 512;

	return true;
    }


    void
    setChildValue(xmlNode* node, const char* name, const Geometry& value)
    {
	xmlNode* tmp = xmlNewChild(node, name);

	setChildValue(tmp, "cylinders", value.cylinders);
	setChildValue(tmp, "heads", value.heads);
	setChildValue(tmp, "sectors", value.sectors);

	setChildValueIf(tmp, "sector_size", value.sector_size, value.sector_size != 512);
    }


    bool
    detectGeometry(const string& device, Geometry& geo)
    {
	y2mil("device:" << device);

	bool ret = false;

	int fd = open(device.c_str(), O_RDONLY | O_CLOEXEC);
	if (fd >= 0)
	{
	    int rcode = ioctl(fd, BLKSSZGET, &geo.sector_size);
	    y2mil("BLKSSZGET rcode:" << rcode << " sector_size:" << geo.sector_size);

	    geo.cylinders = 16;
	    geo.heads = 255;
	    geo.sectors = 63;

	    struct hd_geometry hd_geo;
	    rcode = ioctl(fd, HDIO_GETGEO, &hd_geo);
	    y2mil("HDIO_GETGEO rcode:" << rcode << " cylinders:" << (unsigned) hd_geo.cylinders <<
		  " heads:" << (unsigned) hd_geo.heads << " sectors:" << (unsigned) hd_geo.sectors);
	    if (rcode == 0)
	    {
		geo.cylinders = hd_geo.cylinders > 0 ? hd_geo.cylinders : geo.cylinders;
		geo.heads = hd_geo.heads > 0 ? hd_geo.heads : geo.heads;
		geo.sectors = hd_geo.sectors > 0 ? hd_geo.sectors : geo.sectors;
	    }

	    uint64_t bytes = 0;
	    rcode = ioctl(fd, BLKGETSIZE64, &bytes);
	    y2mil("BLKGETSIZE64 rcode:" << rcode << " bytes:" << bytes);
	    if (rcode == 0 && bytes != 0)
	    {
		geo.cylinders = bytes / (uint64_t)(geo.cylinderSize());
		y2mil("calc cylinders:" << geo.cylinders);
		ret = true;
	    }
	    else
	    {
		unsigned long blocks;
		rcode = ioctl(fd, BLKGETSIZE, &blocks);
		y2mil("BLKGETSIZE rcode:" << rcode << " blocks:" << blocks);
		if (rcode == 0 && blocks != 0)
		{
		    geo.cylinders = (uint64_t)(blocks) * 512 / (uint64_t)(geo.cylinderSize());
		    y2mil("calc cylinders:" << geo.cylinders);
		    ret = true;
		}
	    }

	    close(fd);
	}
    else
	    y2err( "failure opening disk:" << device << " errno:" << errno <<
	           " (" << strerror(errno) << ")" );

	y2mil("device:" << device << " ret:" << ret << " geo:" << geo);
	return ret;
    }

}
