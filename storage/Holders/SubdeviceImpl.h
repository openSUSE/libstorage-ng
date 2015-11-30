#ifndef STORAGE_SUBDEVICE_IMPL_H
#define STORAGE_SUBDEVICE_IMPL_H


#include "storage/Holders/Subdevice.h"
#include "storage/Holders/HolderImpl.h"


namespace storage
{

    template <> struct HolderTraits<Subdevice> { static const char* classname; };


    class Subdevice::Impl : public Holder::Impl
    {
    public:

	Impl()
	    : Holder::Impl() {}

	Impl(const xmlNode* node);

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	virtual const char* get_classname() const override { return "Subdevice"; }

	virtual void print(std::ostream& out) const override;

    };

}

#endif
