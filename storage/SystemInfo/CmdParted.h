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


#ifndef STORAGE_CMD_PARTED_H
#define STORAGE_CMD_PARTED_H


#include "storage/Geometry.h"
#include "storage/Utils/Region.h"
#include "storage/Devices/PartitionTableImpl.h"


namespace storage
{

    /**
     * Class for probing for partitions with the 'parted' command.
     */
    class Parted
    {

    public:

	/**
	 * Constructor: Probe the specified device
	 * with the 'parted' command and parse its output.
	 * This may throw a SystemCmdException or a ParseException.
	 */
	Parted(const string& device);

        /**
	 * Entry for one partition.
	 */
	struct Entry
	{
	    Entry() : num(0), type(PartitionType::PRIMARY), id(0), boot(false) {}

	    unsigned num;	// Partition number (1..n)
	    Region cylRegion;	// Partition region in cylinders
	    Region secRegion;	// Partition region in sectors
	    PartitionType type;	// primary / extended / logical / any
	    unsigned id;	// Numeric partition ID (Linux: 0x83 etc.)
	    bool boot;		// Boot flag of the partition
	};

	friend std::ostream& operator<<(std::ostream& s, const Parted& parted);
	friend std::ostream& operator<<(std::ostream& s, const Entry& entry);

	/**
	 * Get the disk label type as string ("msdos", "gpt", ...) as returned
	 * by the 'parted' command.
	 */
	PtType getLabel() const { return label; }

	/**
	 * S/390 arch : zfcp dasds create implicit partitions if there is none
	 * on that disk yet. This function returns if this is the case for this
	 * device.
	 */
	bool getImplicit() const { return implicit; }

	/**
	 * Get the disk geometry (cylinders, heads, sectors).
	 */
	const Geometry& getGeometry() const { return geometry; }

	/**
	 * Special for GPT disk labels: If disk was enlarged, the backup
	 * partition table at the end of the disk might need to be moved to the
	 * new actual end of the disk. This function returns that flag.
	 */
	bool getGptEnlarge() const { return gpt_enlarge; }

	/**
	 * Special for GPT disk labels: True if the backup GPT is broken.
	 */
	bool getGptFixBackup() const { return gpt_fix_backup; }

	/**
	 * Get the partition entries.
	 */
	typedef vector<Entry>::const_iterator const_iterator;

	/**
	 * Get the partition entry with the specified number (1..n) and return
	 * it in 'entry'. Return 'true' upon success, 'false' if there is no
	 * entry with that number.
	 */
	const vector<Entry>& getEntries() const { return entries; }

	/**
	 * Parse the output of the 'parted' command in 'lines'.
	 * This may throw a ParseException.
	 */
	bool getEntry(unsigned num, Entry& entry) const;

       /**
	 * Parse the output of the 'parted' command in 'lines'.
	 * This may throw a ParseException.
	 */
	void parse(const vector<string>& stdout, const vector<string>& stderr);

	/**
	 * Return the stderr output of parted command.
	 *
	 * Unlike many similar classes in the SystemInfo/ directory, This class
	 * does not throw an exception whenever the underlying command (here:
	 * parted) writes any output to stderr: It is quite common for the
	 * parted command to write messages to stderr in certain situations
	 * that may not necessarily be fatal.
	 *
	 * Any stderr messages are written to the log, though.
	 */
	const vector<string> & getStderr() const { return stderr; }

    private:

	typedef vector<Entry>::iterator iterator;

	string device;
	PtType label;
	bool implicit;
	Geometry geometry;
	bool gpt_enlarge;
	bool gpt_fix_backup;
	vector<Entry> entries;
	vector<string> stderr;

	void scanDiskFlags(const string& line);
	void scanSectorSizeLine(const string& line);
	void scanGeometryLine(const string& line);
	void scanCylEntryLine(const string& line);
	void scanSecEntryLine(const string& line);

    };

}


#endif
