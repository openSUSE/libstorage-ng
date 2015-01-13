

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <boost/noncopyable.hpp>

#include "storage/StorageInterface.h"
#include "storage/Utils/AppUtil.h"
#include "storage/Utils/Region.h"
#include "storage/Storage.h"
#include "storage/Devices/Disk.h"
#include "storage/Environment.h"


namespace storage_legacy
{
    using namespace std;
    using namespace storage;


    void
    initDefaultLogger(const string& logdir)
    {
	string path(logdir);
	string file("libstorage");

	if (geteuid())
	{
	    struct passwd* pw = getpwuid(geteuid());
	    if (pw)
	    {
		path = pw->pw_dir;
		file = ".libstorage";
	    }
	}

	createLogger(path, file);
    }


    class StorageLegacy : public StorageInterface, private boost::noncopyable
    {

    public:

	StorageLegacy(const Environment& env);
	virtual ~StorageLegacy();

	void setCacheChanges(bool val = true) override;
	bool isCacheChanges() const override;

	void rescanEverything() override;
	bool rescanCryptedObjects() override;

	void getContainers(deque<ContainerInfo>& infos) override;
	int getDiskInfo(const string& disk, DiskInfo& info) override;
	int getLvmVgInfo(const string& name, LvmVgInfo& info) override;
	int getDmraidCoInfo(const string& name, DmraidCoInfo& info) override;
	int getDmmultipathCoInfo(const string& name, DmmultipathCoInfo& info) override;
	int getContDiskInfo(const string& disk, ContainerInfo& cinfo, DiskInfo& info) override;
	int getContLvmVgInfo(const string& name, ContainerInfo& cinfo, LvmVgInfo& info) override;
	int getContDmraidCoInfo(const string& name, ContainerInfo& cinfo, DmraidCoInfo& info) override;
	int getContDmmultipathCoInfo(const string& name, ContainerInfo& cinfo, DmmultipathCoInfo& info) override;
	void getVolumes(deque<VolumeInfo>& vlist) override;
	int getVolume(const string& device, VolumeInfo& info) override;
	int getPartitionInfo(const string& disk, deque<PartitionInfo>& plist) override;
	int getLvmLvInfo(const string& name, deque<LvmLvInfo>& plist) override;
	int getMdInfo(deque<MdInfo>& plist) override;
	int getMdPartInfo(const string& device, deque<MdPartInfo>& plist) override;
	int getDmInfo(deque<DmInfo>& plist) override;
	int getNfsInfo(deque<NfsInfo>& plist) override;
	int getLoopInfo(deque<LoopInfo>& plist) override;
	int getBtrfsInfo(deque<BtrfsInfo>& plist) override;
	int getTmpfsInfo(deque<TmpfsInfo>& plist) override;
	int getDmraidInfo(const string& name, deque<DmraidInfo>& plist) override;
	int getDmmultipathInfo(const string& name, deque<DmmultipathInfo>& plist) override;
	int getContVolInfo(const string& dev, ContVolInfo& info) override;

	bool getFsCapabilities(FsType fstype, FsCapabilities& fscapabilities) const override;
	bool getDlabelCapabilities(const string& dlabel, DlabelCapabilities& dlabelcapabilities) const override;

	list<string> getAllUsedFs() const override;
	int createPartition(const string& disk, PartitionType type, const RegionInfo& cylRegion,
			    string& device) override;
	int resizePartition(const string& device, unsigned long sizeCyl) override;
	int resizePartitionNoFs(const string& device, unsigned long sizeCyl) override;
	int nextFreePartition(const string& disk, PartitionType type, unsigned& nr, string& device) override;
	int updatePartitionArea(const string& device, const RegionInfo& cylRegion) override;
	int freeCylindersAroundPartition(const string& device, unsigned long& freeCylsBefore,
					 unsigned long& freeCylsAfter) override;
	int createPartitionKb(const string& disk, PartitionType type, const RegionInfo& kRegion,
			      string& device) override;
	int createPartitionAny(const string& disk, unsigned long long size, string& device) override;
	int createPartitionMax(const string& disk, PartitionType type, string& device) override;
	unsigned long kbToCylinder(const string& disk, unsigned long long size) override;
	unsigned long long cylinderToKb(const string& disk, unsigned long size) override;
	int removePartition(const string& partition) override;
	int changePartitionId(const string& partition, unsigned id) override;
	int forgetChangePartitionId(const string& partition) override;

	string getPartitionPrefix(const string& disk) override;
	string getPartitionName(const string& disk, int partition_no) override;

	int getUnusedPartitionSlots(const string& disk, list<PartitionSlotInfo>& slots) override;
	int destroyPartitionTable(const string& disk, const string& label) override;
	int initializeDisk(const string& disk, bool value) override;
	string defaultDiskLabel(const string& device) override;

	int changeFormatVolume(const string& device, bool format, FsType fs) override;
	int changeLabelVolume(const string& device, const string& label) override;
	int changeMkfsOptVolume(const string& device, const string& opts) override;
	int changeTunefsOptVolume(const string& device, const string& opts) override;
	int changeDescText(const string& device, const string& txt) override;
	int changeMountPoint(const string& device, const string& mount) override;
	int getMountPoint(const string& device, string& mount) override;
	int changeMountBy(const string& device, MountByType mby) override;
	int getMountBy(const string& device, MountByType& mby) override;
	int changeFstabOptions(const string&, const string& options) override;
	int getFstabOptions(const string& device, string& options) override;
	int addFstabOptions(const string&, const string& options) override;
	int removeFstabOptions(const string&, const string& options) override;
	int setCryptPassword(const string& device, const string& pwd) override;
	int verifyCryptPassword(const string& device, const string& pwd, bool erase) override;
	bool needCryptPassword(const string& device) override;
	int forgetCryptPassword(const string& device) override;
	int getCryptPassword(const string& device, string& pwd) override;
	int setCrypt(const string& device, bool val) override;
	int setCryptType(const string& device, bool val, EncryptType typ) override;
	int getCrypt(const string& device, bool& val) override;
	int setIgnoreFstab(const string& device, bool val) override;
	int getIgnoreFstab(const string& device, bool& val) override;
	int addFstabEntry(const string& device, const string& mount, const string& vfs,
			  const string& options, unsigned freq, unsigned passno) override;
	int resizeVolume(const string& device, unsigned long long newSizeK) override;
	int resizeVolumeNoFs(const string& device, unsigned long long newSizeK) override;
	int forgetResizeVolume(const string& device) override;
	void setRecursiveRemoval(bool val = true) override;
	bool getRecursiveRemoval() const;

	int getRecursiveUsing(const list<string>& devices, bool itself, list<string>& using_devices) override;
	int getRecursiveUsedBy(const list<string>& devices, bool itself, list<string>& usedby_devices) override;

	void setZeroNewPartitions(bool val = true) override;
	bool getZeroNewPartitions() const override;

	void setPartitionAlignment(PartAlign val) override;
	PartAlign getPartitionAlignment() const override;

	void setDefaultMountBy(MountByType mby) override;
	MountByType getDefaultMountBy() const override;

	void setDefaultFs(FsType fs) override;
	FsType getDefaultFs() const override;

	void setDefaultSubvolName(const string& val) override;
	string getDefaultSubvolName() const override;

	void setDetectMountedVolumes(bool val = true) override;
	bool getDetectMountedVolumes() const override;
	bool getEfiBoot() override;
	void setRootPrefix(const string& root) override;
	string getRootPrefix() const override;
	int removeVolume(const string& device) override;
	bool checkDeviceMounted(const string& device, list<string>& mps) override;
	bool umountDevice(const string& device) override;
	bool umountDeviceUns(const string& device, bool unsetup) override;
	bool mountDevice(const string& device, const string& mp) override;
	bool mountDeviceOpts(const string& device, const string& mp, const string& opts) override;
	bool mountDeviceRo(const string& device, const string& mp, const string& opts) override;
	int activateEncryption(const string& device, bool on) override;
	bool readFstab(const string& dir, deque<VolumeInfo>& infos) override;

	bool getFreeInfo(const string& device, bool get_resize, ResizeInfo& resize_info,
			 bool get_content, ContentInfo& content_info, bool use_cache) override;

	int createBackupState(const string& name) override;
	int removeBackupState(const string& name) override;
	int restoreBackupState(const string& name) override;
	bool checkBackupState(const string& name) const override;
	bool equalBackupStates(const string& lhs, const string& rhs, bool verbose_log) const override;

	int createLvmVg(const string& name, unsigned long long peSizeK, bool lvm1,
			const deque<string>& devs) override;
	int removeLvmVg(const string& name) override;
	int extendLvmVg(const string& name, const deque<string>& devs) override;
	int shrinkLvmVg(const string& name, const deque<string>& devs) override;
	int createLvmLv(const string& vg, const string& name, unsigned long long sizeK, unsigned stripes,
			string& device) override;
	int removeLvmLvByDevice(const string& device) override;
	int removeLvmLv(const string& vg, const string& name) override;
	int changeLvStripeCount(const string& vg, const string& name, unsigned long stripes) override;
	int changeLvStripeSize(const string& vg, const string& name, unsigned long long stripeSize) override;

	int createLvmLvSnapshot(const string& vg, const string& origin, const string& name,
				unsigned long long cowSizeK, string& device) override;
	int removeLvmLvSnapshot(const string& vg, const string& name) override;
	int getLvmLvSnapshotStateInfo(const string& vg, const string& name, LvmLvSnapshotStateInfo& info) override;
	int createLvmLvPool(const string& vg, const string& name, unsigned long long sizeK,
			    string& device) override;
	int createLvmLvThin(const string& vg, const string& name, const string& pool,
			    unsigned long long sizeK, string& device) override;
	int changeLvChunkSize(const string& vg, const string& name, unsigned long long chunkSizeK) override;

	int nextFreeMd(unsigned& nr, string& device) override;
	int createMd(const string& name, MdType rtype, const list<string>& devs,
		     const list<string>& spares) override;
	int createMdAny(MdType rtype, const list<string>& devs, const list<string>& spares,
			string& device) override;
	int removeMd(const string& name, bool destroySb = true) override;
	int extendMd(const string& name, const list<string>& devs, const list<string>& spares) override;
	int updateMd(const string& name, const list<string>& devs, const list<string>& spares) override;
	int shrinkMd(const string& name, const list<string>& devs, const list<string>& spares) override;
	int changeMdType(const string& name, MdType rtype) override;
	int changeMdChunk(const string& name, unsigned long chunk) override;
	int changeMdParity(const string& name, MdParity ptype) override;
	int checkMd(const string& name) override;
	int getMdStateInfo(const string& name, MdStateInfo& info) override;
	int computeMdSize(MdType md_type, const list<string>& devices, const list<string>& spares,
			   unsigned long long& sizeK) override;
	list<int> getMdAllowedParity(MdType md_type, unsigned devices) override;
	void setMultipathAutostart(MultipathAutostart val) override;
	MultipathAutostart getMultipathAutostart() const override;

	int getMdPartCoInfo(const string& name, MdPartCoInfo& info) override;
	int getContMdPartCoInfo(const string& name, ContainerInfo& cinfo, MdPartCoInfo& info) override;
	int getMdPartCoStateInfo(const string& name, MdPartCoStateInfo& info) override;
	int removeMdPartCo(const string& devName, bool destroySb) override;

	int addNfsDevice(const string& nfsDev, const string& opts, unsigned long long sizeK,
			 const string& mp, bool nfs4) override;
	int checkNfsDevice(const string& nfsDev, const string& opts, bool nfs4,
			   unsigned long long& sizeK) override;

	int createFileLoop(const string& lname, bool reuseExisting, unsigned long long sizeK,
			   const string& mp, const string& pwd, string& device) override;
	int modifyFileLoop(const string& device, const string& lname, bool reuseExisting,
			   unsigned long long sizeK) override;
	int removeFileLoop(const string& lname, bool removeFile) override;

	int removeDmraid(const string& name) override;

	bool existSubvolume(const string& device, const string& name) override;
	int createSubvolume(const string& device, const string& name) override;
	int removeSubvolume(const string& device, const string& name) override;
	int extendBtrfsVolume(const string& device, const deque<string>& devs) override;
	int shrinkBtrfsVolume(const string& device, const deque<string>& devs) override;

	int addTmpfsMount(const string& mp, const string& opts) override;
	int removeTmpfsMount(const string& mp) override;

	void getCommitInfos(list<CommitInfo>& infos) const override;
	const string& getLastAction() const override;
	const string& getExtendedErrorMessage() const override;

	int commit() override;

	string getErrorString(int error) const override;

	void activateHld(bool val = true) override;
	void activateMultipath(bool val = true) override;

	void removeDmTableTo(const string& device) override;
	int renameCryptDm(const string& device, const string& new_name) override;
	bool checkDmMapsTo(const string& dev) override;
	void dumpObjectList() override;
	void dumpCommitInfos() const override;

	int setUserdata(const string& device, const map<string, string>& userdata) override;
	int getUserdata(const string& device, map<string, string>& userdata) override;

	void setCallbackProgressBar(CallbackProgressBar pfnc) override { progress_bar_cb = pfnc; }
        CallbackProgressBar getCallbackProgressBar() const override { return progress_bar_cb; }
        void setCallbackShowInstallInfo(CallbackShowInstallInfo pfnc) override { install_info_cb = pfnc; }
        CallbackShowInstallInfo getCallbackShowInstallInfo() const override { return install_info_cb; }
        void setCallbackInfoPopup(CallbackInfoPopup pfnc) override { info_popup_cb = pfnc; }
        CallbackInfoPopup getCallbackInfoPopup() const override { return info_popup_cb; }
        void setCallbackYesNoPopup(CallbackYesNoPopup pfnc) override { yesno_popup_cb = pfnc; }
        CallbackYesNoPopup getCallbackYesNoPopup() const override { return yesno_popup_cb; }
        void setCallbackCommitErrorPopup(CallbackCommitErrorPopup pfnc) override { commit_error_popup_cb = pfnc; }
        CallbackCommitErrorPopup getCallbackCommitErrorPopup() const override { return commit_error_popup_cb; }
        void setCallbackPasswordPopup(CallbackPasswordPopup pfnc) override { password_popup_cb = pfnc; }
        CallbackPasswordPopup getCallbackPasswordPopup() const override { return password_popup_cb; }

    private:

	Storage* storage;

        CallbackProgressBar progress_bar_cb;
        CallbackShowInstallInfo install_info_cb;
        CallbackInfoPopup info_popup_cb;
        CallbackYesNoPopup yesno_popup_cb;
        CallbackCommitErrorPopup commit_error_popup_cb;
        CallbackPasswordPopup password_popup_cb;

    };


    StorageInterface*
    createStorageInterface(const Environment& env)
    {
	y2mil("legacy " << __FUNCTION__);

	return new StorageLegacy(env);
    }


    StorageInterface*
    createStorageInterfacePid(const Environment& env, int& locker_pid)
    {
	y2mil("legacy " << __FUNCTION__);

	return new StorageLegacy(env);
    }


    void
    destroyStorageInterface(StorageInterface * s)
    {
	y2mil("legacy " << __FUNCTION__);

	delete s;
    }


    CallbackLogDo logger_do_fnc = defaultLogDo;
    CallbackLogQuery logger_query_fnc = defaultLogQuery;


    void
    setLogDoCallback(CallbackLogDo pfnc)
    {
	logger_do_fnc = pfnc;
    }


    CallbackLogDo
    getLogDoCallback()
    {
	return logger_do_fnc;
    }


    void
    setLogQueryCallback(CallbackLogQuery pfnc)
    {
	logger_query_fnc = pfnc;
    }


    CallbackLogQuery
    getLogQueryCallback()
    {
	return logger_query_fnc;
    }


    StorageLegacy::StorageLegacy(const Environment& env)
    {
	y2mil("legacy " << __FUNCTION__);

	storage::Environment environment(true);

	storage = new Storage(environment);

	progress_bar_cb = NULL;
	install_info_cb = NULL;
	info_popup_cb = NULL;
	yesno_popup_cb = NULL;
	commit_error_popup_cb = NULL;
	password_popup_cb = NULL;
    }


    StorageLegacy::~StorageLegacy()
    {
	y2mil("legacy " << __FUNCTION__);

	delete storage;
    }


    void
    StorageLegacy::setCacheChanges(bool val)
    {
	y2mil("legacy " << __FUNCTION__);
    }


    bool
    StorageLegacy::isCacheChanges() const
    {
	y2mil("legacy " << __FUNCTION__);

	return true;
    }


    void
    StorageLegacy::rescanEverything()
    {
	y2mil("legacy " << __FUNCTION__);
    }


    bool
    StorageLegacy::rescanCryptedObjects()
    {
	y2mil("legacy " << __FUNCTION__);

	return false;
    }


    void
    StorageLegacy::getContainers(deque<ContainerInfo>& infos)
    {
	y2mil("legacy " << __FUNCTION__);

	infos.clear();

	for (const Disk* disk : Disk::get_all(storage->get_current()))
	{
	    ContainerInfo info;
	    info.type = DISK;
	    info.name = disk->get_name().substr(5);
	    info.device = disk->get_name();
	    info.readonly = false;

	    info.udevPath = disk->get_udev_path();
	    info.udevId = list<string>(disk->get_udev_ids().begin(), disk->get_udev_ids().end());

	    y2mil("DISK " << info.name << " " <<  info.device);
	    infos.push_back(info);
	}
    }


    int
    StorageLegacy::getDiskInfo(const string& name, DiskInfo& info)
    {
	y2mil("legacy " << __FUNCTION__ << " " << name);

	const BlkDevice* blkdevice = BlkDevice::find(storage->get_current(), name);
	const Disk* disk = to_disk(blkdevice);

	if (disk)
	{
	    info.sizeK = disk->get_size_k();
	    info.transport = disk->get_transport();

	    const PartitionTable* partitiontable = disk->get_partition_table();
	    if (partitiontable)
	    {
		info.disklabel = partitiontable->get_displayname();
	    }

	    y2mil("DISK " << info.sizeK);
	    return 0;
	}

	return STORAGE_DISK_NOT_FOUND;
    }


    int
    StorageLegacy::getLvmVgInfo(const string& name, LvmVgInfo& info)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::getDmraidCoInfo(const string& name, DmraidCoInfo& info)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::getDmmultipathCoInfo(const string& name, DmmultipathCoInfo& info)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::getContDiskInfo(const string& disk, ContainerInfo& cinfo, DiskInfo& info)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::getContLvmVgInfo(const string& name, ContainerInfo& cinfo, LvmVgInfo& info)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::getContDmraidCoInfo(const string& name, ContainerInfo& cinfo, DmraidCoInfo& info)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::getContDmmultipathCoInfo(const string& name, ContainerInfo& cinfo,
					    DmmultipathCoInfo& info)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    void
    StorageLegacy::getVolumes(deque<VolumeInfo>& vlist)
    {
	y2mil("legacy " << __FUNCTION__);
    }


    int
    StorageLegacy::getVolume(const string& device, VolumeInfo& info)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::getPartitionInfo(const string& name, deque<PartitionInfo>& plist)
    {
	y2mil("legacy " << __FUNCTION__ << " " << name);

	plist.clear();

	const BlkDevice* blkdevice = BlkDevice::find(storage->get_current(), name);
	const Disk* disk = to_disk(blkdevice);

	if (disk)
	{
	    const PartitionTable* partitiontable = disk->get_partition_table();

	    for (const Partition* partition : partitiontable->get_partitions())
	    {
		PartitionInfo info;

		info.v.name = partition->get_name().substr(5);
		info.v.device = partition->get_name();
		info.v.sizeK = partition->get_size_k();

		info.v.udevPath = partition->get_udev_path();
		info.v.udevId = list<string>(partition->get_udev_ids().begin(), partition->get_udev_ids().end());

		info.cylRegion = partition->get_region();
		info.nr = partition->get_number();
		info.partitionType = partition->get_type();
		info.id = partition->get_id();
		info.boot = partition->get_boot();

		plist.push_back(info);
	    }

	    return 0;
	}

	return STORAGE_DISK_NOT_FOUND;
    }


    int
    StorageLegacy::getLvmLvInfo(const string& name, deque<LvmLvInfo>& plist)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::getMdInfo(deque<MdInfo>& plist)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::getMdPartInfo(const string& device, deque<MdPartInfo>& plist)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::getDmInfo(deque<DmInfo>& plist)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::getNfsInfo(deque<NfsInfo>& plist)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::getLoopInfo(deque<LoopInfo>& plist)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::getBtrfsInfo(deque<BtrfsInfo>& plist)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::getTmpfsInfo(deque<TmpfsInfo>& plist)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::getDmraidInfo(const string& name, deque<DmraidInfo>& plist)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::getDmmultipathInfo(const string& name, deque<DmmultipathInfo>& plist)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::getContVolInfo(const string& dev, ContVolInfo& info)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    bool
    StorageLegacy::getFsCapabilities(FsType fstype, FsCapabilities& fscapabilities) const
    {
	y2mil("legacy " << __FUNCTION__);

	return false;
    }


    bool
    StorageLegacy::getDlabelCapabilities(const string& dlabel, DlabelCapabilities& dlabelcapabilities) const
    {
	y2mil("legacy " << __FUNCTION__);

	return false;
    }


    list<string>
    StorageLegacy::getAllUsedFs() const
    {
	y2mil("legacy " << __FUNCTION__);

	return {};
    }


    int
    StorageLegacy::createPartition(const string& disk, PartitionType type, const RegionInfo& cylRegion,
				   string& device)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::resizePartition(const string& device, unsigned long sizeCyl)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::resizePartitionNoFs(const string& device, unsigned long sizeCyl)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::nextFreePartition(const string& disk, PartitionType type, unsigned& nr, string& device)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::updatePartitionArea(const string& device, const RegionInfo& cylRegion)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::freeCylindersAroundPartition(const string& device, unsigned long& freeCylsBefore,
						unsigned long& freeCylsAfter)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::createPartitionKb(const string& disk, PartitionType type, const RegionInfo& kRegion,
				     string& device)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::createPartitionAny(const string& disk, unsigned long long size, string& device)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::createPartitionMax(const string& disk, PartitionType type, string& device)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    unsigned long
    StorageLegacy::kbToCylinder(const string& disk, unsigned long long size)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    unsigned long long
    StorageLegacy::cylinderToKb(const string& disk, unsigned long size)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::removePartition(const string& partition)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::changePartitionId(const string& partition, unsigned id)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::forgetChangePartitionId(const string& partition)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    string
    StorageLegacy::getPartitionPrefix(const string& disk)
    {
	y2mil("legacy " << __FUNCTION__);

	return "";
    }


    string
    StorageLegacy::getPartitionName(const string& disk, int partition_no)
    {
	y2mil("legacy " << __FUNCTION__);

	return "";
    }


    int
    StorageLegacy::getUnusedPartitionSlots(const string& disk, list<PartitionSlotInfo>& slots)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::destroyPartitionTable(const string& disk, const string& label)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::initializeDisk(const string& disk, bool value)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    string
    StorageLegacy::defaultDiskLabel(const string& device)
    {
	y2mil("legacy " << __FUNCTION__);

	return "";
    }


    int
    StorageLegacy::changeFormatVolume(const string& device, bool format, FsType fs)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::changeLabelVolume(const string& device, const string& label)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::changeMkfsOptVolume(const string& device, const string& opts)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::changeTunefsOptVolume(const string& device, const string& opts)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::changeDescText(const string& device, const string& txt)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::changeMountPoint(const string& device, const string& mount)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::getMountPoint(const string& device, string& mount)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::changeMountBy(const string& device, MountByType mby)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::getMountBy(const string& device, MountByType& mby)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::changeFstabOptions(const string&, const string& options)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::getFstabOptions(const string& device, string& options)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::addFstabOptions(const string&, const string& options)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::removeFstabOptions(const string&, const string& options)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::setCryptPassword(const string& device, const string& pwd)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::verifyCryptPassword(const string& device, const string& pwd, bool erase)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    bool
    StorageLegacy::needCryptPassword(const string& device)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::forgetCryptPassword(const string& device)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::getCryptPassword(const string& device, string& pwd)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::setCrypt(const string& device, bool val)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::setCryptType(const string& device, bool val, EncryptType typ)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::getCrypt(const string& device, bool& val)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::setIgnoreFstab(const string& device, bool val)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::getIgnoreFstab(const string& device, bool& val)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::addFstabEntry(const string& device, const string& mount, const string& vfs,
				 const string& options, unsigned freq, unsigned passno)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::resizeVolume(const string& device, unsigned long long newSizeK)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::resizeVolumeNoFs(const string& device, unsigned long long newSizeK)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::forgetResizeVolume(const string& device)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    void
    StorageLegacy::setRecursiveRemoval(bool val)
    {
	y2mil("legacy " << __FUNCTION__);
    }


    bool
    StorageLegacy::getRecursiveRemoval() const
    {
	y2mil("legacy " << __FUNCTION__);

	return false;
    }


    int
    StorageLegacy::getRecursiveUsing(const list<string>& devices, bool itself, list<string>& using_devices)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::getRecursiveUsedBy(const list<string>& devices, bool itself, list<string>& usedby_devices)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    void
    StorageLegacy::setZeroNewPartitions(bool val)
    {
	y2mil("legacy " << __FUNCTION__);
    }


    bool
    StorageLegacy::getZeroNewPartitions() const
    {
	y2mil("legacy " << __FUNCTION__);

	return true;
    }


    void
    StorageLegacy::setPartitionAlignment(PartAlign val)
    {
	y2mil("legacy " << __FUNCTION__);
    }


    PartAlign
    StorageLegacy::getPartitionAlignment() const
    {
	y2mil("legacy " << __FUNCTION__);

	return ALIGN_OPTIMAL;
    }


    void
    StorageLegacy::setDefaultMountBy(MountByType mby)
    {
	y2mil("legacy " << __FUNCTION__);
    }


    MountByType
    StorageLegacy::getDefaultMountBy() const
    {
	y2mil("legacy " << __FUNCTION__);

	return MOUNTBY_UUID;
    }


    void
    StorageLegacy::setDefaultFs(FsType fs)
    {
	y2mil("legacy " << __FUNCTION__);
    }


    FsType
    StorageLegacy::getDefaultFs() const
    {
	y2mil("legacy " << __FUNCTION__);

	return BTRFS;
    }


    void
    StorageLegacy::setDefaultSubvolName(const string& val)
    {
	y2mil("legacy " << __FUNCTION__);
    }


    string
    StorageLegacy::getDefaultSubvolName() const
    {
	y2mil("legacy " << __FUNCTION__);

	return "";
    }


    void
    StorageLegacy::setDetectMountedVolumes(bool val)
    {
	y2mil("legacy " << __FUNCTION__);
    }


    bool
    StorageLegacy::getDetectMountedVolumes() const
    {
	y2mil("legacy " << __FUNCTION__);

	return false;
    }


    bool
    StorageLegacy::getEfiBoot()
    {
	y2mil("legacy " << __FUNCTION__);

	return false;
    }


    void
    StorageLegacy::setRootPrefix(const string& root)
    {
	y2mil("legacy " << __FUNCTION__);
    }


    string
    StorageLegacy::getRootPrefix() const
    {
	y2mil("legacy " << __FUNCTION__);

	return "";
    }


    int
    StorageLegacy::removeVolume(const string& device)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    bool
    StorageLegacy::checkDeviceMounted(const string& device, list<string>& mps)
    {
	y2mil("legacy " << __FUNCTION__);

	return false;
    }


    bool
    StorageLegacy::umountDevice(const string& device)
    {
	y2mil("legacy " << __FUNCTION__);

	return false;
    }


    bool
    StorageLegacy::umountDeviceUns(const string& device, bool unsetup)
    {
	y2mil("legacy " << __FUNCTION__);

	return false;
    }


    bool
    StorageLegacy::mountDevice(const string& device, const string& mp)
    {
	y2mil("legacy " << __FUNCTION__);

	return false;
    }


    bool
    StorageLegacy::mountDeviceOpts(const string& device, const string& mp, const string& opts)
    {
	y2mil("legacy " << __FUNCTION__);

	return false;
    }


    bool
    StorageLegacy::mountDeviceRo(const string& device, const string& mp, const string& opts)
    {
	y2mil("legacy " << __FUNCTION__);

	return false;
    }


    int
    StorageLegacy::activateEncryption(const string& device, bool on)
    {
	y2mil("legacy " << __FUNCTION__);

	return false;
    }


    bool
    StorageLegacy::readFstab(const string& dir, deque<VolumeInfo>& infos)
    {
	y2mil("legacy " << __FUNCTION__);

	return false;
    }


    bool
    StorageLegacy::getFreeInfo(const string& device, bool get_resize, ResizeInfo& resize_info,
			       bool get_content, ContentInfo& content_info, bool use_cache)
    {
	y2mil("legacy " << __FUNCTION__);

	return false;
    }


    int
    StorageLegacy::createBackupState(const string& name)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::removeBackupState(const string& name)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::restoreBackupState(const string& name)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    bool
    StorageLegacy::checkBackupState(const string& name) const
    {
	y2mil("legacy " << __FUNCTION__);

	return false;
    }


    bool
    StorageLegacy::equalBackupStates(const string& lhs, const string& rhs, bool verbose_log) const
    {
	y2mil("legacy " << __FUNCTION__);

	return false;
    }


    int
    StorageLegacy::createLvmVg(const string& name, unsigned long long peSizeK, bool lvm1,
			       const deque<string>& devs)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::removeLvmVg(const string& name)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::extendLvmVg(const string& name, const deque<string>& devs)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::shrinkLvmVg(const string& name, const deque<string>& devs)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::createLvmLv(const string& vg, const string& name, unsigned long long sizeK,
			       unsigned stripes, string& device)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::removeLvmLvByDevice(const string& device)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::removeLvmLv(const string& vg, const string& name)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::changeLvStripeCount(const string& vg, const string& name, unsigned long stripes)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::changeLvStripeSize(const string& vg, const string& name, unsigned long long stripeSize)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::createLvmLvSnapshot(const string& vg, const string& origin, const string& name,
				       unsigned long long cowSizeK, string& device)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::removeLvmLvSnapshot(const string& vg, const string& name)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::getLvmLvSnapshotStateInfo(const string& vg, const string& name,
					     LvmLvSnapshotStateInfo& info)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::createLvmLvPool(const string& vg, const string& name,
					unsigned long long sizeK,
					string& device)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::createLvmLvThin(const string& vg, const string& name, const string& pool,
				   unsigned long long sizeK, string& device)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::changeLvChunkSize(const string& vg, const string& name, unsigned long long chunkSizeK)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::nextFreeMd(unsigned& nr, string& device)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::createMd(const string& name, MdType rtype, const list<string>& devs,
			    const list<string>& spares)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::createMdAny(MdType rtype, const list<string>& devs, const list<string>& spares,
			       string& device)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::removeMd(const string& name, bool destroySb)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::extendMd(const string& name, const list<string>& devs, const list<string>& spares)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::updateMd(const string& name, const list<string>& devs, const list<string>& spares)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::shrinkMd(const string& name, const list< string>& devs, const list<string>& spares)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::changeMdType(const string& name, MdType rtype)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::changeMdChunk(const string& name, unsigned long chunk)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::changeMdParity(const string& name, MdParity ptype)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::checkMd(const string& name)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::getMdStateInfo(const string& name, MdStateInfo& info)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::computeMdSize(MdType md_type, const list<string>& devices,
				 const list<string>& spares,
				 unsigned long long& sizeK)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    list<int>
    StorageLegacy::getMdAllowedParity(MdType md_type, unsigned devices)
    {
	y2mil("legacy " << __FUNCTION__);

	return {};
    }


    void
    StorageLegacy::setMultipathAutostart(MultipathAutostart val)
    {
	y2mil("legacy " << __FUNCTION__);
    }


    MultipathAutostart
    StorageLegacy::getMultipathAutostart() const
    {
	y2mil("legacy " << __FUNCTION__);

	return MPAS_UNDECIDED;
    }


    int
    StorageLegacy::getMdPartCoInfo(const string& name, MdPartCoInfo& info)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::getContMdPartCoInfo(const string& name, ContainerInfo& cinfo, MdPartCoInfo& info)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::getMdPartCoStateInfo(const string& name, MdPartCoStateInfo& info)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::removeMdPartCo(const string& devName, bool destroySb)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::addNfsDevice(const string& nfsDev, const string& opts, unsigned long long sizeK,
				     const string& mp, bool nfs4)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::checkNfsDevice(const string& nfsDev, const string& opts, bool nfs4,
				  unsigned long long& sizeK)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::createFileLoop(const string& lname, bool reuseExisting, unsigned long long sizeK,
				  const string& mp, const string& pwd, string& device)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::modifyFileLoop(const string& device, const string& lname, bool reuseExisting,
				  unsigned long long sizeK)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::removeFileLoop(const string& lname, bool removeFile)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }



    int
    StorageLegacy::removeDmraid(const string& name)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    bool
    StorageLegacy::existSubvolume(const string& device, const string& name)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::createSubvolume(const string& device, const string& name)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::removeSubvolume(const string& device, const string& name)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::extendBtrfsVolume(const string& device, const deque<string>& devs)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::shrinkBtrfsVolume(const string& device, const deque<string>& devs)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::addTmpfsMount(const string& mp, const string& opts)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    int
    StorageLegacy::removeTmpfsMount(const string& mp)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    void
    StorageLegacy::getCommitInfos(list<CommitInfo>& infos) const
    {
	y2mil("legacy " << __FUNCTION__);
    }


    const string&
    StorageLegacy::getLastAction() const
    {
	y2mil("legacy " << __FUNCTION__);

	static const string ret = "some error";

	return ret;
    }


    const string&
    StorageLegacy::getExtendedErrorMessage() const
    {
	y2mil("legacy " << __FUNCTION__);

	static const string ret = "some error";

	return ret;
    }


    int
    StorageLegacy::commit()
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    string
    StorageLegacy::getErrorString(int error) const
    {
	y2mil("legacy " << __FUNCTION__);

	return "";
    }


    void
    StorageLegacy::activateHld(bool val)
    {
	y2mil("legacy " << __FUNCTION__);
    }


    void
    StorageLegacy::activateMultipath(bool val)
    {
	y2mil("legacy " << __FUNCTION__);
    }


    void
    StorageLegacy::removeDmTableTo(const string& device)
    {
	y2mil("legacy " << __FUNCTION__);
    }



    int
    StorageLegacy::renameCryptDm(const string& device,
				      const string& new_name)
    {
	y2mil("legacy " << __FUNCTION__);

	return -1;
    }


    bool
    StorageLegacy::checkDmMapsTo(const string& dev)
    {
	y2mil("legacy " << __FUNCTION__);

	return false;
    }


    void
    StorageLegacy::dumpObjectList()
    {
	y2mil("legacy " << __FUNCTION__);
    }


    void
    StorageLegacy::dumpCommitInfos() const
    {
	y2mil("legacy " << __FUNCTION__);
    }


    int
    StorageLegacy::setUserdata(const string& device, const map<string, string>& userdata)
    {
	y2mil("legacy " << __FUNCTION__);

	return false;
    }


    int
    StorageLegacy::getUserdata(const string& device, map<string, string>& userdata)
    {
	y2mil("legacy " << __FUNCTION__);

	return false;
    }


    CallbackProgressBar progress_bar_cb_ycp;
    CallbackShowInstallInfo install_info_cb_ycp;
    CallbackInfoPopup info_popup_cb_ycp;
    CallbackYesNoPopup yesno_popup_cb_ycp;
    CallbackCommitErrorPopup commit_error_popup_cb_ycp;
    CallbackPasswordPopup password_popup_cb_ycp;

}
