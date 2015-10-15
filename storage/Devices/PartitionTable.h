#ifndef PARTITION_TABLE_H
#define PARTITION_TABLE_H


#include <vector>

#include "storage/Devices/Device.h"
#include "storage/Devices/Partition.h"


namespace storage
{
    class Disk;


    //! Partition Table Type.
    enum class PtType {
	PT_UNKNOWN, PT_LOOP, MSDOS, GPT, DASD, MAC
    };


    // abstract class

    class PartitionTable : public Device
    {
    public:

	Partition* create_partition(const std::string& name, PartitionType type);

	void delete_partition(const std::string& name);

	std::vector<const Partition*> get_partitions() const;

	Partition* get_partition(const std::string& name);

	const Disk* get_disk() const;

	list<PartitionSlotInfo> get_unused_partition_slots(bool all = true, bool logical = true) const;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

    protected:

	PartitionTable(Impl* impl);

    };


    bool is_partition_table(const Device* device);

    PartitionTable* to_partition_table(Device* device);
    const PartitionTable* to_partition_table(const Device* device);

}

#endif
