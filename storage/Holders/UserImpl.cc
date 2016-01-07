

#include "storage/Holders/UserImpl.h"


namespace storage
{

    const char* HolderTraits<User>::classname = "User";


    User::Impl::Impl(const xmlNode* node)
	: Holder::Impl(node)
    {
    }


    void
    User::Impl::save(xmlNode* node) const
    {
	Holder::Impl::save(node);
    }


    bool
    User::Impl::equal(const Holder::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	return Holder::Impl::equal(rhs);
    }


    void
    User::Impl::log_diff(std::ostream& log, const Holder::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Holder::Impl::log_diff(log, rhs);
    }


    void
    User::Impl::print(std::ostream& out) const
    {
	Holder::Impl::print(out);
    }

}
