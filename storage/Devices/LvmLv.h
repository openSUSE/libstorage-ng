#ifndef LVM_LV_H
#define LVM_LV_H


#include "storage/Devices/BlkDevice.h"


namespace storage
{

    class LvmLv : public BlkDevice
    {
    public:

	static LvmLv* create(Devicegraph* devicegraph, const std::string& name);
	static LvmLv* load(Devicegraph* devicegraph, const xmlNode* node);

	virtual void check() const override;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual LvmLv* clone() const override;

    protected:

	LvmLv(Impl* impl);

    };


    inline bool
    is_lvm_lv(const Device* device)
    {
	return dynamic_cast<const LvmLv*>(device) != nullptr;
    }


    inline LvmLv*
    to_lvm_lv(Device* device)
    {
	return dynamic_cast<LvmLv*>(device);
    }


    inline const LvmLv*
    to_lvm_lv(const Device* device)
    {
	return dynamic_cast<const LvmLv*>(device);
    }

}

#endif
