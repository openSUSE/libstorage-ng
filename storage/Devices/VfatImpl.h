#ifndef STORAGE_VFAT_IMPL_H
#define STORAGE_VFAT_IMPL_H


#include "storage/Devices/Vfat.h"
#include "storage/Devices/FilesystemImpl.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Vfat> { static const char* classname; };


    class Vfat::Impl : public Filesystem::Impl
    {
    public:

	Impl()
	    : Filesystem::Impl() {}

	Impl(const xmlNode* node);

	virtual FsType get_type() const override { return FsType::VFAT; }

	virtual const char* get_classname() const override { return DeviceTraits<Vfat>::classname; }

	virtual string get_displayname() const override { return "vfat"; }

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void do_create() const override;

	virtual void do_set_label() const override;

    };

}

#endif
