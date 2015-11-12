

#include "storage/Holders/UserImpl.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    User*
    User::create(Devicegraph* devicegraph, const Device* source, const Device* target)
    {
	User* ret = new User(new User::Impl());
	ret->Holder::create(devicegraph, source, target);
	return ret;
    }


    User*
    User::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	User* ret = new User(new User::Impl(node));
	ret->Holder::load(devicegraph, node);
	return ret;
    }


    User::User(Impl* impl)
	: Holder(impl)
    {
    }


    User*
    User::clone() const
    {
	return new User(get_impl().clone());
    }


    User::Impl&
    User::get_impl()
    {
	return dynamic_cast<Impl&>(Holder::get_impl());
    }


    const User::Impl&
    User::get_impl() const
    {
	return dynamic_cast<const Impl&>(Holder::get_impl());
    }


    bool
    is_user(const Holder* holder)
    {
	return dynamic_cast<const User*>(holder);
    }


    User*
    to_user(Holder* holder)
    {
	User* user = dynamic_cast<User*>(holder);
	if (!user)
	    ST_THROW(HolderHasWrongType("holder is not a User"));

	return user;
    }


    const User*
    to_using(const Holder* holder)
    {
	const User* user = dynamic_cast<const User*>(holder);
	if (!user)
	    ST_THROW(HolderHasWrongType("holder is not a User"));

	return user;
    }

}
