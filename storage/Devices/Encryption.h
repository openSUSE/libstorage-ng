#ifndef ENCRYPTION_H
#define ENCRYPTION_H


#include "storage/Devices/BlkDevice.h"


namespace storage
{

    using namespace std;


    class Encryption : public BlkDevice
    {
    public:

	Encryption(const string& name);

	virtual Encryption* clone() const override;

    protected:

	class Impl;

	Encryption(Impl* impl);

	Impl& getImpl();
	const Impl& getImpl() const;

    };

}

#endif
