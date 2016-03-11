#ifndef STORAGE_MD_USER_IMPL_H
#define STORAGE_MD_USER_IMPL_H


#include "storage/Holders/MdUser.h"
#include "storage/Holders/UserImpl.h"


namespace storage
{

    template <> struct HolderTraits<MdUser> { static const char* classname; };


    class MdUser::Impl : public User::Impl
    {
    public:

	Impl()
	    : User::Impl(), spare(false), faulty(false) {}

	Impl(const xmlNode* node);

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	virtual const char* get_classname() const override { return HolderTraits<MdUser>::classname; }

	virtual bool equal(const Holder::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Holder::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	bool is_spare() const { return spare; }
	void set_spare(bool spare) { Impl::spare = spare; }

	bool is_faulty() const { return faulty; }
	void set_faulty(bool faulty) { Impl::faulty = faulty; }

    private:

	bool spare;
	bool faulty;

    };

}

#endif
