#ifndef USER_H
#define USER_H


#include "storage/Holders/Holder.h"


namespace storage
{

    class User : public Holder
    {
    public:

	static User* create(Devicegraph* devicegraph, const Device* source, const Device* target);
	static User* load(Devicegraph* devicegraph, const xmlNode* node);

	virtual User* clone() const override;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

    private:

	User(Impl* impl);

    };


    inline bool
    is_user(const Holder* holder)
    {
	return dynamic_cast<const User*>(holder) != 0;
    }


    inline User*
    to_user(Holder* device)
    {
	return dynamic_cast<User*>(device);
    }


    inline const User*
    to_user(const Holder* device)
    {
	return dynamic_cast<const User*>(device);
    }

}

#endif
