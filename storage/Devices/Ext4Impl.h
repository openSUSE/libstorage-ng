#ifndef STORAGE_EXT4_IMPL_H
#define STORAGE_EXT4_IMPL_H


#include "storage/Devices/Ext4.h"
#include "storage/Devices/FilesystemImpl.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Ext4> { static const char* classname; };


    class Ext4::Impl : public Filesystem::Impl
    {
    public:

	Impl()
	    : Filesystem::Impl() {}

	Impl(const xmlNode* node);

	virtual FsType get_type() const override { return EXT4; }

	virtual const char* get_classname() const override { return "Ext4"; }

	virtual string get_displayname() const override { return "ext4"; }

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void do_create() const override;

	virtual void do_set_label() const override;

    };

}

#endif
