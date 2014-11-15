#ifndef LVM_VG_H
#define LVM_VG_H


#include "storage/Devices/Device.h"


namespace storage
{

    using namespace std;


    class LvmVg : public Device
    {
    public:

	LvmVg(const string& name);

	virtual LvmVg* clone() const override;

	const string& getName() const;
	void setName(const string& name);

	virtual string display_name() const override { return getName(); }

	virtual void check() const override;

    protected:

	class Impl;

	LvmVg(Impl* impl);

	Impl& getImpl();
	const Impl& getImpl() const;

    };

}

#endif
