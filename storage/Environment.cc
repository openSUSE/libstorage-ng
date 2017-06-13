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


#include "storage/EnvironmentImpl.h"


namespace storage
{

    Environment::Environment(bool read_only)
	: Environment(read_only, ProbeMode::STANDARD, TargetMode::DIRECT)
    {
    }


    Environment::Environment(bool read_only, ProbeMode probe_mode, TargetMode target_mode)
	: impl(new Impl(read_only, probe_mode, target_mode))
    {
    }


    Environment::Environment(const Environment& environment)
	: impl(new Impl(environment.get_impl()))
    {
    }


    Environment::~Environment()
    {
    }


    bool
    Environment::is_read_only() const
    {
	return get_impl().is_read_only();
    }


    ProbeMode
    Environment::get_probe_mode() const
    {
	return get_impl().get_probe_mode();
    }


    TargetMode
    Environment::get_target_mode() const
    {
	return get_impl().get_target_mode();
    }


    const string&
    Environment::get_devicegraph_filename() const
    {
	return get_impl().get_devicegraph_filename();
    }


    void
    Environment::set_devicegraph_filename(const string& devicegraph_filename)
    {
	get_impl().set_devicegraph_filename(devicegraph_filename);
    }


    const string&
    Environment::get_arch_filename() const
    {
	return get_impl().get_arch_filename();
    }


    void
    Environment::set_arch_filename(const string& arch_filename)
    {
	get_impl().set_arch_filename(arch_filename);
    }


    const string&
    Environment::get_mockup_filename() const
    {
	return get_impl().get_mockup_filename();
    }


    void
    Environment::set_mockup_filename(const string& mockup_filename)
    {
	get_impl().set_mockup_filename(mockup_filename);
    }


    std::ostream&
    operator<<(std::ostream& out, const Environment& environment)
    {
	return operator<<(out, environment.get_impl());
    }

}
