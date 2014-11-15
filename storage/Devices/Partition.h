#ifndef PARTITION_H
#define PARTITION_H


#include "storage/Devices/BlkDevice.h"


namespace storage
{

    using namespace std;


    class Partition : public BlkDevice
    {
    public:

	Partition(const string& name);

	virtual Partition* clone() const override;

	unsigned int getNumber() const;

    protected:

	class Impl;

	Partition(Impl* impl);

	Impl& getImpl();
	const Impl& getImpl() const;

    };

}

#endif
