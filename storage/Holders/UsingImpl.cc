

#include "storage/Holders/UsingImpl.h"


namespace storage
{

    Using::Impl::Impl(const xmlNode* node)
	: Holder::Impl(node)
    {
    }


    void
    Using::Impl::save(xmlNode* node) const
    {
	Holder::Impl::save(node);
    }


    void
    Using::Impl::print(std::ostream& out) const
    {
	Holder::Impl::print(out);
    }

}
