#ifndef SWAP_H
#define SWAP_H


#include "storage/Devices/Filesystem.h"


namespace storage
{

    class Swap : public Filesystem
    {
    public:

	static Swap* create(Devicegraph* devicegraph);
	static Swap* load(Devicegraph* devicegraph, const xmlNode* node);

	bool supports_label() const override { return false; }
	unsigned int max_labelsize() const override { return 0; }

	bool supports_uuid() const override { return false; }

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Swap* clone() const override;

    protected:

	Swap(Impl* impl);

    };


    inline bool
    is_swap(const Device* device)
    {
	return dynamic_cast<const Swap*>(device) != 0;
    }

}

#endif
