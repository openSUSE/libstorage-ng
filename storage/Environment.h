/*
 * Copyright (c) [2014-2015] Novell, Inc.
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

    //! Is the target a disk, chroot, or image?
    enum class TargetMode {
	DIRECT,			// direct target
	CHROOT,			// the target is chrooted, e.g. inst-sys
	IMAGE			// the target is image based
    };


    class Environment
    {
    public:

	Environment(bool read_only);
	Environment(bool read_only, ProbeMode probe_mode, TargetMode target_mode);
	Environment(const Environment& environment);
	~Environment();

	Environment& operator=(const Environment& environment) = delete;

	bool is_read_only() const;

	ProbeMode get_probe_mode() const;

	TargetMode get_target_mode() const;

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
