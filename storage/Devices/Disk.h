#ifndef DISK_H
#define DISK_H


#include "storage/Devices/BlkDevice.h"
#include "storage/Devices/PartitionTable.h"


namespace storage
{

    using namespace std;


    class Disk : public BlkDevice
    {
    public:

	Disk(DeviceGraph& device_graph, const string& name);

	static Disk* create(DeviceGraph& device_graph, const string& name)
	    { return new Disk(device_graph, name); }

	PartitionTable* createPartitionTable(const string& type);

    public:

	class Impl;

	Impl& getImpl();
	const Impl& getImpl() const;

	virtual Disk* clone(DeviceGraph& device_graph) const override;

    protected:

	Disk(Impl* impl);
	// Disk(DeviceGraph& device_graph, Impl* impl);

    };

}

#endif
