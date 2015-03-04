#ifndef SUBDEVICE_H
#define SUBDEVICE_H


#include "storage/Holders/Holder.h"


namespace storage
{

    class Subdevice : public Holder
    {
    public:

	static Subdevice* create(Devicegraph* devicegraph, const Device* source, const Device* target);
	static Subdevice* load(Devicegraph* devicegraph, const xmlNode* node);

	virtual Subdevice* clone() const override;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

    protected:

	Subdevice(Impl* impl);

    };


    bool is_subdevice(const Holder* holder);

    Subdevice* to_subdevice(Holder* device);
    const Subdevice* to_subdevice(const Holder* device);

}

#endif
