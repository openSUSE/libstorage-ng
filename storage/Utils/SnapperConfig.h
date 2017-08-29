/*
 * Copyright (c) 2017 SUSE LLC
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
 * with this program; if not, contact SUSE LLC.
 *
 * To contact SUSE LLC about this file by physical or electronic mail, you may
 * find current contact information at www.suse.com.
 */


#ifndef STORAGE_SNAPPER_CONFIG_H
#define STORAGE_SNAPPER_CONFIG_H


#include <string>
#include <vector>


namespace storage
{
    using std::string;
    using std::vector;

    class Btrfs;


    /**
     * Helper class to configure snapper during system installation for a Btrfs
     * root filesystem. This calls the installation-helper binary from the
     * snapper project as an external command.
     *
     * This handles the first few steps of that process (step1-3) that need to
     * be done while the Btrfs filesystem is being created. Subsequent steps
     * (4-6) need to be called from the outside after package installation is
     * complete to create the snapper configuration and the first snapshot.
     **/
    class SnapperConfig
    {
    public:
        SnapperConfig( Btrfs * btrfs );

        /**
         * Hook to be called after the Btrfs root filesystem is created.
         * This executes installation-helper step 1:
         *
         * Temporarily mount the btrfs on the root device, create the
         * directories (recursively) for /etc/snapper/configs and a
         * configuration file for the root filesystem. Then create a first
         * single snapshot and set this to the default snapshot.
         **/
        void post_filesystem_create();

        /**
         * Hook to be called after the Btrfs root filesystem is mounted.
         * This executes installation-helper step 2:
         * Mount the @/.snapshots or .snapshots subvolume.
         **/
        void post_mount();

        /**
         * Hook to be called after the Btrfs root filesystem is added to
         * /etc/fstab.
         *
         * This executes installation-helper step 3:
         * Add @/.snapshots or .snapshots to /etc/fstab.
         **/
        void post_add_to_etc_fstab();

        /**
         * Return the Btrfs filesystem this object works on.
         **/
        Btrfs * get_btrfs() const { return btrfs; }

        /**
         * Enable or disable actually executing the external commands.  By
         * default, they are enabled. Disable this for test suites and check
         * what command would have been executed with get_last_command_line().
         **/
        void set_do_exec( bool exec ) { do_exec = exec; }

        /**
         * Return the current value of enabling or disabling executing external
         * commands.
         **/
        bool get_do_exec() const { return do_exec; }

        /**
         * Return the last executed command line.
         **/
        const string & get_last_command_line() const { return command_line; }

    protected:

        /**
         * Call the external installation-helper program with arguments.
         * Return true if success, false if error.
         **/
        bool installation_helper( const vector<string> & args );

        /**
         * Build a command line from a command and its arguments.
         * Wrap all arguments that don't start with '--' in single quotes.
         **/
        string build_command_line( const string & command,
                                   const vector<string> & args ) const;

        /**
         * Check if snapper can and should be configured for this btrfs.
         * Return true if okay, false if not.
         **/
        bool sanity_check() const;

        /**
         * Return the device name of the parent of the btrfs (the partition or
         * disk or LVM volume group the btrfs is on).
         **/
        string get_parent_device_name() const;

        /**
         * Return the name of the default subvolume of the btrfs.
         **/
        string get_default_subvolume_name() const;

        /**
         * Return the root prefix of the installation target (the path where
         * the new root filesystem of the target is or will be mounted).
         **/
        string get_root_prefix() const;


    private:
        Btrfs * btrfs;
        bool    do_exec;
        string  command_line;
    };
}


#endif // STORAGE_SNAPPER_CONFIG_H
