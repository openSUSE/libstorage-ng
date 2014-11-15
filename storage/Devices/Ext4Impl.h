#ifndef EXT4_IMPL_H
#define EXT4_IMPL_H


#include "storage/Devices/Ext4.h"
#include "storage/Devices/FilesystemImpl.h"


namespace storage
{

    using namespace std;


    class Ext4::Impl : public Filesystem::Impl
    {
    public:

	Impl() : Filesystem::Impl() {}

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void add_create_actions(ActionGraph& action_graph) const override;

    };

}

#endif
