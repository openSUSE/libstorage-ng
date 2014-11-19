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

	Impl(DeviceGraph& device_graph)
	    : Filesystem::Impl(device_graph) {}

	Impl(DeviceGraph& device_graph, const Impl& impl)
	    : Filesystem::Impl(device_graph, impl) {}

	virtual Impl* clone(DeviceGraph& device_graph) const override { return new Impl(device_graph, *this); }

	virtual void add_create_actions(ActionGraph& action_graph) const override;

    };

}

#endif
