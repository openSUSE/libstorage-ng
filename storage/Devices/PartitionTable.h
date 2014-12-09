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

	Partition* create_partition(const string& name);

	vector<const Partition*> get_partitions() const;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

    protected:

	PartitionTable(Impl* impl);

    };

}

#endif
