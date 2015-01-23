

#include "storage/Devices/LvmLvImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


using namespace std;


namespace storage
{

    LvmLv::Impl::Impl(const xmlNode* node)
	: BlkDevice::Impl(node)
    {
    }


    void
    LvmLv::Impl::save(xmlNode* node) const
    {
	BlkDevice::Impl::save(node);
    }


    bool
    LvmLv::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!BlkDevice::Impl::equal(rhs))
	    return false;

	return true;
    }


    void
    LvmLv::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	BlkDevice::Impl::log_diff(log, rhs);
    }


    void
    LvmLv::Impl::print(std::ostream& out) const
    {
	BlkDevice::Impl::print(out);
    }


    Text
    LvmLv::Impl::do_create_text(bool doing) const
    {
	return sformat(_("Create logical volume %1$s (%2$s)"), get_displayname().c_str(),
		       get_size_string().c_str());
    }

}
