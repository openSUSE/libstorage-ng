#ifndef DISK_H
#define DISK_H


#include "storage/Devices/BlkDevice.h"


namespace storage
{

    using namespace std;


    class Disk : public BlkDevice
    {
    public:

	Disk(const string& name);

	virtual Disk* clone() const override;

    protected:

	class Impl;

	Disk(Impl* impl);

	Impl& getImpl();
	const Impl& getImpl() const;

    };

}

#endif
