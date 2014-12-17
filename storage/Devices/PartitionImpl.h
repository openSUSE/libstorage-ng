#ifndef PARTITION_IMPL_H
#define PARTITION_IMPL_H


#include "storage/Devices/Partition.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Utils/Region.h"


namespace storage
{

    using namespace std;


    class Partition::Impl : public BlkDevice::Impl
    {
    public:

	Impl(const string& name)
	    : BlkDevice::Impl(name) {}

	Impl(const xmlNode* node);

	virtual Impl* clone() const override { return new Impl(*this); }

	void probe(SystemInfo& systeminfo);

	virtual void save(xmlNode* node) const override;

	unsigned int get_number() const;

	Region get_region() const { return region; }
	void set_region(const Region& region) { Impl::region = region; }

	PartitionType get_type() const { return type; }
	unsigned get_id() const { return id; }
	bool get_boot() const { return boot; }

	virtual void add_create_actions(Actiongraph& actiongraph) const override;

    private:

	Region region;
	PartitionType type;
	unsigned id;
	bool boot;

    };

}

#endif
