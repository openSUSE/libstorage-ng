#ifndef STORAGE_PARTITION_TABLE_H
#define STORAGE_PARTITION_TABLE_H


#include <vector>

#include "storage/Utils/Region.h"
#include "storage/Utils/Topology.h"
#include "storage/Devices/Device.h"
#include "storage/Devices/Partition.h"


namespace storage
{
    class Partitionable;


    //! Partition Table Type.
    enum class PtType {
	PT_UNKNOWN, PT_LOOP, MSDOS, GPT, DASD, MAC
    };

    std::string get_pt_type_name(PtType pt_type);


    struct PartitionSlot
    {
	PartitionSlot();

	Region region;
	unsigned nr;
	std::string name;
	bool primary_slot;
	bool primary_possible;
	bool extended_slot;
	bool extended_possible;
	bool logical_slot;
	bool logical_possible;
    };


    // abstract class

    class PartitionTable : public Device
    {
    public:

	PtType get_type() const;

	Partition* create_partition(const std::string& name, const Region& region, PartitionType type);

	void delete_partition(const std::string& name);

	unsigned int max_primary() const;
	bool extended_possible() const;
	unsigned int max_logical() const;

	unsigned int num_primary() const;
	bool has_extended() const;
	unsigned int num_logical() const;

	/**
	 * Sorted by partition number.
	 */
	std::vector<Partition*> get_partitions();

	/**
	 * @copydoc get_partitions()
	 */
	std::vector<const Partition*> get_partitions() const;

	Partition* get_partition(const std::string& name);

	const Partitionable* get_partitionable() const;

	std::vector<PartitionSlot> get_unused_partition_slots(AlignPolicy align_policy = AlignPolicy::KEEP_END) const;

	Region align(const Region& region, AlignPolicy align_policy = AlignPolicy::ALIGN_END) const;

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
