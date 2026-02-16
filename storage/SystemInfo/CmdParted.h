/*
 * Copyright (c) [2004-2014] Novell, Inc.
 * Copyright (c) [2016-2023] SUSE LLC
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


#include "storage/Utils/Region.h"
#include "storage/Utils/JsonFile.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Utils/Udev.h"


namespace storage
{
    using std::string;
    using std::vector;
    using std::map;


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
	Parted(Udevadm& udevadm, const string& device);

	/**
	 * Entry for one partition.
	 */
	struct Entry
	{
	    /** Partition number (1..n) */
	    unsigned int number = 0;

	    /** Partition region in sectors */
	    Region region;

	    /** Partition type (primary, extended or logical) */
	    PartitionType type = PartitionType::PRIMARY;

	    /** Numeric partition ID (Linux = 0x83 etc.) */
	    unsigned int id = 0;

	    /** Boot flag of the partition (only MS-DOS) */
	    bool boot = false;

	    /** Legacy boot flag of the partition (only GPT) */
	    bool legacy_boot = false;

	    /** No automount flag of the partition (only GPT) */
	    bool no_automount = false;

	    /** Partition name (only GPT) */
	    string name;
	};

	friend std::ostream& operator<<(std::ostream& s, const Parted& parted);
	friend std::ostream& operator<<(std::ostream& s, const Entry& entry);

	/**
	 * Get the disk label type as enum PtType.
	 */
	PtType get_label() const { return label; }

	/**
	 * Region spanning whole device. Used to report device size in sectors
	 * and sector size in bytes.
	 */
	const Region& get_region() const { return region; }

	/**
	 * Get the number of primary partition slots. If parted did not report the
	 * value it is -1 (reporting the value is a SUSE specific patch).
	 */
	int get_primary_slots() const { return primary_slots; }

	/**
	 * S/390 arch: zFCP DASDs create implicit partitions if there is none on that disk
	 * yet. This function returns true if this is the case for this device.
	 */
	bool is_implicit() const { return implicit; }

	/**
	 * Special for GPT disk labels: If disk was enlarged, the backup
	 * partition table at the end of the disk might need to be moved to the
	 * new actual end of the disk. This function returns that flag.
	 */
	bool is_gpt_undersized() const { return gpt_undersized; }

	/**
	 * Special for GPT disk labels: True if the backup GPT is broken.
	 */
	bool is_gpt_backup_broken() const { return gpt_backup_broken; }

	/**
	 * Special for GPT disk labels: True if the protective MBR boot flag is set.
	 */
	bool is_gpt_pmbr_boot() const { return gpt_pmbr_boot; }

	/**
	 * Get the partition entries.
	 */
	const vector<Entry>& get_entries() const { return entries; }

	/**
	 * Get the partition entry with the specified number (1..n) and return
	 * it in 'entry'. Return 'true' upon success, 'false' if there is no
	 * entry with that number.
	 */
	bool get_entry(unsigned int number, Entry& entry) const;

	/**
	 * Mapping from partition ids to parted flag names.
	 */
	static const map<unsigned int, const char*> id_to_name;

	/**
	 * Mapping from partition ids to GPT partition type UUIDs.
	 */
	static const map<unsigned int, const char*> id_to_uuid;

    private:

	typedef vector<Entry>::const_iterator const_iterator;

	const string device;

	PtType label = PtType::UNKNOWN;
	Region region;
	int primary_slots = -1;
	bool implicit = false;
	bool gpt_undersized = false;
	bool gpt_backup_broken = false;
	bool gpt_pmbr_boot = false;
	vector<Entry> entries;
	vector<string> stderr;

	int logical_sector_size = 0;
	int physical_sector_size = 0;

	/**
	 * Parse the output of the 'parted' command in 'lines'.
	 * This may throw a ParseException.
	 */
	void parse(const vector<string>& stdout, const vector<string>& stderr);

	/**
	 * parted reports wrong sector sizes on DASDs, see
	 * https://bugzilla.suse.com/show_bug.cgi?id=866535 and
	 * http://lists.alioth.debian.org/pipermail/parted-devel/2014-March/004443.html.
	 */
	void fix_dasd_sector_size();

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
	const vector<string>& get_stderr() const { return stderr; }

	void scan_device_line(const string& line);
	void scan_device_flags(const string& s);

	void scan_device(json_object* node);
	void scan_device_flags(json_object* node);

	PtType scan_partition_table_type(const string& s) const;

	void scan_entry_line(const string& line);
	void scan_entry_flags(const string& s, Entry& entry) const;

	void scan_entry(json_object* node);
	void scan_entry_flags(json_object* node, Entry& entry) const;

	unsigned long long scan_sectors(const string& s) const;

	void scan_stderr(const vector<string>& stderr);

	vector<string> tokenize(const string& line) const;

	static const map<const char*, PtType> name_to_pt_type;

    };


    class CmdPartedVersion
    {
    public:

	static void query_version();
	static void parse_version(const string& version);

	static bool supports_json_option();
	static bool supports_type_command();
	static bool supports_old_type_flag();
	static bool supports_wipe_signatures();
	static bool supports_ignore_busy();
	static bool print_triggers_udev();
	static bool supports_no_automount_flag();

    private:

	static bool did_set_version;

	static int major;
	static int minor;
	static int patchlevel;

    };

}


#endif
