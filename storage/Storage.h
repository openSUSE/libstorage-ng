/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2018] SUSE LLC
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
#include <utility>
#include <memory>
#include <boost/noncopyable.hpp>

#include "storage/Filesystems/Mountable.h"
#include "storage/CommitOptions.h"
#include "storage/Utils/Callbacks.h"


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


    /**
     *
     *
     * Other storage subsystems are activated automatically, e.g. LVM and MD
     * RAID. This cannot be controlled since it is partly done by udev. This
     * also means that the callbacks may change anytime when e.g. udev
     * changes.
     */
    class ActivateCallbacks : public Callbacks
    {
    public:

	virtual ~ActivateCallbacks() {}

	/**
	 * Decide whether multipath should be activated.
	 *
	 * The looks_like_real_multipath paramter is not reliable.
	 */
	virtual bool multipath(bool looks_like_real_multipath) const = 0;

	/**
	 * Decide whether the LUKS with uuid should be activated.
	 *
	 * TODO Add label parameter for LUKS2.
	 */
	virtual std::pair<bool, std::string> luks(const std::string& uuid, int attempt) const = 0;

    };


    /**
     * Provides information whether deactivate() was able to deactivate
     * subsystems. True indicates that the corresponding subsystem was
     * deactivate or not activate in the first place.
     */
    struct DeactivateStatus
    {
	bool multipath;
	bool dm_raid;
	bool md;
	bool lvm_lv;
	bool luks;
    };


    class ProbeCallbacks : public Callbacks
    {
    public:

	virtual ~ProbeCallbacks() {}

    };


    class CheckCallbacks
    {
    public:

	virtual ~CheckCallbacks() {}

	virtual void error(const std::string& message) const = 0;

    };


    class CommitCallbacks : public Callbacks
    {
    public:

	virtual ~CommitCallbacks() {}

    };


    //! The main entry point to libstorage.
    class Storage : private boost::noncopyable
    {
    public:

	/**
	 * Construct Storage object. Does not call activate() nor probe().
	 *
	 * @throw Exception
	 */
	Storage(const Environment& environment);

	~Storage();

    public:

	const Environment& get_environment() const;
	const Arch& get_arch() const;

	/**
	 * @throw Exception
	 */
	Devicegraph* create_devicegraph(const std::string& name);

	/**
	 * @throw Exception
	 */
	Devicegraph* copy_devicegraph(const std::string& source_name, const std::string& dest_name);

	/**
	 * @throw Exception
	 */
	void remove_devicegraph(const std::string& name);

	/**
	 * @throw Exception
	 */
	void restore_devicegraph(const std::string& name);

	bool equal_devicegraph(const std::string& lhs, const std::string& rhs) const;

	bool exist_devicegraph(const std::string& name) const;
	std::vector<std::string> get_devicegraph_names() const;

	/**
	 * Return a devicegraph by name.
	 *
	 * @throw Exception
	 */
	Devicegraph* get_devicegraph(const std::string& name);

	/**
	 * Return a devicegraph by name.
	 *
	 * @throw Exception
	 */
	const Devicegraph* get_devicegraph(const std::string& name) const;

	/**
	 * Return the staging devicegraph.
	 *
	 * @throw Exception
	 */
	Devicegraph* get_staging();

	/**
	 * Return the staging devicegraph.
	 *
	 * @throw Exception
	 */
	const Devicegraph* get_staging() const;

	/**
	 * Return the probed devicegraph.
	 *
	 * @throw Exception
	 */
	const Devicegraph* get_probed() const;

	/**
	 * Return the system devicegraph.
	 *
	 * @throw Exception
	 */
	Devicegraph* get_system();

	/**
	 * Return the system devicegraph.
	 *
	 * @throw Exception
	 */
	const Devicegraph* get_system() const;

	/**
	 * Checks all devicegraphs.
	 *
	 * There are two types of errors that can be found:
	 *
	 * Errors that indicate a problem inside the library or a severe
	 * misuse of the library, e.g. attaching a BlkFilesystem directly to a
	 * PartitionTable. For these errors an exception is thrown.
	 *
	 * Errors that can be easily fixed by the user, e.g. an overcommitted
	 * volume group. For these errors CheckCallbacks::error() is called.
	 *
	 * The checks are WIP.
	 *
	 * @throw Exception
	 */
	void check(const CheckCallbacks* check_callbacks = nullptr) const;

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
	 * Prepends the root prefix to a mount point if necessary.
	 */
	std::string prepend_rootprefix(const std::string& mount_point) const;

	/**
	 * The actiongraph is only valid until either the probed or staging
	 * devicegraph is modified.
	 *
	 * @throw Exception
	 */
	const Actiongraph* calculate_actiongraph();

	/**
	 * Activate devices like multipath, DM and MD RAID, LVM and LUKS. It
	 * is not required to have probed the system to call this function. On
	 * the other hand after calling activate() the system should be
	 * probed.
	 *
	 * The message callback is unreliable since certain subsystems can be
	 * activated automatically (e.g. MD RAIDs by udev).
	 *
	 * If an error reported via activate_callbacks is not ignored the
	 * function throws Aborted.
	 *
	 * This function is only intended for the installation system.
	 *
	 * @throw Aborted, Exception
	 */
	void activate(const ActivateCallbacks* activate_callbacks) const;

	/**
	 * Deactivate devices like multipath, DM and MD RAID, LVM and LUKS. It
	 * is not required to have probed the system to call this function. On
	 * the other hand after calling this function activate() should be
	 * called and the system should be probed.
	 *
	 * The purpose of this function is to reverse the actions of
	 * activate() and auto-assemble.
	 *
	 * This function is only intended for the installation system.
	 *
	 * @throw Exception
	 */
	DeactivateStatus deactivate() const;

	/**
	 * Probe the system and replace the probed, system and staging
	 * devicegraphs.
	 *
	 * If an error reported via probe_callbacks is not ignored the
	 * function throws Aborted.
	 *
	 * @throw Aborted, Exception
	 */
	void probe(const ProbeCallbacks* probe_callbacks = nullptr);

	/**
	 * The actiongraph must be valid.
	 *
	 * If an error reported via commit_callbacks is not ignored the
	 * function throws Aborted.
	 *
	 * @throw Aborted, Exception
	 */
	void commit(const CommitOptions& commit_options, const CommitCallbacks* commit_callbacks = nullptr);

	/**
	 * The actiongraph must be valid.
	 *
	 * If an error reported via commit_callbacks is not ignored the
	 * function throws Aborted.
	 *
	 * @throw Aborted, Exception
	 */
	void commit(const CommitCallbacks* commit_callbacks = nullptr) ST_DEPRECATED;

    public:

	class Impl;

	Impl& get_impl() { return *impl; }
	const Impl& get_impl() const { return *impl; }

    private:

	const std::unique_ptr<Impl> impl;

    };

}

#endif
