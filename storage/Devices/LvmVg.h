#ifndef LVM_VG_H
#define LVM_VG_H


#include "storage/Devices/Device.h"
#include "storage/Devices/LvmLv.h"


namespace storage_bgl
{

    using namespace std;


    class LvmVg : public Device
    {
    public:

	static LvmVg* create(Devicegraph* devicegraph, const string& name);
	static LvmVg* load(Devicegraph* devicegraph, const xmlNode* node);

	const string& get_name() const;
	void set_name(const string& name);

	virtual string get_displayname() const override { return get_name(); }

	virtual void check() const override;

	LvmLv* create_lvm_lv(const string& name);

    protected:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual const char* get_classname() const override { return "LvmVg"; }

	virtual LvmVg* clone() const override;

    protected:

	LvmVg(Impl* impl);

    };

}

#endif
