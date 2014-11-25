#ifndef EXT4_H
#define EXT4_H


#include "storage/Devices/Filesystem.h"


namespace storage
{

    using namespace std;


    class Ext4 : public Filesystem
    {
    public:

	Ext4(DeviceGraph& device_graph);

	static Ext4* create(DeviceGraph& device_graph)
	    { return new Ext4(device_graph); }

	virtual string display_name() const override { return "ext4"; }

	bool supportsLabel() const override { return true; }
	unsigned int maxLabelSize() const override { return 16; }

	bool supportsUuid() const override { return true; }

    public:

	class Impl;

	Impl& getImpl();
	const Impl& getImpl() const;

	virtual Ext4* clone(DeviceGraph& device_graph) const override;

    protected:

	Ext4(Impl* impl);
	// Ext4(DeviceGraph& device_graph, Impl* impl);

    };

}

#endif
