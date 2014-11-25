#ifndef LVM_LV_H
#define LVM_LV_H


#include "storage/Devices/BlkDevice.h"


namespace storage
{

    using namespace std;


    class LvmLv : public BlkDevice
    {
    public:

	LvmLv(DeviceGraph& device_graph, const string& name);

	static LvmLv* create(DeviceGraph& device_graph, const string& name)
	    { return new LvmLv(device_graph, name); }

	virtual void check() const override;

    public:

	class Impl;

	Impl& getImpl();
	const Impl& getImpl() const;

	virtual LvmLv* clone(DeviceGraph& device_graph) const override;

    protected:

	LvmLv(Impl* impl);
	LvmLv(DeviceGraph& device_graph, Impl* impl);

    };

}

#endif
