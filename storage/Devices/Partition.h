#ifndef PARTITION_H
#define PARTITION_H


#include "storage/Devices/BlkDevice.h"


namespace storage
{

    using namespace std;


    class Partition : public BlkDevice
    {
    public:

	static Partition* create(DeviceGraph* device_graph, const string& name);
	static Partition* load(DeviceGraph* device_graph, const xmlNode* node);

	unsigned int getNumber() const;

    public:

	class Impl;

	Impl& getImpl();
	const Impl& getImpl() const;

	virtual const char* getClassName() const override { return "Partition"; }

	virtual Partition* clone() const override;

    protected:

	Partition(Impl* impl);

    };

}

#endif
