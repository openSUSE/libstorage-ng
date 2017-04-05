/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) 2016 SUSE LLC
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


#ifndef STORAGE_STORAGE_H
#define STORAGE_STORAGE_H


#include <string>
#include <vector>
#include <memory>
#include <boost/noncopyable.hpp>

#include "storage/Filesystems/Mountable.h"


namespace storage
{
    /**
     * \mainpage libstorage-ng Documentation
     *
     * \section Thread-Safety Thread Safety
     *
     * There is no guarantee about the thread safety of libstorage.
     *
     * \section Exceptions-and-Side-Effects Exceptions and Side Effects
     *
     * There is no guarantee that functions have no side effects if an
     * exception is thrown unless stated differently.
     *
     * \section Sorting Sorting
     *
     * Results are unsorted unless stated differently.
     *
     * \section Locking Locking
     *
     * During initialisation libstorage installs a global lock so that several
     * programs trying to use libstorage at the same time do not
     * interfere. This lock is either read-only or read-write depending on the
     * read_only parameter used in \link storage::Environment() \endlink.
     *
     * Several processes may hold a read-lock, but only one process may hold a
     * read-write lock. An read-write lock excludes all other locks, both
     * read-only and read-write.
     *
     * The support for multiple read-only locks is experimental.
     *
     * Locking may also fail for other reasons, e.g. limited permissions.
     */


    class Environment;
    class Arch;
    class Devicegraph;
    class Actiongraph;


    namespace Action
    {
	class Base;
    }


    class CommitCallbacks
    {
    public:

	virtual ~CommitCallbacks() {}

	virtual void message(const std::string& message) const = 0;
	virtual bool error(const std::string& message, const std::string& what) const = 0;

	// TODO to make pre and post generally usable the Action classes must
	// be included in the public libstorage interface
	virtual void pre(const Action::Base* action) const {}
	virtual void post(const Action::Base* action) const {}

    };


    //! The main entry point to libstorage.
    class Storage : private boost::noncopyable
    {
    public:

	Storage(const Environment& environment);
	~Storage();

    public:

	const Environment& get_environment() const;
	const Arch& get_arch() const;

	Devicegraph* create_devicegraph(const std::string& name);
	Devicegraph* copy_devicegraph(const std::string& source_name, const std::string& dest_name);
	void remove_devicegraph(const std::string& name);
	void restore_devicegraph(const std::string& name);

	bool equal_devicegraph(const std::string& lhs, const std::string& rhs) const;

	bool exist_devicegraph(const std::string& name) const;
	std::vector<std::string> get_devicegraph_names() const;

	/**
	 * Return a devicegraph by name.
	 */
	Devicegraph* get_devicegraph(const std::string& name);

	/**
	 * Return a devicegraph by name.
	 */
	const Devicegraph* get_devicegraph(const std::string& name) const;

	/**
	 * Return the staging devicegraph.
	 */
	Devicegraph* get_staging();

	/**
	 * Return the staging devicegraph.
	 */
	const Devicegraph* get_staging() const;

	/**
	 * Return the probed devicegraph.
	 */
	const Devicegraph* get_probed() const;

	void check() const;

	/**
	 * Query the default mount-by method.
	 */
	MountByType get_default_mount_by() const;

	/**
	 * Set the default mount-by method.
	 */
	void set_default_mount_by(MountByType default_mount_by);

	const std::string& get_rootprefix() const;
	void set_rootprefix(const std::string& rootprefix);

	/**
	 * Prepends the root prefix to a mountpoint if necessary.
	 */
	std::string prepend_rootprefix(const std::string& mountpoint) const;

	/**
	 * The actiongraph is only valid until either the probed or staging
	 * devicegraph is modified.
	 */
	const Actiongraph* calculate_actiongraph();

	/**
	 * The actiongraph must be valid.
	 */
	void commit(const CommitCallbacks* commit_callbacks = nullptr);

    public:

	class Impl;

	Impl& get_impl() { return *impl; }
	const Impl& get_impl() const { return *impl; }

    private:

	const std::unique_ptr<Impl> impl;

    };

}

#endif
