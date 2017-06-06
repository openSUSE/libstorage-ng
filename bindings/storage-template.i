
%template(VectorString) std::vector<std::string>;
%template(MapStringString) std::map<std::string, std::string>;
%template(PairBoolString) std::pair<bool, std::string>;

%template(VectorCompoundActionPtr) std::vector<CompoundAction*>;
%template(VectorConstCompoundActionPtr) std::vector<const CompoundAction*>;

%template(VectorDevicePtr) std::vector<Device*>;
%template(VectorConstDevicePtr) std::vector<const Device*>;

%template(VectorBlkDevicePtr) std::vector<BlkDevice*>;
%template(VectorConstBlkDevicePtr) std::vector<const BlkDevice*>;

%template(VectorDiskPtr) std::vector<Disk*>;
%template(VectorConstDiskPtr) std::vector<const Disk*>;

%template(VectorDasdPtr) std::vector<Dasd*>;
%template(VectorConstDasdPtr) std::vector<const Dasd*>;

%template(VectorMdPtr) std::vector<Md*>;
%template(VectorConstMdPtr) std::vector<const Md*>;

%template(VectorLvmPvPtr) std::vector<LvmPv*>;
%template(VectorConstLvmPvPtr) std::vector<const LvmPv*>;

%template(VectorLvmVgPtr) std::vector<LvmVg*>;
%template(VectorConstLvmVgPtr) std::vector<const LvmVg*>;

%template(VectorLvmLvPtr) std::vector<LvmLv*>;
%template(VectorConstLvmLvPtr) std::vector<const LvmLv*>;

%template(VectorPartitionablePtr) std::vector<Partitionable*>;
%template(VectorConstPartitionablePtr) std::vector<const Partitionable*>;

%template(VectorPartitionPtr) std::vector<Partition*>;
%template(VectorConstPartitionPtr) std::vector<const Partition*>;

%template(VectorEncryptionPtr) std::vector<Encryption*>;
%template(VectorConstEncryptionPtr) std::vector<const Encryption*>;

%template(VectorLuksPtr) std::vector<Luks*>;
%template(VectorConstLuksPtr) std::vector<const Luks*>;

%template(VectorBcachePtr) std::vector<Bcache*>;
%template(VectorConstBcachePtr) std::vector<const Bcache*>;

%template(VectorBcacheCsetPtr) std::vector<BcacheCset*>;
%template(VectorConstBcacheCsetPtr) std::vector<const BcacheCset*>;

%template(VectorMountablePtr) std::vector<Mountable*>;
%template(VectorConstMountablePtr) std::vector<const Mountable*>;

%template(VectorFilesystemPtr) std::vector<Filesystem*>;
%template(VectorConstFilesystemPtr) std::vector<const Filesystem*>;

%template(VectorBlkFilesystemPtr) std::vector<BlkFilesystem*>;
%template(VectorConstBlkFilesystemPtr) std::vector<const BlkFilesystem*>;

%template(VectorBtrfsSubvolumePtr) std::vector<BtrfsSubvolume*>;
%template(VectorConstBtrfsSubvolumePtr) std::vector<const BtrfsSubvolume*>;

%template(VectorNtfsPtr) std::vector<Ntfs*>;
%template(VectorConstNtfsPtr) std::vector<const Ntfs*>;

%template(VectorSwapPtr) std::vector<Swap*>;
%template(VectorConstSwapPtr) std::vector<const Swap*>;

%template(VectorNfsPtr) std::vector<Nfs*>;
%template(VectorConstNfsPtr) std::vector<const Nfs*>;

%template(VectorMountPointPtr) std::vector<MountPoint*>;
%template(VectorConstMountPointPtr) std::vector<const MountPoint*>;

%template(VectorHolderPtr) std::vector<Holder*>;
%template(VectorConstHolderPtr) std::vector<const Holder*>;

%template(VectorPartitionSlot) std::vector<PartitionSlot>;

%template(VectorPtType) std::vector<PtType>;

