#ifndef STORAGE_PARTITIONABLE_IMPL_H
#define STORAGE_PARTITIONABLE_IMPL_H


#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Devices/Partitionable.h"
#include "storage/Geometry.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Partitionable> { static const char* classname; };


    // abstract class

    class Partitionable::Impl : public BlkDevice::Impl
    {
    public:

	const Geometry& get_geometry() const { return geometry; }
	void set_geometry(const Geometry& geometry) { Impl::geometry = geometry; }

	unsigned int get_range() const { return range; }
	void set_range(unsigned int range) { Impl::range = range; }

	PtType get_default_partition_table_type() const;

	std::vector<PtType> get_possible_partition_table_types() const;

	virtual void probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo) override;

	PartitionTable* create_partition_table(PtType pt_type);

	PartitionTable* get_partition_table();
	const PartitionTable* get_partition_table() const;

	virtual string partition_name(int number) const;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

    protected:

	Impl(const string& name)
	    : BlkDevice::Impl(name), range(0) {}

	Impl(const xmlNode* node);

	void save(xmlNode* node) const override;

    private:

	Geometry geometry;

	unsigned int range;

    };

}

#endif
