#ifndef PARTITION_IMPL_H
#define PARTITION_IMPL_H


#include "storage/Devices/Partition.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Utils/Region.h"


namespace storage_bgl
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

	virtual void add_create_actions(Actiongraph& actiongraph) const override;

    private:

	Region region;
	storage::PartitionType type;
	unsigned id;
	bool boot;

    };

}

#endif
