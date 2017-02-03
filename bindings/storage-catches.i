
%exceptionclass storage::Exception;

%exceptionclass storage::LogicException;

%exceptionclass storage::NullPointerException;
%exceptionclass storage::OutOfMemoryException;
%exceptionclass storage::IndexOutOfRangeException;
%exceptionclass storage::OverflowException;
%exceptionclass storage::ParseException;

%exceptionclass storage::DeviceHasWrongType;
%exceptionclass storage::DeviceNotFound;

%exceptionclass storage::HolderHasWrongType;
%exceptionclass storage::HolderNotFound;
%exceptionclass storage::HolderAlreadyExists;

%exceptionclass storage::WrongNumberOfChildren;
%exceptionclass storage::WrongNumberOfParents;

%exceptionclass storage::InvalidBlockSize;
%exceptionclass storage::NoIntersection;
%exceptionclass storage::DifferentBlockSizes;
%exceptionclass storage::NotInside;


%catches(storage::Exception) storage::Storage::Storage(const Environment&);

%catches(storage::ParseException, storage::OverflowException) storage::humanstring_to_byte(const std::string&, bool);

%catches(storage::DeviceHasWrongType) storage::to_blk_device(Device*);
%catches(storage::DeviceHasWrongType) storage::to_blk_device(const Device*);

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

%catches(storage::DeviceNotFound, storage::DeviceHasWrongType) storage::BlkDevice::find_by_name(Devicegraph*, const std::string&);
%catches(storage::DeviceNotFound, storage::DeviceHasWrongType) storage::BlkDevice::find_by_name(const Devicegraph*, const std::string&);

%catches(storage::DeviceNotFound, storage::DeviceHasWrongType) storage::Disk::find_by_name(Devicegraph*, const std::string&);
%catches(storage::DeviceNotFound, storage::DeviceHasWrongType) storage::Disk::find_by_name(const Devicegraph*, const std::string&);

%catches(storage::DeviceNotFound, storage::DeviceHasWrongType) storage::Partition::find_by_name(Devicegraph*, const std::string&);
%catches(storage::DeviceNotFound, storage::DeviceHasWrongType) storage::Partition::find_by_name(const Devicegraph*, const std::string&);

%catches(storage::WrongNumberOfChildren, storage::DeviceHasWrongType) storage::Partitionable::get_partition_table();
%catches(storage::WrongNumberOfChildren, storage::DeviceHasWrongType) storage::Partitionable::get_partition_table() const;

%catches(storage::WrongNumberOfChildren, storage::NotImplementedException) storage::Partitionable::create_partition_table(PtType);

%catches(storage::NotInside) storage::PartitionTable::get_unused_partition_slots() const;
%catches(storage::NotInside) storage::PartitionTable::get_unused_partition_slots(AlignPolicy) const;

%catches(storage::WrongNumberOfChildren, storage::DeviceHasWrongType) storage::BlkDevice::get_filesystem();
%catches(storage::WrongNumberOfChildren, storage::DeviceHasWrongType) storage::BlkDevice::get_filesystem() const;

%catches(storage::WrongNumberOfChildren, storage::NotImplementedException) storage::BlkDevice::create_filesystem(FsType);

%catches(storage::WrongNumberOfChildren) storage::Md::add_device(BlkDevice*);

%catches(storage::Exception) storage::Storage::create_devicegraph(const std::string&);
%catches(storage::Exception) storage::Storage::copy_devicegraph(const std::string&, const std::string&);
%catches(storage::Exception) storage::Storage::remove_devicegraph(const std::string&);
%catches(storage::Exception) storage::Storage::restore_devicegraph(const std::string&);
%catches(storage::Exception) storage::Storage::Storage(const Environment&);
