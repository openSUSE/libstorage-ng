#ifndef STORAGE_PARTITION_TABLE_IMPL_H
#define STORAGE_PARTITION_TABLE_IMPL_H


#include "storage/Devices/PartitionTable.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Utils/Enum.h"


namespace storage
{

    using namespace std;


    template <> struct EnumTraits<PtType> { static const vector<string> names; };

    std::ostream& operator<<(std::ostream& s, const PartitionSlot& partition_slot);


    template <> struct DeviceTraits<PartitionTable> { static const char* classname; };


    // abstract class

    class PartitionTable::Impl : public Device::Impl
    {
    public:

	virtual void probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo) override;

	Partition* create_partition(const string& name, const Region& region, PartitionType type);

	void delete_partition(const string& name);

	virtual PtType get_type() const = 0;

	Partition* get_partition(const string& name);

	vector<Partition*> get_partitions();
	vector<const Partition*> get_partitions() const;

	Partitionable* get_partitionable();
	const Partitionable* get_partitionable() const;

	virtual bool equal(const Device::Impl& rhs) const override = 0;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override = 0;

	virtual void print(std::ostream& out) const override = 0;

	virtual unsigned int max_primary() const = 0;
	virtual bool extended_possible() const { return false; }
	virtual unsigned int max_logical() const { return 0; }

	virtual unsigned int num_primary() const;
	virtual bool has_extended() const { return false; }
	virtual unsigned int num_logical() const { return 0; }

	const Partition* get_extended() const;

	virtual Region get_usable_region() const;

	std::vector<PartitionSlot> get_unused_partition_slots(bool all = true, bool logical = true) const;

	Region align(const Region& region, AlignPolicy align_policy = AlignPolicy::ALIGN_END) const;

    protected:

	Impl()
	    : Device::Impl(), read_only(false) {}

	Impl(const xmlNode* node);

	virtual void save(xmlNode* node) const override;

    private:

	bool read_only;

    };

}

#endif
