#ifndef STORAGE_NTFS_IMPL_H
#define STORAGE_NTFS_IMPL_H


#include "storage/Devices/Ntfs.h"
#include "storage/Devices/FilesystemImpl.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Ntfs> { static const char* classname; };


    class Ntfs::Impl : public Filesystem::Impl
    {
    public:

	Impl()
	    : Filesystem::Impl() {}

	Impl(const xmlNode* node);

	virtual FsType get_type() const override { return FsType::NTFS; }

	virtual const char* get_classname() const override { return DeviceTraits<Ntfs>::classname; }

	virtual string get_displayname() const override { return "ntfs"; }

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void do_create() const override;

	virtual void do_set_label() const override;

	virtual void do_resize(ResizeMode resize_mode) const override;

    };

}

#endif
