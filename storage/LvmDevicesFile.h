/*
 * Copyright (c) 2025 SUSE LLC
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


#ifndef STORAGE_LVM_DEVICES_FILE_H
#define STORAGE_LVM_DEVICES_FILE_H


namespace storage
{

    class Devicegraph;


    class LvmDevicesFile
    {
    public:

	/**
	 * Status of LVM devices file.
	 */
	enum class Status
	{
	    /**
	     * Use of the LVM devices file is disabled.
	     */
	    DISABLED,

	    /**
	     * Use of the LVM devices file is enabled.
	     */
	    ENABLED,

	    /**
	     * Use of the LVM devices file is enabled but the file itself is missing.
	     */
	    MISSING
	};


	/**
	 * Query the status of the LVM devices file.
	 *
	 * @throw Exception
	 */
	static Status status();


	/**
	 * Add all LVM PVs in the devicegraph to the LVM devices file thus creating it. If
	 * the LVM devices file already exists libstorage-ng cannot see PVs on disk that
	 * are not included in the LVM devices file.
	 *
	 * Should only be called if the status is MISSING.
	 *
	 * If the status is ENABLED even libstorage-ng does not see LVM devices not
	 * already in the LVM devices file.
	 *
	 * If the status is DISABLED the LVM devices file is ignored anyway.
	 *
	 * @throw Exception
	 */
	static void create(const Devicegraph* devicegraph);

    };

}


#endif
