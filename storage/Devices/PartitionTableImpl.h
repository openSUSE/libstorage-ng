#ifndef PARTITION_TABLE_IMPL_H
#define PARTITION_TABLE_IMPL_H


#include "storage/Devices/PartitionTable.h"
#include "storage/Devices/DeviceImpl.h"


namespace storage
{

    using namespace std;


    // abstract class

    class PartitionTable::Impl : public Device::Impl
    {
    public:

	// read-only

    protected:

	Impl()
	    : Device::Impl() {}

	Impl(const xmlNode* node);

	virtual void save(xmlNode* node) const override;

    };

}

#endif
