#ifndef LVM_LV_H
#define LVM_LV_H


#include "storage/Devices/BlkDevice.h"


namespace storage
{

    using namespace std;


    class LvmLv : public BlkDevice
    {
    public:

	static LvmLv* create(DeviceGraph* device_graph, const string& name);
	static LvmLv* load(DeviceGraph* device_graph, const xmlNode* node);

	virtual void check() const override;

    public:

	class Impl;

	Impl& getImpl();
	const Impl& getImpl() const;

	virtual const char* getClassName() const override { return "LvmLv"; }

	virtual LvmLv* clone() const override;

    protected:

	LvmLv(Impl* impl);

    };

}

#endif
