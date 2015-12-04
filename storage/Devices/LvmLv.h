#ifndef STORAGE_LVM_LV_H
#define STORAGE_LVM_LV_H


#include "storage/Devices/BlkDevice.h"


namespace storage
{

    //! A Logical Volume of the Logical Volume Manager (LVM).
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


    bool is_lvm_lv(const Device* device);

    LvmLv* to_lvm_lv(Device* device);
    const LvmLv* to_lvm_lv(const Device* device);

}

#endif
