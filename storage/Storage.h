/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#ifndef STORAGE_STORAGE_H
#define STORAGE_STORAGE_H


#include <string>
#include <vector>
#include <map>
#include <utility>
#include <memory>
#include <boost/noncopyable.hpp>

#include "storage/CommitOptions.h"
#include "storage/Actions/Base.h"
#include "storage/Utils/Callbacks.h"
#include "storage/Utils/Swig.h"


/**
 * The storage namespace.
 */
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
    class Pool;
    enum class PtType;
    enum class FsType;
    enum class MountByType;


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
	 * The result of the callback is cached (for the lifetime of
	 * the program). So the callback for a specific LUKS will not
	 * be called again (not even if Storage::activate() is called
	 * again) if 1. the activation was canceled or 2. the
	 * activation was successful.
	 *
	 * TODO Also allow to return a key file?
	 */
	virtual std::pair<bool, std::string> luks(const std::string& uuid, int attempt) const = 0;

    };


    /**
     * Stores information about a LUKS device.
     */
    class LuksInfo : private boost::noncopyable
    {
    public:

	LuksInfo();
	~LuksInfo();

	const std::string& get_device_name() const;
	unsigned long long get_size() const;
	const std::string& get_dm_table_name() const;
	bool is_dm_table_name_generated() const;
	const std::string& get_uuid() const;
	const std::string& get_label() const;

    public:

	class Impl;

	Impl& get_impl() { return *impl; }
	const Impl& get_impl() const { return *impl; }

    private:

	const std::unique_ptr<Impl> impl;

    };


    /**
     * Specialized callbacks with a more generic parameter for LUKS activation.
     */
    class ActivateCallbacksLuks : public ActivateCallbacks
    {
    public:

	virtual ~ActivateCallbacksLuks() {}

	/**
	 * Decide whether the LUKS should be activated.
	 *
	 * Parameter "info" contains all known information about the LUKS device.
	 */
	virtual std::pair<bool, std::string> luks(const LuksInfo& luks_info, int attempt) const = 0;

    };


    /**
     * Stores information about a Bitlocker device.
     */
    class BitlockerInfo : private boost::noncopyable
    {
    public:

	BitlockerInfo();
	~BitlockerInfo();

	const std::string& get_device_name() const;
	unsigned long long get_size() const;
	const std::string& get_dm_table_name() const;
	bool is_dm_table_name_generated() const;
	const std::string& get_uuid() const;

    public:

	class Impl;

	Impl& get_impl() { return *impl; }
	const Impl& get_impl() const { return *impl; }

    private:

	const std::unique_ptr<Impl> impl;

    };


    /**
     * Specialized callbacks with a more generic parameter for Bitlocker activation.
     */
    class ActivateCallbacksV3 : public ActivateCallbacksLuks
    {
    public:

	virtual ~ActivateCallbacksV3() {}

	/**
	 * Called at the begin of activation.
	 */
	virtual void begin() const {}

	/**
	 * Called at the end of activation.
	 */
	virtual void end() const {}

	/**
	 * Decide whether the BitLocker should be activated.
	 *
	 * Parameter "info" contains all known information about the BitLocker device.
	 */
	virtual std::pair<bool, std::string> bitlocker(const BitlockerInfo& bitlocker_info, int attempt) const = 0;

    };


    struct ST_DEPRECATED DeactivateStatus
    {
	bool multipath;
	bool dm_raid;
	bool md;
	bool lvm_lv;
	bool luks;
    };


    /**
     * Provides information whether deactivate_v2() was able to deactivate
     * subsystems. True indicates that the corresponding subsystem was deactivate or not
     * activate in the first place.
     */
    class DeactivateStatusV2
    {
    public:

	DeactivateStatusV2();
	DeactivateStatusV2(const DeactivateStatusV2& deactivate_status);
	virtual ~DeactivateStatusV2();

	DeactivateStatusV2& operator=(const DeactivateStatusV2& deactivate_status);

	bool multipath() const;
	bool dm_raid() const;
	bool md() const;
	bool lvm_lv() const;
	bool luks() const;
	bool bitlocker() const;

    public:

	class Impl;

	Impl& get_impl() { return *impl; }
	const Impl& get_impl() const { return *impl; }

    private:

	const std::unique_ptr<Impl> impl;

    };


    class ProbeCallbacks : public Callbacks
    {
    public:

	virtual ~ProbeCallbacks() {}

    };


    class ProbeCallbacksV2 : public ProbeCallbacks
    {
    public:

	virtual ~ProbeCallbacksV2() {}

	/**
	 * Callback for missing commands.
	 *
	 * message is translated. what is usually not translated and
	 * may be empty. command is the missing command.
	 *
	 * used_features reports for which storage feature the command
	 * is needed. used_features may be zero, e.g. if a command
	 * fails for which the library has hard dependencies.
	 *
	 * If it returns true the error is ignored as good as possible.
	 */
	virtual bool missing_command(const std::string& message, const std::string& what,
				     const std::string& command, uint64_t used_features) const = 0;

    };


    class ProbeCallbacksV3 : public ProbeCallbacksV2
    {
    public:

	virtual ~ProbeCallbacksV3() {}

	/**
	 * Called at the begin of probing.
	 */
	virtual void begin() const {}

	/**
	 * Called at the end of probing.
	 */
	virtual void end() const {}

    };


    class ProbeCallbacksV4 : public ProbeCallbacksV3
    {
    public:

	virtual ~ProbeCallbacksV4() {}

	/**
	 * This error callback is called when a partition table and a filesystem are found
	 * on a device.
	 *
	 * @see error()
	 */
	virtual bool ambiguity_partition_table_and_filesystem(const std::string& message, const std::string& what,
							      const std::string& name, PtType pt_type,
							      FsType fs_type) const = 0;

	/**
	 * This error callback is called when a partition table and a LUKS are found on a
	 * device.
	 *
	 * @see error()
	 */
	virtual bool ambiguity_partition_table_and_luks(const std::string& message, const std::string& what,
							const std::string& name, PtType pt_type) const = 0;

	/**
	 * This error callback is called when a partition table and a LVM PV are found on
	 * a device.
	 *
	 * @see error()
	 */
	virtual bool ambiguity_partition_table_and_lvm_pv(const std::string& message, const std::string& what,
							  const std::string& name, PtType pt_type) const = 0;

	/**
	 * This error callback is called when a known but unsupported partition table
	 * type, e.g. AIX, is found. If parted reports an (to libstorage-ng) unknown
	 * partition table type, e.g. "super-pt", no error is reported. And surely there
	 * can be partition table types not even parted knows.
	 *
	 * @see error()
	 */
	virtual bool unsupported_partition_table(const std::string& message, const std::string& what,
						 const std::string& name, PtType pt_type) const = 0;

	/**
	 * This error callback is called when a known but unsupported filesystem type,
	 * e.g. MINIX, is found. If blkid reports an (to libstorage-ng) unknown filesystem
	 * type, e.g. "super-fs", no error is reported. And surely there can be filesystem
	 * types not even blkid knows.
	 *
	 * @see error()
	 */
	virtual bool unsupported_filesystem(const std::string& message, const std::string& what,
					    const std::string& name, FsType fs_type) const = 0;

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


    class CommitCallbacksV2 : public CommitCallbacks
    {
    public:

	/**
	 * Called at the begin of commit.
	 */
	virtual void begin() const {}

	/**
	 * Called at the end of commit.
	 */
	virtual void end() const {}

	/**
	 * Called at the begin of commit of a single action.
	 */
	virtual void begin_action(const Action::Base* action) const {}

	/**
	 * Called at the end of commit of a single action.
	 */
	virtual void end_action(const Action::Base* action) const {}

	virtual ~CommitCallbacksV2() {}

    };

    //! The main entry point to libstorage.
    class Storage : private boost::noncopyable
    {
    public:

	/**
	 * Construct Storage object. Does not call activate() nor probe().
	 *
	 * @throw LockException, Exception
	 */
	Storage(const Environment& environment);

	~Storage();

    public:

	const Environment& get_environment() const;
	const Arch& get_arch() const;

	/**
	 * Create a devicegraph with name.
	 *
	 * @throw Exception
	 */
	Devicegraph* create_devicegraph(const std::string& name);

	/**
	 * @throw Exception
	 */
	Devicegraph* copy_devicegraph(const std::string& source_name, const std::string& dest_name);

	/**
	 * Remove a devicegraph by name.
	 *
	 * @throw Exception
	 */
	void remove_devicegraph(const std::string& name);

	/**
	 * @throw Exception
	 */
	void restore_devicegraph(const std::string& name);

	bool equal_devicegraph(const std::string& lhs, const std::string& rhs) const;

	/**
	 * Check whether a devicegraph exists by name.
	 */
	bool exist_devicegraph(const std::string& name) const;

	/**
	 * Get the names of all devicegraphs.
	 */
	std::vector<std::string> get_devicegraph_names() const ST_DEPRECATED;

	/**
	 * Get all devicegraphs with their names.
	 *
	 * Note: There is no non-const version since that would provide non-const access
	 * to the probed devicegraph.
	 */
	std::map<std::string, const Devicegraph*> get_devicegraphs() const;

	/**
	 * Return a devicegraph by name.
	 *
	 * @throw Exception
	 */
	Devicegraph* get_devicegraph(const std::string& name);

	/**
	 * @copydoc get_devicegraph()
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
	void set_rootprefix(const std::string& rootprefix) ST_DEPRECATED;

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
	 * @throw Exception
	 */
	DeactivateStatus deactivate() const ST_DEPRECATED;

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
	DeactivateStatusV2 deactivate_v2() const;

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

	/**
	 * Generate pools, e.g. "HDDs (512 B)" for HDDs with 512 B sector size and "SSDs
	 * (4 KiB)" for SSDs with 4 KiB sector size. The pools generated and their names
	 * may change anytime.
	 *
	 * This function does not remove pools or devices from pools. It only adds things.
	 */
	void generate_pools(const Devicegraph* devicegraph);

	/**
	 * Create a pool with name.
	 *
	 * @throw Exception
	 */
	Pool* create_pool(const std::string& name);

	/**
	 * Remove a pool by name.
	 *
	 * @throw Exception
	 */
	void remove_pool(const std::string& name);

	/**
	 * Rename a pool.
	 *
	 * @throw Exception
	 */
	void rename_pool(const std::string& old_name, const std::string& new_name);

	/**
	 * Check whether a pool exists by name.
	 */
	bool exists_pool(const std::string& name) const;

	/**
	 * Get the names of all pools.
	 */
	std::vector<std::string> get_pool_names() const ST_DEPRECATED;

	/**
	 * Get all pools with their names.
	 */
	std::map<std::string, Pool*> get_pools();

	/**
	 * @copydoc get_pools()
	 */
	std::map<std::string, const Pool*> get_pools() const;

	/**
	 * Return a pool by name.
	 *
	 * @throw Exception
	 */
	Pool* get_pool(const std::string& name);

	/**
	 * @copydoc get_pool()
	 */
	const Pool* get_pool(const std::string& name) const;

    public:

	class Impl;

	Impl& get_impl() { return *impl; }
	const Impl& get_impl() const { return *impl; }

    private:

	const std::unique_ptr<Impl> impl;

    };

}

#endif
