#ifndef STORAGE_SWAP_H
#define STORAGE_SWAP_H


#include "storage/Devices/Filesystem.h"


namespace storage
{

    class Swap : public Filesystem
    {
    public:

	static Swap* create(Devicegraph* devicegraph);
	static Swap* load(Devicegraph* devicegraph, const xmlNode* node);

	bool supports_label() const override { return true; }
	unsigned int max_labelsize() const override { return 15; }

	bool supports_uuid() const override { return true; }

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Swap* clone() const override;

    protected:

	Swap(Impl* impl);

    };


    bool is_swap(const Device* device);

    Swap* to_swap(Device* device);
    const Swap* to_swap(const Device* device);

}

#endif
