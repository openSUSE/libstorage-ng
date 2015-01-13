#ifndef USING_IMPL_H
#define USING_IMPL_H


#include "storage/Holders/Using.h"
#include "storage/Holders/HolderImpl.h"


namespace storage
{

    class Using::Impl : public Holder::Impl
    {
    public:

	Impl()
	    : Holder::Impl() {}

	Impl(const xmlNode* node);

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	virtual const char* get_classname() const override { return "Using"; }

	virtual void print(std::ostream& out) const override;

    };

}

#endif
