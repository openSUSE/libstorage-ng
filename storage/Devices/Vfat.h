#ifndef STORAGE_VFAT_H
#define STORAGE_VFAT_H


#include "storage/Devices/Filesystem.h"


namespace storage
{

    class Vfat : public Filesystem
    {
    public:

	static Vfat* create(Devicegraph* devicegraph);
	static Vfat* load(Devicegraph* devicegraph, const xmlNode* node);

	bool supports_label() const override { return true; }
	unsigned int max_labelsize() const override { return 11; }

	bool supports_uuid() const override { return true; }

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Vfat* clone() const override;

    protected:

	Vfat(Impl* impl);

    };


    bool is_vfat(const Device* device);

    Vfat* to_vfat(Device* device);
    const Vfat* to_vfat(const Device* device);

}

#endif
