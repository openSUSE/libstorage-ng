#ifndef USING_H
#define USING_H


#include "storage/Holders/Holder.h"


namespace storage
{

    class Using : public Holder
    {
    public:

	static Using* create(Devicegraph* devicegraph, const Device* source, const Device* target);
	static Using* load(Devicegraph* devicegraph, const xmlNode* node);

	virtual Using* clone() const override;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

    private:

	Using(Impl* impl);

    };


    inline Using*
    to_using(Holder* device)
    {
	return dynamic_cast<Using*>(device);
    }


    inline const Using*
    to_using(const Holder* device)
    {
	return dynamic_cast<const Using*>(device);
    }

}

#endif
