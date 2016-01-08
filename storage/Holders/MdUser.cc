

#include "storage/Holders/MdUserImpl.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    MdUser*
    MdUser::create(Devicegraph* devicegraph, const Device* source, const Device* target)
    {
	MdUser* ret = new MdUser(new MdUser::Impl());
	ret->Holder::create(devicegraph, source, target);
	return ret;
    }


    MdUser*
    MdUser::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	MdUser* ret = new MdUser(new MdUser::Impl(node));
	ret->Holder::load(devicegraph, node);
	return ret;
    }


    MdUser::MdUser(Impl* impl)
	: User(impl)
    {
    }


    MdUser*
    MdUser::clone() const
    {
	return new MdUser(get_impl().clone());
    }


    MdUser::Impl&
    MdUser::get_impl()
    {
	return dynamic_cast<Impl&>(Holder::get_impl());
    }


    const MdUser::Impl&
    MdUser::get_impl() const
    {
	return dynamic_cast<const Impl&>(Holder::get_impl());
    }


    bool
    MdUser::is_spare() const
    {
	return get_impl().is_spare();
    }


    void
    MdUser::set_spare(bool spare)
    {
	get_impl().set_spare(spare);
    }


    bool
    is_md_user(const Holder* holder)
    {
	return is_holder_of_type<const MdUser>(holder);
    }


    MdUser*
    to_md_user(Holder* holder)
    {
	return to_holder_of_type<MdUser>(holder);
    }


    const MdUser*
    to_md_user(const Holder* holder)
    {
	return to_holder_of_type<const MdUser>(holder);
    }

}
