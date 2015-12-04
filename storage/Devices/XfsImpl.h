#ifndef STORAGE_XFS_IMPL_H
#define STORAGE_XFS_IMPL_H


#include "storage/Devices/Xfs.h"
#include "storage/Devices/FilesystemImpl.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Xfs> { static const char* classname; };


    class Xfs::Impl : public Filesystem::Impl
    {
    public:

	Impl()
	    : Filesystem::Impl() {}

	Impl(const xmlNode* node);

	virtual FsType get_type() const override { return XFS; }

	virtual const char* get_classname() const override { return "Xfs"; }

	virtual string get_displayname() const override { return "xfs"; }

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void do_create() const override;

	virtual void do_set_label() const override;

    };

}

#endif
