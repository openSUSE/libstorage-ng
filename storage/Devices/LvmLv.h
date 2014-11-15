#ifndef LVM_LV_H
#define LVM_LV_H


#include "storage/Devices/BlkDevice.h"


namespace storage
{

    using namespace std;


    class LvmLv : public BlkDevice
    {
    public:

	LvmLv(const string& name);

	virtual LvmLv* clone() const override;

	virtual void check() const override;

    protected:

	class Impl;

	LvmLv(Impl* impl);

	Impl& getImpl();
	const Impl& getImpl() const;

    };

}

#endif
