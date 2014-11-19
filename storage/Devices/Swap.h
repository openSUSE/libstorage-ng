#ifndef SWAP_H
#define SWAP_H


#include "storage/Devices/Filesystem.h"


namespace storage
{

    using namespace std;


    class Swap : public Filesystem
    {
    public:

	Swap(DeviceGraph& device_graph);

	virtual string display_name() const override { return "swap"; }

	bool supportsLabel() const override { return false; }
	unsigned int maxLabelSize() const override { return 0; }

	bool supportsUuid() const override { return false; }

    public:

	class Impl;

	Impl& getImpl();
	const Impl& getImpl() const;

	virtual Swap* clone(DeviceGraph& device_graph) const override;

    protected:

	Swap(Impl* impl);
	// Swap(DeviceGraph& device_graph, Impl* impl);

    };

}

#endif
