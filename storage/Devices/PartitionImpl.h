#ifndef PARTITION_IMPL_H
#define PARTITION_IMPL_H


#include "storage/Devices/Partition.h"
#include "storage/Devices/BlkDeviceImpl.h"


namespace storage
{

    using namespace std;


    class Partition::Impl : public BlkDevice::Impl
    {
    public:

	Impl(const string& name) : BlkDevice::Impl(name) {}

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void add_create_actions(ActionGraph& action_graph) const override;

    };

}

#endif
