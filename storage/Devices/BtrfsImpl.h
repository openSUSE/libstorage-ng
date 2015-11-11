#ifndef STORAGE_BTRFS_IMPL_H
#define STORAGE_BTRFS_IMPL_H


#include "storage/Devices/Btrfs.h"
#include "storage/Devices/FilesystemImpl.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    class Btrfs::Impl : public Filesystem::Impl
    {
    public:

	Impl()
	    : Filesystem::Impl() {}

	Impl(const xmlNode* node);

	virtual FsType get_type() const override { return BTRFS; }

	virtual const char* get_classname() const override { return "Btrfs"; }

	virtual string get_displayname() const override { return "btrfs"; }

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void do_create() const override;

	virtual void do_set_label() const override;

    };

}

#endif
