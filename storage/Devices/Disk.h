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

	static Disk* create(DeviceGraph* device_graph, const string& name);
	static Disk* load(DeviceGraph* device_graph, const xmlNode* node);

	PartitionTable* createPartitionTable(PtType pt_type);

    public:

	class Impl;

	Impl& getImpl();
	const Impl& getImpl() const;

	virtual const char* getClassName() const override { return "Disk"; }

	virtual Disk* clone() const override;

    protected:

	Disk(Impl* impl);

    };

}

#endif
