#ifndef STORAGE_USER_IMPL_H
#define STORAGE_USER_IMPL_H


#include "storage/Holders/User.h"
#include "storage/Holders/HolderImpl.h"


namespace storage
{

    template <> struct HolderTraits<User> { static const char* classname; };


    class User::Impl : public Holder::Impl
    {
    public:

	Impl()
	    : Holder::Impl() {}

	Impl(const xmlNode* node);

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	virtual const char* get_classname() const override { return HolderTraits<User>::classname; }

	virtual bool equal(const Holder::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Holder::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

    };

}

#endif
