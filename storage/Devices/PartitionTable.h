#ifndef PARTITION_TABLE_H
#define PARTITION_TABLE_H


#include <vector>

#include "storage/Devices/Device.h"
#include "storage/Devices/Partition.h"


namespace storage
{

    using namespace std;


    enum class PtType {
	MSDOS, GPT, DASD
    };


    // abstract class

    class PartitionTable : public Device
    {
    public:

	Partition* createPartition(const string& name);

	vector<const Partition*> getPartitions() const;

    public:

	class Impl;

	Impl& getImpl();
	const Impl& getImpl() const;

    protected:

	PartitionTable(Impl* impl);

    };

}

#endif
