#ifndef SWAP_IMPL_H
#define SWAP_IMPL_H


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

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	virtual void add_create_actions(Actiongraph& actiongraph) const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

    };

}

#endif
