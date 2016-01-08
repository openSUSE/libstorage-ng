
%catches(storage::ParseError) storage::humanstring_to_byte(const std::string&, bool);

%catches(storage::DeviceHasWrongType) storage::to_blkdevice(Device*);
%catches(storage::DeviceHasWrongType) storage::to_blkdevice(const Device*);

%catches(storage::DeviceHasWrongType) storage::to_partitionable(Device*);
%catches(storage::DeviceHasWrongType) storage::to_partitionable(const Device*);

%catches(storage::DeviceHasWrongType) storage::to_disk(Device*);
%catches(storage::DeviceHasWrongType) storage::to_disk(const Device*);

%catches(storage::DeviceHasWrongType) storage::to_md(Device*);
%catches(storage::DeviceHasWrongType) storage::to_md(const Device*);

%catches(storage::DeviceHasWrongType) storage::to_partition_table(Device*);
%catches(storage::DeviceHasWrongType) storage::to_partition_table(const Device*);

%catches(storage::DeviceHasWrongType) storage::to_msdos(Device*);
%catches(storage::DeviceHasWrongType) storage::to_msdos(const Device*);

%catches(storage::DeviceHasWrongType) storage::to_gpt(Device*);
%catches(storage::DeviceHasWrongType) storage::to_gpt(const Device*);

%catches(storage::DeviceHasWrongType) storage::to_partition(Device*);
%catches(storage::DeviceHasWrongType) storage::to_partition(const Device*);

%catches(storage::DeviceHasWrongType) storage::to_ext4(Device*);
%catches(storage::DeviceHasWrongType) storage::to_ext4(const Device*);

%catches(storage::DeviceHasWrongType) storage::to_xfs(Device*);
%catches(storage::DeviceHasWrongType) storage::to_xfs(const Device*);

%catches(storage::DeviceHasWrongType) storage::to_btrfs(Device*);
%catches(storage::DeviceHasWrongType) storage::to_btrfs(const Device*);

%catches(storage::DeviceHasWrongType) storage::to_swap(Device*);
%catches(storage::DeviceHasWrongType) storage::to_swap(const Device*);

%catches(storage::DeviceHasWrongType) storage::to_ntfs(Device*);
%catches(storage::DeviceHasWrongType) storage::to_ntfs(const Device*);

%catches(storage::DeviceHasWrongType) storage::to_vfat(Device*);
%catches(storage::DeviceHasWrongType) storage::to_vfat(const Device*);

%catches(storage::HolderHasWrongType) storage::to_subdevice(Holder*);
%catches(storage::HolderHasWrongType) storage::to_subdevice(const Holder*);

%catches(storage::HolderHasWrongType) storage::to_user(Holder*);
%catches(storage::HolderHasWrongType) storage::to_user(const Holder*);

%catches(storage::DeviceNotFound) storage::Devicegraph::find_device(sid_t);
%catches(storage::DeviceNotFound) storage::Devicegraph::find_device(sid_t) const;

%catches(storage::HolderNotFound) storage::Devicegraph::find_holder(sid_t, sid_t);
%catches(storage::HolderNotFound) storage::Devicegraph::find_holder(sid_t, sid_t) const;

%catches(storage::DeviceNotFound, storage::DeviceHasWrongType) storage::BlkDevice::find(Devicegraph*, const std::string&);
%catches(storage::DeviceNotFound, storage::DeviceHasWrongType) storage::BlkDevice::find(const Devicegraph*, const std::string&);

%catches(storage::DeviceNotFound, storage::DeviceHasWrongType) storage::Disk::find(Devicegraph*, const std::string&);
%catches(storage::DeviceNotFound, storage::DeviceHasWrongType) storage::Disk::find(const Devicegraph*, const std::string&);

%catches(storage::DeviceNotFound, storage::DeviceHasWrongType) storage::Partition::find(Devicegraph*, const std::string&);
%catches(storage::DeviceNotFound, storage::DeviceHasWrongType) storage::Partition::find(const Devicegraph*, const std::string&);

%catches(storage::WrongNumberOfChildren, storage::DeviceHasWrongType) storage::Partitionable::get_partition_table();
%catches(storage::WrongNumberOfChildren, storage::DeviceHasWrongType) storage::Partitionable::get_partition_table() const;

%catches(storage::WrongNumberOfChildren, storage::NotImplementedException) storage::Partitionable::create_partition_table(PtType);

%catches(storage::WrongNumberOfChildren, storage::DeviceHasWrongType) storage::BlkDevice::get_filesystem();
%catches(storage::WrongNumberOfChildren, storage::DeviceHasWrongType) storage::BlkDevice::get_filesystem() const;

%catches(storage::WrongNumberOfChildren, storage::NotImplementedException) storage::BlkDevice::create_filesystem(FsType);

%catches(storage::WrongNumberOfChildren) storage::Md::add_device(BlkDevice*);

