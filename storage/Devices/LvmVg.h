#ifndef LVM_VG_H
#define LVM_VG_H


#include "storage/Devices/Device.h"
#include "storage/Devices/LvmLv.h"


namespace storage
{

    using namespace std;


    class LvmVg : public Device
    {
    public:

	static LvmVg* create(DeviceGraph* device_graph, const string& name);
	static LvmVg* load(DeviceGraph* device_graph, const xmlNode* node);

	const string& getName() const;
	void setName(const string& name);

	virtual string getDisplayName() const override { return getName(); }

	virtual void check() const override;

	LvmLv* createLvmLv(const string& name);

    protected:

	class Impl;

	Impl& getImpl();
	const Impl& getImpl() const;

	virtual const char* getClassName() const override { return "LvmVg"; }

	virtual LvmVg* clone() const override;

    protected:

	LvmVg(Impl* impl);

    };

}

#endif
