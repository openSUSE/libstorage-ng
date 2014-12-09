#ifndef LVM_LV_H
#define LVM_LV_H


#include "storage/Devices/BlkDevice.h"


namespace storage_bgl
{

    using namespace std;


    class LvmLv : public BlkDevice
    {
    public:

	static LvmLv* create(Devicegraph* devicegraph, const string& name);
	static LvmLv* load(Devicegraph* devicegraph, const xmlNode* node);

	virtual void check() const override;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual const char* get_classname() const override { return "LvmLv"; }

	virtual LvmLv* clone() const override;

    protected:

	LvmLv(Impl* impl);

    };

}

#endif
