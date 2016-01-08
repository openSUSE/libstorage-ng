#ifndef STORAGE_MD_USER_H
#define STORAGE_MD_USER_H


#include "storage/Holders/User.h"


namespace storage
{

    class MdUser : public User
    {
    public:

	static MdUser* create(Devicegraph* devicegraph, const Device* source, const Device* target);
	static MdUser* load(Devicegraph* devicegraph, const xmlNode* node);

	virtual MdUser* clone() const override;

	bool is_spare() const;
	void set_spare(bool spare);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

    protected:

	MdUser(Impl* impl);

    };


    bool is_md_user(const Holder* holder);

    MdUser* to_md_user(Holder* device);
    const MdUser* to_md_user(const Holder* device);

}

#endif
