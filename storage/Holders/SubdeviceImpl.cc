

#include "storage/Holders/SubdeviceImpl.h"


namespace storage
{

    const char* HolderTraits<Subdevice>::classname = "Subdevice";


    Subdevice::Impl::Impl(const xmlNode* node)
	: Holder::Impl(node)
    {
    }


    void
    Subdevice::Impl::save(xmlNode* node) const
    {
	Holder::Impl::save(node);
    }


    bool
    Subdevice::Impl::equal(const Holder::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	return Holder::Impl::equal(rhs);
    }


    void
    Subdevice::Impl::log_diff(std::ostream& log, const Holder::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Holder::Impl::log_diff(log, rhs);
    }


    void
    Subdevice::Impl::print(std::ostream& out) const
    {
	Holder::Impl::print(out);
    }

}
