

#include "storage/Holders/MdUserImpl.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage
{

    const char* HolderTraits<MdUser>::classname = "MdUser";


    MdUser::Impl::Impl(const xmlNode* node)
	: User::Impl(node), spare(false)
    {
	getChildValue(node, "spare", spare);
    }


    void
    MdUser::Impl::save(xmlNode* node) const
    {
	User::Impl::save(node);

	setChildValueIf(node, "spare", spare, spare);
    }


    bool
    MdUser::Impl::equal(const Holder::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!User::Impl::equal(rhs))
	    return false;

	return spare == rhs.spare;
    }


    void
    MdUser::Impl::log_diff(std::ostream& log, const Holder::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	User::Impl::log_diff(log, rhs);

	storage::log_diff(log, "spare", spare, rhs.spare);
    }


    void
    MdUser::Impl::print(std::ostream& out) const
    {
	User::Impl::print(out);

	if (spare)
	    out << " spare";
    }

}
