#ifndef XFS_H
#define XFS_H


#include "storage/Devices/Filesystem.h"


namespace storage
{

    class Xfs : public Filesystem
    {
    public:

	static Xfs* create(Devicegraph* devicegraph);
	static Xfs* load(Devicegraph* devicegraph, const xmlNode* node);

	bool supports_label() const override { return true; }
	unsigned int max_labelsize() const override { return 12; }

	bool supports_uuid() const override { return true; }

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Xfs* clone() const override;

    protected:

	Xfs(Impl* impl);

    };


    inline bool
    is_xfs(const Device* device)
    {
	return dynamic_cast<const Xfs*>(device) != nullptr;
    }


    inline Xfs*
    to_xfs(Device* device)
    {
	return dynamic_cast<Xfs*>(device);
    }


    inline const Xfs*
    to_xfs(const Device* device)
    {
	return dynamic_cast<const Xfs*>(device);
    }

}

#endif
