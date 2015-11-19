#ifndef STORAGE_SWAP_IMPL_H
#define STORAGE_SWAP_IMPL_H


#include "storage/Devices/Swap.h"
#include "storage/Devices/FilesystemImpl.h"


namespace storage
{

    using namespace std;


    class Swap::Impl : public Filesystem::Impl
    {
    public:

	Impl()
	    : Filesystem::Impl() {}

	Impl(const xmlNode* node);

	virtual FsType get_type() const override { return SWAP; }

	virtual const char* get_classname() const override { return "Swap"; }

	virtual string get_displayname() const override { return "swap"; }

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void do_create() const override;
	virtual void do_mount(const Actiongraph::Impl& actiongraph, const string& mountpoint) const override;

    };

}

#endif
