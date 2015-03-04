

#include "storage/Holders/UserImpl.h"


namespace storage
{

    User::Impl::Impl(const xmlNode* node)
	: Holder::Impl(node)
    {
    }


    void
    User::Impl::save(xmlNode* node) const
    {
	Holder::Impl::save(node);
    }


    void
    User::Impl::print(std::ostream& out) const
    {
	Holder::Impl::print(out);
    }

}
