#ifndef STORAGE_USER_H
#define STORAGE_USER_H


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

    protected:

	User(Impl* impl);

    };


    bool is_user(const Holder* holder);

    User* to_user(Holder* device);
    const User* to_user(const Holder* device);

}

#endif
