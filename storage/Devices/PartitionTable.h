#ifndef PARTITION_TABLE_H
#define PARTITION_TABLE_H


#include "storage/Devices/Device.h"


namespace storage
{

    using namespace std;


    // abstract class

    class PartitionTable : public Device
    {
    public:

    protected:

	class Impl;

	PartitionTable(Impl* impl);

	Impl& getImpl();
	const Impl& getImpl() const;

    };

}

#endif
