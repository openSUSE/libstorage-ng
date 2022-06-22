/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) 2022 SUSE LLC
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


#ifndef STORAGE_ENVIRONMENT_H
#define STORAGE_ENVIRONMENT_H


#include <string>
#include <memory>


namespace storage
{

    //! How to probe the system
    enum class ProbeMode {
	STANDARD,			// probe system during init
	STANDARD_WRITE_DEVICEGRAPH,	// probe system during init, write devicegraph
	STANDARD_WRITE_MOCKUP,		// probe system during init, write mockup
	NONE,				// no probing - for testsuite
	READ_DEVICEGRAPH,		// fake probe - for testsuite
	READ_MOCKUP			// fake probe - for testsuite
    };


    /**
     * Enum with target modes.
     */
    enum class TargetMode {

	/** Direct target. */
	DIRECT,

	/** The target is chrooted, e.g. inst-sys. */
	CHROOT,

	/** The target is image based. Experimental. */
	IMAGE

    };


    class Environment
    {
    public:

	Environment(bool read_only);
	Environment(bool read_only, ProbeMode probe_mode, TargetMode target_mode);
	Environment(const Environment& environment);
	~Environment();

	Environment& operator=(const Environment& environment) = delete;

	/**
	 * Return whether the Environment has read-only set.
	 */
	bool is_read_only() const;

	/**
	 * Return the probe mode.
	 *
	 * @see ProbeMode
	 */
	ProbeMode get_probe_mode() const;

	/**
	 * return the target mode.
	 *
	 * @see TargetMode
	 */
	TargetMode get_target_mode() const;

	/**
	 * Query the rootprefix.
	 *
	 * The rootprefix is prepended to certain paths when accessing the system,
	 * e.g. when opening files like /etc/fstab and when mounting filesystems (that
	 * have rootprefixed set).
	 *
	 * The rootprefix is at least used for activate(), probe() and commit().
	 */
	const std::string& get_rootprefix() const;

	/**
	 * Set the rootprefix.
	 *
	 * If a rootprefix is not needed it has to be empty.
	 *
	 * @see get_rootprefix()
	 */
	void set_rootprefix(const std::string& rootprefix);

	const std::string& get_devicegraph_filename() const;
	void set_devicegraph_filename(const std::string& devicegraph_filename);

	const std::string& get_arch_filename() const;
	void set_arch_filename(const std::string& arch_filename);

	const std::string& get_mockup_filename() const;
	void set_mockup_filename(const std::string& mockup_filename);

	friend std::ostream& operator<<(std::ostream& out, const Environment& environment);

    public:

        class Impl;

        Impl& get_impl() { return *impl; }
        const Impl& get_impl() const { return *impl; }

    private:

	const std::unique_ptr<Impl> impl;

    };

}

#endif
