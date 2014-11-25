#ifndef LVM_VG_H
#define LVM_VG_H


#include "storage/Devices/Device.h"


namespace storage
{

    using namespace std;


    class LvmVg : public Device
    {
    public:

	LvmVg(DeviceGraph& device_graph, const string& name);

	static LvmVg* create(DeviceGraph& device_graph, const string& name)
	    { return new LvmVg(device_graph, name); }

	const string& getName() const;
	void setName(const string& name);

	virtual string display_name() const override { return getName(); }

	virtual void check() const override;

    protected:

	class Impl;

	Impl& getImpl();
	const Impl& getImpl() const;

	virtual LvmVg* clone(DeviceGraph& device_graph) const override;

    protected:

	LvmVg(Impl* impl);
	// LvmVg(DeviceGraph& device_graph, Impl* impl);

    };

}

#endif
