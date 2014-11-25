#ifndef PARTITION_H
#define PARTITION_H


#include "storage/Devices/BlkDevice.h"


namespace storage
{

    using namespace std;


    class Partition : public BlkDevice
    {
    public:

	static Partition* create(DeviceGraph& device_graph, const string& name)
	    { return new Partition(device_graph, name); }

	virtual Partition* clone(DeviceGraph& device_graph) const override;

	unsigned int getNumber() const;

    public:

	class Impl;

	Impl& getImpl();
	const Impl& getImpl() const;

    protected:

	Partition(Impl* impl);
	// Partition(DeviceGraph& device_graph, Impl* impl);

    private:

	Partition(DeviceGraph& device_graph, const string& name);

    };

}

#endif
