#ifndef EXT4_H
#define EXT4_H


#include "storage/Devices/Filesystem.h"


namespace storage
{

    class Ext4 : public Filesystem
    {
    public:

	virtual FsType get_type() const override { return EXT4; }

	static Ext4* create(Devicegraph* devicegraph);
	static Ext4* load(Devicegraph* devicegraph, const xmlNode* node);

	bool supports_label() const override { return true; }
	unsigned int max_labelsize() const override { return 16; }

	bool supports_uuid() const override { return true; }

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Ext4* clone() const override;

    protected:

	Ext4(Impl* impl);

    };

}

#endif
