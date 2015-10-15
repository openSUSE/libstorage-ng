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


#ifndef STORAGE_INTERFACE_H
#define STORAGE_INTERFACE_H


#include <string>
#include <deque>
#include <list>
#include <map>

using std::string;
using std::deque;
using std::list;
using std::map;


#include "storage/StorageVersion.h"
#include "storage/StorageSwig.h"


/*!
 * \mainpage libstorage Documentation
 *
 * \section Interface
 *
 * The functionality of libstorage is entirely accessed through the abstract
 * interface class \link storage::StorageInterface StorageInterface\endlink.
 * To ensure maximal possible compatibility user of libstorage must only
 * include the header file StorageInterface.h.
 *
 * \section Caching
 *
 * All modifying functions of libstorage can either operate on an internal
 * cache or directly on the system.
 *
 * When the caching mode is enabled a call of e.g. \link
 * storage::StorageInterface::createPartition() createPartition() \endlink
 * will only change the internal cache.  The user has to call \link
 * storage::StorageInterface::commit() commit() \endlink later on to actually
 * create the partition on the disk.
 *
 * When caching mode is disabled the call of e.g. \link
 * storage::StorageInterface::createPartition() createPartition() \endlink
 * will immediately create the partition on the disk.
 *
 * Caching mode can be set with \link
 * storage::StorageInterface::setCacheChanges() setCacheChanges() \endlink and
 * queried with \link storage::StorageInterface::isCacheChanges()
 * isCacheChanges()\endlink.
 *
 * \section Locking
 *
 * During initialisation libstorage installs a global lock so that several
 * programs trying to use libstorage at the same time do not interfere. This
 * lock is either read-only or read-write depending on the readonly parameter
 * used in \link storage::StorageInterface::createStorageInterface()
 * createStorageInterface() \endlink.
 *
 * Several processes may hold a read-lock, but only one process may hold a
 * read-write lock. An read-write lock excludes all other locks, both
 * read-only and read-write.
 *
 * The support for multiple read-only locks is experimental.
 *
 * Locking may also fail for other reasons, e.g. limited permissions.
 *
 * \section Configuration File
 *
 * During initialisation libstorage reads the default filesystem and default
 * mount-by method from /etc/sysconfig/storage. Libstorage does not write that
 * file.
 *
 * \section Nomenclature
 *
 * Sizes with postfix K are in kilobytes (1024 bytes).
 *
 * The only size in bytes instead of kilobytes is the cylinder size of disks.
 *
 * \section Example
 *
 * Here is a simple example to demonstrate the usage of libstorage:
 *
 * \code
 *
 * #include <storage/StorageInterface.h>
 *
 * using namespace storage;
 *
 * int
 * main()
 * {
 *     // Environment for StorageInterface with read-write enabled.
 *     Environment env(false);
 *
 *     // Create a concrete StorageInterface object.
 *     StorageInterface* s = createStorageInterface(env);
 *
 *     int ret;
 *     string name;
 *
 *     // Create a primary partition on /dev/sdb.
 *     ret = s->createPartitionKb("/dev/sdb", PRIMARY, 0, 100000, name);
 *
 *     // Commit the change to the system.
 *     ret = s->commit();
 *
 *     // Finally destroy the StorageInterface object.
 *     destroyStorageInterface(s);
 * }
 *
 * \endcode
 *
 * If you have installed the latest libstorage-devel package you can find more
 * examples in the directory /usr/share/doc/packages/libstorage/examples.
 */

namespace storage
{
    class Storage;
}

//! The legacy API
namespace storage_legacy
{
    //! Filesystem type (ext4, btrfs, ...)
    enum FsType { FSUNKNOWN, REISERFS, EXT2, EXT3, EXT4, BTRFS, VFAT, XFS, JFS, HFS, NTFS,
		  SWAP, HFSPLUS, NFS, NFS4, TMPFS, ISO9660, UDF, FSNONE };

    //! Partition type (primary, logical, ...)
    enum PartitionType { PRIMARY, EXTENDED, LOGICAL, PTYPE_ANY };

    //! The key by which the mount program identifies a filesystem
    enum MountByType { MOUNTBY_DEVICE, MOUNTBY_UUID, MOUNTBY_LABEL, MOUNTBY_ID, MOUNTBY_PATH };

    enum EncryptType { ENC_NONE, ENC_TWOFISH, ENC_TWOFISH_OLD,
		       ENC_TWOFISH256_OLD, ENC_LUKS, ENC_UNKNOWN };

    enum MdType { RAID_UNK, RAID0, RAID1, RAID5, RAID6, RAID10, MULTIPATH };

    enum MdParity { PAR_DEFAULT, LEFT_ASYMMETRIC, LEFT_SYMMETRIC,
		    RIGHT_ASYMMETRIC, RIGHT_SYMMETRIC, PAR_FIRST, PAR_LAST,
		    LEFT_ASYMMETRIC_6, LEFT_SYMMETRIC_6, RIGHT_ASYMMETRIC_6,
		    RIGHT_SYMMETRIC_6, PAR_FIRST_6,
		    PAR_NEAR_2, PAR_OFFSET_2, PAR_FAR_2,
		    PAR_NEAR_3, PAR_OFFSET_3, PAR_FAR_3 };

    enum MdArrayState { UNKNOWN, CLEAR, INACTIVE, SUSPENDED, READONLY, READ_AUTO,
			CLEAN, ACTIVE, WRITE_PENDING, ACTIVE_IDLE };

    enum UsedByType { UB_NONE, UB_LVM, UB_MD, UB_MDPART, UB_DM, UB_DMRAID, UB_DMMULTIPATH, UB_BTRFS };

    enum CType { CUNKNOWN, DISK, MD, LOOP, LVM, DM, DMRAID, NFSC, DMMULTIPATH, MDPART, BTRFSC, TMPFSC };

    //! Data Transport Layer
    enum Transport { TUNKNOWN, SBP, ATA, FC, ISCSI, SAS, SATA, SPI, USB, FCOE };

    enum MultipathAutostart { MPAS_UNDECIDED, MPAS_ON, MPAS_OFF };

    enum PartAlign { ALIGN_OPTIMAL, ALIGN_CYLINDER };


    /**
     * typedef for a pointer to a function that is called on progress bar
     * events.
     */
    typedef void (*CallbackProgressBar)(const string& id, unsigned cur, unsigned max);

    /**
     * typedef for a pointer to a function that is called with strings telling
     * the user what is currently going on.
     */
    typedef void (*CallbackShowInstallInfo)(const string& id);

    /**
     * typedef for a pointer to a function that displays a popup with the
     * given text and waits for user confirmation.
     */
    typedef void (*CallbackInfoPopup)(const string& text);

    /**
     * typedef for a pointer to a function that displays a popup with the
     * given text and two buttons labels "Yes" and "No". The user has to press
     * on of these buttons. If the user presses "Yes" true is returned, false
     * otherwise.
     */
    typedef bool (*CallbackYesNoPopup)(const string& text);

    /**
     * typedef for a pointer to a function that asks whether to continue after
     * an commit error. If true is returned the commit error is ignored and
     * the commit continues.
     */
    typedef bool (*CallbackCommitErrorPopup)(int error, const string& last_action,
					     const string& extended_message);

    /**
     * typedef for a pointer to a function that asks for the password of the
     * given device. If the user does not want to decrypt the device false is
     * returned, true otherwise.
     */
    typedef bool (*CallbackPasswordPopup)(const string& device, int attempts, string& password);


#if !defined(SWIG) || defined(LIBSTORAGE_SWIG_RUBY_LEGACY)

    /**
     * Abstract class for some callbacks during commit. Currently only works
     * for btrfs as root filesystem.
     */
    class CommitCallbacks
    {
    public:

	virtual ~CommitCallbacks() {}

	virtual void post_root_filesystem_create() const {}
	virtual void post_root_mount() const {}
	virtual void post_root_fstab_add() const {}

    };

#endif


    /**
     * Contains capabilities of a filesystem type.
     */
    struct FsCapabilities
    {
	FsCapabilities() {}
	bool isExtendable;
	bool isExtendableWhileMounted;
	bool isReduceable;
	bool isReduceableWhileMounted;
	bool supportsUuid;
	bool supportsLabel;
	bool labelWhileMounted;
	unsigned int labelLength;
	bool canFormat;
	unsigned long long minimalFsSizeK;
    };

    /**
     * Contains capabilities of a disk label.
     */
    struct DlabelCapabilities
    {
	DlabelCapabilities() {}
	unsigned maxPrimary;
	bool extendedPossible;
	unsigned maxLogical;
	unsigned long long maxSectors;
    };


    struct UsedByInfo
    {
	UsedByInfo() : type(UB_NONE) {} // only for swig bindings
	UsedByInfo(UsedByType type, const string& device) : type(type), device(device) {}
	UsedByType type;
	string device;
    };


    struct ResizeInfo
    {
	ResizeInfo() : df_freeK(0), resize_freeK(0), usedK(0), resize_ok(false) {}
	unsigned long long df_freeK;
	unsigned long long resize_freeK;
	unsigned long long usedK;
	bool resize_ok;
    };


    struct ContentInfo
    {
	ContentInfo() : windows(false), efi(false), homes(0) {}
	bool windows;
	bool efi;
	unsigned homes;
    };


    struct DeviceInfo
    {
	DeviceInfo() {}

	string device;
	string name;

	string udevPath;
	list<string> udevId;

	list<UsedByInfo> usedBy;

	map<string, string> userdata;
    };


    /**
     * Contains info about a generic container.
     */
    struct ContainerInfo : public DeviceInfo
    {
	ContainerInfo() {}
	CType type;
	bool readonly;
    };

    /**
     * Contains info about a disk
     */
    struct DiskInfo
    {
	DiskInfo() {}
	unsigned long long sizeK;
	unsigned long long cylSize;
	unsigned long cyl;
	unsigned long heads;
	unsigned long sectors;
	unsigned int sectorSize;
	string disklabel;
	string orig_disklabel;
	unsigned maxPrimary;
	bool extendedPossible;
	unsigned maxLogical;
	bool initDisk;
	Transport transport;
	bool has_fake_partition;
    };

    /**
     * Contains info about a LVM VG
     */
    struct LvmVgInfo
    {
	LvmVgInfo() {}
	unsigned long long sizeK;
	unsigned long long peSizeK;
	unsigned long peCount;
	unsigned long peFree;
	string uuid;
	bool lvm2;
	bool create;
	list<string> devices;
	list<string> devices_add;
	list<string> devices_rem;
    };

    /**
     * Contains info about a DmPart disk
     */
    struct DmPartCoInfo
    {
	DmPartCoInfo() {}
	DiskInfo d;
	list<string> devices;
	unsigned long minor;
    };

    struct DmraidCoInfo
    {
	DmraidCoInfo() {}
	DmPartCoInfo p;
    };

    struct DmmultipathCoInfo
    {
	DmmultipathCoInfo() {}
	DmPartCoInfo p;
	string vendor;
	string model;
    };

    /**
     * Contains info about a volume.
     */
    struct VolumeInfo : public DeviceInfo
    {
	VolumeInfo() {}
	unsigned long long sizeK;
	unsigned long major;
	unsigned long minor;
	string mount;
	string crypt_device;
	MountByType mount_by;
	bool ignore_fstab;
	string fstab_options;
	string uuid;
	string label;
	string mkfs_options;
	string tunefs_options;
	string loop;
	string dtxt;
	EncryptType encryption;
	string crypt_pwd;
	FsType fs;
	FsType detected_fs;
	bool format;
	bool create;
	bool is_mounted;
	bool resize;
	bool ignore_fs;
	unsigned long long origSizeK;
    };

    /**
     * Contains info about a region.
     */
    struct RegionInfo
    {
	RegionInfo() : start(0), len(0) {}
	RegionInfo(unsigned long long start, unsigned long long len) : start(start), len(len) {}
	unsigned long long start;
	unsigned long long len;
    };

    struct PartitionAddInfo
    {
	PartitionAddInfo() {}
	unsigned nr;
	RegionInfo cylRegion;
	PartitionType partitionType;
	unsigned id;
	bool boot;
    };

    /**
     * Contains info about a partition.
     */
    struct PartitionInfo
    {
	PartitionInfo() {}
	PartitionInfo& operator=( const PartitionAddInfo& rhs );
	VolumeInfo v;
	unsigned nr;
	RegionInfo cylRegion;
	PartitionType partitionType;
	unsigned id;
	bool boot;
    };

    /**
     * Contains info about a LVM LV.
     */
    struct LvmLvInfo
    {
	LvmLvInfo() {}
	VolumeInfo v;
	unsigned stripes;
	unsigned stripeSizeK;
	string uuid;
	string status;
	string dm_table;
	string dm_target;
	string origin;
	string used_pool;
        bool pool;
    };

    /**
     *
     */
    struct LvmLvSnapshotStateInfo
    {
	LvmLvSnapshotStateInfo() {}
	bool active;
	double allocated;
    };

    /**
     * Contains info about a software raid device.
     */
    struct MdInfo
    {
	MdInfo() {}
	VolumeInfo v;
	unsigned nr;
	unsigned type;
	unsigned parity;
	string uuid;
	string sb_ver;
	unsigned long chunkSizeK;
	list<string> devices;
	list<string> spares;
	bool inactive;
    };

    /**
     * Contains state info about a software raid device.
     */
    struct MdStateInfo
    {
	MdStateInfo() {}
	MdArrayState state;
    };

    /**
     * Contains info about a software raid device which is a container
     * for partitions.
     */
    struct MdPartCoInfo
    {
        MdPartCoInfo() {}
        DiskInfo d;
	unsigned type;        // RAID level
        unsigned nr;          // MD device number
        unsigned parity;      // Parity (not for all RAID level)
        string   uuid;        // MD Device UUID
        string   sb_ver;      // Metadata version
	unsigned long chunkSizeK;  // Chunksize (strip size)
	list<string> devices;
	list<string> spares;
    };

    struct MdPartCoStateInfo
    {
        MdPartCoStateInfo() {}
        MdArrayState state;
    };

    /**
     * Contains info about a partition on SW RAID device.
     */
    struct MdPartInfo
    {
        MdPartInfo() {}
        VolumeInfo v;
        PartitionAddInfo p;
        bool part;
    };

    /**
     * Contains info about a nfs volumes
     */
    struct NfsInfo
    {
	NfsInfo() {}
	VolumeInfo v;
    };

    /**
     * Contains info about a file based loop devices.
     */
    struct LoopInfo
    {
	LoopInfo() {}
	VolumeInfo v;
	bool reuseFile;
	unsigned nr;
	string file;
    };

    /**
     * Contains info about btrfs volume.
     */
    struct BtrfsInfo
    {
	BtrfsInfo() {}
	VolumeInfo v;
	list<string> devices;
	list<string> devices_add;
	list<string> devices_rem;
	list<string> subvol;
	list<string> subvol_add;
	list<string> subvol_rem;
    };

    /**
     * Contains info about tmpfs volume.
     */
    struct TmpfsInfo
    {
	TmpfsInfo() {}
	VolumeInfo v;
    };

    /**
     * Contains info about a DM volume.
     */
    struct DmInfo
    {
	DmInfo() {}
	VolumeInfo v;
	unsigned nr;
	string table;
	string target;
    };

    /**
     * Contains info about a DmPart volume.
     */
    struct DmPartInfo
    {
	DmPartInfo() {}
	VolumeInfo v;
	PartitionAddInfo p;
	bool part;
	string table;
	string target;
    };

    /**
     * Contains info about a DMRAID volume.
     */
    struct DmraidInfo
    {
	DmraidInfo() {}
	DmPartInfo p;
    };

    /**
     * Contains info about a DMMULTIPATH volume.
     */
    struct DmmultipathInfo
    {
	DmmultipathInfo() {}
	DmPartInfo p;
    };

    /**
     * Contains info about container and volume.
     */
    struct ContVolInfo
    {
	ContVolInfo() : ctype(CUNKNOWN), num(-1) {}
	CType ctype;
	string cname;
	string cdevice;
	string vname;
	string vdevice;
	int num;
    };

    /**
     * Contains info about a partition slot.
     */
    struct PartitionSlotInfo
    {
	PartitionSlotInfo() {}
	RegionInfo cylRegion;
	unsigned nr;
	string device;
	bool primarySlot;
	bool primaryPossible;
	bool extendedSlot;
	bool extendedPossible;
	bool logicalSlot;
	bool logicalPossible;
    };

    /**
     * Contains info about actions performed during commit().
     */
    struct CommitInfo
    {
	CommitInfo() {}
	bool destructive;
	string text;
    };


    /**
     * preliminary list of error codes, must have negative values
     */
    enum ErrorCodes
    {
	STORAGE_NO_ERROR = 0,

	DISK_PARTITION_OVERLAPS_EXISTING = -1000,
	DISK_PARTITION_EXCEEDS_DISK = -1001,
	DISK_CREATE_PARTITION_EXT_ONLY_ONCE = -1002,
	DISK_CREATE_PARTITION_EXT_IMPOSSIBLE = -1003,
	DISK_PARTITION_NO_FREE_NUMBER = -1004,
	DISK_CREATE_PARTITION_INVALID_VOLUME = -1005,
	DISK_CREATE_PARTITION_INVALID_TYPE = -1006,
	DISK_CREATE_PARTITION_PARTED_FAILED = -1007,
	DISK_PARTITION_NOT_FOUND = -1008,
	DISK_CREATE_PARTITION_LOGICAL_NO_EXT = -1009,
	DISK_PARTITION_LOGICAL_OUTSIDE_EXT = -1010,
	DISK_SET_TYPE_INVALID_VOLUME = -1011,
	DISK_SET_TYPE_PARTED_FAILED = -1012,
	DISK_SET_LABEL_PARTED_FAILED = -1013,
	DISK_REMOVE_PARTITION_PARTED_FAILED = -1014,
	DISK_REMOVE_PARTITION_INVALID_VOLUME = -1015,
	DISK_REMOVE_PARTITION_LIST_ERASE = -1016,
	DISK_DESTROY_TABLE_INVALID_LABEL = -1017,
	DISK_PARTITION_ZERO_SIZE = -1018,
	DISK_CHANGE_READONLY = -1019,
	DISK_RESIZE_PARTITION_INVALID_VOLUME = -1020,
	DISK_RESIZE_PARTITION_PARTED_FAILED = -1021,
	DISK_RESIZE_NO_SPACE = -1022,
	DISK_CHECK_RESIZE_INVALID_VOLUME = -1023,
	DISK_REMOVE_PARTITION_CREATE_NOT_FOUND = -1024,
	DISK_COMMIT_NOTHING_TODO = -1025,
	DISK_CREATE_PARTITION_NO_SPACE = -1026,
	DISK_REMOVE_USED_BY = -1027,
	DISK_INIT_NOT_POSSIBLE = -1028,
	DISK_INVALID_PARTITION_ID = -1029,

	STORAGE_DISK_NOT_FOUND = -2000,
	STORAGE_VOLUME_NOT_FOUND = -2001,
	STORAGE_REMOVE_PARTITION_INVALID_CONTAINER = -2002,
	STORAGE_CHANGE_PARTITION_ID_INVALID_CONTAINER = -2003,
	STORAGE_CHANGE_READONLY = -2004,
	STORAGE_DISK_USED_BY = -2005,
	STORAGE_LVM_VG_EXISTS = -2006,
	STORAGE_LVM_VG_NOT_FOUND = -2007,
	STORAGE_LVM_INVALID_DEVICE = -2008,
	STORAGE_CONTAINER_NOT_FOUND = -2009,
	STORAGE_VG_INVALID_NAME = -2010,
	STORAGE_REMOVE_USED_VOLUME = -2011,
	STORAGE_REMOVE_USING_UNKNOWN_TYPE = -2012,
	STORAGE_NOT_YET_IMPLEMENTED = -2013,
	STORAGE_MD_INVALID_NAME = -2014,
	STORAGE_MD_NOT_FOUND = -2015,
	STORAGE_MEMORY_EXHAUSTED = -2016,
	STORAGE_LOOP_NOT_FOUND = -2017,
	STORAGE_CREATED_LOOP_NOT_FOUND = -2018,
	STORAGE_CHANGE_AREA_INVALID_CONTAINER = -2023,
	STORAGE_BACKUP_STATE_NOT_FOUND = -2024,
	STORAGE_INVALID_FSTAB_VALUE = -2025,
	STORAGE_NO_FSTAB_PTR = -2026,
	STORAGE_DEVICE_NODE_NOT_FOUND = -2027,
	STORAGE_DMRAID_CO_NOT_FOUND = -2028,
	STORAGE_RESIZE_INVALID_CONTAINER = -2029,
	STORAGE_DMMULTIPATH_CO_NOT_FOUND = -2030,
	STORAGE_ZERO_DEVICE_FAILED = -2031,
	STORAGE_INVALID_BACKUP_STATE_NAME = -2032,
	STORAGE_MDPART_CO_NOT_FOUND = -2033,
	STORAGE_DEVICE_NOT_FOUND = -2034,
	STORAGE_BTRFS_CO_NOT_FOUND = -2035,
	STORAGE_TMPFS_CO_NOT_FOUND = -2036,
	STORAGE_VOLUME_NOT_ENCRYPTED = -2037,
	STORAGE_DM_RENAME_FAILED = -2038,

	VOLUME_COMMIT_UNKNOWN_STAGE = -3000,
	VOLUME_FSTAB_EMPTY_MOUNT = -3001,
	VOLUME_UMOUNT_FAILED = -3002,
	VOLUME_MOUNT_FAILED = -3003,
	VOLUME_FORMAT_UNKNOWN_FS = -3005,
	VOLUME_FORMAT_FS_UNDETECTED = -3006,
	VOLUME_FORMAT_FS_TOO_SMALL = -3007,
	VOLUME_FORMAT_FAILED = -3008,
	VOLUME_TUNE2FS_FAILED = -3009,
	VOLUME_MKLABEL_FS_UNABLE = -3010,
	VOLUME_MKLABEL_FAILED = -3011,
	VOLUME_LOSETUP_NO_LOOP = -3012,
	VOLUME_LOSETUP_FAILED = -3013,
	VOLUME_CRYPT_NO_PWD = -3014,
	VOLUME_CRYPT_PWD_TOO_SHORT = -3015,
	VOLUME_CRYPT_NOT_DETECTED = -3016,
	VOLUME_FORMAT_EXTENDED_UNSUPPORTED = -3017,
	VOLUME_MOUNT_EXTENDED_UNSUPPORTED = -3018,
	VOLUME_MOUNT_POINT_INVALID = -3019,
	VOLUME_MOUNTBY_NOT_ENCRYPTED = -3020,
	VOLUME_MOUNTBY_UNSUPPORTED_BY_FS = -3021,
	VOLUME_LABEL_NOT_SUPPORTED = -3022,
	VOLUME_LABEL_TOO_LONG = -3023,
	VOLUME_LABEL_WHILE_MOUNTED = -3024,
	VOLUME_RESIZE_UNSUPPORTED_BY_FS = -3025,
	VOLUME_RESIZE_UNSUPPORTED_BY_CONTAINER = -3026,
	VOLUME_RESIZE_FAILED = -3027,
	VOLUME_ALREADY_IN_USE = -3028,
	VOLUME_LOUNSETUP_FAILED = -3029,
	VOLUME_DEVICE_NOT_PRESENT = -3030,
	VOLUME_DEVICE_NOT_BLOCK = -3031,
	VOLUME_MOUNTBY_UNSUPPORTED_BY_VOLUME = -3032,
	VOLUME_CRYPTFORMAT_FAILED = -3033,
	VOLUME_CRYPTSETUP_FAILED = -3034,
	VOLUME_CRYPTUNSETUP_FAILED = -3035,
	VOLUME_FORMAT_NOT_IMPLEMENTED = -3036,
	VOLUME_FORMAT_IMPOSSIBLE = -3037,
	VOLUME_CRYPT_NFS_IMPOSSIBLE = -3038,
	VOLUME_REMOUNT_FAILED = -3039,
	VOLUME_TUNEREISERFS_FAILED = -3040,
	VOLUME_UMOUNT_NOT_MOUNTED = -3041,
	VOLUME_BTRFS_ADD_FAILED = -3042,
	VOLUME_CANNOT_TMP_MOUNT = -3043,
	VOLUME_CANNOT_TMP_UMOUNT = -3044,
	VOLUME_BTRFS_SUBVOL_INIT_FAILED = -3045,
	VOLUME_BTRFS_SUBVOL_SETDEFAULT = -3046,

	LVM_CREATE_PV_FAILED = -4000,
	LVM_PV_ALREADY_CONTAINED = -4001,
	LVM_PV_DEVICE_UNKNOWN = -4002,
	LVM_PV_DEVICE_USED = -4003,
	LVM_VG_HAS_NONE_PV = -4004,
	LVM_LV_INVALID_NAME = -4005,
	LVM_LV_DUPLICATE_NAME = -4006,
	LVM_LV_NO_SPACE = -4007,
	LVM_LV_UNKNOWN_NAME = -4008,
	LVM_LV_NOT_IN_LIST = -4009,
	LVM_VG_CREATE_FAILED = -4010,
	LVM_VG_EXTEND_FAILED = -4011,
	LVM_VG_REDUCE_FAILED = -4012,
	LVM_VG_REMOVE_FAILED = -4013,
	LVM_LV_CREATE_FAILED = -4014,
	LVM_LV_REMOVE_FAILED = -4015,
	LVM_LV_RESIZE_FAILED = -4016,
	LVM_PV_STILL_ADDED = -4017,
	LVM_PV_REMOVE_NOT_FOUND = -4018,
	LVM_CREATE_LV_INVALID_VOLUME = -4019,
	LVM_REMOVE_LV_INVALID_VOLUME = -4020,
	LVM_RESIZE_LV_INVALID_VOLUME = -4021,
	LVM_CHANGE_READONLY = -4022,
	LVM_CHECK_RESIZE_INVALID_VOLUME = -4023,
	LVM_COMMIT_NOTHING_TODO = -4024,
	LVM_LV_REMOVE_USED_BY = -4025,
	LVM_LV_ALREADY_ON_DISK = -4026,
	LVM_LV_NO_STRIPE_SIZE = -4027,
	LVM_LV_UNKNOWN_ORIGIN = -4028,
	LVM_LV_NOT_ON_DISK = -4029,
	LVM_LV_NOT_SNAPSHOT = -4030,
	LVM_LV_HAS_SNAPSHOTS = -4031,
	LVM_LV_IS_SNAPSHOT = -4032,
	LVM_LIST_EMPTY = -4033,
	LVM_LV_NO_POOL_OR_SNAP = -4034,
	LVM_LV_NO_POOL = -4035,
	LVM_LV_UNKNOWN_POOL = -4036,
	LVM_LV_INVALID_CHUNK_SIZE = -4037,
	LVM_LV_POOL_NO_FORMAT = -4038,
	LVM_LV_POOL_NO_MOUNT = -4039,

	FSTAB_ENTRY_NOT_FOUND = -5000,
	FSTAB_CHANGE_PREFIX_IMPOSSIBLE = -5001,
	FSTAB_REMOVE_ENTRY_NOT_FOUND = -5002,
	FSTAB_UPDATE_ENTRY_NOT_FOUND = -5003,
	FSTAB_ADD_ENTRY_FOUND = -5004,

	MD_CHANGE_READONLY = -6000,
	MD_DUPLICATE_NUMBER = -6001,
	MD_TOO_FEW_DEVICES = -6002,
	MD_DEVICE_UNKNOWN = -6003,
	MD_DEVICE_USED = -6004,
	MD_CREATE_INVALID_VOLUME = -6005,
	MD_REMOVE_FAILED = -6006,
	MD_NOT_IN_LIST = -6007,
	MD_CREATE_FAILED = -6008,
	MD_UNKNOWN_NUMBER = -6009,
	MD_REMOVE_USED_BY = -6010,
	MD_NUMBER_TOO_LARGE = -6011,
	MD_REMOVE_INVALID_VOLUME = -6012,
	MD_REMOVE_CREATE_NOT_FOUND = -6013,
	MD_NO_RESIZE_ON_DISK = -6014,
	MD_ADD_DUPLICATE = -6015,
	MD_REMOVE_NONEXISTENT = -6016,
	MD_NO_CHANGE_ON_DISK = -6017,
	MD_NO_CREATE_UNKNOWN = -6018,
	MD_STATE_NOT_ON_DISK = -6019,
	MD_PARTITION_NOT_FOUND = -6020,
	MD_INVALID_PARITY = -6021,
	MD_TOO_MANY_SPARES = -6022,
	MD_GET_STATE_FAILED = -6023,
	MD_DUPLICATE_NAME = -6024,

	MDPART_CHANGE_READONLY = -6100,
	MDPART_INTERNAL_ERR = -6101,
	MDPART_INVALID_VOLUME = -6012,
	MDPART_PARTITION_NOT_FOUND = -6103,
	MDPART_REMOVE_PARTITION_LIST_ERASE = -6104,
	MDPART_COMMIT_NOTHING_TODO = -6105,
	MDPART_NO_REMOVE = -6106,
	MDPART_DEVICE_NOT_FOUND = -6107,

	LOOP_CHANGE_READONLY = -7000,
	LOOP_DUPLICATE_FILE = -7001,
	LOOP_UNKNOWN_FILE = -7002,
	LOOP_REMOVE_USED_BY = -7003,
	LOOP_FILE_CREATE_FAILED = -7004,
	LOOP_CREATE_INVALID_VOLUME = -7005,
	LOOP_REMOVE_FILE_FAILED = -7006,
	LOOP_REMOVE_INVALID_VOLUME = -7007,
	LOOP_NOT_IN_LIST = -7008,
	LOOP_REMOVE_CREATE_NOT_FOUND = -7009,
	LOOP_MODIFY_EXISTING = -7010,

	PEC_PE_SIZE_INVALID = -9000,
	PEC_PV_NOT_FOUND = -9001,
	PEC_REMOVE_PV_IN_USE = -9002,
	PEC_REMOVE_PV_SIZE_NEEDED = -9003,
	PEC_LV_NO_SPACE_STRIPED = -9004,
	PEC_LV_NO_SPACE_SINGLE = -9005,
	PEC_LV_PE_DEV_NOT_FOUND = -9006,

	DM_CHANGE_READONLY = -10000,
	DM_UNKNOWN_TABLE = -10001,
	DM_REMOVE_USED_BY = -10002,
	DM_REMOVE_CREATE_NOT_FOUND = -10003,
	DM_REMOVE_INVALID_VOLUME = -10004,
	DM_REMOVE_FAILED = -10005,
	DM_NOT_IN_LIST = -10006,

	DASD_NOT_POSSIBLE = -11000,
	DASD_DASDFMT_FAILED = -11002,

	DMPART_CHANGE_READONLY = -12001,
	DMPART_INTERNAL_ERR = -12002,
	DMPART_INVALID_VOLUME = -12003,
	DMPART_PARTITION_NOT_FOUND = -12004,
	DMPART_REMOVE_PARTITION_LIST_ERASE = -12005,
	DMPART_COMMIT_NOTHING_TODO = -12006,
	DMPART_NO_REMOVE = -12007,

	DMRAID_REMOVE_FAILED = -13001,

	NFS_VOLUME_NOT_FOUND = -14001,
	NFS_CHANGE_READONLY = -14002,
	NFS_REMOVE_VOLUME_CREATE_NOT_FOUND = -14003,
	NFS_REMOVE_VOLUME_LIST_ERASE = -14004,
	NFS_REMOVE_INVALID_VOLUME = -14005,

	BTRFS_COMMIT_INVALID_VOLUME = -15001,
	BTRFS_CANNOT_TMP_MOUNT = -15002,
	BTRFS_CANNOT_TMP_UMOUNT = -15003,
	BTRFS_DELETE_SUBVOL_FAIL = -15004,
	BTRFS_CREATE_SUBVOL_FAIL = -15005,
	BTRFS_VOLUME_NOT_FOUND = -15006,
	BTRFS_SUBVOL_EXISTS = -15007,
	BTRFS_SUBVOL_NON_EXISTS = -15008,
	BTRFS_REMOVE_NOT_FOUND = -15009,
	BTRFS_REMOVE_NO_BTRFS = -15010,
	BTRFS_REMOVE_INVALID_VOLUME = -15011,
	BTRFS_CHANGE_READONLY = -15012,
	BTRFS_DEV_ALREADY_CONTAINED = -15013,
	BTRFS_DEVICE_UNKNOWN = -15014,
	BTRFS_DEVICE_USED = -15015,
	BTRFS_HAS_NONE_DEV = -15016,
	BTRFS_DEV_NOT_FOUND = -15017,
	BTRFS_EXTEND_FAIL = -15018,
	BTRFS_REDUCE_FAIL = -15019,
	BTRFS_LIST_EMPTY = -15020,
	BTRFS_RESIZE_INVALID_VOLUME = -15021,
	BTRFS_MULTIDEV_SHRINK_UNSUPPORTED = -15022,

	TMPFS_REMOVE_INVALID_VOLUME = -16001,
	TMPFS_REMOVE_NO_TMPFS = -16002,
	TMPFS_REMOVE_NOT_FOUND = -16003,

	CONTAINER_INTERNAL_ERROR = -99000,
	CONTAINER_INVALID_VIRTUAL_CALL = -99001,

    };


#if !defined(SWIG) || defined(LIBSTORAGE_SWIG_RUBY_LEGACY)


    /**
     * \brief Abstract class defining the interface for libstorage.
     */
    class StorageInterface
    {
    public:

	StorageInterface () {}
	virtual ~StorageInterface () {}

	virtual storage::Storage* getStorage() = 0;

	/**
	 * Query all containers found in system
	 */
	virtual void getContainers( deque<ContainerInfo>& infos) = 0;

	/**
	 * Query disk info for a disk device
	 *
	 * @param disk device name of disk, e.g. /dev/hda
	 * @param info record that get filled with disk special data
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getDiskInfo( const string& disk, DiskInfo& info) = 0;

	/**
	 * Query disk info for a disk device
	 *
	 * @param disk device name of disk, e.g. /dev/hda
	 * @param cinfo record that gets filled with container general data
	 * @param info record that gets filled with disk special data
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getContDiskInfo( const string& disk, ContainerInfo& cinfo,
	                             DiskInfo& info ) = 0;

	/**
	 * Query info for a LVM volume group
	 *
	 * @param name name of volume group, e.g. system
	 * @param info record that gets filled with LVM VG special data
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getLvmVgInfo( const string& name, LvmVgInfo& info) = 0;

	/**
	 * Query info for a LVM volume group
	 *
	 * @param name name of volume group, e.g. system
	 * @param cinfo record that gets filled with container general data
	 * @param info record that gets filled with LVM VG special data
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getContLvmVgInfo( const string& name, ContainerInfo& cinfo,
	                              LvmVgInfo& info) = 0;

	/**
	 * Query container info for a DMRAID container
	 *
	 * @param name name of container, e.g. pdc_ccaihgii
	 * @param info record that gets filled with DMRAID Container special data
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getDmraidCoInfo( const string& name, DmraidCoInfo& info) = 0;

	/**
	 * Query container info for a DMRAID container
	 *
	 * @param name name of container, e.g. pdc_ccaihgii
	 * @param cinfo record that gets filled with container general data
	 * @param info record that gets filled with DMRAID Container special data
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getContDmraidCoInfo( const string& name, ContainerInfo& cinfo,
				         DmraidCoInfo& info) = 0;

	/**
	 * Query container info for a DMMULTIPATH container
	 *
	 * @param name name of container, e.g. 3600508b400105f590000900000300000
	 * @param info record that gets filled with DMMULTIPATH Container special data
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getDmmultipathCoInfo( const string& name, DmmultipathCoInfo& info) = 0;

	/**
	 * Query container info for a DMMULTIPATH container
	 *
	 * @param name name of container, e.g. 3600508b400105f590000900000300000
	 * @param cinfo record that gets filled with container general data
	 * @param info record that gets filled with DMMULTIPATH Container special data
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getContDmmultipathCoInfo( const string& name, ContainerInfo& cinfo,
					      DmmultipathCoInfo& info) = 0;

	/**
	 * Query container info for a MDPART container
	 *
	 * @param name name of container, e.g. md126
	 * @param info record that gets filled with MDPART Container special data
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getMdPartCoInfo( const string& name, MdPartCoInfo& info) = 0;


	/**
	 * Query container info for a MDPART container
	 *
	 * @param name name of container, e.g. md126
	 * @param cinfo record that gets filled with container general data
	 * @param info record that gets filled with MDPART Container special data
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getContMdPartCoInfo( const string& name, ContainerInfo& cinfo,
                                         MdPartCoInfo& info) = 0;

	/**
	 * Set whether multipath should be started automatically if detected.
	 *
	 * @param multipath_autostart value for automatically start of multipath.
	 */
	virtual void setMultipathAutostart(MultipathAutostart multipath_autostart) = 0;

        /**
	 * Query whether multipath should be started automatically if detected.
         *
         * @return value for automatically start of multipath.
         */
	virtual MultipathAutostart getMultipathAutostart() const = 0;

	/**
	 * Query all volumes found in system
	 *
	 * @param infos list of records that get filled with volume info
	 */
	virtual void getVolumes( deque<VolumeInfo>& infos) = 0;

	/**
	 * Query a volume by device name found in system
	 *
	 * @param device device name , e.g. /dev/hda1
	 * @param info record that gets filled with data
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getVolume( const string& device, VolumeInfo& info) = 0;

	/**
	 * Query infos for partitions of a disk
	 *
	 * @param disk device name of disk, e.g. /dev/hda
	 * @param plist list of records that get filled with partition specific info
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getPartitionInfo( const string& disk,
	                              deque<PartitionInfo>& plist ) = 0;

	/**
	 * Query infos for LVM LVs of a LVM VG
	 *
	 * @param name name of volume group, e.g. system
	 * @param plist list of records that get filled with LV specific info
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getLvmLvInfo( const string& name,
				  deque<LvmLvInfo>& plist ) = 0;

	/**
	 * Query infos for software raid devices in system
	 *
	 * @param plist list of records that get filled with MD specific info
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getMdInfo( deque<MdInfo>& plist ) = 0;

	/**
	 * Query infos for partitions on raid device in system
	 *
	 * @param device device name of the parent MdPartCo, e.g. /dev/md125
	 * @param plist list of records that get filled with MdPart specific info
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getMdPartInfo( const string& device,
	                           deque<MdPartInfo>& plist ) = 0;

	/**
	 * Query infos for nfs devices in system
	 *
	 * @param plist list of records that get filled with nfs info
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getNfsInfo( deque<NfsInfo>& plist ) = 0;

	/**
	 * Query infos for file based loop devices in system
	 *
	 * @param plist list of records that get filled with loop specific info
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getLoopInfo( deque<LoopInfo>& plist ) = 0;

	/**
	 * Query infos for dm devices in system
	 *
	 * @param plist list of records that get filled with dm specific info
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getDmInfo( deque<DmInfo>& plist ) = 0;

	/**
	 * Query infos for btrfs devices in system
	 *
	 * @param plist list of records that get filled with btrfs specific info
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getBtrfsInfo( deque<BtrfsInfo>& plist ) = 0;

	/**
	 * Query infos for tmpfs devices in system
	 *
	 * @param plist list of records that get filled with tmpfs specific info
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getTmpfsInfo( deque<TmpfsInfo>& plist ) = 0;

	/**
	 * Query infos for dmraid devices in system
	 *
	 * @param plist list of records that get filled with dmraid specific info
	 * @param name name of dmraid, e.g. pdc_igeeeadj
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getDmraidInfo( const string& name,
	                           deque<DmraidInfo>& plist ) = 0;

	/**
	 * Query infos for dmmultipath devices in system
	 *
	 * @param plist list of records that get filled with dmmultipath specific info
	 * @param name name of dmmultipath, e.g. 3600508b400105f590000900000300000
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getDmmultipathInfo( const string& name,
					deque<DmmultipathInfo>& plist ) = 0;

	/**
	 * Query capabilities of a filesystem type.
	 */
	virtual bool getFsCapabilities (FsType fstype, FsCapabilities& fscapabilities) const = 0;

	/**
	 * Query capabilities of a disk label.
	 */
	virtual bool getDlabelCapabilities(const string& dlabel,
					   DlabelCapabilities& dlabelcapabilities) const = 0;

	/**
	 * Get list of filesystem types present on any block devices.
	 */
	virtual list<string> getAllUsedFs() const = 0;

	/**
	 * Create a new partition. Units given in disk cylinders.
	 *
	 * @param disk device name of disk, e.g. /dev/hda
	 * @param type type of partition to create, e.g. primary or extended
	 * @param cylRegion region in cylinder of partition (cylinders are numbered starting with 0)
	 * @param device is set to the device name of the new partition
	 * The name is returned instead of the number since creating the name from the
	 * number is not straight-forward.
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int createPartition(const string& disk, PartitionType type,
				    const RegionInfo& cylRegion,
				    string& SWIG_OUTPUT(device)) = 0;

	/**
	 * Resize an existing disk partition. Units given in disk cylinders.
	 * Filesystem data is resized accordingly.
	 *
	 * @param device device name of partition
	 * @param sizeCyl new size of partition in disk cylinders
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int resizePartition( const string& device,
				     unsigned long sizeCyl ) = 0;

	/**
	 * Resize an existing disk partition. Units given in disk cylinders.
	 * Filesystem data is ignored.
	 *
	 * @param device device name of partition
	 * @param sizeCyl new size of partition in disk cylinders
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int resizePartitionNoFs( const string& device,
					 unsigned long sizeCyl ) = 0;

	/**
	 * Update area used by a new partition. Units given in disk cylinders.
	 * This function can only be used with a partition created but not yet
	 * committed.
	 *
	 * @param device device name of partition, e.g. /dev/hda1
	 * @param cylRegion region in cylinder of partition (cylinders are numbered starting with 0)
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int updatePartitionArea(const string& device, const RegionInfo& cylRegion) = 0;

	/**
	 * Return the number of free cylinders before and after a partition.
	 *
	 * @param device device name of partition, e.g. /dev/sda1
	 * @param freeCylsBefore is set to the number of free cylinders before the partition
	 * @param freeCylsAfter is set to the number of free cylinders after the partition
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int freeCylindersAroundPartition(const string& device,
	                                         unsigned long& SWIG_OUTPUT(freeCylsBefore),
						 unsigned long& SWIG_OUTPUT(freeCylsAfter)) = 0;

	/**
	 * Determine the device name of the next created partition
	 *
	 * @param disk device name of disk, e.g. /dev/hda
	 * @param type type of partition to create, e.g. primary or extended
	 * @param nr is set to the number of the next created partition
	 * @param device is set to the device name of the next created partition
	 * @return zero if all is ok, a negative number to indicate an error
	 *
	 * Deprecated. Do not use. Use getUnusedPartitionSlots() to get number
	 * and device name of partition since number and device name depend on
	 * region, e.g. for DASD partition table.
	 */
	virtual int nextFreePartition( const string& disk, PartitionType type,
	                               unsigned & SWIG_OUTPUT(nr),
	                               string& SWIG_OUTPUT(device) ) = 0;

	/**
	 * Create a new partition. Units given in Kilobytes.
	 *
	 * @param disk device name of disk, e.g. /dev/hda
	 * @param type type of partition to create, e.g. primary or extended
	 * @param kRegion region in kilobyte of partition
	 * @param device is set to the device name of the new partition
	 * The name is returned instead of the number since creating the name from the
	 * number is not straight-forward.
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int createPartitionKb(const string& disk, PartitionType type,
				      const RegionInfo& kRegion,
				      string& SWIG_OUTPUT(device)) = 0;

	/**
	 * Create a new partition of any type anywhere on the disk. Units given in Kilobytes.
	 *
	 * @param disk device name of disk, e.g. /dev/hda
	 * @param sizeK size of partition in kilobytes
	 * @param device is set to the device name of the new partition
	 * The name is returned instead of the number since creating the name from the
	 * number is not straight-forward.
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int createPartitionAny( const string& disk,
					unsigned long long sizeK,
					string& SWIG_OUTPUT(device) ) = 0;

	/**
	 * Create a new partition of given type as large as possible.
	 *
	 * @param disk device name of disk, e.g. /dev/hda
	 * @param type type of partition to create, e.g. primary or extended
	 * @param device is set to the device name of the new partition
	 * The name is returned instead of the number since creating the name from the
	 * number is not straight-forward.
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int createPartitionMax( const string& disk, PartitionType type,
					string& SWIG_OUTPUT(device) ) = 0;

	/**
	 * Compute number of kilobytes of a given number of disk cylinders
	 *
	 * @param disk device name of disk, e.g. /dev/hda
	 * @param sizeCyl number of disk cylinders
	 * @return number of kilobytes of given cylinders
	 */
	virtual unsigned long long cylinderToKb( const string& disk,
	                                         unsigned long sizeCyl) = 0;

	/**
	 * Compute number of disk cylinders needed for given space
	 *
	 * @param disk device name of disk, e.g. /dev/hda
	 * @param sizeK number of kilobytes
	 * @return number of disk cylinders needed
	 */
	virtual unsigned long kbToCylinder( const string& disk,
					    unsigned long long sizeK) = 0;

	/**
	 * Remove a partition
	 *
	 * @param partition name of partition, e.g. /dev/hda1
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int removePartition (const string& partition) = 0;

	/**
	 * Change partition id of a partition
	 *
	 * @param partition name of partition, e.g. /dev/hda1
	 * @param id new partition id (e.g. 0x82 swap, 0x8e for lvm, ...)
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int changePartitionId (const string& partition, unsigned id) = 0;

	/**
	 * Forget previously issued change of partition id
	 *
	 * @param partition name of partition, e.g. /dev/hda1
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int forgetChangePartitionId (const string& partition ) = 0;

	/**
	 * Return the prefix that is inserted between the disk name and the
	 * partition number.
	 *
	 * @param disk name of disk, e.g. /dev/sda
	 * @return prefix for partitions, e.g. "", "p" or "-part"
	 */
	virtual string getPartitionPrefix(const string& disk) = 0;

	/**
	 * Construct the device name for a partiton from the disk name and
	 * partiton number.
	 *
	 * @param disk name of disk, e.g. /dev/sda
	 * @param partition_no number of partition, e.g. 1
	 * @return device name of partition, e.g. /dev/sda1
	 */
	virtual string getPartitionName(const string& disk, int partition_no) = 0;

	/**
	 * Query unused slots on a disk suitable for creating partitions. The
	 * returned information gets invalid as soon as one partition is
	 * created. If more partitions should be created the slots must be
	 * queried again.
	 *
	 * @param disk name of disk, e.g. /dev/hda1
	 * @param slots list of records that get filled with partition slot specific info
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getUnusedPartitionSlots(const string& disk, list<PartitionSlotInfo>& slots) = 0;

	/**
	 * Destroys the partition table of a disk. An empty disk label
	 * of the given type without any partition is created.
	 *
	 * @param disk device name of disk, e.g. /dev/hda
	 * @param label disk label to create on disk, e.g. msdos, gpt, ...
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int destroyPartitionTable (const string& disk, const string& label) = 0;

	/**
	 * Do what is needed for low level initialisation of a disk. This
	 * function does nothing on normal disks but is needed e.g. on S/390
	 * DASD devices where it executes dasdfmt. If should be considered as
	 * destroying all data on the disk.
	 *
	 * @param disk device name of disk, e.g. /dev/hda
	 * @param value toggle if disk should be initialized or not
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int initializeDisk( const string& disk, bool value ) = 0;

	/**
	 * Query the default disk label of the architecture of the
	 * machine (e.g. msdos for ix86, gpt for ia64, ...) for a disk
	 *
	 * @param device device of disk
	 *
	 * @return default disk label of the disk
	 */
	virtual string defaultDiskLabel(const string& device) = 0;

	/**
	 * Sets or unsets the format flag for the given volume.
	 *
	 * @param device name of volume, e.g. /dev/hda1
	 * @param format flag if format is set on or off
	 * @param fs type of filesystem to create if format is true
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int changeFormatVolume( const string& device, bool format, FsType fs ) = 0;

	/**
	 * Sets the value of the filesystem label.
	 *
	 * @param device name of volume, e.g. /dev/hda1
	 * @param label value of the label
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int changeLabelVolume( const string& device, const string& label ) = 0;

	/**
	 * Sets the value of mkfs options.
	 *
	 * @param device name of volume, e.g. /dev/hda1
	 * @param opts options for mkfs command
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int changeMkfsOptVolume( const string& device, const string& opts ) = 0;

	/**
	 * Sets the value of tunefs options.
	 *
	 * @param device name of volume, e.g. /dev/hda1
	 * @param opts options for tunefs command
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int changeTunefsOptVolume( const string& device, const string& opts ) = 0;

	/**
	 * Changes the mount point of a volume
	 *
	 * @param device name of volume, e.g. /dev/hda1
	 * @param mount new mount point of the volume (e.g. /home).
	 *    it is valid to set an empty mount point
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int changeMountPoint( const string& device, const string& mount ) = 0;

	/**
	 * Get the mount point of a volume
	 *
	 * @param device name of volume, e.g. /dev/hda1
	 * @param mount will be set to the mount point of the volume (e.g. /home).
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getMountPoint( const string& device,
				   string& SWIG_OUTPUT(mount) ) = 0;

	/**
	 * Changes mount by value in fstab of a volume
	 *
	 * @param device name of volume, e.g. /dev/hda1
	 * @param mby new mount by value of the volume.
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int changeMountBy( const string& device, MountByType mby ) = 0;

	/**
	 * Get mount by value in fstab of a volume
	 *
	 * @param device name of volume, e.g. /dev/hda1
	 * @param mby will be set to the mount by value of the volume.
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getMountBy( const string& device,
	                        MountByType& SWIG_OUTPUT(mby) ) = 0;

	/**
	 * Changes the fstab options of a volume
	 *
	 * @param device name of volume, e.g. /dev/hda1
	 * @param options new fstab options of the volume (e.g. noauto,user,sync).
	 *    Multiple options are separated by ",".
	 *    It is valid to set an empty fstab option.
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int changeFstabOptions( const string& device, const string& options ) = 0;

	/**
	 * Get the fstab options of a volume
	 *
	 * @param device name of volume, e.g. /dev/hda1
	 * @param options will be set to the fstab options of the volume (e.g. noauto,user,sync).
	 *    Multiple options are separated by ",".
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getFstabOptions( const string& device,
				     string& SWIG_OUTPUT(options) ) = 0;


	/**
	 * Add to the fstab options of a volume
	 *
	 * @param device name of volume, e.g. /dev/hda1
	 * @param options fstab options to add to already exiting options of the volume (e.g. noauto,user,sync).
	 *    Multiple options are separated by ",".
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int addFstabOptions( const string& device, const string& options ) = 0;

	/**
	 * Remove from the fstab options of a volume
	 *
	 * @param device name of volume, e.g. /dev/hda1
	 * @param options fstab options to remove from already existing options of the volume (e.g. noauto).
	 *    Multiple options are separated by ",".
	 *    It is possible to specify wildcards, so "uid=.*" matches every option starting with the string "uid=".
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int removeFstabOptions( const string& device, const string& options ) = 0;

	/**
	 * Set crypt password of a volume
	 *
	 * @param device name of volume, e.g. /dev/hda1
	 * @param pwd crypt password for this volume
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int setCryptPassword( const string& device, const string& pwd ) = 0;

	/**
	 * Makes library forget a crypt password of a volume
	 *
	 * @param device name of volume, e.g. /dev/hda1
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int forgetCryptPassword( const string& device ) = 0;

	/**
	 * Get crypt password of a volume
	 *
	 * @param device name of volume, e.g. /dev/hda1
	 * @param pwd crypt password for this volume
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getCryptPassword( const string& device,
				      string& SWIG_OUTPUT(pwd) ) = 0;

	/**
	 * Verify password of a volume
	 *
	 * @param device name of volume, e.g. /dev/hda1
	 * @param pwd crypt password for this volume
	 * @param erase if true remove password even after successful verification
	 * @return zero if password is ok, a negative number to indicate an error
	 */
	virtual int verifyCryptPassword( const string& device,
	                                 const string& pwd, bool erase ) = 0;

	/**
	 * Check if crypt password is required
	 *
	 * @param device name of volume, e.g. /dev/hda1
	 * @return true if password is required, false otherwise
	 */
	virtual bool needCryptPassword( const string& device ) = 0;

	/**
	 * Set encryption state of a volume
	 *
	 * @param device name of volume, e.g. /dev/hda1
	 * @param val flag if encryption should be activated
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int setCrypt( const string& device, bool val ) = 0;

	/**
	 * Set encryption state of a volume
	 *
	 * @param device name of volume, e.g. /dev/hda1
	 * @param val flag if encryption should be activated
	 * @param typ type of encryption to set up
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int setCryptType( const string& device, bool val, EncryptType typ ) = 0;

	/**
	 * Get encryption state of a volume
	 *
	 * @param device name of volume, e.g. /dev/hda1
	 * @param val will be set if encryption is activated
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getCrypt( const string& device, bool& SWIG_OUTPUT(val) ) = 0;

	/**
	 * Set fstab handling state of a volume. This way one can make
	 * libstorage ignore fstab handling for a volume.
	 * Use this with care.
	 *
	 * @param device name of volume, e.g. /dev/hda1
	 * @param val flag if fstab should be ignored for this volume
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int setIgnoreFstab( const string& device, bool val ) = 0;

	/**
	 * Get fstab handling state of a volume.
	 *
	 * @param device name of volume, e.g. /dev/hda1
	 * @param val will be set if fstab should be ignored for this volume
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getIgnoreFstab( const string& device, bool& SWIG_OUTPUT(val) ) = 0;

	/**
	 * Sets the value of description text.
	 * This text will be returned together with the text returned by
	 * getCommitInfos().
	 *
	 * @param device name of volume, e.g. /dev/hda1
	 * @param txt description text for this partition
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int changeDescText( const string& device, const string& txt ) = 0;

	/**
	 * Adds the specified entry to /etc/fstab
	 *
	 * This function does not cache the changes but writes them
	 * immediately.
	 *
	 * @param device name of volume, e.g. /dev/hda1
	 * @param mount mount point, e.g. /home
	 * @param vfs virtual filesystem type, e.g. reiserfs or ext3
	 * @param options fstab options e.g. noauto,user,sync
	 * @param freq value for fifth fstab field
	 * @param passno value for sixth fstab field
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int addFstabEntry( const string& device, const string& mount,
	                           const string& vfs, const string& options,
				   unsigned freq, unsigned passno ) = 0;


	/**
	 * Resizes a volume while keeping the data on the filesystem
	 *
	 * @param device name of volume, e.g. /dev/hda1
	 * @param newSizeK new size desired volume in kilobytes
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int resizeVolume(const string& device, unsigned long long newSizeK) = 0;

	/**
	 * Resizes a volume while ignoring the data on the filesystem
	 *
	 * @param device name of volume, e.g. /dev/hda1
	 * @param newSizeK new size desired volume in kilobytes
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int resizeVolumeNoFs(const string& device, unsigned long long newSizeK) = 0;

	/**
	 * Forget about possible resize of an volume.
	 *
	 * @param device device name of volume
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int forgetResizeVolume( const string& device ) = 0;

	/**
	 * Set handling of deletion of entities that belong to other
	 * volumes. Normally it is not possible to remove a volume that
	 * is part of another entity (e.g. you cannot remove a partition
	 * that is part of an LVM Volume group or a Software raid).
	 * This setting makes the removal recursive, this means all
	 * entities where the removed volume is a part of are also
	 * removed. Use this setting with extreme care, it may cause
	 * the removal of LVM Volume group spanning multiple disks just
	 * because one partition of the LVM Volume group got deleted.
	 * Default value of this flag is false.
	 *
	 * @param val flag if removal is done recursive
	 */
	virtual void setRecursiveRemoval( bool val ) = 0;

	/**
	 * Get value of the flag for recursive removal
	 *
	 * @return value of the flag for recursive removal
	 */
	virtual bool getRecursiveRemoval() const = 0;

	/**
	 * Recursively get all devices using devices. Volumes of containers are
	 * also considered as using the devices.
	 *
	 * @param devices name of device, e.g. /dev/sda
	 * @param itself whether the device itself is included in the result
	 * @param using_devices name of devices using device, e.g. /dev/sda1 /dev/sda2
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getRecursiveUsing(const list<string>& devices, bool itself,
				      list<string>& using_devices) = 0;

	/**
	 * Recursively get all devices used by devices. Containers of volumes are
	 * also considered as used by the devices.
	 *
	 * @param devices list of name of devices, e.g. /dev/sda1
	 * @param itself whether the device itself is included in the result
	 * @param usedby_devices name of devices used by devices, e.g. /dev/sda
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getRecursiveUsedBy(const list<string>& devices, bool itself,
				       list<string>& usedby_devices) = 0;

	/**
	 * Set handling of newly created partitions. With this flag
	 * once can make the library overwrite start and end of newly
	 * created partitions with zeroes. This prevents that obsolete
	 * structures (e.g. LVM VGs or MD superblocks) still exists on
	 * newly created partitions since the area on disk previously
	 * contained a LVM PV or a device of a software raid.
	 * volumes. Use this setting with extreme care, it make libstorage
	 * behave fundamentally different from all other partitioning tools.
	 * Default value of this flag is false.
	 *
	 * @param val flag if newly created partitions should be zeroed
	 */
	virtual void setZeroNewPartitions( bool val ) = 0;

	/**
	 * Get value of the flag for zeroing newly created partitions
	 *
	 * @return value of the flag for zeroing newly created partitions
	 */
	virtual bool getZeroNewPartitions() const = 0;

	/**
	 * Set alignment of newly created partitions.
	 * PartAlign has value ALIGN_OPTIMAL or ALIGN_CYLINDER.
	 * ALIGN_CYLINDER aligns partiton start to cylinder boundaries like
	 *     it was traditionally for a long time.
	 * ALIGN_OPTIMAL aligns according to values provided by kernel.
	 *     Default alignment is to multiples of 2048 sectors which is
	 *     compatible to partition layout of Windows Vista and Windows 7.
	 *
	 * @param val value of new alignment type
	 */
	virtual void setPartitionAlignment( PartAlign val ) = 0;

	/**
	 * Get value for the alignment of newly created partitions
	 *
	 * @return value for the alignment
	 */
	virtual PartAlign getPartitionAlignment() const = 0;

	/**
	 * Set default value for mount by.
	 *
	 * @param val new default mount by value.
	 */
	virtual void setDefaultMountBy( MountByType val ) = 0;

	/**
	 * Get default value for mount by.
	 *
	 * @return default value for mount by
	 */
	virtual MountByType getDefaultMountBy() const = 0;

	/**
	 * Set default filesystem.
	 *
	 * @param val new default filesystem.
	 */
	virtual void setDefaultFs(FsType val) = 0;

	/**
	 * Get default filesystem.
	 *
	 * @return default filesystem.
	 */
	virtual FsType getDefaultFs() const = 0;

	/**
	 * Set default subvolume name.
	 *
	 * @param val new default subvolume name.
	 */
	virtual void setDefaultSubvolName( const string& val) = 0;

	/**
	 * Get default filesystem.
	 *
	 * @return default filesystem.
	 */
	virtual string getDefaultSubvolName() const = 0;

	/**
	 * Get value for EFI boot.
	 *
	 * @return value for efi boot
	 */
	virtual bool getEfiBoot() = 0;

	/**
	 * Set value for root prefix.
	 *
	 * This value is appended to all mount points of volumes, when
	 * changes are commited. Config files fstab, crypttab and
	 * mdadm.conf are also created relative to this prefix.
	 * This variable must be set before first call to commit.
	 *
	 * @param root new value for root prefix
	 */
	virtual void setRootPrefix( const string& root ) = 0;

	/**
	 * Get value for root prefix.
	 *
	 * @return value for root prefix
	 */
	virtual string getRootPrefix() const = 0;

	/**
	 * Determine of libstorage should detect mounted volumes.
	 *
	 * @param val flag if mounted volumes should be detected
	 */
	virtual void setDetectMountedVolumes( bool val ) = 0;

	/**
	 * Get value of the flag for detection of mounted volumes.
	 *
	 * @return value of the flag for detection of mounted volumes
	 */
	virtual bool getDetectMountedVolumes() const = 0;

	/**
	 * Removes a volume from the system. This function can be used
	 * for removing all types of volumes (partitions, LVM LVs, MD devices ...)
	 *
	 * @param device name of volume, e.g. /dev/hda1
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int removeVolume( const string& device ) = 0;

	/**
	 * Create a LVM volume group
	 *
	 * @param name name of volume group, must not contain blanks, colons
	 * and shell special characters (e.g. system)
	 * @param peSizeK physical extent size in kilobytes
	 * @param lvm1 flag if lvm1 compatible format should be used
	 * @param devs list with physical devices to add to that volume group
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int createLvmVg( const string& name,
	                         unsigned long long peSizeK, bool lvm1,
	                         const deque<string>& devs ) = 0;

	/**
	 * Remove a LVM volume group. If the volume group contains
	 * logical volumes, these are automatically also removed.
	 *
	 * @param name name of volume group
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int removeLvmVg( const string& name ) = 0;

	/**
	 * Extend a LVM volume group with additional physical devices
	 *
	 * @param name name of volume group
	 * @param devs list with physical devices to add to that volume group
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int extendLvmVg( const string& name,
	                         const deque<string>& devs ) = 0;

	/**
	 * Shrink a LVM volume group
	 *
	 * @param name name of volume group
	 * @param devs list with physical devices to remove from that volume group
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int shrinkLvmVg( const string& name,
	                         const deque<string>& devs ) = 0;

	/**
	 * Create a LVM logical volume
	 *
	 * @param vg name of volume group
	 * @param name of logical volume
	 * @param sizeK size of logical volume in kilobytes
	 * @param stripes stripe count of logical volume (use 1 unless you know
	 * exactly what you are doing)
	 * @param device is set to the device name of the new LV
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int createLvmLv( const string& vg, const string& name,
	                         unsigned long long sizeK, unsigned stripes,
				 string& SWIG_OUTPUT(device) ) = 0;

	/**
	 * Remove a LVM logical volume
	 *
	 * @param device name of logical volume
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int removeLvmLvByDevice( const string& device ) = 0;

	/**
	 * Remove a LVM logical volume
	 *
	 * @param vg name of volume group
	 * @param name of logical volume
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int removeLvmLv( const string& vg, const string& name ) = 0;

	/**
	 * Change stripe count of a LVM logical volume.
	 * This can only be before the volume is created on disk.
	 *
	 * @param vg name of volume group
	 * @param name of logical volume
	 * @param stripes new stripe count of logical volume
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int changeLvStripeCount( const string& vg, const string& name,
	                                 unsigned long stripes ) = 0;

	/**
	 * Change stripe size of a LVM logical volume.
	 * This can only be before the volume is created on disk.
	 *
	 * @param vg name of volume group
	 * @param name of logical volume
	 * @param stripeSizeK new stripe size of logical volume
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int changeLvStripeSize( const string& vg, const string& name,
	                                unsigned long long stripeSizeK) = 0;

	/**
	 * Create a LVM logical volume snapshot
	 *
	 * @param vg name of volume group
	 * @param origin name of logical volume origin
	 * @param name of logical volume snapshot
	 * @param cowSizeK size of snapshot in kilobytes
	 * @param device is set to the device name of the new snapshot
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int createLvmLvSnapshot(const string& vg, const string& origin,
					const string& name, unsigned long long cowSizeK,
					string& SWIG_OUTPUT(device) ) = 0;

	/**
	 * Remove a LVM logical volume snapshot
	 *
	 * @param vg name of volume group
	 * @param name name of logical volume snapshot
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int removeLvmLvSnapshot(const string& vg, const string& name) = 0;

	/**
	 * Get state of a LVM logical volume snapshot
	 *
	 * @pre This can only be done after the snapshot has been created on disk.
	 *
	 * @param vg name of volume group
	 * @param name name of logical volume snapshot
	 * @param info record that gets filled with snapshot special data
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getLvmLvSnapshotStateInfo(const string& vg, const string& name,
					      LvmLvSnapshotStateInfo& info) = 0;

	/**
	 * Create a LVM thin pool
	 *
	 * @param vg name of volume group
	 * @param name of thin pool
	 * @param sizeK size of thin pool in kilobytes
	 * @param device is set to the device name of the new pool
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int createLvmLvPool(const string& vg, const string& name,
                                    unsigned long long sizeK,
                                    string& SWIG_OUTPUT(device) ) = 0;

	/**
	 * Create a LVM logical volume that is thin provisioned
	 *
	 * @param vg name of volume group
	 * @param name of logical volume
	 * @param pool name of the pool this logical volume allocates from
	 * @param sizeK virtual size of logical volume in kilobytes
	 * @param device is set to the device name of the logical volume
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int createLvmLvThin(const string& vg, const string& name,
                                    const string& pool,
                                    unsigned long long sizeK,
                                    string& SWIG_OUTPUT(device) ) = 0;

	/**
	 * Change chunk size of a LVM pool or snapshot.
	 * This can only be before the volume is created on disk.
	 *
	 * @param vg name of volume group
	 * @param name of thin pool or snapshot
	 * @param chunkSizeK new chunk size
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int changeLvChunkSize( const string& vg, const string& name,
                                       unsigned long long chunkSizeK) = 0;

	/**
         * Determine the device name of the next created software raid device
         *
         * @param nr is set to the number of the next created software raid device
         * @param device is set to the device name of the next created software raid device
         * @return zero if all is ok, a negative number to indicate an error
         */
	virtual int nextFreeMd(unsigned& SWIG_OUTPUT(nr),
			       string& SWIG_OUTPUT(device)) = 0;

	/**
	 * Create a Software raid device by name
	 *
	 * @param name name of software raid device to create (e.g. /dev/md0)
	 * @param md_type raid personality of the new software raid
	 * @param devices list with physical devices for the new software raid
	 * @param spares list with spare physical devices for the new software raid
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int createMd(const string& name, MdType md_type, const list<string>& devices,
			     const list<string>& spares) = 0;

	/**
	 * Create a Software raid device. Name determined by library.
	 *
	 * @param md_type raid personality of the new software raid
	 * @param devices list with physical devices for the new software raid
	 * @param spares list with spare physical devices for the new software raid
	 * @param device device name of created software raid device
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int createMdAny(MdType md_type, const list<string>& devices,
				const list<string>& spares,
				string& SWIG_OUTPUT(device) ) = 0;

	/**
	 * Remove a Software raid device.
	 *
	 * @param name name of software raid device to remove (e.g. /dev/md0)
	 * @param destroySb flag if the MD superblocks on the physical devices
	 *        should be destroyed after md device is deleted
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int removeMd( const string& name, bool destroySb ) = 0;

	/**
	 * Add a partition to a raid device.
	 * This can only be done before the raid is created on disk.
	 *
	 * @param name name of software raid device (e.g. /dev/md0)
	 * @param devices list with physical devices to add to the raid
	 * @param spares list with spare physical devices to add to the raid
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int extendMd(const string& name, const list<string>& devices,
			     const list<string>& spares) = 0;

	/**
	 * Update all partitions of a raid device.
	 * This can only be done before the raid is created on disk.
	 *
	 * @param name name of software raid device (e.g. /dev/md0)
	 * @param devices list with physical devices to create the raid with
	 * @param spares list with spare physical devices to create the raid with
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int updateMd(const string& name, const list<string>& devices,
			     const list<string>& spares) = 0;

	/**
	 * Remove a partition from a raid device.
	 * This can only be done before the raid is created on disk.
	 *
	 * @param name name of software raid device (e.g. /dev/md0)
	 * @param devices list of physical devices to remove from the raid
	 * @param spares list of spare physical devices to remove from the raid
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int shrinkMd(const string& name, const list<string>& devices,
			     const list<string>& spares) = 0;

	/**
	 * Change raid type of a raid device.
	 * This can only be done before the raid is created on disk.
	 *
	 * @param name name of software raid device (e.g. /dev/md0)
	 * @param md_type new raid personality of the software raid
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int changeMdType(const string& name, MdType md_type) = 0;

	/**
	 * Change chunk size of a raid device.
	 * This can only be done before the raid is created on disk.
	 *
	 * @param name name of software raid device (e.g. /dev/md0)
	 * @param chunkSizeK new chunk size of the software raid
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int changeMdChunk(const string& name, unsigned long chunkSizeK) = 0;

	/**
	 * Change parity of a raid device with raid type raid5, raid6 or raid10.
	 * This can only be done before the raid is created on disk.
	 *
	 * @param name name of software raid device (e.g. /dev/md0)
	 * @param ptype new parity of the software raid
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int changeMdParity( const string& name, MdParity ptype ) = 0;

	/**
	 * Check if a raid device is valid
	 *
	 * @param name name of software raid device (e.g. /dev/md0)
	 * @return true if all is ok, a false to indicate an error
	 */
	virtual int checkMd( const string& name ) = 0;

	/**
	 * Get state of a raid device.
	 *
	 * @pre This can only be done after the raid has been created on disk.
	 *
	 * @param name name of software raid device (e.g. /dev/md0)
	 * @param info record that gets filled with raid special data
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int getMdStateInfo(const string& name, MdStateInfo& info) = 0;

        /**
         * Get state of a MD software raid device.
         *
         * @pre This can only be done after the raid has been created on disk.
         *
         * @param name name of software raid device (e.g. /dev/md125)
         * @param info record that gets filled with raid special data
         * @return zero if all is ok, a negative number to indicate an error
         */
	virtual int getMdPartCoStateInfo(const string& name,
                                         MdPartCoStateInfo& info) = 0;

	/**
	 * Compute the size of a raid device.
	 *
	 * The size compute may not be accurate. It must not be used for
	 * further computations. Do not used in new code.
	 *
	 * @param md_type raid type of the software raid
	 * @param devices list with physical devices for the software raid
	 * @param spares list with spare physical devices for the software raid
	 * @param sizeK will contain the computed size in kilobytes
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int computeMdSize(MdType md_type, const list<string>& devices,
				  const list<string>& spares, unsigned long long& SWIG_OUTPUT(sizeK)) = 0;

	/**
	 * Determine allowed parity types for raid type.
	 *
	 * @param md_type raid type of the software raid
	 * @param devnr number of physical devices for the software raid
	 * @return list of allowed parity typed for this raid
	 */
	virtual list<int> getMdAllowedParity(MdType md_type, unsigned devnr) = 0;

        /**
         * Remove a Partitionable Software raid device.
         *
         * Only RAID with persistent superblock can be removed. IMSM and DDF
         * RAIDs cannot be removed.
         * @param name name of software raid device to remove (e.g. /dev/md0)
         * @param destroySb flag if the MD superblocks on the physical devices
         *        should be destroyed after md device is deleted
         * @return zero if all is ok, a negative number to indicate an error
         */
	virtual int removeMdPartCo(const string& name, bool destroySb ) = 0;

	/**
	 * Add knowledge about existence of nfs device.
	 *
	 * @param nfsDev name of nfs device
	 * @param sizeK size of the nfs device
	 * @param opts mount options for nfs mount
	 * @param mp mount point of the nfs device
	 * @param nfs4 use NFS4 for device
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int addNfsDevice(const string& nfsDev, const string& opts,
				 unsigned long long sizeK, const string& mp,
				 bool nfs4) = 0;

	/**
	 * Check accessibility and size of nfs device.
	 *
	 * @param nfsDev name of nfs device
	 * @param opts mount options for nfs mount
	 * @param nfs4 use NFS4 for device
	 * @param sizeK size of the nfs device
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int checkNfsDevice(const string& nfsDev, const string& opts,
				   bool nfs4, unsigned long long& SWIG_OUTPUT(sizeK)) = 0;

	/**
	 * Create a file based loop device. Encryption is automatically
	 * activated on the loop device.
	 *
	 * @param lname name of file the loop device is based on
	 * @param reuseExisting if true an already existing file will be
	 *    reused. if false the file will be created new. if false
	 *    the format flag for the device is set by default.
	 * @param sizeK size of the created file, this parameter is ignored
	 *    if reuseExisting is true and a file already exists.
	 * @param mp mount point of the file based loop device
	 * @param pwd crypt password for the loop device, encryption type
	 *    is determined automatically by the system
	 * @param device the name of the created loop device
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int createFileLoop( const string& lname, bool reuseExisting,
	                            unsigned long long sizeK,
				    const string& mp, const string& pwd,
				    string& SWIG_OUTPUT(device) ) = 0;

	/**
	 * Modify size and pathname of a file based loop device.
	 * This function can only be used between the creation of a
	 * device and the next call to commit(). Containers that
	 * are already created cannot have these properties changed.
	 * The size has only a meaning if reuseExisting is true,
	 * otherwise it is ignored.
	 *
	 * @param device device name of the loop device
	 * @param lname name of file the loop device is based on
	 * @param reuseExisting if true an already existing file will be
	 *    reused. if false the file will be created new. if false
	 *    the format flag for the device is set by default.
	 * @param sizeK size of the created file, this parameter is ignored
	 *    if reuseExisting is false
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int modifyFileLoop( const string& device, const string& lname,
	                            bool reuseExisting,
	                            unsigned long long sizeK ) = 0;

	/**
	 * Remove a file based loop device from the system.
	 *
	 * @param lname name of file the loop device is based on
	 * @param removeFile if true the file is removed together with
	 *    the based loop device. If false the file is not touched.
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int removeFileLoop( const string& lname, bool removeFile ) = 0;

	/**
	 * Remove a Software raid device.
	 *
	 * @param name name of dmraid device to remove (e.g. pdc_dabaheedj)
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int removeDmraid( const string& name ) = 0;

	/**
	 * Check for existence of a BTRFS subvolume
	 *
	 * @param device of the main BTRFS volume
	 * @param name of subvolume
	 * @return true if subvolume exists, false otherwise
	 */
	virtual bool existSubvolume( const string& device, const string& name ) = 0;

	/**
	 * Create a BTRFS subvolume
	 *
	 * @param device of the main BTRFS volume
	 * @param name of subvolume
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int createSubvolume( const string& device, const string& name ) = 0;

	/**
	 * Remove a BTRFS subvolume
	 *
	 * @param device of the main BTRFS volume
	 * @param name of subvolume
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int removeSubvolume( const string& device, const string& name ) = 0;

	/**
	 * Extend a BTRFS volume with additional devices
	 *
	 * @param name name of BTRFS volume (this can contain a device name
	 *    or be specified as "UUID=<uuid>")
	 * @param devs list with devices to add to that BTRFS volume
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int extendBtrfsVolume( const string& name,
				       const deque<string>& devs ) = 0;

	/**
	 * Shrink a BTRFS volume by some devices
	 *
	 * @param name name of BTRFS volume (this can contain a device name
	 *    or be specified as "UUID=<uuid>")
	 * @param devs list with devices to remove from that BTRFS volume
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int shrinkBtrfsVolume( const string& name,
				       const deque<string>& devs ) = 0;

	/**
	 * Add new tmpfs filesystem
	 *
	 * @param mp mount point for the tmpfs
	 * @param opts mount options for tmpfs mount
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int addTmpfsMount( const string& mp, const string& opts ) = 0;

	/**
	 * Remove tmpfs filesystem
	 *
	 * @param mp mount point for the tmpfs
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int removeTmpfsMount( const string& mp ) = 0;

	/**
	 * Gets info about actions to be executed after next call to commit().
	 *
	 * @param infos list of records that gets filled with infos
	 */
	virtual void getCommitInfos(list<CommitInfo>& infos) const = 0;

	/**
	 * Gets action performed last during previous call to commit()
	 *
	 * @return string presentable to the user
	 */
	virtual const string& getLastAction() const = 0;

	/**
	 * Gets a possible existing extended error message describing failure
	 * of to last call commit()
	 *
	 * @return string error text provided by external program
	 */
	virtual const string& getExtendedErrorMessage() const = 0;

// temporarily disable callback function for swig
#ifndef SWIG

	/**
	 * Sets the callback function called on progress bar events
	 *
	 * @param pfnc pointer to function
	 */
	virtual void setCallbackProgressBar(CallbackProgressBar pfnc) = 0;

	/**
	 * Query the callback function called on progress bar events
	 *
	 * @return pointer to function currently called for progress bar events
	 */
	virtual CallbackProgressBar getCallbackProgressBar() const = 0;


	/**
	 * Sets the callback function called to display install info
	 *
	 * @param pfnc pointer to function
	 */
	virtual void setCallbackShowInstallInfo(CallbackShowInstallInfo pfnc) = 0;

	/**
	 * Query the callback function called to display install info
	 *
	 * @return pointer to function currently called for progress bar events
	 */
	virtual CallbackShowInstallInfo getCallbackShowInstallInfo() const = 0;


	/**
	 * Sets the callback function called to display a info popup to the
	 * user
	 *
	 * @param pfnc pointer to function
	 */
	virtual void setCallbackInfoPopup(CallbackInfoPopup pfnc) = 0;

	/**
	 * Query the callback function called to display info popup to the
	 * user
	 *
	 * @return pointer to function currently called for progress bar events
	 */
	virtual CallbackInfoPopup getCallbackInfoPopup() const = 0;


	/**
	 * Sets the callback function called to get a Yes/No decision by
	 * the user.
	 *
	 * @param pfnc pointer to function
	 */
	virtual void setCallbackYesNoPopup(CallbackYesNoPopup pfnc) = 0;

	/**
	 * Query the callback function called to get a Yes/No decision by
	 * the user.
	 *
	 * @return pointer to function currently called for progress bar events
	 */
	virtual CallbackYesNoPopup getCallbackYesNoPopup() const = 0;


	/**
	 * Sets the callback function called for errors during commit.
	 *
	 * @param pfnc pointer to function
	 */
	virtual void setCallbackCommitErrorPopup(CallbackCommitErrorPopup pfnc) = 0;

	/**
	 * Query the callback function called for errors during commit.
	 *
	 * @return pointer to function currently called for progress bar events
	 */
	virtual CallbackCommitErrorPopup getCallbackCommitErrorPopup() const = 0;


	/**
	 * Sets the callback function called to query a password by the user.
	 *
	 * @param pfnc pointer to function.
	 */
	virtual void setCallbackPasswordPopup(CallbackPasswordPopup pfnc) = 0;

	/**
	 * Query the callback function called to query a password from the
	 * user.
	 *
	 * @return pointer to function currently called for password queries.
	 */
	virtual CallbackPasswordPopup getCallbackPasswordPopup() const = 0;

#endif

	/**
	 * With the function setCacheChanges you can turn the caching mode on
	 * and off.  Turning of caching mode will cause all changes done so
	 * far to be committed upto the next modifying function.
	 */
	virtual void setCacheChanges (bool cache) = 0;

	/**
	 * Query the caching mode.
	 */
	virtual bool isCacheChanges () const = 0;

#if !defined(SWIG) || defined(LIBSTORAGE_SWIG_RUBY_LEGACY)

        /**
	 * Set the commit callbacks.
	 */
	virtual void setCommitCallbacks(const CommitCallbacks* commit_callbacks) = 0;

#endif

	/**
	 * Commit the current state to the system.  Only useful in caching
	 * mode.
	 */
	virtual int commit() = 0;

	/**
	 * Get a textual message for an error code. Can be empty.
	 */
	virtual string getErrorString(int error) const = 0;

	/**
	 * Create backup of current state of all containers
	 *
	 * @param name name under which the backup should be created
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int createBackupState( const string& name ) = 0;

	/**
	 * Restore state to a previously created backup
	 *
	 * @param name name of the backup to restore
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int restoreBackupState( const string& name ) = 0;

	/**
	 * Checks if a backup with a certain name already exists
	 *
	 * @param name name of the backup to check
	 * @return true if the backup exists
	 */
	virtual bool checkBackupState(const string& name) const = 0;

	/**
	 * Compare two backup states
	 *
	 * @param lhs name of backup to compare, empty string means active state
	 * @param rhs name of backup to compare, empty string means active state
	 * @param verbose_log flag if differences should be logged in detail
	 * @return true if states are equal
	 */
	virtual bool equalBackupStates(const string& lhs, const string& rhs,
				       bool verbose_log) const = 0;

	/**
	 * Remove existing backup state
	 *
	 * @param name name of backup to remove, empty string means to remove
	 *     all existing backup states
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int removeBackupState( const string& name ) = 0;

	/**
	 * Determine if the given device is known and mounted somewhere
	 *
	 * @param device device name to check (checks also all alias names)
	 * @param mps set to current mount points if mounted
	 * @return bool that is true if device is mounted
	 */
	virtual bool checkDeviceMounted(const string& device, list<string>& mps) = 0;

	/**
	 * Umount the given device and do what is necessary to remove
	 * underlying volume (e.g. do losetup -d if loop is set up or
	 * dmsetup remove if dmcrypt is used)
	 *
	 * The function umounts at once, /etc/fstab is unaffected
	 *
	 * @param device device name to umount
	 * @return bool if umount succeeded
	 */
	virtual bool umountDevice( const string& device ) = 0;

	/**
	 * Umount the given device and dependent of parameter unsetup
	 * do what is necessary to remove underlying volume (e.g. do
	 * losetup -d if loop is set up or dmsetup remove if dmcrypt
	 * is used
	 *
	 * The function umounts at once, /etc/fstab is unaffected
	 *
	 * @param device device name to umount
	 * @param unsetup flag if losetup/dmcrypt should be removed
	 * @return bool if umount succeeded
	 */
	virtual bool umountDeviceUns( const string& device, bool unsetup ) = 0;

	/**
	 * Mount the given device and do what is necessary to access
	 * volume (e.g. do losetup if loop is set up)
	 *
	 * The function mounts at once, /etc/fstab is unaffected
	 *
	 * @param device device name to mount
	 * @param mp mount point to mount to
	 * @return bool if mount succeeded
	 */
	virtual bool mountDevice( const string& device, const string& mp ) = 0;

	/**
	 * Mount the given device and do what is necessary to access
	 * volume (e.g. do losetup if loop is set up)
	 *
	 * The function mounts at once, /etc/fstab is unaffected
	 *
	 * @param device device name
	 * @param on if true activate access to encrypted data, otherwise deactivate it
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int activateEncryption( const string& device, bool on ) = 0;

	/**
	 * Mount the given device with given options and do what is necessary
	 * to access volume (e.g. do losetup if loop is set up)
	 *
	 * The function mounts at once, /etc/fstab is unaffected
	 *
	 * @param device device name to mount
	 * @param mp mount point to mount to
	 * @param opts options to use for mount
	 * @return bool if mount succeeded
	 */
	virtual bool mountDeviceOpts( const string& device, const string& mp,
	                              const string& opts ) = 0;

	/**
	 * Mount the given device readonly and do what is necessary to access
	 * volume (e.g. do losetup if loop is set up)
	 *
	 * The function mounts at once, /etc/fstab is unaffected
	 *
	 * @param device device name to mount
	 * @param mp mount point to mount to
	 * @param opts options to use for mount
	 * @return bool if mount succeeded
	 */
	virtual bool mountDeviceRo( const string& device, const string& mp,
	                            const string& opts ) = 0;

	/**
	 * Check if there are dm maps to a given device
	 *
	 * @param device device name for which dm maps should be checked
	 * @return bool true if there are map to device
	 */
	virtual bool checkDmMapsTo( const string& device ) = 0;

	/**
	 * Remove all possibly existing dm maps to a given device
	 *
	 * @param device device name for which dm maps should be removed
	 */
	virtual void removeDmTableTo( const string& device ) = 0;

	/**
	 * If device has an encrypted dm map, rename it en device
	 *
	 * @param device device name for which dm map should be renamed
	 * @param new_name new dm name used for the device
	 *
	 * @return zero if all is ok, a negative number to indicate an error
	 */
	virtual int renameCryptDm( const string& device,
                                   const string& new_name ) = 0;

	/**
	 * Detect potentially available free space on a partition
	 *
	 * @param device device to check
	 * @param get_resize flag to indicate that resize_info should be queried
	 * @param resize_info struct that gets filled with resize info
	 * @param get_content flag to indicate that content_info should be queried
	 * @param content_info struct that gets filled with content info
	 * @param use_cache function should return cached data if available
	 * @return bool if values could be successfully determined
	 */
	virtual bool getFreeInfo(const string& device, bool get_resize, ResizeInfo& resize_info,
				 bool get_content, ContentInfo& content_info, bool use_cache) = 0;

	/**
	 * Read fstab and cryptotab, if existent, from a specified directory and
	 * return the volumes found in fstab and cryptotab
	 *
	 * @param dir directory where fstab and cryptotab are read from
	 * @param infos list of records that get filled with volume info
	 */
	virtual bool readFstab( const string& dir, deque<VolumeInfo>& infos) = 0;

	/**
	 * Activate or deactivate higher level devices as MD, LVM, DM
	 *
	 * Multipath is not activate by this function. Only use in instsys
	 * mode.
	 *
	 * @param val flag if devices should be activated or deactivated
	 * @return bool if values could be successfully determined
	 */
	virtual void activateHld( bool val ) = 0;

	/**
	 * Activate or deactivate multipath
	 *
	 * Only use in instsys mode.
	 *
	 * @param val flag if multipath should be activated or deactivated
	 * @return bool if values could be successfully determined
	 */
	virtual void activateMultipath( bool val ) = 0;

	/**
	 * Rescan all disks.
	 * All currently detected objects are forgotten and a new scan
	 * for all type of objects (disks, LVM, MD) is initiated.
	 * This function makes sense to be called after something outside
	 * of libstorage changed disk layout or created storage objects.
	 * Any changes already cached are lost.
	 */
	virtual void rescanEverything() = 0;

	/**
	 * Rescan after unlocked encrypted volume.
	 * Rescan for objects that might be newly found after at least one
	 * encrypted volume has been unlocked. Currently the only supported
	 * containers on an decrypted volume is a LVM Volume Group.
	 *
	 * @return true if at least on encrypted container has been found
	 */
	virtual bool rescanCryptedObjects() = 0;

	/**
	 * Dump list of all objects to log file.
	 */
	virtual void dumpObjectList() = 0;

	/**
	 * Dump list of commit actions to log file.
	 */
	virtual void dumpCommitInfos() const = 0;

	/**
	 * Split volume device name up into container name and a volume
	 * name. For Containers where this is appropriate (e.g. disks,
	 * MD, loop) also a volume number is provided.
	 *
	 * @param dev device name of volume, e.g. /dev/hda1
	 * @param info record that get filled with split data
	 * @return zero if all is ok, negative number to indicate an error
	 */
	virtual int getContVolInfo(const string& dev, ContVolInfo& info) = 0;

	/**
	 * Set new userdata of a device. The userdata is only stored by
	 * libstorage but not processed.
	 *
	 * @param device device name of volume, e.g. /dev/sda1
	 * @param userdata new userdata for the device
	 * @return zero if all is ok, negative number to indicate an error
	 */
	virtual int setUserdata(const string& device, const map<string, string>& userdata) = 0;

	/**
	 * Get the userdata of a device.
	 *
	 * @param device device name of volume, e.g. /dev/sda1
	 * @param userdata gets filled with the userdata of the device
	 * @return zero if all is ok, negative number to indicate an error
	 */
	virtual int getUserdata(const string& device, map<string, string>& userdata) = 0;

    };


#endif


    /**
     * Initializes default logging with explicit log dir.
     */
    void initDefaultLogger( const string& logdir );

    /**
     * typedef for a pointer to a function that gets called for every logged
     * entry. Called function should be able to split content at newlines
     */
    typedef void (*CallbackLogDo)( int level, const string& component, const char* file,
                                   int line, const char* function, const string& content );

    /**
     * typedef for a pointer to a function that returns if specified level
     * should be logged
     */
    typedef bool (*CallbackLogQuery)( int level, const string& component );

    /**
     * Set logging callback function
     */
    void setLogDoCallback( CallbackLogDo pfc );

    /**
     * Get logging callback function
     */
    CallbackLogDo getLogDoCallback();

    /**
     * Set logging query callback function
     */
    void setLogQueryCallback( CallbackLogQuery pfc );

    /**
     * Get logging callback function
     */
    CallbackLogQuery getLogQueryCallback();


#if !defined(SWIG) || defined(LIBSTORAGE_SWIG_RUBY_LEGACY)


    /**
     * Contains basic environment settings controlling the behaviour of libstorage.
     */
    struct Environment
    {
	Environment(bool readonly, const string& logdir = "/var/log/YaST2")
	    : readonly(readonly), testmode(false), autodetect(true),
	      instsys(false), logdir(logdir), testdir("tmp")
            {
		initDefaultLogger( logdir );
            }

	bool readonly;
	bool testmode;
	bool autodetect;
	bool instsys;
	string logdir;
	string testdir;
    };


    /**
     * Factory for creating a concrete StorageInterface object.
     *
     * Throws an exception when locking fails.
     */
    StorageInterface* createStorageInterface(const Environment& env);


    /**
     * Factory for creating a concrete StorageInterface object.
     *
     * Returns NULL if locking failed. In that case locker_pid is set to the
     * pid of one process holding a conflicting lock. If the pid cannot be
     * determined it is set to 0. The lock holder may run on another system.
     */
    StorageInterface* createStorageInterfacePid(const Environment& env, int& SWIG_OUTPUT(locker_pid));


    /**
     * Destroy a StorageInterface object.
     */
    void destroyStorageInterface(StorageInterface*);


#endif

}


#endif
