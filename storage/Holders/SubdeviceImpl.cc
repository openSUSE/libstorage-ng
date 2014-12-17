

#include "storage/Holders/SubdeviceImpl.h"


namespace storage
{

    Subdevice::Impl::Impl(const xmlNode* node)
	: Holder::Impl(node)
    {
    }


    void
    Subdevice::Impl::save(xmlNode* node) const
    {
	Holder::Impl::save(node);
    }

}
