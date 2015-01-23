

#include "storage/Devices/LvmVgImpl.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage
{

    using namespace std;


    LvmVg::Impl::Impl(const xmlNode* node)
	: Device::Impl(node)
    {
	if (!getChildValue(node, "name", name))
	    throw runtime_error("no name");
    }


    void
    LvmVg::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValue(node, "name", name);
    }


    bool
    LvmVg::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Device::Impl::equal(rhs))
	    return false;

	return name == rhs.name;
    }


    void
    LvmVg::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Device::Impl::log_diff(log, rhs);

	storage::log_diff(log, "name", name, rhs.name);
    }


    void
    LvmVg::Impl::print(std::ostream& out) const
    {
	Device::Impl::print(out);
    }


    Text
    LvmVg::Impl::do_create_text(bool doing) const
    {
	return sformat(_("Create volume group %1$s"), get_displayname().c_str());
    }

}
