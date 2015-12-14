#ifndef STORAGE_NTFS_H
#define STORAGE_NTFS_H


#include "storage/Devices/Filesystem.h"


namespace storage
{

    class Ntfs : public Filesystem
    {
    public:

	static Ntfs* create(Devicegraph* devicegraph);
	static Ntfs* load(Devicegraph* devicegraph, const xmlNode* node);

	bool supports_label() const override { return true; }
	unsigned int max_labelsize() const override { return 128; }

	bool supports_uuid() const override { return true; }

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Ntfs* clone() const override;

    protected:

	Ntfs(Impl* impl);

    };


    bool is_ntfs(const Device* device);

    Ntfs* to_ntfs(Device* device);
    const Ntfs* to_ntfs(const Device* device);

}

#endif
